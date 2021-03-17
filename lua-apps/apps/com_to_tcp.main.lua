require("common");
require("utils");

function app_short_help()
    return "forward serial data to network.";
end

function app_name()
    return "com_to_tcp";
end

local g_serial = nil;
local g_all_clients = {};

function auto_remove_disconnected_clients()
    local len = #g_all_clients;
    for i=1,len,1 do
        local client = g_all_clients[i];
        if client and not client:IsConnected() then
            printf("client %d disconnected.",i);
            table.remove(g_all_clients,i);
        end
    end
end

function write_data_to_all_clients(thread,buf,n)
    for _,client in ipairs(g_all_clients) do
        if client:IsConnected() then
            client:Write(buf,n);
        end
    end
end

function com_forward_thread_default(thread)
    local buf = new_mem();
    while true do
        buf:SetSize(0);
        local n = g_serial:Read(buf,buf:GetMaxSize());
        if n > 0 then
            buf:SetSize(n);
            write_data_to_all_clients(thread,buf,n);
        else
            thread:Sleep(1);
            auto_remove_disconnected_clients();
        end
    end
end


local g_tmp_buffer = nil;
local g_tmp_line = nil;
function has_gpgsv()
    g_tmp_buffer:Seek(0);
    if g_tmp_buffer:SearchStr("$GPGSV",true,false) >= 0 then
        return true;
    end
end

function try_to_forward_for_ublox(thread,qbuf)
    g_tmp_line:SetSize(0);
    qbuf:ReadLine(g_tmp_line);
    g_tmp_line:Seek(0);
    if g_tmp_line:StartWith("$GPRMC",true,false) then
        if has_gpgsv() then
            write_data_to_all_clients(thread,g_tmp_buffer,g_tmp_buffer:GetSize());
        end
        g_tmp_buffer:SetSize(0);
        g_tmp_buffer:Puts(g_tmp_line);
        g_tmp_buffer:Puts("\r\n");
    else
        g_tmp_buffer:Puts(g_tmp_line);
        g_tmp_buffer:Puts("\r\n");
    end
end

function com_forward_thread_for_ublox(thread)
    g_tmp_buffer = new_mem();
    g_tmp_line = new_mem();

    local qbuf = QueueFile.new(256*1024);
    local tmp = new_mem();

    while true do
        tmp:SetSize(0);
        local n = g_serial:Read(tmp,tmp:GetMaxSize());
        if n > 0 then
            tmp:SetSize(n);
            qbuf:Write(tmp,n);
            while qbuf:HasWholeLine() do
                try_to_forward_for_ublox(thread,qbuf);
            end
        else
            thread:Sleep(1);
            auto_remove_disconnected_clients();
        end
    end
end

local kDev="--dev";
local kPort = "--port";
local kUbloxMode ="--ublox-mode"

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kDev,TYPE_KEY_EQUAL_VALUE,MUST,"serial device name");
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,MUST,"tcp listening port");
    cmd:AddKeyType(kUbloxMode,TYPE_KEY,OPTIONAL,"keep only data with $GPGSV to compatible with ublox");

    cmd:LoadFromArgv(args);
    if cmd:CheckForErrors() then
        return;
    end

    local port = cmd:GetValueByKey(kPort);
    local dev = cmd:GetValueByKey(kDev);
    local ublox_mode = cmd:HasKey(kUbloxMode);

    if ublox_mode then
        print("u-blox mode");
    end

    local serial = Serial.new();
    if not serial:Open(dev) then
        printf("can not open dev %s",dev);
        return;
    end

    serial:Configure(115200,8,1,'N');
    g_serial = serial;

    TcpSocket.NewTcpAcceptor(port,function(event,new_socket)
        if event == EVENT_STOP then
            exit("tcp bind fail on port "..port);
            return
        end

        if event == EVENT_NEW_CLIENT then
            printfnl("new client %d arrive on port %d",
                new_socket:GetSocketFd(),
                port
            );
            table.insert(g_all_clients,new_socket);
        end
    end);

    co = CoThread.new(1);

    if ublox_mode then
        co:Start(com_forward_thread_for_ublox);
    else
        co:Start(com_forward_thread_default);
    end

    App.MainLoop();
end
