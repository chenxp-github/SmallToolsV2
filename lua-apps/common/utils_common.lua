require("common_consts");

--创建mem类的函数,可以传入大小和文件名--
--也可以不写参数，创建默认大小的mem类--
function new_mem(size)
    local mem = Mem.new();
    
    if not size then
        size = LBUF_SIZE;
    end
    
    if type(size) == "number" then
        mem:Malloc(size);
    end
        
    local mem_file = mem:FileBase();
    
    if type(size) == "string" then
        local filename = size;
        if not mem_file:LoadFile(filename) then
            printnl("load file "..filename.." fail");
            return nil;
        end
    end

    return mem,mem_file;
end


--创建memfile类的函数,可以传入大小和文件名--
--也可以不写参数，创建默认大小的memfile类--
function new_memfile(block_size, max_blocks)
    
    local mf = MemFile.new();
    
    if not block_size then
        mf:Init();
    end
    
    if type(block_size) == "number" then
        mf:Init(block_size,max_blocks);
    end
        
    local mf_file = mf:FileBase();
    
    if type(block_size) == "string" then
        local filename = block_size;
        if not mf_file:LoadFile(filename) then
            printnl("load file "..filename.." fail");
            return nil;
        end
    end
    
    return mf,mf_file;    
end

--创建File类的帮助函数--
function new_file(filename,mode)
    if not mode then mode = "rb" end
    
    local file = File.new();
    
    if mode == "r" or mode == "rb" then
        file:SetBufSize(4096*1024);        
    end
    
    if not file:OpenFile(filename,mode) then
        printnl("open file "..filename.." error.");
        return
    end
    
    return file, file:FileBase();    
end

function new_file_no_buffer(filename,mode)
    if not mode then mode = "rb" end
    
    local file = File.new();

    if not file:OpenFile(filename,mode) then
        print("open file "..filename.." error.");
        return
    end
    
    return file;
end



--创建mmapfile的方法，不能创建超过4G的文件，因为-
--是32位程序
function new_mmapfile(filename,mode)
    if not mode then mode = "r" end
    
    local mapfile = MMapFile.new();
    local success;
    
    if mode == "r" then
        success = mapfile:OpenReadOnly(filename);
    end
    
    if not success then
        printnl("map file "..filename.." fail");
        return;
    end
    
    return mapfile,mapfile:Stream();
end



--移除文件名的前缀--
--比如 C:\1.txt ,如果把前缀C:移除，就变成了 1.txt 了 
function remove_path_prefix(full_name,prefix)
    if string.find(full_name,prefix,1,true) ~= 1 then
        return;
    end
    local len = string.len(prefix);
    if len == 0 then return full_name end
    local rpath = string.sub(full_name,len+1);
    local first = string.char(string.byte(rpath,1));
    if first == "\\" or first =="/" then
        return string.sub(rpath,2);
    else
        return rpath;
    end
end


--分隔文件后缀的列表，逗号分隔， e.g. "c,cpp,h,lua,js"
--返回一个hash表
function split_file_exts_list(exts)
    local tab = {};
    local flag = false;
    
    for k in string.gmatch(exts, "([%a%d]+)") do
        tab[k] = true;
        flag = true;
    end
    
    if flag then
        return tab;
    end
end

--对FileBase类中每一行进行迭代的函数--
--filename 可以是文件名，也可以是FileBase类--
function for_each_line(filename,callback)
    local list_file = filename;
	if type(filename) == "string" then
		list_file = new_file(filename,"rb");
		if not list_file then
			printnl("open list file "..filename.." fail");
			return
		end
	end

    local mem = new_mem();    
    list_file:Seek(0);
    while list_file:ReadLine(mem) do
		mem:Seek(0);
        if callback(mem) then
            break;
        end
    end
	
    if type(filename) == "string" then
        list_file:Destroy();
    end
end

--执行一个命令行，并且返回结果--
function execute_command(cmd)
    local f = io.popen(cmd);
    if not f then return end
    local str = f:read("*a");
    f:close();
    return str;
end

--执行一个命令，支持格式化--
function execf(...)
    local cmd = string.format(...);
    printnl("exec:"..cmd);
    return os.execute(cmd);
end

--得到当前的git 的 branch 值--
function git_current_branch(folder)
    local cd = FileManager.GetCurDir();
    local git_folder = FileManager.ToAbsPath(folder);
    FileManager.ChangeDir(git_folder);
    local str = execute_command("git branch");
    FileManager.ChangeDir(cd);
    return string.match(str,"%*%s*([%w_]+)");
end


--按字母顺序排列的pair函数--
function pairs_ordered(tab)
    local order_tab={};
    for k,v in pairs(tab) do
        local tmp = {key=k,val=v};
        table.insert(order_tab,tmp);
    end

    table.sort(order_tab,function(e1,e2)
        if type(e1.key) == "number" and type(e2.key) == number then
            return e1.key < e2.key;
        end
        return string.lower(e1.key) < string.lower(e2.key);
    end);

    local i = 0;
    
    function iter()       
        i = i + 1;
        
        if not order_tab[i] then
            return nil;
        end
        
        return order_tab[i].key,order_tab[i].val;
    end
    
    return iter;
end

--打印函数,printf--
function printf(...)
    print(string.format(...));
end

--打印函数，printf最后加上换行符--
function printfnl(...)
    printnl(string.format(...));
end


--把一个filebase类转换成string--
function file_to_string(file)
    local mem = new_mem(file:GetSize());
    mem:WriteFile(file);
    local str = mem:CStr();
    mem:Destroy();
    return str;
end


--把一个filebase类转换成mem--
function file_to_mem(file)
    local mem = new_mem(file:GetSize());
    mem:WriteFile(file);    
    return mem;
end


--去掉字符串前后的空格--
string.trim=function(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end

--把一个Table变成lua源代码
function object_to_lua(obj,pbuf)
    if type(obj) ~= "table" then
        return;
    end

    for k,v in pairs(obj) do      
        local key = k;

        if type(k) == "number"  then
            key = string.format("[%d]",k);
        else
            key = "[\""..k.."\"]";
        end

        if type(v) == "table" then
            pbuf:Log("%s = {",key);
            pbuf:IncLogLevel(1);
            object_to_lua(v,pbuf);
            pbuf:IncLogLevel(-1);
            pbuf:Log("},");
        else
            pbuf:Tab();
            pbuf:Printf("%s = ",key);
            if type(v) == "boolean" then
                pbuf:Print(v and "true" or "false");
            elseif type(v) == "string" then
                pbuf:Print("[[");
                pbuf:Print(v);
                pbuf:Print("]]")
            else
                pbuf:Print(v);
            end
            pbuf:Print(",");
            pbuf:Eol();
        end
    end
end

--使用tcp协议链接服务器，直到成功为止。需要在一个CoThread中执行
function tcp_connect_to_server(thread,server, port)
    local is_end = false;
    local remote_socket = nil;
    TcpSocket.NewTcpConnector(server,port,
        function(event,new_socket)
            if event == EVENT_CONNECTED then
                remote_socket = new_socket;
            end
            is_end = true;
        end
    );
    while not is_end do
        thread:Sleep(10);
    end
    return remote_socket;
end

--读取unix系统下的伪文件，比如/proc下面的文件,返回mem类
function read_pseudo_file(filename)
    local file = new_file(filename);
    if not file then return end
    local mf = new_memfile();
    file:WriteToFile(mf,0,0x7fffffff);
    local mem = new_mem(mf:GetSize());
    mem:Write(mf,mf:GetSize());
    mf:Destroy();
    return mem;
end
