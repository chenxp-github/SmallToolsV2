require("print_buffer");

local function escape_lua_string(str)
    if string.len(str) < 1024 then
        local mf_file = new_mem();    
        mf_file:Puts("\"");
        for i=1,string.len(str),1 do    
            local b = string.byte(str,i);
            if b == 10 then
                mf_file:Puts("\\n");
            elseif b == 13 then
                mf_file:Puts("\\r");
            elseif b ==92 or b==39 or b==34 or b==96 then        
                mf_file:Putc(92);        
                mf_file:Putc(b);
            else
                mf_file:Putc(b);
            end
        end
        mf_file:Puts("\"");

        local r = mf_file:CStr();
        mf_file:Destroy();
        return r;
    else
        return "[["..str.."]]";
    end
end

local function escape_lua_key(k)
    if type(k) == "number" then
        return string.format("[%s]",tostring(k));
    elseif string.find(k,"[%- ]") then
        return string.format("[\"%s\"]",k);
    else
        return k;
    end
end

local function lua_table_to_lua_inner(lua_table,pbuf,convert_userdata)
    if not lua_table then return end
    for k,v in pairs_ordered(lua_table) do    
        if type(v) == "table" then
            pbuf:Tab();
            pbuf:Printf("%s =",escape_lua_key(k));
            pbuf:Printf("{");
            pbuf:Eol();                
            pbuf:IncLogLevel(1);
            lua_table_to_lua_inner(v,pbuf);
            pbuf:IncLogLevel(-1);
            pbuf:Log("},");
        elseif type(v) == "string" then
            pbuf:Log("%s = %s,",escape_lua_key(k),escape_lua_string(v)); 
        else
            pbuf:Tab();
            pbuf:Printf(escape_lua_key(k));
            pbuf:Printf(" = ");
            if type(v) == "userdata" then
                if convert_userdata then
                    convert_userdata(v,pbuf);
                end
            else
                pbuf:Printf(tostring(v));
            end

            pbuf:Printf(",");
            pbuf:Eol();
        end
    end       
end

function lua_table_to_lua(lua_table,convert_userdata)
    local pbuf = PrintBuffer.new();
    pbuf:Log("{");
    pbuf:IncLogLevel(1);
    lua_table_to_lua_inner(lua_table,pbuf,convert_userdata);
    pbuf:IncLogLevel(-1);
    pbuf:Log("}");
    return pbuf:GetText();    
end
