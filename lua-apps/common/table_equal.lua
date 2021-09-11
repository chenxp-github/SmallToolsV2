
function table_equal_recursive(obj1, obj1_name , obj2, obj2_name, comp_userdata)
    if obj1 == nil and obj2 == nil then
        return true,"";
    end

    if obj1 == nil and obj2 ~= nil then
        return false, string.format("%s = %s, but %s = %s",obj1_name,tostring(obj1),obj2_name,tostring(obj2));
    end

    if obj2 == nil and obj1 ~= nil then
        return false, string.format("%s = %s, but %s = %s",obj1_name,tostring(obj1),obj2_name,tostring(obj2));
    end

    if type(obj1) ~= type(obj2) then
        return false, string.format("%s has type of '%s', but type of %s is '%s'", obj1_name, type(obj1),obj2_name,type(obj2));
    end

    local obj_type = type(obj1);

    if obj_type == "userdata" then
        if obj1 == obj2 then
            return true,"";
        elseif comp_userdata then
            return comp_userdata(obj1,obj1_name,obj2,obj2_name);
        else
            return false,string.format("can not compare userdata: %s and %s",obj1_name,obj2_name);
        end
    end

    if obj_type == "table" then
        for k,v in pairs(obj1) do
            local key = "."..k;
            if type(k) == "number" then
                key="["..k.."]";
            end         
            local e,r = table_equal_recursive(v,obj1_name..key,obj2[k],obj2_name..key,comp_userdata);
            if not e then
                return e,r;
            end
        end

        for k,v in pairs(obj2) do
            local key = "."..k;
            if type(k) == "number" then
                key="["..k.."]";
            end

            local e,r = table_equal_recursive(obj1[k],obj1_name..key,v,obj2_name..key,comp_userdata);
            if not e then
                return e,r;
            end
        end

        return true,"";
    end

    --basic type
    if obj1 ~= obj2 then
        return false, string.format("%s = %s, but %s = %s",obj1_name,tostring(obj1),obj2_name,tostring(obj2));
    end

    return true,"";
end

local function is_type(obj,type)
    local ud = tostring(obj);
    local pos = string.find(ud,type..":",1,true);
    return pos == 1;
end

local function is_filebase(obj)
    return is_type(obj,"userdata:mem") 
        or is_type(obj,"userdata:memfile") 
        or is_type(obj,"userdata:file");
end

local function comp_filebase(obj1,obj1_name,obj2,obj2_name)
    if is_filebase(obj1) and is_filebase(obj2) then

        if obj1:GetSize() ~= obj2:GetSize() then
            return false,string.format("%s.size=%d but %s.size=%d",
                obj1_name,obj1:GetSize(),obj2_name,obj2:GetSize());
        elseif FileManager.IsFileSame(obj1,obj2) then
            return true,"";
        else
            return false,string.format("binary content of %s and %s are not same",obj1_name,obj2_name);
        end
    else
        return false,string.format("can not compare userdata: %s and %s",obj1_name,obj2_name);
    end
end

function table_equal(obj1, obj2, comp_userdata)
    if not comp_userdata then
        comp_userdata = comp_filebase;
    end
    return table_equal_recursive(obj1,"object1", obj2, "object2",comp_userdata);
end


