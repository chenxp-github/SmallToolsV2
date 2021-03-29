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

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kCmd,TYPE_KEY_EQUAL_VALUE,MUST,"command to run");
    cmd:AddKeyType(kTimeout,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"timeout in ms");
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

    os.execute(command);
end
