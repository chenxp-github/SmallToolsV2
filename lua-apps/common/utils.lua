require("common_consts");
require("utils_common");

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

function exit(...)
    local args = table.pack(...);
    local code = args[1];

    if type(code) == "string" then
        print(string.format(...));
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

function split_file_lines(filename,callback)
    for_each_line(filename,function(mem)
        mem:Trim();
        if mem:C(0) ~= 0 then
            return callback(mem:CStr());
        end
    end);
end

function get_self_path(rpath)
    if not rpath then rpath="" end
    local path = FileManager.SliceFileName(
        FileManager.ToAbsPath(arg[0]),FN_PATH
    );
    return FileManager.ToAbsPath(path.."/"..rpath);
end

function printnl(str)
    print(str);
end

function self_call(...)
	local args = App.GetArgs();
	local user_cmd = string.format(...);	
	return os.execute(args[1].." "..args[2].." "..user_cmd);
end

function ping_net_addr(name,retry)
    if not retry then retry = 10 end
    for i=1,retry,1 do
        printfnl("try ping %d...",i);
        if is_windows() then
            if execf("ping %s -w 1000 -n 1",name) then
                return true;
            end
        else
            if execf("ping %s -W 1 -c 1",name) then
                return true;
            end
        end        
    end
end

--把十六进制转换成二进制文件--
function hex_file_to_bin_file(hex_file, bin_file)
    hex_file:Seek(0);    
    local part;
    while not hex_file:IsEnd() do
        local ch = hex_file:Getc();
        local bval = HEX_TO_BIN_TABLE[ch];
        if bval then
            if not part then 
                part = bval*16             
            else 
                bin_file:Putc(part+bval);
                part = nil;
            end
        end
    end
end

