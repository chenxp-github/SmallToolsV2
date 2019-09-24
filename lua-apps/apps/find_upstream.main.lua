require("common");
require("utils");

function dir_up_level(level)
    local up="";
    for i=1,level,1 do
            if i > 1 then
                    up = up.."/";
            end
            up = up.."..";
    end
    return up;
end

function app_short_help()
    return "find the file or folder upstream";
end

function app_name()
    return "find_upstream"
end

function print_help(args)
    print(args[1].." <path or filename> [--path-only]");
end

function app_main(args)
    local argc = #args;
    
    if argc < 2 then
        return print_help(args);
    end
    
    local path_only = args[3] == "--path-only";

    local dest_dir = args[2];    
    local current_directory = FileManager.GetCurDir();
    
    for i=0,100,1 do
        local dir = FileManager.ToAbsPath(current_directory.."/"..dir_up_level(i));
        local test_dir = FileManager.ToAbsPath(dir.."/"..dest_dir);
        
        if FileManager.IsDirExist(test_dir) or FileManager.IsFileExist(test_dir) then
            if not path_only then
                print(test_dir);
            else
                local path = FileManager.SliceFileName(test_dir,FN_PATH);
                print(path);
            end
            break;
        end
        
        if string.len(dir) == 0 then
            break;
        end
    end
end

