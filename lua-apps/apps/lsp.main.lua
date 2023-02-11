require("common");
require("utils");
require("lua_to_js");

function app_short_help()
    return "expand a lua server pages file.";
end

function app_name()
    return "lsp";
end

--<@@
function is_start_tag_1(lsp)
    local save_off = lsp:GetOffset();
    local ch = lsp:Getc();
    if ch ~= 60 then
        lsp:Seek(save_off);
        return false;
    end
    
    ch = lsp:Getc();
    if ch ~= 64 then
        lsp:Seek(save_off);
        return false;
    end

    ch = lsp:Getc();
    if ch ~= 64 then
        lsp:Seek(save_off);
        return false;
    end

    return true;
end

--</@@>
function is_end_tag_1(lsp)
    local save_off = lsp:GetOffset();
    local ch = lsp:Getc();
    if ch ~= 60 then
        lsp:Seek(save_off);
        return false;
    end
    
    ch = lsp:Getc();
    if ch ~= 47 then
        lsp:Seek(save_off);
        return false;
    end

    ch = lsp:Getc();
    if ch ~= 64 then
        lsp:Seek(save_off);
        return false;
    end

    ch = lsp:Getc();
    if ch ~= 64 then
        lsp:Seek(save_off);
        return false;
    end

    ch = lsp:Getc();
    if ch ~= 62 then
        lsp:Seek(save_off);
        return false;
    end

    return true;
end

--${{
function is_start_tag_2(lsp)
    local save_off = lsp:GetOffset();
    local ch = lsp:Getc();
    if ch ~= 36 then
        lsp:Seek(save_off);
        return false;
    end
    
    ch = lsp:Getc();
    if ch ~= 123 then
        lsp:Seek(save_off);
        return false;
    end

    ch = lsp:Getc();
    if ch ~= 123 then
        lsp:Seek(save_off);
        return false;
    end

    return true;
end

--}}$
function is_end_tag_2(lsp)
    local save_off = lsp:GetOffset();     
    local ch = lsp:Getc();
    if ch ~= 125 then
        lsp:Seek(save_off);
        return false;
    end

    ch = lsp:Getc();
    if ch ~= 125 then
        lsp:Seek(save_off);
        return false;
    end

    ch = lsp:Getc();
    if ch ~= 36 then
        lsp:Seek(save_off);
        return false;
    end

    return true;
end

function is_start_tag(lsp)
    if is_start_tag_1(lsp) then
        return 1;
    end
    if is_start_tag_2(lsp) then
        return 2;
    end
end

function is_end_tag(lsp,mode)
    if mode == 1 then
        return is_end_tag_1(lsp);
    end

    if mode == 2 then
        return is_end_tag_2(lsp);
    end
end

function read_lua_block(lsp,mode)
    local attributes = {};
    local tmp = new_mem();

    if mode == 1 then --模式1，可以包含一些属性
        while not lsp:IsEnd() do
            local key = lsp:NextWord();
            if key == ">" then
                break;
            end

            local eq = lsp:NextWord();
            if eq ~= "=" then
                return
            end

            local quote = lsp:NextWord();
            if quote ~= "\"" then
                return
            end
            
            lsp:ReadCStr(tmp);
            attributes[key] = tmp:CStr();
        end
    end

    local lua_code = new_memfile();

    while not lsp:IsEnd() do
        if is_end_tag(lsp,mode) then
            break;
        else
            ch = lsp:Getc();
            lua_code:Putc(ch);
        end
    end

    local code = "";
    if lua_code:GetSize() > 0 then
        code = file_to_string(lua_code);
    end

    return attributes,code;
end

function calcu_line(lsp,offset)
    local save_off = lsp:GetOffset();
    local tmp = new_mem(64*1024);
    lsp:Seek(0);
    local line = 1;
    while lsp:ReadLine(tmp) do
        if lsp:GetOffset() >= offset then
            break;
        end
        line = line+1;
    end
    lsp:Seek(save_off);
    return line;
end

function split_lsp_file(lsp,lsp_name)
    local blocks={};
    local tmp = new_memfile();

    lsp:Seek(0);
    while not lsp:IsEnd() do
        local start_tag = is_start_tag(lsp);
        if start_tag then
            if tmp:GetSize() > 0 then
                table.insert( blocks,{
                    text = file_to_string(tmp)
                });
                tmp:SetSize(0);
            end

            local attr,code = read_lua_block(lsp,start_tag);
            if not attr then
                exit("lsp syntax error at line %d",calcu_line(lsp,lsp:GetOffset()));
                return
            end

            if not attr.name then
                attr.name = code;
            end

            table.insert( blocks,{
                attributes = attr,
                code = code,
            });
        else
            tmp:Putc(lsp:Getc());
        end
    end

    if tmp:GetSize() > 0 then
        table.insert( blocks,{
            text = file_to_string(tmp)
        });
        tmp:SetSize(0);
    end

    return blocks;
end

--tool function, convert a file to escaped js string
function add_js_string(code,filename,param)
    local tmp_code = PrintBuffer.new();
    add_lsp_file(tmp_code,filename,param);

    code:Log("[");
    
    local source = tmp_code:GetInnerFile();
    source:Seek(0);
    local mem = new_mem();
    
    while source:ReadLine(mem) do
        local escaped = escape_js_string(mem:CStr());
        code:Log("\"%s\",",escaped);
    end

    code:Log("].join(\"\\n\")");
end

function add_lua_file(filename)
    local old_path = FileManager.GetCurDir();
    local path = FileManager.SliceFileName(filename,FN_PATH);
    FileManager.ChangeDir(path);        
    local fn = FileManager.SliceFileName(filename,FN_FILENAME);
    if not exec_file(fn) then
        return exit("load lua file '%s' fail.",filename);
    end
    FileManager.ChangeDir(old_path);
end

function add_lsp_file(code,filename,param)
    local old_path = FileManager.GetCurDir();
    local tmp = new_mem(filename);
    if not tmp then
        exit("can not open file "..filename);
    end    
    local path = FileManager.SliceFileName(filename,FN_PATH);
    FileManager.ChangeDir(path);        
    expand_lsp(tmp,filename,code:GetInnerFile(),param);
    code:Eol(); --add a empty line
    FileManager.ChangeDir(old_path);
end

function save_lsp_file(filename, new_name)
    local code = PrintBuffer.new();
    add_lsp_file(code,filename);
    local fn = FileManager.ToAbsPath(new_name);
    local mf = code:GetInnerFile();
    mf:WriteToFile(fn);
end

local ifdef_stack=nil;
function ifdef(condition)
    if condition==nil then
        condition = false;
    end
    if not ifdef_stack then
        ifdef_stack={};
    end
    table.insert(ifdef_stack,condition);
end

function endif()
    if not ifdef_stack then
        return
    end
    table.remove(ifdef_stack);
end

local function ifdef_value()
    if not ifdef_stack then
        return true;
    end    

    for _,v in ipairs(ifdef_stack) do
        if not v then
            return false;
        end
    end
    return true;
end


local function default_can_be_added(root,info,exts,header_table,footer_table)
    local can_add = false;
    
    local ext = FileManager.SliceFileName(info.full_name,FN_EXT);    
    for _,v in pairs(exts) do    
        if ext == v then
            can_add = true;
            break;
        end
    end
    
    if not can_add then
        return false;
    end
    
    local rpath = remove_path_prefix(info.full_name,root);

    if header_table then
        for _,fn in pairs(header_table) do
            if rpath == fn then
                return false;
            end
        end
    end

    if footer_table then
        for _,fn in pairs(footer_table) do        
            if rpath == fn then
                return false;
            end
        end
    end

    return true;
end

local function add_files(code,path,files_table)
    for k,filename in ipairs(files_table) do
        add_lsp_file(code,path.."/"..filename);
    end
end


function add_lsp_folder(code,folder,exts,param1, param2)        
    local fullname = FileManager.ToAbsPath(folder);
    if not FileManager.IsDirExist(fullname) then
        return
    end

    local cur_dir = FileManager.GetCurDir();
    if type(exts) == "string" then
        exts = {exts};
    end
    
    local can_be_added = default_can_be_added;
    local header_table = nil;
    local footer_table = nil;
    
    if type(param1) == "function" then
        can_be_added = param1;
    end
    
    local header_footer_mode = false;
    
    if type(param1) == "table" then
        header_footer_mode = true;
        header_table = param1;
        footer_table = param2;        
        add_files(code,fullname,header_table);
    end
        
    if not can_be_added then
        can_be_added = default_can_be_added;
    end

    local tmp_list = {};    
    FileManager.SearchDir(fullname,true,function(info)    
        if info.event == EVENT_SINGLE_FILE then        
            if can_be_added(fullname,info,exts,header_table,footer_table) then
                table.insert(tmp_list,info.full_name);
            end
        end
    end);

    table.sort(tmp_list);
    for k,v in ipairs(tmp_list) do
        add_lsp_file(code,v);
    end               

    if header_footer_mode then
        add_files(code,fullname,footer_table);
    end

    FileManager.ChangeDir(cur_dir);
end

g_current_code = nil;
-----------------------------------------
function add_code_block(block,out,param)    
    local code = PrintBuffer.new();
    __gen_code__ = nil;

    local new_lua_code = "function __gen_code__(code,param)"..EOL;
    new_lua_code = new_lua_code..block.code..EOL;
    new_lua_code = new_lua_code.."end";
    if not exec_string(new_lua_code,block.attributes.name) then
        print_table(block);
        exit("execute lua chunk fail.");
    end
    g_current_code = code;
    __gen_code__(code,param);
    g_current_code = code;
    out:Puts(code:GetInnerFile());
end

function expand_lsp(lsp,lsp_name,out,param)
    local blocks = split_lsp_file(lsp,lsp_name);
    if not blocks then return end
    for _,block in ipairs(blocks) do
        if block.text then
            if ifdef_value() then
                out:Puts(block.text);
            end
        elseif block.code then
            local tmp = new_memfile();
            add_code_block(block,tmp,param);
            
            if ifdef_value() then
                expand_lsp(tmp,block.attributes.name,out);
            end

            tmp:Destroy();
        end
    end
end

function print_help(args)
    printf("usage: %s <lsp> <out>",args[1]);
    printf([[
    the lsp file format like:
        <@@ name="xxx">
            code:Log("something");
            add_lsp_file(code,"ttt/2.lsp");
        </@@>
    the parameter 'code' is a PrintBuffer object, and 
    add_lsp_file(code,filename,param), can be used to add another lsp file.
    add_lua_file(filename), load a lua file directly.
    add_js_string(code,filename,param), add escaped js string.
    save_lsp_file(filename,new_name),  expand lsp file and save.
    add_lsp_folder(code,folder,exts,param1, param2), recursively add a folder.
    ifdef(condition), control whether current code is kept.
    endif(), pair with ifdef function.
    ]]);
end

function app_main(args)
    local argc = #args;
    if argc < 3 then
        return print_help(args);        
    end

    local in_file = args[2];
    local out_file = args[3];

    local in_mem = new_mem(in_file);
    if not in_mem then
        return exit("open file %s fail.",in_file);       
    end

    local work_dir = FileManager.SliceFileName(in_file,FN_PATH);
    local old_dir = FileManager.GetCurDir();
    FileManager.ChangeDir(work_dir);

    local out_mf = new_memfile();
    expand_lsp(in_mem,in_file,out_mf);
    FileManager.ChangeDir(old_dir);

    if out_mf:WriteToFile(out_file) > 0 then
        printf("save to file %s",out_file);
    else
        return exit("fail to save file %s",out_file);
    end
end

