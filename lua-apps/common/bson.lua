local BSON_TYPE_DOUBLE=0x01;
local BSON_TYPE_STRING=0x02;
local BSON_TYPE_DOCUMENT=0x03;
local BSON_TYPE_ARRAY=0x04;
local BSON_TYPE_BINARY=0x05;
local BSON_TYPE_BOOLEAN=0x08;
local BSON_TYPE_INT32=0x10;
local BSON_TYPE_INT64=0x12;

function BsonToObject(bson,is_array)
    if not bson then return end

    local obj = {};
    bson:ResetPointer();

    while not bson:IsEnd() do
        local type_,name = bson:PeekNext();
        local val;
        if type_ == BSON_TYPE_INT32 then
            val = bson:GetInt32(name);
        elseif type_ == BSON_TYPE_INT64 then
            val = bson:GetInt64(name);
        elseif type_ == BSON_TYPE_DOUBLE then
            val = bson:GetDouble(name);
        elseif type_ == BSON_TYPE_STRING then
            val = bson:GetString(name);
        elseif type_ == BSON_TYPE_BOOLEAN then
            val = bson:GetBoolean(name);
        elseif type_ == BSON_TYPE_DOCUMENT then
            local doc = bson:GetDocument(name);
            val = BsonToObject(doc);
        elseif type_ == BSON_TYPE_ARRAY then
            local doc = bson:GetArray(name);
            val = BsonToObject(doc,true);
        elseif type_ == BSON_TYPE_BINARY then
            local mem = Mem.new();
            bson:GetBinary(name,mem);
            val = {_binary_ = mem};
        else
            break;
        end
        
        if is_array then
            table.insert(obj,val);
        else
            local name_num = tonumber(name);
            if name_num then
                obj[name_num]=val;
            else
                obj[name] = val;
            end
        end
    end

    return obj;
end

function value_to_bson(bson,key,value)
    if type(value) == "table" then    
        if value._is_array_ then
            local off_a = bson:StartArray(key);
            local length = 0;
            for k,v in ipairs(value) do
                length = length + 1;
                value_to_bson(bson,tostring(k-1),v);
            end
            bson:EndArray(off_a,length);
        elseif value._binary_ then
            bson:PutBinary(key,value._binary_);
        elseif value._int64_ then
            bson:PutInt64(key,value._int64_);
        elseif value._double_ then
            bson:PutDouble(key,value._double_);
        else        
            local off = bson:StartDocument(key);
            for k,v in pairs_ordered(value) do
                value_to_bson(bson,k,v);
            end 
            bson:EndDocument(off);
        end        
    elseif type(value) == "string" then
        bson:PutString(key,value);
    elseif type(value) == "number" then
        bson:PutInt32(key,value);
    elseif type(value) == "boolean" then
        bson:PutBoolean(key,value);
    end
end


--pairs is not stable,so use pairs_ordered
function ObjectToBson(obj)
    if not obj then return end
    local bson = Bson.new();
    bson:StartDocument();

    for k,v in pairs_ordered(obj) do
        value_to_bson(bson,k,v);
    end 

    bson:EndDocument();
    return bson;
end

function ObjectToJson(obj)
    local json = "{";
    local i = 0;
    for k,v in pairs(obj) do
        local k_str="\""..k.."\"";
        if i > 0 then
            json = json..",";
        end
        if type(v) == "table" then
            json = json..k_str.. ":"..ObjectToJson(v);
        elseif type(v) == "string" then
            json = json..k_str..":\""..v.."\"";
        elseif type(v) ~= "function" then
            json = json..k_str..":"..v;
        end
        i = i + 1;
    end
    json = json .. "}";
    return json;
end
