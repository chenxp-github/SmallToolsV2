require("common");
require("utils");

local g_folder_to_delete = nil;
local g_exts = nil;
local g_no_exts = nil;
local g_file_list_fn = nil;
local g_no_file_list_fn = nil;
local g_lua_config = nil;
local g_recursive = false;
local g_test_only = false;

function app_short_help()
    return "delete folder.";
end

function app_name()
    return "delete_folder";
end

--default delete_this_file
function delete_this_file(fullpath,root,is_dir)
    return true;
end

function delete_file_wrapper(full_name,folder)
    if delete_this_file(full_name,folder,false) then
        if g_test_only then
            print("delete file:"..full_name);
        else
            FileManager.DeleteFile(full_name);
        end
    end
end

function delete_empty_folder_wrapper(full_name,folder)
    if delete_this_file(full_name,folder,true) then
        if g_test_only then
            print("delete folder:"..full_name);
        else
            FileManager.DeleteDir(full_name);
        end
    end
end

function delete_folder_raw(folder)
    FileManager.SearchDir(folder,g_recursive,function(info)
        if info.event == EVENT_SINGLE_FILE then            
            delete_file_wrapper(info.full_name,folder);
        end
        if info.event == EVENT_END_DIR then
            delete_empty_folder_wrapper(info.full_name,folder);
        end
    end);
end

function delete_folder_file_list(folder)
    split_file_lines(g_file_list_fn,function(line)        
        local full_name = folder.."/"..line;
        if FileManager.IsDirExist(full_name) then
            delete_empty_folder_wrapper(full_name);
        else
            delete_file_wrapper(full_name);
        end
    end);
end

local flag_can_not_delete = 0;

function delete_folder_no_file_list(folder)
    local no_table={};
    split_file_lines(g_no_file_list_fn,function(line)
        no_table[line] = true;
    end);

     FileManager.SearchDir(folder,g_recursive,function(info)
        if info.event == EVENT_SINGLE_FILE then            
            local rpath = remove_path_prefix(info.full_name,folder);            
            if not no_table[rpath] and flag_can_not_delete == 0then   
                delete_file_wrapper(info.full_name,folder);
            end
        end
        
        if info.event == EVENT_BEGIN_DIR then            
            local rpath = remove_path_prefix(info.full_name,folder);            
            if no_table[rpath] then            
                flag_can_not_delete = flag_can_not_delete + 1;
            end
        end
 
       if info.event == EVENT_END_DIR then            
            local rpath = remove_path_prefix(info.full_name,folder);            
            if no_table[rpath] then
                flag_can_not_delete = flag_can_not_delete - 1;
            elseif flag_can_not_delete == 0 then
                delete_empty_folder_wrapper(info.full_name,folder);
            end
        end        
    end);    
end

function delete_folder_exts(folder,exts_table,not_mode)
    FileManager.SearchDir(folder,g_recursive,function(info)
        if info.event == EVENT_SINGLE_FILE then            
            local rpath = remove_path_prefix(info.full_name,folder);            
            local ext = FileManager.SliceFileName(info.full_name,FN_EXT);            
            
            local do_delete = true;
            
            if not_mode then
                do_delete = not exts_table[ext];    
            else
                do_delete = exts_table[ext];
            end
            
            if do_delete then            
                delete_file_wrapper(info.full_name);
            end
        end
    end);
end

function do_delete_folder()
    if  not g_exts and 
        not g_no_exts and 
        not g_file_list_fn and 
        not g_no_file_list_fn 
    then
        delete_folder_raw(g_folder_to_delete);
        delete_empty_folder_wrapper(g_folder_to_delete,g_folder_to_delete);
        return
    end

    if g_file_list_fn then
        return delete_folder_file_list(g_folder_to_delete);
    end
    
    if g_no_file_list_fn then
        return delete_folder_no_file_list(g_folder_to_delete);
    end        


    if g_exts then
        return delete_folder_exts(g_folder_to_delete,g_exts);
    end

    if g_no_exts then
        return delete_folder_exts(g_folder_to_delete,g_no_exts,true);
    end

end

local kFolder = "--folder"
local kExts = "--exts";
local kNoExts = "--no-exts";
local kFileList = "--file-list";
local kNoFileList = "--no-file-list";
local kLuaConfig = "--lua-config";
local kRecursive = "--recursive";
local kTestOnly = "--test-only";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kFolder,TYPE_KEY_EQUAL_VALUE,MUST,"folder to be deleted");
    cmd:AddKeyType(kRecursive,TYPE_KEY,OPTIONAL,"is recursive search folder");
    cmd:AddKeyType(kTestOnly,TYPE_KEY,OPTIONAL,"test only, do not real delete files.");
    cmd:AddKeyType(kExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files with special exts will be deleted");
    cmd:AddKeyType(kFileList,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files in file-list will be deleted");    
    cmd:AddKeyType(kNoExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files WITHOUT special exts will be deleted");    
    cmd:AddKeyType(kNoFileList,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files NOT in file-list will be deleted");    
    cmd:AddKeyType(kLuaConfig,TYPE_KEY_EQUAL_VALUE,OPTIONAL,
        "use a lua as config file."..EOL..
        "        lua function delete_this_file(fullpath,root,is_dir) must be exist."
    );
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
        
    g_folder_to_delete = FileManager.ToAbsPath(cmd:GetValueByKey(kFolder));
    
    if cmd:HasKey(kRecursive) then
        g_recursive = true;
    end
    
    if cmd:HasKey(kTestOnly) then
        g_test_only = true;
    end

    if cmd:HasKey(kExts) then
        local value = cmd:GetValueByKey(kExts);
        g_exts = split_file_exts_list(value);
    end
    
    if cmd:HasKey(kNoExts) then
        local value = cmd:GetValueByKey(kNoExts);
        g_no_exts = split_file_exts_list(value);
    end

    if cmd:HasKey(kFileList) then
        g_file_list_fn = cmd:GetValueByKey(kFileList);        
    end
   
    if cmd:HasKey(kNoFileList) then
        g_no_file_list_fn = cmd:GetValueByKey(kNoFileList);        
    end 
    
    if cmd:HasKey(kLuaConfig) then
        g_lua_config = cmd:GetValueByKey(kLuaConfig);
        exec_file(g_lua_config);
    end
    
    do_delete_folder(); 
end
