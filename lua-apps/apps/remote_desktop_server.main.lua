require("common");
require("utils");

function app_short_help()
    return "remote desktop server (X11 only).";
end

function app_name()
    return "remote_desktop_server";
end

local kAsClient="--as-client";
local kPeerServer="--peer-server";
local kPeerPort="--peer-port";
local kPeerName="--peer-name";

g_snapshottor_manager = nil;
g_desktop_server = nil;

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kAsClient,TYPE_KEY,OPTIONAL,"as message peer client?");
    cmd:AddKeyType(kPeerServer,TYPE_KEY_EQUAL_VALUE,MUST,"message peer server address");
    cmd:AddKeyType(kPeerPort,TYPE_KEY_EQUAL_VALUE,MUST,"message center service port");    
    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,MUST,"self message peer name");            

    cmd:AddKeyTypeDep(kAsClient,"",kPeerServer);

    cmd:LoadFromArgv(args);  
    if cmd:CheckForErrors() then
        return 1;
    end
    
    local as_client = cmd:HasKey(kAsClient);

    local peer_server = cmd:GetValueByKey(kPeerServer);
    local peer_port = tonumber(cmd:GetValueByKey(kPeerPort));
    local peer_name = cmd:GetValueByKey(kPeerName);    


    local xdisplay = XDisplay.new();
    if not xdisplay:OpenDisplay(":0") then
        printfnl("can not open display");
        return App.QuitMainLoop();
    end

    local root_window = xdisplay:GetDefaultRootWindow();
    if not root_window then
        printfnl("can not get default root window");
        return App.QuitMainLoop();
    end

    avoid_gc(xdisplay,root_window);

	g_snapshottor_manager = RemoteDesktopSnapshottorManager.new();
    local snapshottor = RemoteDesktopSnapshottor_X11.new();
    snapshottor:SetName("DISPLAY:0");
    snapshottor:SetXDisplay(xdisplay);
    snapshottor:SetXWindow(root_window);

    avoid_gc(snapshottor);
    g_snapshottor_manager:AddSnapshotter(snapshottor);
    
    g_desktop_server = RemoteDesktopServer.new();

    if not as_client then
        App.StartMessageCenter(peer_port,true);
        printfnl("start message center on port %d",peer_port);
        g_desktop_server:InitServiceSidePeer();
    else
        g_desktop_server:InitClientSidePeer(peer_server,peer_port);
    end

    g_desktop_server:SetName(peer_name);
	g_desktop_server:SetRemoteDesktopSnapshottorManager(g_snapshottor_manager);
	g_desktop_server:Start();

    App.MainLoop();
end
