require("common");
require("utils");

local g_root = null;

function app_short_help()
    return "take snapshot of a folder.";
end

function app_name()
    return "folder_snapshot";
end

function need_this_file(info,root)
    return true;
end

function search_folder(folder,callback)
    FileManager.SearchDir(folder,false,function(info)
        if info.event == EVENT_BEGIN_DIR then
            if need_this_file(info,g_root) then        
                callback(info);                
                search_folder(info.full_name,callback);                
            end            
        elseif info.event == EVENT_SINGLE_FILE then                
            if need_this_file(info,g_root) then        
                callback(info);                
            end 
        elseif info.event == EVENT_END_DIR then
            if need_this_file(info,g_root) then
                callback(info);
            end
        end        
    end);       
end

local kFolder = "--folder";
local kLuaConfig = "--lua-config"
local kOut = "--out";
local kWithMd5 = "--with-md5"

function app_main(args)
    local cmd = CommandLine.new();
    cmd:AddKeyType(kFolder,TYPE_KEY_EQUAL_VALUE,MUST,"folder to snapshot");    
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"output snapshot file.");
    cmd:AddKeyType(kWithMd5,TYPE_KEY,OPTIONAL,"calcu md5 of files.");    
    cmd:AddKeyType(kLuaConfig,TYPE_KEY_EQUAL_VALUE,OPTIONAL,
        "use a lua as config file."..EOL..
        "        lua function need_this_file(info,root) must be exist."
    );
    
    cmd:LoadFromArgv(args);            
    
    if cmd:CheckForErrors() then
        return
    end
    
    if cmd:HasKey(kLuaConfig) then
        local lua_config = cmd:GetValueByKey(kLuaConfig);
        exec_file(lua_config);
    end
    
    local with_md5 = false;
    if cmd:HasKey(kWithMd5) then
        with_md5 = true;
    end

    local folder_name = FileManager.ToAbsPath(cmd:GetValueByKey(kFolder));
    
    g_root = folder_name;
    
    local bson = Bson.new();    
    bson:StartDocument();
        
    search_folder(folder_name,function(info)
        if info.event == EVENT_BEGIN_DIR then            
            local off = bson:StartDocument("b");
            bson:PutString("n",info.file_name);
            bson:EndDocument(off,"b");
        elseif info.event == EVENT_SINGLE_FILE then                
            local off = bson:StartDocument("f");
            bson:PutString("n",info.file_name);
            bson:PutInt64("s",info.size);
            bson:PutInt64("t",info.last_write_time);
            if with_md5 then
                local md5 = FileManager.Md5Sum(info.full_name);
                if md5 then
                    bson:PutString("m",md5);
                end
            end            
            bson:EndDocument(off,"f");            
        elseif info.event == EVENT_END_DIR then        
            local off = bson:StartDocument("e");
            bson:PutString("n",info.file_name);
            bson:EndDocument(off,"e");            
        end        
    end);   
    
    bson:EndDocument();    
    
    local out = FileManager.ToAbsPath(cmd:GetValueByKey(kOut));    
    local data = bson:GetRawData();    
    if data:FileBase():WriteToFile(out) > 0 then    
        print("snapshot save to "..out);    
    end    
end

