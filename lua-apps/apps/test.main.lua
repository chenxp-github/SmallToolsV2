require("common");
require("utils");

function app_short_help()
    return "just for test.";
end

function app_name()
    return "test";
end

local kTest="--test"

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kTest,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"test");
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
    
    print("Hello world!");
end
