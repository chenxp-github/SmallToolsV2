require("common");
require("utils");

function app_short_help()
    return "find repeat filenames in a folder.";
end

function app_name()
    return "repeat_filenames";
end

local kExts="--exts";
local kFolder="--folder";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only check files with the extensions");
    cmd:AddKeyType(kFolder,TYPE_KEY_EQUAL_VALUE,MUST,"the folder to check");
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end

    local exts = nil;
    if cmd:HasKey(kExts) then
        local exts_str = cmd:GetValueByKey(kExts);
        exts = split_file_exts_list(exts_str);        
    end

    local folder = cmd:GetValueByKey(kFolder);
    folder = FileManager.ToAbsPath(folder);

    local name_table={};

    FileManager.SearchDir(folder,true,function(info)        
        if info.event == EVENT_SINGLE_FILE then

            if exts then
                local ext = string.lower(FileManager.SliceFileName(info.file_name,FN_EXT));
                if not exts[ext] then
                    return;
                end
            end

            if name_table[info.file_name] then
                table.insert(name_table[info.file_name],info.full_name);
            else
                name_table[info.file_name] = {info.full_name};
            end
        end
    end);


    for _, names in pairs(name_table) do      
        if #names > 1 then  
            print("---------------------------")
            for _,name in ipairs(names) do
                print(name);
            end
        end        
    end

end

