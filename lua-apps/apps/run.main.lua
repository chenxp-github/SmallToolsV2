require("common");
require("utils");

function app_short_help()
    return "run a command or a shell file.";
end

function app_name()
    return "run";
end

local kCmd="--cmd";
local kTimeout="--timeout";
local kResult="--result";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kCmd,TYPE_KEY_EQUAL_VALUE,MUST,"command to run");
    cmd:AddKeyType(kTimeout,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"timeout in ms");
    cmd:AddKeyType(kResult,TYPE_KEY_EQUAL_VALUE, OPTIONAL,"result file");
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
    
    local command = cmd:GetValueByKey(kCmd);
    local timeout = -1;

    if cmd:HasKey(kTimeout) then
        timeout = tonumber(cmd:GetValueByKey(kTimeout));        
    end

    if timeout > 0 then
        App.StartAutoExitThread(timeout,1);
    end

    local r,exit,sig = os.execute(command);

    if cmd:HasKey(kResult) then
        local filename = cmd:GetValueByKey(kResult);
        local mem = new_mem();
        mem:Puts("result = "..(r and 1 or 0)..EOL);
        if exit then
            mem:Puts("exit = "..exit..EOL);
        end    
        if sig then
            mem:Puts("sig = "..sig..EOL);
        end    
        mem:WriteToFile(filename);
    end
end
