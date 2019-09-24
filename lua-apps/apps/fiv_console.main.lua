require("common");
require("utils");
require("cothread");

g_peer = nil;
config = nil;

function app_short_help()
    return "send string to lua or js to execute.";
end

function app_name()
    return "fiv_console";
end

function send_command(cmd)
    g_peer:SendMessage(
        config.to,
        0x12345678,0,
        BODY_TYPE_STRING|MSG_TYPE_SIGNAL,
        cmd
    );
end

function monitor_thread(thread)
    local exit_count = 0;    
    while true do
        if g_peer:GetSendingQueueLength() == 0 then
            exit_count = exit_count + 1;
            if exit_count >= 10 then
                return App.QuitMainLoop();
            end
        else
            exit_count = 0;
        end
        thread:Sleep(100);
    end
end

g_monitor_thread = nil;
function start_monitor_thread()
    g_monitor_thread = CoThread.new(10);
    g_monitor_thread:Start(monitor_thread,g_monitor_thread);    
end

function app_main(args)
    local argc = #args;
    
    if argc < 2 then
        print(args[1].." <file> [config]");
        return;
    end
    
    local config_file = "config.txt";
    
    if args[3] then
        config_file = args[3];
    end
        
    local file_to_send = args[2];

    config={
        server = "127.0.0.1",
        port = 2086;
        name = "fiv-lua-console",
        to = "client-lua",
    };
    
    if not exec_file(config_file) then
        print("load config file fail "..config_file);
    end
   
    g_peer = MessagePeer.new();
    g_peer:SetServer(config.server);
    g_peer:SetPort(config.port);
    g_peer:SetName(config.name);
    g_peer:Start();

    local mem = new_mem(file_to_send);    
    send_command(mem:CStr());    
    start_monitor_thread();
    
    App.MainLoop();
end

