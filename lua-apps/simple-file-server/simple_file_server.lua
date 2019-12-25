require("common")
require("peer_service_base")
require("linkrpc_sfs_types")
require("file_server_helper")

SimpleFileServer = class(PeerServiceBase);

function SimpleFileServer:ctor()
    self.m_root_dir = "/";
    self.m_cur_big_file=nil;
    self.m_pulling_files = 0;
end

function SimpleFileServer:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
    if method == METHOD_SFS_LIST then
        self:OnList(_context,_param);
    end
    if method == METHOD_SFS_PUSHSMALLFILE then
        self:OnPushSmallFile(_context,_param);
    end
    if method == METHOD_SFS_PUSHBIGFILE then
        self:OnPushBigFile(_context,_param);
    end
    if method == METHOD_SFS_PULLFILE then
        self:OnPullFile(_context,_param);
    end
    if method == METHOD_SFS_RUN_CMD then
        self:OnRunCmd(_context,_param);
    end
--##End OnRequest ##--
end

--@@Begin Method OnList @@--
function SimpleFileServer:OnList(_context,_param)    
    local local_path = self:MakeLocalPath(_param.dir);
    local list = FsHelper.ListFolder(local_path);    

    local _ret={
        list = list,
        success = (list~=nil)
    };
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnList @@--


--@@Begin Method OnPushSmallFile @@--
function SimpleFileServer:OnPushSmallFile(_context,_param)    
    local data = _param.data._binary_;
    local file_name = self:MakeLocalPath(_param.filename);
    local ok = FsHelper.SaveSmallFile(file_name,data);
    local _ret={
        success = ok
    };
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnPushSmallFile @@--

function SimpleFileServer:SetCurBigFile(filename)
    if not self.m_cur_big_file then
        self.m_cur_big_file = {};
    end

    if self.m_cur_big_file.filename == filename then
        return;
    end

    self.m_cur_big_file.filename = filename;

    if self.m_cur_big_file.file then
        self.m_cur_big_file.file:Destroy();
    end

    if filename then
        filename = self:MakeLocalPath(filename);

        local path=FileManager.SliceFileName(filename,FN_PATH);
        if not FileManager.IsDirExist(path) then
            FileManager.CreateDir(path);
        end

        self.m_cur_big_file.file = new_file(filename,"w");
        if not self.m_cur_big_file.file then
            printfnl("open file %s error",filename)
        else
            printfnl("open file %s ok",filename)
        end
    end

end

--@@Begin Method OnPushBigFile @@--
function SimpleFileServer:OnPushBigFile(_context,_param)
    
    self:SetCurBigFile(_param.filename);
    if not self.m_cur_big_file.file then
        local _ret={
            success = false,
            message = "can not create file."
        };
        return self:SendReturnValue(_context,_ret);        
    end

    local file = self.m_cur_big_file.file;
    local data = _param.data._binary_;

    file:Seek(_param.offset);
    file:Write(data,data:GetSize());

    if file:GetOffset() >= _param.total_size then
        self:SetCurBigFile(nil);
    end

    local _ret={
        success = true,
    };
    return self:SendReturnValue(_context,_ret);
end
--@@End Method OnPushBigFile @@--

local g_buf = new_mem(4*1024*1024);
function SimpleFileServer:SendFileOnThread(_context,_filename)

    self.m_pulling_files = self.m_pulling_files + 1;

    local function send_thread(thread)
        local file = new_file_no_buffer(_filename,"rb");
        if not file then
            local _ret={
                success = false
            };
            self.m_pulling_files = self.m_pulling_files - 1;
            return self:SendReturnValue(_context,_ret);
        end

        file:Seek(0);
        local has_clients = true;

        while not file:IsEnd() do
            if self:GetAliveClientNumber() == 0 then              
                printnl("all clients disconnected");
                has_clients = false;
                break;
            end

            if self:GetSendingQueueLength() < 20 then
                g_buf:SetSize(0);
                local offset = file:GetOffset();
                file:Read(g_buf,g_buf:GetMaxSize());
                if g_buf:GetSize() > 0 then
                    local _ret={
                        success = true,
                        offset = {_int64_=offset},
                        data = {_binary_=g_buf},
                    };
                    self:SendPartReturnValue(_context,_ret);                    
                end            
            else
                thread:Sleep(1);            
            end
        end

        file:Destroy();
        
        local _ret={
            success = true,
        };

        self.m_pulling_files = self.m_pulling_files - 1;
        if has_clients then
            self:SendReturnValue(_context,_ret);               
        end
    end

    local co = CoThread.new(1);
    co:Start(send_thread);
    return co;
end

local g_all_pending_files = {};
local g_pull_file_thread = nil;
function SimpleFileServer:SendFile(_context,_filename)

    table.insert(g_all_pending_files,{
        context = _context,
        filename = _filename,
    });

    function sending_thread(thread)
        while #g_all_pending_files > 0 do
            if self.m_pulling_files < 20 then
                local head = g_all_pending_files[1];
                table.remove(g_all_pending_files,1);
                self:SendFileOnThread(head.context,head.filename);
            else
                thread:Sleep(1);
            end    
        end
        g_pull_file_thread = nil;
    end
        
    if not g_pull_file_thread then
        g_pull_file_thread = CoThread.new(1);
        g_pull_file_thread:Start(sending_thread);
    end
end

--@@Begin Method OnPullFile @@--
function SimpleFileServer:OnPullFile(_context,_param)
    return self:SendFile(_context,self:MakeLocalPath(_param.filename));
end
--@@End Method OnPullFile @@--



--@@Begin Method OnRunCmd @@--
function SimpleFileServer:OnRunCmd(_context,_param)
    printf("run cmd: %s",_param.command);
    local r = os.execute(_param.command);    
    local _ret={
        result = r and 1 or 0
    };
    return self:SendReturnValue(_context,_ret);
end
--@@End Method OnRunCmd @@--


--@@ Insert Method Here @@--

function SimpleFileServer:SetRootDir(root)
    self.m_root_dir = root;
end

function SimpleFileServer:MakeLocalPath(dir)
    local tmp_path = FileManager.ToAbsPath("",dir);
    return FileManager.ToAbsPath(tmp_path,self.m_root_dir);
end

