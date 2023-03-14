require("common")
require("utils")
require("time_sync_server")
require("time_sync_client")
require("cothread")

function app_short_help()
    return "sync time between computers.";
end

function app_name()
    return "time_sync";
end

local kAsServer="--as-server";
local kAsClient="--as-client";
local kServer="--server";
local kPort="--port";
local kMaxDelta = "--max-delta";

function sync_thread(thread,client,max_delta)
    while true do
        local r = client:GetHostTime_Async(thread);
        if r.result == E_OK then
            local host_now = r.value.now;
            
            local tm = CommonTime.new();
            tm:SetNow();
            local now = tm:GetLong();
            
            if math.abs(now - host_now) > max_delta then
                tm:Set(host_now+10);
                tm:SetSystemTime();
                printf("adjust time to %s",tm:ToString());
            end
        end
        thread:Sleep(1000);
    end
end

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kAsServer,TYPE_KEY,OPTIONAL,"as a server");
    cmd:AddKeyType(kAsClient,TYPE_KEY,OPTIONAL,"as a client");
    cmd:AddKeyType(kServer,TYPE_KEY_EQUAL_VALUE,MUST,"message peer server addr");
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,MUST,"message peer port");
    cmd:AddKeyType(kMaxDelta,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"max delta time before sync time(ms)");
    
    cmd:AddKeyTypeDep(kAsClient,"",kMaxDelta);
    cmd:AddKeyTypeDep(kAsClient,"",kServer);

    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
    
    local as_server = cmd:HasKey(kAsServer);
    local as_client = cmd:HasKey(kAsClient);
    local port = tonumber(cmd:GetValueByKey(kPort));

    if as_server then
        App.StartMessageCenter(port,true);
        printfnl("listening on port %d",port);
        local server = TimeSyncServer.new();
        server:InitServerSidePeer();
        server:SetName("time-sync-server");
        server:Start();
    end

    if as_client then
        local server = cmd:GetValueByKey(kServer);

        local max_delta = 1000;
        if cmd:HasKey(kMaxDelta) then
            max_delta = tonumber(cmd:GetValueByKey(kMaxDelta));
        end

        local client = TimeSyncClient.new();
        client:InitClientSidePeer(server,port);
        client:SetName("time-sync-client-"..CFunc.generate_uuid());
        client:SetDestPeerName("time-sync-server");
        client:Start();

        co = CoThread.new();
        co:Start(sync_thread,client,max_delta);
    end

    App.MainLoop();
end



