require("common");
require("utils");

local g_pattern = nil;
local g_signal = 15;
local g_test_only = false;

function app_short_help()
    return "killall support pattern match";
end

function app_name()
    return "killall";
end

local kPattern="--pattern";
local kSignal="--signal";
local kTestOnly="--test-only";

function cmdline_to_string(cmdline)
    if not cmdline then return end
    local str = "";
    for k,v in ipairs(cmdline) do
        str = str.." "..v;
    end
    return str;
end

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();

    cmd:AddKeyType(kPattern,TYPE_KEY_EQUAL_VALUE,MUST,
        "command line match pattern,(lua pattern)");
    
    cmd:AddKeyType(kPattern,TYPE_KEY_EQUAL_VALUE,OPTIONAL,
        "signal to send, default is 15(SIGKILL)");

    cmd:AddKeyType(kTestOnly,TYPE_KEY,OPTIONAL,
        "just print result, do not send signal really");

    cmd:LoadFromArgv(args);
    if cmd:CheckForErrors() then
        return;
    end

    g_pattern = cmd:GetValueByKey(kPattern);

    if cmd:HasKey(kSignal) then
        g_signal = tonumber(cmd:GetValueByKey(kSignal));
    end

    g_test_only = cmd:HasKey(kTestOnly);

    NativeProcessManager.EnumAllProcess(function(process)
        process:UpdateCmdLine();        
        if process:GetPid() == CFunc.getpid() then
            return
        end

        local cmdline = process:GetCmdLine();
        local str = cmdline_to_string(cmdline);
        if not str then 
            return 
        end
        
        if string.match(str,g_pattern) then
            if g_test_only then
                printnl(str);
            else
                printnl(str);
                execf("kill -%d %d",g_signal, process:GetPid());
            end
        end
    end);
end
