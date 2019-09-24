require("common");
require("utils");

local g_from = nil;
local g_to = nil;
local g_exts = nil;
local g_no_exts = nil;
local g_file_list_fn = nil;
local g_no_file_list_fn = nil;
local g_copy_mode = 0;
local g_lua_config = nil;

function app_short_help()
    return "copy folder.";
end

function app_name()
    return "copy_folder";
end

--default need_this_file
function need_this_file(fullpath,from)
    return true;
end

function copy_single_file_wrapper(from,to,mode,rpath)
    if need_this_file(from,g_from) then
        FileManager.CopySingleFile(from,to,mode);
    end
end

function copy_folder_raw()
    FileManager.SearchDir(g_from,true,function(info)
        if info.event == EVENT_SINGLE_FILE then            
            local rpath = remove_path_prefix(info.full_name,g_from);
            copy_single_file_wrapper(info.full_name,g_to.."/"..rpath,g_copy_mode);
        end
    end);
end

function copy_folder_file_list()
    split_file_lines(g_file_list_fn,function(line)        
        if CFunc.wild_match("<*>",line) then
            return
        end
        
        copy_single_file_wrapper(
            g_from.."/"..line,
            g_to.."/"..line,
            g_copy_mode
        );            
    end);
end

function copy_folder_no_file_list()
    local no_table={};
    split_file_lines(g_no_file_list_fn,function(line)
        no_table[line] = true;
    end);

     FileManager.SearchDir(g_from,true,function(info)
        if info.event == EVENT_SINGLE_FILE then            
            local rpath = remove_path_prefix(info.full_name,g_from);            
            if not no_table[rpath] then            
                copy_single_file_wrapper(info.full_name,g_to.."/"..rpath,g_copy_mode);          
            end
        end
    end);    
end

function copy_folder_exts(exts_table,not_mode)
    FileManager.SearchDir(g_from,true,function(info)
        if info.event == EVENT_SINGLE_FILE then            
            local rpath = remove_path_prefix(info.full_name,g_from);            
            local ext = FileManager.SliceFileName(info.full_name,FN_EXT);            
            
            local do_copy = true;
            
            if not_mode then
                do_copy = not exts_table[ext];    
            else
                do_copy = exts_table[ext];
            end
            
            if do_copy then            
                copy_single_file_wrapper(info.full_name,g_to.."/"..rpath,g_copy_mode);          
            end
        end
    end);
end

function copy_folder()
    if  not g_exts and 
        not g_no_exts and 
        not g_file_list_fn and 
        not g_no_file_list_fn 
    then
        return copy_folder_raw();
    end

    if g_file_list_fn then
        return copy_folder_file_list();
    end    
    
    if g_no_file_list_fn then
        return copy_folder_no_file_list();
    end        
    
    if g_exts then
        return copy_folder_exts(g_exts);
    end
    
    if g_no_exts then
        return copy_folder_exts(g_no_exts,true);
    end
end

local kFrom = "--from";
local kTo = "--to";
local kCheckSame = "--check-same"
local kOverwrite = "--overwrite"
local kNoLog = "--no-log";
local kIgnoreLinks = "--ignore-links";
local kExts = "--exts";
local kNoExts = "--no-exts";
local kFileList = "--file-list";
local kNoFileList = "--no-file-list";
local kLuaConfig = "--lua-config";

function calcu_copy_mode(cmd)
    local mode = 0;
    
    if cmd:HasKey(kOverwrite) then
        mode = mode | MODE_OVERWRITE;        
    end
    
    if cmd:HasKey(kCheckSame) then
        mode = mode | MODE_CHECKSAME | MODE_OVERWRITE;
    end
    
    if not cmd:HasKey(kNoLog) then
        mode = mode | MODE_USE_LOG;
    end
    
    if not cmd:HasKey(kIgnoreLinks) then
        mode = mode | MODE_COPY_LINK;
    end
    
    return mode;    
end

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kFrom,TYPE_KEY_EQUAL_VALUE,MUST,"source folder");
    cmd:AddKeyType(kTo,TYPE_KEY_EQUAL_VALUE,MUST,"destination path");
    cmd:AddKeyType(kExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files with special exts will be copyed");    
    cmd:AddKeyType(kFileList,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files in file-list will be copyed");    
    cmd:AddKeyType(kNoExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files WITHOUT special exts will be copyed");    
    cmd:AddKeyType(kNoFileList,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files NOT in file-list will be copyed");    
    cmd:AddKeyType(kCheckSame,TYPE_KEY,OPTIONAL,"check if the file is same before copy");
    cmd:AddKeyType(kOverwrite,TYPE_KEY,OPTIONAL,"use overwrite mode?");
    cmd:AddKeyType(kNoLog,TYPE_KEY,OPTIONAL,"do not print log.");
    cmd:AddKeyType(kIgnoreLinks,TYPE_KEY,OPTIONAL,"treat links as normal file.");
    cmd:AddKeyType(kLuaConfig,TYPE_KEY_EQUAL_VALUE,OPTIONAL,
        "use a lua as config file."..EOL..
        "        lua function need_this_file(fullpath,from) must be exist."
    );
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end

    g_from = FileManager.ToAbsPath(cmd:GetValueByKey(kFrom));
    g_to = FileManager.ToAbsPath(cmd:GetValueByKey(kTo));
    
    g_copy_mode = calcu_copy_mode(cmd);
    
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

    copy_folder();    
end
