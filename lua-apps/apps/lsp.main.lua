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
function is_start_tag(lsp)
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
function is_end_tag(lsp)
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

function read_lua_block(lsp)
    local attributes = {};
    local tmp = new_mem();

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

    local lua_code = new_memfile();

    while not lsp:IsEnd() do
        if is_end_tag(lsp) then
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
    local block_num = 1;

    lsp:Seek(0);
    while not lsp:IsEnd() do
        if is_start_tag(lsp) then
            if tmp:GetSize() > 0 then
                table.insert( blocks,{
                    text = file_to_string(tmp)
                });
                tmp:SetSize(0);
            end

            local attr,code = read_lua_block(lsp);
            if not attr then
                printf("lsp syntax error at line %d",calcu_line(lsp,lsp:GetOffset()));
                return
            end

            if not attr.name then
                attr.name = "/"..lsp_name.."/"..block_num;
            end

            table.insert( blocks,{
                attributes = attr,
                code = code,
            });

            block_num = block_num + 1;
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

function add_lsp_file(code,filename,param)
    local old_path = FileManager.GetCurDir();
    local tmp = new_mem(filename);
    if not tmp then
        exit("can not open file "..filename);
    end    
    local path = FileManager.SliceFileName(filename,FN_PATH);
    FileManager.ChangeDir(path);        
    expand_lsp(tmp,filename,code:GetInnerFile(),param);
    FileManager.ChangeDir(old_path);
end

function add_code_block(block,out,param)    
    local code = PrintBuffer.new();
    __gen_code__ = nil;

    local new_lua_code = "function __gen_code__(code,param)"..EOL;
    new_lua_code = new_lua_code..block.code..EOL;
    new_lua_code = new_lua_code.."end";
    if not exec_string(new_lua_code,block.attributes.name) then
        exit("execute lua chunk fail: "..block.attributes.name);
    end
    __gen_code__(code,param);
    out:Puts(code:GetInnerFile());
end

function expand_lsp(lsp,lsp_name,out,param)
    local blocks = split_lsp_file(lsp,lsp_name);
    if not blocks then return end
    for _,block in ipairs(blocks) do
        if block.text then
            out:Puts(block.text);
        elseif block.code then
            local tmp = new_memfile();
            add_code_block(block,tmp,param);
            expand_lsp(tmp,block.attributes.name,out);
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
    add_lsp_file(code,filename,param) can be used to add another lsp file 
    add_js_string(code,filename,param) add escaped js string
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
        return printf("open file %s fail.",in_file);
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
        printf("fail to save file %s",out_file);
    end
end

