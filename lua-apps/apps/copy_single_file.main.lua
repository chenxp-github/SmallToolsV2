require("common");
require("utils");

function app_short_help()
    return "copy a single file.";
end

function app_name()
    return "copy_single_file";
end

local kIn="--in";
local kOut="--out";
local kStart="--start";
local kSize="--size";
local kOutStart="--out-start";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kIn,TYPE_KEY_EQUAL_VALUE,MUST,"in file");
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"out file");
    cmd:AddKeyType(kStart,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"start position of in file, default is 0");
    cmd:AddKeyType(kSize,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"copy size, default is whole file");
    cmd:AddKeyType(kOutStart,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"start position of out file, default is 0");    
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
    
    local in_file = FileManager.ToAbsPath(cmd:GetValueByKey(kIn));
    local out_file = FileManager.ToAbsPath(cmd:GetValueByKey(kOut));
    
    local start = 0;
    if cmd:HasKey(kStart) then
        start = tonumber(cmd:GetValueByKey(kStart));
    end
    
    local size = -1;    
    if cmd:HasKey(kSize) then
        size = tonumber(cmd:GetValueByKey(kSize));
    end
    
    local out_start = 0;
    if cmd:HasKey(kOutStart) then
        out_start = tonumber(cmd:GetValueByKey(kOutStart));
    end
    
    FileManager.CopySingleFile(in_file,out_file,start,size,out_start,MODE_USE_LOG|MODE_OVERWRITE|MODE_PART_COPY);    
end

