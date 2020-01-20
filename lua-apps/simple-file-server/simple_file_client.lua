require("common")
require("peer_service_base")
require("linkrpc_sfs_types")
require("file_server_helper");

SimpleFileClient = class(PeerServiceBase);

function SimpleFileClient:ctor(thread)
    self.m_thread = thread;
end

function SimpleFileClient:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
--##End OnRequest ##--
end

--@@Begin Method List @@--
function SimpleFileClient:List(_dir)    
    local complete = false;
    local list = nil;

    local _cbid = self:AddCallback(function(ret,val)
        complete = true;
        if val then
            list = val.list;
        end
    end,-1);

    local _param={
        dir = _dir,
    };
    self:SendRequest(_param,METHOD_SFS_LIST,_cbid);

    while not complete and not self:IsClosedPermanently() do
        self.m_thread:Sleep(1);
    end

    return list;
end
--@@End Method List @@--


--@@Begin Method PushSmallFile @@--
function SimpleFileClient:PushSmallFile(_filename, _data, _callback)
    local _cbid = self:AddCallback(_callback,-1);
    local _param={
        filename = _filename,
        data = {_binary_=_data},
    };
    return self:SendRequest(_param,METHOD_SFS_PUSHSMALLFILE,_cbid);
end
--@@End Method PushSmallFile @@--

--@@Begin Method PushBigFile @@--
function SimpleFileClient:PushBigFile(local_file,remote_file,no_wait)
    local error_occur = false;
    local response_number = 0;
    function inner_callback(ret,val)
        if (not val) or (not val.success) then            
            if not error_occur then
                error_occur = true;
                printfnl("push big file fail:%s",local_file);
            end
        end
        response_number = response_number + 1;
    end

    local request_number = 0;
    function send_part_data(offset,total_size,buf)
        local _cbid = self:AddCallback(inner_callback,-1);

        local _param={
            filename = remote_file,
            total_size = {_int64_=total_size},
            offset = {_int64_=offset},
            data = {_binary_=buf},
        };
        request_number = request_number+1;
        return self:SendRequest(_param,METHOD_SFS_PUSHBIGFILE,_cbid);
    end

    local file = new_file(local_file,"rb");
    if not file then return end

    file:Seek(0);    
    local buf = new_mem(4*1024*1024);

    while not file:IsEnd() and not error_occur do                
        if self:GetSendingQueueLength() < 30 then
            buf:SetSize(0);
            local offset = file:GetOffset();
            file:Read(buf,buf:GetMaxSize());                
            if buf:GetSize() > 0 then
                send_part_data(offset,file:GetSize(),buf);
            end
        else
            self.m_thread:Sleep(1);
        end
    end

    while not no_wait 
        and not error_occur 
        and request_number > response_number 
    do
        self.m_thread:Sleep(1);
    end

    buf:Destroy();
    file:Destroy();    

    return not error_occur;
end
--@@End Method PushBigFile @@--

--@@Begin Method PullFile @@--
function SimpleFileClient:PullFile(remote_file, local_file,on_complete)
    local all_complete = false;
    local error_occur = false;
    local file_to_close = nil;
    local _cbid;

    function on_pull_file(ret,val)
        if (not val) or (not val.success) then
            error_occur = true;
            all_complete = true;
            
            if file_to_close then
                file_to_close:Destroy();
            end

            if on_complete then
                on_complete(error_occur);
            end

            return
        end

        if ret == E_PART_OK then
            if type(local_file) == "string" then
                local str = local_file;

                local path=FileManager.SliceFileName(str,FN_PATH);
                if not FileManager.IsDirExist(path) then
                    FileManager.CreateDir(path);
                end

                local_file = new_file_no_buffer(local_file,"wb+");
                file_to_close = local_file;

                if not local_file then
                    printfnl("create new file fail:%s",str);
                    error_occur = true;
                    all_complete = true;
                    self:CancelCallback(_cbid);

                    if on_complete then
                        on_complete(error_occur);
                    end
                    return 
                end
            end

            local data = val.data._binary_;
            local offset = val.offset;
            if local_file then
                local_file:Seek(offset);
                local_file:Puts(data)
            end
        elseif ret == E_OK then
            all_complete = true;     
            error_occur = false;  
            
            if file_to_close then
                file_to_close:Destroy();
            end

            if on_complete then
                on_complete(error_occur);
            end
        else
            all_complete = true;
            error_occur = true;

            if file_to_close then
                file_to_close:Destroy();
            end

            if on_complete then
                on_complete(error_occur);
            end
        end   
    end

    _cbid = self:AddCallback(on_pull_file,-1);

    local _param={
        filename = remote_file,
    };
    
    self:SendRequest(_param,METHOD_SFS_PULLFILE,_cbid);

    while not on_complete and not all_complete and not self:IsClosedPermanently() do
        self.m_thread:Sleep(1);
    end

    return not error_occur;
end
--@@End Method PullFile @@--


--@@Begin Method RunCmd @@--
function SimpleFileClient:RunCommand(_command, _callback)
    local _cbid = self:AddCallback(_callback,-1);
    local _param={
        command = _command
    };
    return self:SendRequest(_param,METHOD_SFS_RUN_CMD,_cbid);
end
--@@End Method RunCmd @@--


--@@Begin Method ChangeDir @@--
function SimpleFileClient:ChangeDir(_dir, _callback)
    local _cbid = self:AddCallback(_callback,-1);
    local _param={
        dir = _dir,
    };
    return self:SendRequest(_param,METHOD_SFS_CHANGEDIR,_cbid);
end
--@@End Method ChangeDir @@--


--@@ Insert Method Here @@--
