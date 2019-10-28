require("utils")

local a_name = new_mem();
local a_value = new_memfile();

function xml_node_to_object(node)
    local ret = {};
    local attributes;

    ret.name=node:GetName();

    while node:GetNextAttrib(a_name,a_value) do
        if not attributes then
            attributes = {};
        end
        a_value:Trim();
        attributes[a_name:CStr()] = file_to_string(a_value);
        a_name:SetSize(0);
        a_value:SetSize(0);
    end

    ret.attributes = attributes;
    ret.value = node:GetValue();

    local children;

    local child = node:GetChild();
    while child do
        if not children then
            children = {};
        end
        table.insert(children,xml_node_to_object(child));
        child = child:GetNext();
    end
    ret.children = children;
    return ret;
end
