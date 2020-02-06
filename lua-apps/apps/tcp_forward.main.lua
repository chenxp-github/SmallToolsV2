require("common");
require("utils");

forward_list = nil;
local EVENT_NEW_CLIENT = 1;
local EVENT_STOP = 2;
local EVENT_CONNECTED = 1;

function app_short_help()
    return "forward tcp data.";
end

function app_name()
    return "tcp_forward";
end

local usage_text=[[

config file is a lua file, like this:

forward_list={
    {
        local_port=2008,
        remote_server="192.168.1.100",
        remote_port=2009,
    }
    ,
    {
        local_port=2009,
        remote_server="192.168.1.101",
        remote_port=2009,
    }
}

]];

function print_usage(args)
    printfnl("usage: %s <config>",args[1]);
    printnl(usage_text);
end

-------------------------------------------
function connect_to_remote(thread,item)
    local is_end = false;
    local remote_socket = nil;
    TcpSocket.NewTcpConnector(item.remote_server,item.remote_port,
        function(event,new_socket)
            if event == EVENT_CONNECTED then
                remote_socket = new_socket;
            end
            is_end = true;
        end
    );

    while not is_end do
        thread:Sleep(10);
    end

    return remote_socket;
end

function item_string(item)
    return string.format("(localhost:%d)=>%s:%d",
        item.local_port,
        item.remote_server,
        item.remote_port);
end

function tcp_forward_thread(thread,local_socket,item)
    local remote_socket = connect_to_remote(thread,item);
    if not remote_socket then
        printfnl("connect to %s fail.",item_string(item));
        printfnl("forward thread exit.");
        return
    end

    printfnl("connect to %s ok.",item_string(item));

    local recv_qbuf = QueueFile.new(256*1024);
    local send_qbuf = QueueFile.new(256*1024);

    local tmp = new_mem(64*1024);

    while true do
        if not local_socket:IsConnected() then
            printfnl("local socket closed, %s",item_string(item));
            break;
        end

        if not remote_socket:IsConnected() then
            printfnl("remote socket closed, %s",item_string(item));
            break;
        end

        tmp:SetSize(0);
        if recv_qbuf:GetFreeSize() >= tmp:GetMaxSize() then
            local rs = local_socket:Read(tmp,tmp:GetMaxSize());
            recv_qbuf:Write(tmp,rs);
        end

        if recv_qbuf:GetSize() > 0 then
            local rs = recv_qbuf:PeekData(tmp,tmp:GetMaxSize());
            local ws = remote_socket:Write(tmp,rs);
            recv_qbuf:Skip(ws);
        end

        tmp:SetSize(0);
        if send_qbuf:GetFreeSize() >= tmp:GetMaxSize() then
            local rs = remote_socket:Read(tmp,tmp:GetMaxSize());
            send_qbuf:Write(tmp,rs);
        end

        if send_qbuf:GetSize() > 0 then
            local rs = send_qbuf:PeekData(tmp,tmp:GetMaxSize());
            local ws = local_socket:Write(tmp,rs);
            send_qbuf:Skip(ws);
        end

        thread:Sleep(1);
    end

    printfnl("forward %s thread exit",item_string(item));
end

function create_forward_thread(local_socket,item)
    local co = CoThread.new(1);
    co:Start(tcp_forward_thread,local_socket,item);
end

function create_listener(item)
    printfnl("listen on port %s",item.local_port);

    TcpSocket.NewTcpAcceptor(item.local_port,function(event,new_socket)
        if event == EVENT_STOP then
            printnl("create tcp acceptor fail on port "..item.local_port);
            return
        end

        if event == EVENT_NEW_CLIENT then
            printfnl("new client %d arrive on port %d",
                new_socket:GetSocketFd(),
                item.local_port
            );

            create_forward_thread(new_socket,item);
        end
    end)
end
-------------------------------------------
function app_main(args)
    local argc = #args;
    if argc < 2 then
        return print_usage(args);
    end

    if not exec_file(args[2]) then
        exit("exec_file "..args[2].." fail.");
    end

    for _,item in ipairs(forward_list) do
        create_listener(item);
    end

    App.MainLoop();
end
