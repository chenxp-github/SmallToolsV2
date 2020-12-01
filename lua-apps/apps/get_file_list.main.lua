require("common");
require("utils");

local g_the_folder = nil;
local g_exts = nil;
local g_no_exts = nil;
local g_recursive = false;
local g_out_file = nil;
local g_full_name = false;
local g_pattern = nil;
local g_only_files = false;
local g_no_links = false;

function app_short_help()
    return "get file list of a folder";
end

function app_name()
    return "get_file_list";
end

--default need_this_file
function need_this_file(info)
    if g_pattern then
        if info.is_dir then return true end
        return CFunc.wild_match(g_pattern,info.full_name);
    end
    return true;
end

function add_file_name_wrapper(info)
    local str = info.full_name;
    
    if not g_full_name then
        str = remove_path_prefix(info.full_name,g_the_folder);
    end

    local need = true;

    if info.is_dir and g_only_files then
        need = false;
    end

    if not need then
        return
    end

    if info.is_dir then
        g_out_file:Puts("<");
    end

    g_out_file:Puts(str);

    if info.is_dir then
        g_out_file:Puts(">");
    end

    g_out_file:Puts(EOL);
end

function inner_need_this_file(info)
    local ext = FileManager.SliceFileName(info.full_name,FN_EXT);
    
    if g_exts then
        return g_exts[ext];
    end
    
    if g_no_exts then
        return not g_no_exts[ext];
    end
    
    return true;
end

function get_file_list(dir)
    if g_no_links then
        if FileManager.ReadLink(dir) then
            return
        end
    end

    FileManager.SearchDir(dir,false,function(info)
        if info.event == EVENT_BEGIN_DIR then
            if need_this_file(info) then
                add_file_name_wrapper(info);
                if g_recursive then
                    get_file_list(info.full_name);
                end
            end                
        end
        
        if info.event == EVENT_SINGLE_FILE then
            if inner_need_this_file(info) then
                if need_this_file(info) then
                    add_file_name_wrapper(info);
                end
            end            
        end
    end);
end

function do_get_file_list()
    get_file_list(g_the_folder);
end

local kFolder = "--folder"
local kExts = "--exts";
local kNoExts = "--no-exts";
local kLuaConfig = "--lua-config";
local kRecursive = "--recursive";
local kFullName = "--full-name";
local kPattern = "--pattern";
local kOut = "--out";
local kOnlyFiles = "--only-files"
local kNoLinks = "--no-links";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kFolder,TYPE_KEY_EQUAL_VALUE,MUST,"get the file list of the folder");
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"output file");
    cmd:AddKeyType(kRecursive,TYPE_KEY,OPTIONAL,"is recursive search folder");
    cmd:AddKeyType(kFullName,TYPE_KEY,OPTIONAL,"use full file name?");
    cmd:AddKeyType(kExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files with special exts will be listed");
    cmd:AddKeyType(kNoExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only files WITHOUT special exts will be listed");    
    cmd:AddKeyType(kPattern,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"only filename with special pattern will be listed.");
    cmd:AddKeyType(kOnlyFiles,TYPE_KEY,OPTIONAL,"only files will be listed.");
    cmd:AddKeyType(kNoLinks,TYPE_KEY,OPTIONAL,"do not read link folder.");
    cmd:AddKeyType(kLuaConfig,TYPE_KEY_EQUAL_VALUE,OPTIONAL,
        "use a lua as config file."..EOL..
        "        lua function need_this_file(info) must be exist."
    );
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
        
    g_the_folder = FileManager.ToAbsPath(cmd:GetValueByKey(kFolder));     
    
    if cmd:HasKey(kRecursive) then
        g_recursive = true;
    end    
    
    if cmd:HasKey(kFullName) then
        g_full_name = true;
    end

    if cmd:HasKey(kExts) then
        local value = cmd:GetValueByKey(kExts);
        g_exts = split_file_exts_list(value);
    end
    
    if cmd:HasKey(kNoExts) then
        local value = cmd:GetValueByKey(kNoExts);
        g_no_exts = split_file_exts_list(value);
    end

    if cmd:HasKey(kPattern) then
        g_pattern = cmd:GetValueByKey(kPattern);
    end
    
    if cmd:HasKey(kLuaConfig) then
        g_lua_config = cmd:GetValueByKey(kLuaConfig);
        exec_file(g_lua_config);
    end

    if cmd:HasKey(kOnlyFiles) then
        g_only_files = true;
    end

    if cmd:HasKey(kNoLinks) then
        g_no_links = true;
    end
    
    local out =  FileManager.ToAbsPath(cmd:GetValueByKey(kOut));
    
    _,g_out_file = new_memfile(1024*1024,1024);        
    do_get_file_list();     
    if g_out_file:GetSize() > 0 then
        print("save to "..out);
        g_out_file:WriteToFile(out);
    end    
end
