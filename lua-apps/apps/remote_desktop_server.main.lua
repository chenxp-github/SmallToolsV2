require("common");
require("utils");
require("bson")

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
local kDisplay="--display";
local kHideConsole="--hide-console"

g_snapshottor_manager = nil;
g_desktop_server = nil;

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kAsClient,TYPE_KEY,OPTIONAL,"as message peer client?");
    cmd:AddKeyType(kPeerServer,TYPE_KEY_EQUAL_VALUE,MUST,"message peer server address");
    cmd:AddKeyType(kPeerPort,TYPE_KEY_EQUAL_VALUE,MUST,"message center service port");    
    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,MUST,"self message peer name");        
    
    if is_linux() then
        cmd:AddKeyType(kDisplay,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"X11 display name, default is :0"); 
    end

    if is_windows() then
        cmd:AddKeyType(kHideConsole,TYPE_KEY,OPTIONAL,"hide console window"); 
    end

    cmd:AddKeyTypeDep(kAsClient,"",kPeerServer);

    cmd:LoadFromArgv(args);  
    if cmd:CheckForErrors() then
        return 1;
    end
    
    local as_client = cmd:HasKey(kAsClient);

    local peer_server = cmd:GetValueByKey(kPeerServer);
    local peer_port = tonumber(cmd:GetValueByKey(kPeerPort));
    local peer_name = cmd:GetValueByKey(kPeerName);    

    if is_linux() then
        local display_name = ":0";
        if cmd:HasKey(kDisplay) then
            display_name = cmd:GetValueByKey(kDisplay);
        end

        local xdisplay = XDisplay.new();

        while not xdisplay:OpenDisplay(display_name) do
            printfnl("can not open display: %s",display_name);
            App.Sleep(3000);
        end

        printfnl("open display %s ok.",display_name);

        local root_window = xdisplay:GetDefaultRootWindow();
        if not root_window then
            printfnl("can not get default root window");
            return App.QuitMainLoop();
        end

        avoid_gc(xdisplay,root_window);

        g_snapshottor_manager = RemoteDesktopSnapshottorManager.new();
        local snapshottor = RemoteDesktopSnapshottor_X11.new();
        snapshottor:SetName(display_name);
        snapshottor:SetXDisplay(xdisplay);
        snapshottor:SetXWindow(root_window);

        avoid_gc(snapshottor);
        g_snapshottor_manager:AddSnapshotter(snapshottor);
    end

    if is_windows() then
        require("win32")
        if cmd:HasKey(kHideConsole) then
            local hwnd = Win32.GetConsoleWindow();
            Win32.ShowWindow(hwnd,SW_HIDE);
        end

        local bson = Win32.GetAllDisplayMonitors();
        g_all_monitors = BsonToObject(bson);	
        g_snapshottor_manager = RemoteDesktopSnapshottorManager.new();

        for _,monitor in ipairs(g_all_monitors.monitors) do
            local hdc;
            local snapshottor = RemoteDesktopSnapshottor_GDI.new();
            snapshottor:SetName(monitor.szDevice);
            printfnl("monitor=%s",monitor.szDevice);

            if monitor.dwFlags & 0x01 ~= 0 then  --primary display
                hdc = Win32.GetDC(0);
            else
                hdc = Hdc.new();
                hdc:Create(nil,monitor.szDevice);
            end

            avoid_gc(hdc);
            avoid_gc(snapshottor);
            
            snapshottor:SetSrcHdc(hdc);
            snapshottor:CreateBitmapBuffer();
            g_snapshottor_manager:AddSnapshotter(snapshottor);
        end
    end
    
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
