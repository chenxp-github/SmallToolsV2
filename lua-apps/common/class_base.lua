local _class={}
function class(super)
    local class_type={}
    class_type.ctor=false
    class_type.super=super
    class_type.new=function(...)
            local obj={}
            setmetatable(obj,{ __index=_class[class_type] })
            do
                local create
                create = function(c,...)
                    if c.super then
                        create(c.super,...)
                    end
                    if c.ctor then
                        c.ctor(obj,...)
                    end
                end
                create(class_type,...)
            end
            return obj
        end
    local vtbl={}
    _class[class_type]=vtbl

    setmetatable(class_type,{__newindex=
        function(t,k,v)
            vtbl[k]=v
        end
    })

    if super then
        setmetatable(vtbl,{__index=
            function(t,k)
                local ret=_class[super][k]
                vtbl[k]=ret
                return ret
            end
        })
    end

    return class_type
end

function super(c)
    return _class[c];
end

function print_table(root)
    local printnl = printnl
    local tconcat = table.concat
    local tinsert = table.insert
    local srep = string.rep
    local type = type
    local pairs = pairs
    local tostring = tostring
    local next = next
    local cache = {  [root] = "." }
    local function _dump(t,space,name)
        local temp = {}
        for k,v in pairs(t) do
            local key = tostring(k)
            if cache[v] then
                tinsert(temp,"+" .. key .. " {" .. cache[v].."}")
            elseif type(v) == "table" then
                local new_key = name .. "." .. key
                cache[v] = new_key
                tinsert(temp,"+" .. key .. _dump(v,space .. (next(t,k) and "|" or " " ).. srep(" ",#key),new_key))
            else
                tinsert(temp,"+" .. key .. " [" .. tostring(v).."]")
            end
        end
        return tconcat(temp,"\r\n"..space)
    end
    printnl(_dump(root, "",""))
end

function clone_table(ori_tab)
    if (type(ori_tab) ~= "table") then
        return nil
    end

    local new_tab = {}
    
    for i,v in pairs(ori_tab) do
        local vtyp = type(v)
        if (vtyp == "table") then
            new_tab[i] = clone_table(v)
        else
            new_tab[i] = v
        end
    end
    
    return new_tab
end

function string_to_table(str)
    local run_code,errors = load("return "..str);
    if not run_code then
        printnl(errors); 
        printnl(debug.traceback());
        return;
    end

    local ret_str = nil;
    local r,errors = pcall(function()
        ret_str = run_code();
    end);
    if not r then printnl(errors); end
    return ret_str;
end

function load_lua_lib(name)
    local r,errors = pcall(function() require(name); end);
    if not r then 
        printnl(errors); 
        printnl(debug.traceback());
    end
    return r;
end

function exec_string(code,name)
    local run_code,errors = load(code,name);
    if not run_code then
        printnl(errors); 
        printnl(debug.traceback());
        return;
    end

    local ret;
    local r,errors = pcall(function() 
        ret = run_code(); 
    end);
    if not r then 
        printnl(errors); 
        printnl(debug.traceback());
    end
    return r,ret;
end

function exec_file(fn)
    local run_code,errors = loadfile(fn);
    if not run_code then
        printnl("loadfile "..fn.." fail.");
        printnl(errors); 
        printnl(debug.traceback());
        return;
    end

    local ret;
    local r,errors = pcall(function() 
        ret = run_code(); 
    end);

    if not r then
        printnl("exec file "..fn.." fail.");
        printnl(errors); 
        printnl(debug.traceback());
    end

    return r,ret;
end
