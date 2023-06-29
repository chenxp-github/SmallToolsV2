require("common");
require("utils");
require("udp_peer_tunnel_server");
require("udp_peer_tunnel_client");

function app_short_help()
    return "make a udp tunnel use message peer.";
end

function app_name()
    return "udp_tunnel";
end

local kAsServer="--as-server";
local kAsClient="--as-client";
local kPeerServer="--peer-server";
local kPeerPort="--peer-port";
local kPeerName="--peer-name";
local kConfig="--config";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kAsServer,TYPE_KEY,OPTIONAL,"as tunnel server?");
    cmd:AddKeyType(kAsClient,TYPE_KEY,OPTIONAL,"as tunnel client?");
    cmd:AddKeyType(kPeerServer,TYPE_KEY_EQUAL_VALUE,MUST,"message peer server address");
    cmd:AddKeyType(kPeerPort,TYPE_KEY_EQUAL_VALUE,MUST,"message center service port");    
    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,MUST,"self message peer name");            
    cmd:AddKeyType(kConfig,TYPE_KEY_EQUAL_VALUE,MUST,"config file as client");

    cmd:AddKeyTypeDep(kAsClient,"",kConfig);


    cmd:LoadFromArgv(args);  
    if cmd:CheckForErrors() then
        return 1;
    end
    
    local is_client = cmd:HasKey(kAsClient);
    local is_server = cmd:HasKey(kAsServer);

    if not is_client and not is_server then
        exit("you must specify at least one %s or %s",kAsServer,kAsClient);
    end
    
    local peer_server = cmd:GetValueByKey(kPeerServer);
    local peer_port = tonumber(cmd:GetValueByKey(kPeerPort));
    local peer_name = cmd:GetValueByKey(kPeerName);    
    local client_table={};

    if is_client then
        local config = cmd:GetValueByKey(kConfig);
        if not exec_file(config) then
            return exit("execute %s fail.",config);
        end

        if not tunnel_table then
            return exit("can not find 'tunnel_table' in config file");
        end
        
        local client_table={};

        for i,v in ipairs(tunnel_table) do
            local local_bind_port = tonumber(v[1]);
            local remote_bind_port = tonumber(v[2]);
            local remote_dest_ip = v[3];
            local remote_dest_port = tonumber(v[4]);

            local client = UdpPeerTunnelClient.new();
            client:InitClientSidePeer(peer_server,peer_port);
            client:SetName("client-of-"..peer_name.."-"..CFunc.generate_uuid());
            client:SetDestPeerName(peer_name);
            client:Start();        
            client:CreateLocalConnection(
                local_bind_port,remote_bind_port,
                remote_dest_ip,remote_dest_port
            );
            
            printfnl("new udp tunnel:%d -> %d, remote dest: %s:%d",
                local_bind_port,remote_bind_port,
                remote_dest_ip,remote_dest_port
            );

            table.insert(client_table,client);
        end


    elseif is_server then
        server = UdpPeerTunnelServer.new();
        server:InitClientSidePeer(peer_server,peer_port);
        server:SetName(peer_name);
        server:Start();
    end

    App.MainLoop();
end
