require("common");
require("utils");

function app_short_help()
    return "search binary data from a file";
end

function app_name()
    return "search_bin";
end

local kIn = "--in";
local kBin = "--bin";
local kStart = "--start";

function app_main(args)
    local cmd = CommandLine.new();
    cmd:AddKeyType(kIn,TYPE_KEY_EQUAL_VALUE,MUST,"the source file you want to search in");
    cmd:AddKeyType(kBin,TYPE_KEY_EQUAL_VALUE,MUST,"the binary file you want to search");
    cmd:AddKeyType(kStart,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"start position of the source file");    
    cmd:LoadFromArgv(args);            
    
    if cmd:CheckForErrors() then
        return
    end
    
    local start_pos = 0;
    
    if cmd:HasKey(kStart) then
        start_pos = tonumber(cmd:GetValueByKey(kStart));
    end
    
    local in_file_name = cmd:GetValueByKey(kIn);
    local _,in_file = new_mmapfile(in_file_name);
    
    if not in_file then 
        return exit("open in file fail.");
    end
    
    local bin_file_name = cmd:GetValueByKey(kBin);
    local _,bin_file = new_mmapfile(bin_file_name)
    
    if not bin_file then
        return exit("open bin file fail.");
    end
    
    in_file:Seek(start_pos);
    
    while not in_file:IsEnd() do
        local pos = in_file:SearchBin(bin_file);
        if pos < 0 then break end        
        print("find at: "..pos);        
    end
    
end

