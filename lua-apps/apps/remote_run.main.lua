require("common");
require("utils");
require("simple_file_client")

function app_short_help()
    return "remote run command via simple file server";
end

function app_name()
    return "remote_run";
end

local kPeerName = "--peer-name";
local kAsServer = "--as-server";
local kServer = "--server";
local kPort = "--port";
local kCmd="--cmd";

function app_main(args)
    local cmd = CommandLine.new();

    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"peer name of the server,default: simple-file-client");
    
    cmd:AddKeyType(kAsServer,TYPE_KEY,OPTIONAL,"as tcp server");
    cmd:AddKeyType(kServer,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer server address,default: 127.0.0.1");
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer port, default: 2016")

    cmd:AddKeyType(kCmd,TYPE_KEY_EQUAL_VALUE,MUST,"command to run");    
    cmd:LoadFromArgv(args);            

    if cmd:CheckForErrors() then
        return
    end

    local as_tcp_server = cmd:HasKey(kAsServer);
    local peer_name = "simple-file-client";

    if cmd:HasKey(kPeerName) then
        peer_name = cmd:GetValueByKey(kPeerName);
    end

    local server = "127.0.0.1";
    local port = 2016;

    if cmd:HasKey(kServer) then
        server = cmd:GetValueByKey(kServer);
    end

    if cmd:HasKey(kPort) then
        port = tonumber(cmd:GetValueByKey(kPort));
    end

    if as_tcp_server  then
        App.StartMessageCenter(port);
        printfnl("start message center on port %d",port);
    end

    local command = cmd:GetValueByKey(kCmd);

    function main_thread(thread)
        local file_client = SimpleFileClient.new(thread);
        if as_tcp_server then
            file_client:InitServerSidePeer();        
        else
            file_client:InitClientSidePeer(server,port);
        end

        file_client:SetName(peer_name);
        file_client:SetDestPeerName("simple-file-server");
        file_client:Start();
    
        file_client:RunCommand(command,function(ret,val)
            print(ret);
            print_table(val);
            App.QuitMainLoop();
        end);
    end

    co = CoThread.new(1);
    co:Start(main_thread);
    App.MainLoop();
end



