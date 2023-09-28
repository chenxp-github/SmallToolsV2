require("common")

function xml_to_table(root,hashMode)
    local obj = {};
    local key = new_mem();
	local val = new_mem();	
    local name = root:GetName();
    obj.name = name;
    root:RestartAttrib();
    while root:GetNextAttrib(key,val) do        
        if not obj.attributes then
            obj.attributes = {};
        end
        obj.attributes[key:CStr()] = val:CStr();       
	end
    key:Destroy();
    val:Destroy();

    local child = root:GetChild();
    while child do	   
        local sub_obj = xml_to_table(child,hashMode);
        if sub_obj then
            if not obj.children then
                obj.children = {};
            end
            table.insert(obj.children,sub_obj);
        end

        child = child:GetNext();
	end

    if hashMode and obj.children then
        local hash_table = {};
        for _,item in ipairs(obj.children) do
            local child_name = item.name;
            if not hash_table[child_name] then
                hash_table[child_name] = {};
            end
            table.insert(hash_table[child_name],item);
        end

        obj.children = {};

        for key,val in pairs(hash_table) do
            local len = #val;
            if len == 1 then
                obj.children[key] = val[1];
            elseif len > 1 then
                local new_key = key.."_Array";
                obj.children[new_key] = val;
            end
        end
    end

    local value = root:GetValue();
    if value and value:GetSize() > 0 then
        obj.value = value:CStr();
    end

    return obj;
end

function xml_file_to_table(fn,hashMode)
    local xml = Xml.new();

    if not xml:LoadXml(fn) then
        printfnl("can not load xml file:%s",fn);
        return
    end
    
    t = xml_to_table(xml:GetRoot(),hashMode);
    xml:Destroy();
    return t;
end

