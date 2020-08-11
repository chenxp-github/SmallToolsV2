require("print_buffer");

local function escape_js_string(str)
    local mf,mf_file = new_memfile();    
    
    for i=1,string.len(str),1 do    
        local b = string.byte(str,i);
        if b == 10 then
            mf_file:Puts("\\n");
        elseif b == 13 then
            mf_file:Puts("\\r");
        elseif b == 92 or b == 39 or b ==34 then        
            mf_file:Putc(92);        
            mf_file:Putc(b);
        else
            mf_file:Putc(b);
        end
    end

    return file_to_string(mf_file);
end

local function is_array_table(lua_table)
    if not lua_table then return end
    for k,v in pairs(lua_table) do
        if type(k) ~= "number" then
            return false;
        end
    end    
    return true;
end

local function lua_table_to_js_inner(lua_table,is_array,pbuf)
    if not lua_table then return end
    for k,v in pairs(lua_table) do    
        if type(v) == "table" then
            pbuf:Tab();

            if not is_array then
                pbuf:Printf("%s :",k);
            end
        
            if is_array_table(v) then            
                pbuf:Printf("[");
                pbuf:Eol();
                pbuf:IncLogLevel(1);
                lua_table_to_js_inner(v,true,pbuf);
                pbuf:IncLogLevel(-1);
                pbuf:Log("],");
            else                        
                pbuf:Printf("{");
                pbuf:Eol();                
                pbuf:IncLogLevel(1);
                lua_table_to_js_inner(v,false,pbuf);
                pbuf:IncLogLevel(-1);
                pbuf:Log("},");
            end
            
        elseif type(v) == "string" then
            if is_array then
                pbuf:Log("\"%s\",",escape_js_string(v));        
            else
                pbuf:Log("%s : \"%s\",",k,escape_js_string(v));        
            end
        else
            pbuf:Tab();
            if not is_array then
                pbuf:Printf(k);
                pbuf:Printf(":");
            end
            pbuf:Printf(tostring(v));
            pbuf:Printf(",");
            pbuf:Eol();
        end
    end       
end

function lua_table_to_js(lua_table)
    local pbuf = PrintBuffer.new();
    if is_array_table(lua_table) then
        pbuf:Log("[");
        pbuf:IncLogLevel(1);
        lua_table_to_js_inner(lua_table,true,pbuf);
        pbuf:IncLogLevel(-1);
        pbuf:Log("]");
    else
        pbuf:Log("{");
        pbuf:IncLogLevel(1);
        lua_table_to_js_inner(lua_table,false,pbuf);
        pbuf:IncLogLevel(-1);
        pbuf:Log("}");
    end
    
    return pbuf:GetText();    
end

