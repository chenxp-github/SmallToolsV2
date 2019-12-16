require("common");
require("utils");

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

function split_jsp_file(lsp)
    local blocks={};
    local tmp = new_memfile();

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
                printf("lsp syntax error at offset %d,",lsp:GetOffset());
                return
            end

            table.insert( blocks,{
                attributes = attr,
                code = code,
            })
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


function print_help(args)
    printf("%s <lsp> <out>",args[1]);
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

    local blocks = split_jsp_file(in_mem);

    print_table(blocks);
    
end
