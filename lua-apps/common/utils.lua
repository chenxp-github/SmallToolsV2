require("common_consts");
require("print_buffer");

function is_windows()
    return App.OS() == "win32";
end

function is_linux()
    return App.OS() == "linux";
end

function copy_files(from,to,use_log)
    local cmd = "";
    if is_linux() then
        cmd = string.format("cp -ar \"%s\" \"%s\"",from,to);
    end
    if is_windows() then
        assert(false);
    end
    
    if use_log then print(cmd);end
    return os.execute(cmd);
end

function checked_copy_files(from,to,use_log)
    local ret = copy_files(from,to,use_log);
    if not ret then
        os.exit(-1);
    end
end

function create_folder(folder,use_log)
    local cmd = "";
    if is_linux() then
        cmd = "mkdir -p "..folder;
    end
    
    if is_windows() then
        assert(false);
    end
    
    if use_log then print(cmd) end
    return os.execute(cmd);
end

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
            print("load file "..filename.." fail");
            return nil;
        end
    end

    return mem,mem_file;
end

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
        mem_file:Init();
        if not mem_file:LoadFile(filename) then
            print("load file "..filename.." fail");
            return nil;
        end
    end
    
    return mf,mf_file;    
end

function new_file(filename,mode)
    if not mode then mode = "rb" end
    
    local file = File.new();
    
    if mode == "r" or mode == "rb" then
        file:SetBufSize(4096*1024);        
    end
    
    if not file:OpenFile(filename,mode) then
        print("open file "..filename.." error.");
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

function new_mmapfile(filename,mode)
    if not mode then mode = "r" end
    
    local mapfile = MMapFile.new();
    local success;
    
    if mode == "r" then
        success = mapfile:OpenReadOnly(filename);
    end
    
    if not success then
        print("map file "..filename.." fail");
        return;
    end
    
    return mapfile,mapfile:Stream():FileBase();
end

function exit(code)        
    if type(code) == "string" then
        print(code);        
    end
    
    print(debug.traceback());

    if type(code) ~= "number" then
        code = -1;
    end
    
    os.exit(code);
end

function check_env(str,do_not_exit)
    local var = os.getenv(str);
    if not var then
        print("environment variable "..str.." must be set.");
        if not do_not_exit then
            exit(-1);
        end        
    end
    return var;
end

function remove_path_prefix(full_name,prefix)
    if string.find(full_name,prefix,1,true) ~= 1 then
        return
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

--split extension list e.g. "c,cpp,h,lua,js"
function split_file_exts_list(exts)
    local tab = {};
    for k in string.gmatch(exts, "([%a%d]+)") do
        tab[k] = true;
    end
    return tab;
end

function for_each_line(filename,callback)
    local list,list_file = new_file(filename,"rb");
    if not list_file then
        exit("open list file "..filename.." fail");
    end
    
    local mem,mem_file = new_mem();
    
    list_file:Seek(0);
    while list_file:ReadLine(mem_file) do
        mem_file:Trim();
        if mem:C(0) ~= 0 then            
            if callback(mem) then
                break;
            end
        end
    end
    list:CloseFile();   
end

function split_file_lines(filename,callback)
    for_each_line(filename,function(mem)
        callback(mem:CStr());
    end);
end

function execute_command(cmd)
    local f = io.popen(cmd);
    local str = f:read("*a");
    f:close();
    return str;
end

function git_current_branch(folder)
    local cd = FileManager.GetCurDir();
    local git_folder = FileManager.ToAbsPath(folder);
    FileManager.ChangeDir(git_folder);
    local str = execute_command("git branch");
    FileManager.ChangeDir(cd);
    return string.match(str,"%*%s*([%w_]+)");
end

function get_self_path(rpath)
    if not rpath then rpath="" end
    local path = FileManager.SliceFileName(
        FileManager.ToAbsPath(arg[0]),FN_PATH
    );
    return FileManager.ToAbsPath(path.."/"..rpath);
end

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

function printnl(str)
    print(str);
end

--打印函数,printf--
function printf(...)
    print(string.format(...));
end

--打印函数，printf最后加上换行符--
function printfnl(...)
    printnl(string.format(...));
end

function self_call(...)
	local args = App.GetArgs();
	local user_cmd = string.format(...);	
	return os.execute(args[1].." "..args[2].." "..user_cmd);
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


    