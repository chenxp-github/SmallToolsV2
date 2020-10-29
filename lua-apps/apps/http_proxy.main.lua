require("common")
require("utils")
require("http_proxy_server")

function app_short_help()
    return "simple http proxy server.";
end

function app_name()
    return "http_proxy";
end

local kPort="--port"

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,MUST,"listening port");
    cmd:LoadFromArgv(args);
   
    if cmd:CheckForErrors() then
        return;
    end

    local port = tonumber(cmd:GetValueByKey(kPort));
    local server = HttpProxyServer.new();
    server:StartListening(port);
    App.MainLoop();
end
