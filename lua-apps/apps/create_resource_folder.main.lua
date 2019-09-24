require("common");
require("utils");
require("default_resource_config");

function create_all_folders_recursive(root,...)
    create_folder(root);
    print("create folder:"..root);
    
    local unknown = table.pack(...);
    local level1 = unknown[1];
    
    if not level1 then
        return;
    end
    
    table.remove(unknown,1);
    
    for _,folder in pairs(level1.folders) do       
        create_all_folders_recursive(
            root.."/"..folder,
            table.unpack(unknown)
        );
    end
end

function app_short_help()
    return "create empty folder for resource files.";
end

function app_name()
    return "create_resource_folder"
end

local kConfig="--config-file";
local kOut="--out";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kConfig,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"set the config file");
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"the output folder");
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
    
    local output_folder = cmd:GetValueByKey(kOut);
    local folder = FileManager.ToAbsPath(output_folder); 
    
    create_all_folders_recursive(
        folder,
        g_resource_type_folders,        
        g_theme_folders,        
        g_language_folders
    );
end
