require("common");
require("utils");
require("default_resource_config");

local g_current_theme = nil;
local g_current_language = nil;
local g_resource_folder = nil;
local g_resource_manager = nil;

function add_resource(res_type,res_id,bson_data)
    local r = Resource.new();
    r:SetTheme(g_current_theme);
    r:SetLanguage(g_current_language);
    r:SetType(res_type);
    r:SetId(res_id);
    r:SetBsonData(bson_data);
    if not g_resource_manager:AddResource(r) then    
        print(
            "resource can not repeat: theme = "..
            g_current_theme.." ,language = "..
            g_current_language.." ,ID = "..
            res_id.."\ndata="..
            bson_data:ToJson()
        );               
        exit(-1);
    end
end

function add_normal_image_resource(info)    
    local img = XImage.new();
    if not XImageHelper.LoadImage(info.full_name,img) then
        print("open image file "..info.full_name.." fail");
        return;
    end
    
    local width = img:GetWidth();
    local height = img:GetHeight();
    
    local url = remove_path_prefix(info.full_name,g_resource_folder);    
    
    local bson = Bson.new();
    
    bson:StartDocument();
    
    bson:PutString("url",url);
    
    local off = bson:StartArray("size");
    bson:PutInt32("0",width);
    bson:PutInt32("1",height);    
    bson:EndArray(off,2);
    
    bson:EndDocument();
    
    local res_id = filename_to_id(RES_TYPE_RAW_IMAGE,info.file_name);
    
    add_resource(RES_TYPE_RAW_IMAGE,res_id,bson);

    img:DestroyAll();
end

function add_9_png_image_resource(info)    
    local img = XImage.new();
    if not XImageHelper.LoadImage(info.full_name,img) then
        print("open image file "..info.full_name.." fail");
        return;
    end
    
    local width = img:GetWidth();
    local height = img:GetHeight();
        
    local img_out = XImage.new();    
    local nine_patches = XImageHelper.Convert9Png(img,img_out);
    
    local path = FileManager.SliceFileName(info.full_name,FN_PATH);
    local file_name = FileManager.SliceFileName(info.full_name,FN_FILENAME);
    
    local new_path = path.."/9patch_gen";
    local new_file_name = new_path.."/"..file_name;
    
    if not FileManager.IsDirExist(new_path) then
        create_folder(new_path);
    end
    
    if not XImageHelper.SaveImage(img_out,new_file_name) then
        print("save "..new_file_name.." fail.");
        exit(-1);
    end
    
    local url = remove_path_prefix(new_file_name,g_resource_folder);    
    
    local bson = Bson.new();
    bson:StartDocument();
    bson:PutString("url",url);
    
    local off = bson:StartArray("size");
    bson:PutInt32("0",width);
    bson:PutInt32("1",height);    
    bson:EndArray(off,2);    
    
    local slice = raw_nine_patches_to_slice_array(
        img_out:GetWidth(),img_out:GetHeight(),nine_patches
    );

    local off = bson:StartArray("slice");    
    for i,v in ipairs(slice) do
        bson:PutInt32(tostring(i-1),v);
    end

    bson:EndArray(off,4);

    bson:EndDocument();

    local res_id = filename_to_id(RES_TYPE_9_PNG,info.file_name);
    
    add_resource(RES_TYPE_9_PNG,res_id,bson);

    img:DestroyAll();
    img_out:DestroyAll();
end

function to_ordered_key_value_table(tab)
    local new_tab = {};    
    if type(tab) ~= "table" then        
        table.insert(new_tab,{
            key = "value",
            value = tab
        });       
        return new_tab;
    end
        
    for k,v in pairs(tab) do
        table.insert(new_tab,{
            key = k,
            value = v,
        });
    end

    table.sort(new_tab,function(e1,e2)
        return e1.key < e2.key;
    end);
   
    return new_tab;
end

function key_value_table_to_bson(kvtab,bson)
    for _,v in ipairs(kvtab) do        
        if type(v.value) == "number" then
           bson:PutInt32(v.key,v.value);
        elseif type(v.value) == "string" then
           bson:PutString(v.key,v.value);
        elseif type(v.value) == "boolean" then
            bson:PutBoolean(v.key,v.value);
        else
            print("unknown value type ",v.value);
            exit(-1);
        end
    end
end

function lua_table_to_bson_ordered(tab,bson)
    local kvtab = to_ordered_key_value_table(tab);
    return key_value_table_to_bson(kvtab,bson);
end

function add_string_resource(info)
    STRING_RESOURCE_TABLE = nil;
    
    local r = exec_file(info.full_name);
    if not r then exit(-1) end

    if not STRING_RESOURCE_TABLE then
       print("can not find  STRING_RESOURCE_TABLE in ".. info.full_name);
       exit(-1);
    end
    
    for id,v in pairs(STRING_RESOURCE_TABLE) do
        local bson = Bson.new();
        bson:StartDocument();               
        lua_table_to_bson_ordered(v,bson);
        bson:EndDocument();        
        add_resource(RES_TYPE_STRING,id,bson);
    end    
end

function add_style_resource(info)
    STYLE_RESOURCE_TABLE = nil;
    
    local r = exec_file(info.full_name);
    if not r then exit(-1) end

    if not STYLE_RESOURCE_TABLE then
       print("can not find  STYLE_RESOURCE_TABLE in ".. info.full_name);
       exit(-1);
    end

    for id,v in pairs(STYLE_RESOURCE_TABLE) do
        local bson = Bson.new();
        bson:StartDocument();               
        lua_table_to_bson_ordered(v,bson);
        bson:EndDocument();        
        add_resource(RES_TYPE_CSS,id,bson);
    end    
end


function collect_one_resource_folder(path)   
    FileManager.SearchDir(path,true,function(info)
        if info.event == EVENT_SINGLE_FILE then 
            if not CFunc.wild_match("*/9patch_gen/*",info.full_name) then
                if is_9png_file(info.full_name) then
                    add_9_png_image_resource(info);
                elseif is_normal_image_file(info.full_name) then
                    add_normal_image_resource(info);
                elseif is_string_file(info.full_name) then
                    add_string_resource(info);
                elseif is_style_file(info.full_name) then
                    add_style_resource(info);                    
                end            
            end
        end
    end);
end

function collect_all_folders_recursive(root,...)

    if g_current_language and g_current_theme then
        collect_one_resource_folder(root)
    end
    
    local unknown = table.pack(...);
    local level1 = unknown[1];
    
    if not level1 then
        return;
    end
    
    table.remove(unknown,1);
    
    for _,folder in pairs(level1.folders) do    
        if level1.is_theme_folder then
            g_current_theme = folder;
        end
        
        if level1.is_language_folder then
            g_current_language = folder;
        end
                
        collect_all_folders_recursive(
            root.."/"..folder,
            table.unpack(unknown)
        );
        
        if level1.is_theme_folder then
            g_current_theme = nil;
        end
        
        if level1.is_language_folder then
            g_current_language = nil;
        end        
    end
end

function app_short_help()
    return "collect resources from resource folder.";
end

function app_name()
    return "collect_resources"
end

local kConfig="--config-file";
local kIn="--in"
local kOut="--out";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kConfig,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"set the config file");
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"the output file");
    cmd:AddKeyType(kIn,TYPE_KEY_EQUAL_VALUE,MUST,"the resource folder");
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
    
    g_resource_manager = ResourceManager.new();
    
    g_resource_folder = FileManager.ToAbsPath(cmd:GetValueByKey(kIn)); 
    
    collect_all_folders_recursive(
        g_resource_folder,
        g_resource_type_folders,
        g_theme_folders,        
        g_language_folders
    );
    
    local out_file = FileManager.ToAbsPath(cmd:GetValueByKey(kOut));
    g_resource_manager:SaveResourceFile(out_file);
    print("save resource file: "..out_file);
end
