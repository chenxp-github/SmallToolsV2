require("common");
require("utils");
require("peer_tunnel_server");
require("peer_tunnel_client");

function app_short_help()
    return "make a tcp tunnel use message peer.";
end

function app_name()
    return "tcp_tunnel";
end

local kAsServer="--as-server";
local kAsClient="--as-client";
local kPeerServer="--peer-server";
local kPeerPort="--peer-port";
local kPeerName="--peer-name";
local kRemotePeerName="--remote-peer-name";
local kRemoteServer="--remote-server";
local kRemotePort="--remote-port";
local kLocalListeningPort="--local-listening-port";
local kTimeout="--timeout";
local kConfig="--config";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kAsServer,TYPE_KEY,OPTIONAL,"as tunnel server?");
    cmd:AddKeyType(kAsClient,TYPE_KEY,OPTIONAL,"as tunnel client?");
    cmd:AddKeyType(kPeerServer,TYPE_KEY_EQUAL_VALUE,MUST,"message peer server address");
    cmd:AddKeyType(kPeerPort,TYPE_KEY_EQUAL_VALUE,MUST,"message center service port");    
    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,MUST,"self message peer name");        
    cmd:AddKeyType(kRemotePeerName,TYPE_KEY_EQUAL_VALUE,MUST,"remote message peer name");    
    cmd:AddKeyType(kTimeout,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"socket inactive timeout (ms).");
    cmd:AddKeyType(kConfig,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"config file as client");

    cmd:LoadFromArgv(args);  
    if cmd:CheckForErrors() then
        return 1;
    end
    
    local is_client = cmd:HasKey(kAsClient);
    local is_server = cmd:HasKey(kAsServer);

    if not is_client and not is_server then
        exit("you must specify at least one %s or %s",kAsServer,kAsServer);
    end

    local remote_peer_name = cmd:GetValueByKey(kRemotePeerName);
    local peer_server = cmd:GetValueByKey(kPeerServer);
    local peer_port = tonumber(cmd:GetValueByKey(kPeerPort));
    local peer_name = cmd:GetValueByKey(kPeerName);    
    local client_table={};

    if is_client then
        if cmd:HasKey(kConfig) then
            local cmd = CommandLine.new();
            cmd:AddKeyType(kConfig,TYPE_KEY_EQUAL_VALUE,MUST,"config file as client");

            cmd:LoadFromArgv(args);
            if cmd:CheckForErrors() then
                return 1;
            end

            local config = cmd:GetValueByKey(kConfig);
            if not exec_file(config) then
                return exit("execute %s fail.",config);
            end

            if not tunnel_table then
                return exit("can not find 'tunnel_table' in config file");
            end

            for i,v in ipairs(tunnel_table) do
                local remote_server = v[1];
                local remote_port = v[2];
                local local_listening_port = v[3];

                local client = PeerTunnelClient.new();
                client:InitClientSidePeer(peer_server,peer_port);
                client:SetName(peer_name.."-"..i);
                client:SetDestPeerName(remote_peer_name);
                client:SetRemote(remote_server,remote_port);
                client:Start();        
                client:StartListeningLocalPort(local_listening_port);
                client:StartAutoClearThread();
                printfnl("listening on local port %d -> %s:%d",
                    local_listening_port,remote_server,remote_port);
                table.insert(client_table,client);
            end
        else
            local cmd = CommandLine.new();
            cmd:AddKeyType(kRemoteServer,TYPE_KEY_EQUAL_VALUE,MUST,"remote server address connect to");
            cmd:AddKeyType(kRemotePort,TYPE_KEY_EQUAL_VALUE,MUST,"remote port connect to");
            cmd:AddKeyType(kLocalListeningPort,TYPE_KEY_EQUAL_VALUE,MUST,"local listening port");            
            cmd:AddKeyType(kConfig,TYPE_KEY_EQUAL_VALUE,MUST,"config file as client");

            cmd:LoadFromArgv(args);
            if cmd:CheckForErrors() then
                return 1;
            end    

            local remote_server = cmd:GetValueByKey(kRemoteServer);
            local remote_port = tonumber(cmd:GetValueByKey(kRemotePort));
            local local_listening_port = tonumber(cmd:GetValueByKey(kLocalListeningPort));
    
            local client = PeerTunnelClient.new();
            client:InitClientSidePeer(peer_server,peer_port);
            client:SetName(peer_name);
            client:SetDestPeerName(remote_peer_name);
            client:SetRemote(remote_server,remote_port);
            client:Start();        
            client:StartListeningLocalPort(local_listening_port);
            client:StartAutoClearThread();
            printfnl("listening on local port %d",local_listening_port);
        end
    elseif is_server then
        local server = PeerTunnelServer.new();
        server:InitClientSidePeer(peer_server,peer_port);
        server:SetName(peer_name);
        server:Start();
        server:StartAutoClearThread();

        if cmd:HasKey(kTimeout) then
            local timeout = cmd:GetValueByKey(kTimeout);
            server:SetTimeout(tonumber(timeout));
        end
    end

    App.MainLoop();
end
