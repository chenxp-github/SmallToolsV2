require("common");
require("utils");

function app_short_help()
    return "dump a resource file.";
end

function app_name()
    return "dump_resource_file";
end

local kIn = "--in";
local kOut = "--out";
local kIdOnly = "--id-only";
local kFormat="--format";

function code_id_only_mode(rm,format,out_file)
    local res = Resource.new();

    local id_tab = {};
    for i=0,rm:GetTotalSlots()-1,1 do
        if rm:GetBySlot(i,res) then
            local _type = res:GetType();
            if not id_tab[_type] then
                id_tab[_type] = {};
            end
            local id = res:GetId();
            id_tab[_type][id] = 1;
        end
    end

    local code = PrintBuffer.new();

    for _type,ids in pairs(id_tab) do
        if format == "lua" then
            code:Log("---------------------");
            for id in pairs_ordered(ids) do
                code:Log("%s   =   \"%s\";",id,id);
            end
        elseif format == "js" then
            code:Log("/////////////////////");
            for id in pairs_ordered(ids) do
                code:Log("const %s   =   \"%s\";",id,id);
            end
        elseif format == "cpp" then
            code:Log("/////////////////////");
            for id in pairs_ordered(ids) do
                code:Log("#define   %s   \"%s\"",id,id);
            end
        else
            for id in pairs_ordered(ids) do
                code:Log("%s",id);
            end
        end
    end

    if code:SaveToFile(out_file) > 0 then
        printf("save to %s ok.",out_file);
    else
        exit("fail to %s.",out_file);
    end
end

function app_main(args)
    local argc = #args;    
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kIn,TYPE_KEY_EQUAL_VALUE,MUST,"input resource file.");
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"output file.");
    cmd:AddKeyType(kIdOnly,TYPE_KEY,OPTIONAL,"only show resource id");
    cmd:AddKeyType(kFormat,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"output source format,default is lua");  
    cmd:LoadFromArgv(args);

    if cmd:CheckForErrors() then
        return -1;
    end

    local in_file = cmd:GetValueByKey(kIn);
    local out_file = cmd:GetValueByKey(kOut);
    local id_only = cmd:HasKey(kIdOnly);
    
    local format = "lua";
    if cmd:HasKey(kFormat) then
        format = cmd:GetValueByKey(kFormat);
    end

    local rm = ResourceManager.new();
    if not rm:LoadResourceFile(in_file) then
        return exit("load resource file "..in_file.." error");
    end

    if id_only then
        code_id_only_mode(rm,format,out_file);
    else
        rm:DumpResourceFile(out_file);
    end

end

