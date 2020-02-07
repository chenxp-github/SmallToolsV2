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

    