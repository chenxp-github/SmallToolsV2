require("common");
require("utils");

forward_list = nil;

function app_short_help()
    return "forward udp data.";
end

function app_name()
    return "udp_forward";
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
function item_string(item)
    return string.format("(localhost:%d)=>%s:%d",
        item.local_port,
        item.remote_server,
        item.remote_port);
end

function udp_forward_thread(thread,item)
    local udp_socket = UdpSocket.new();
    udp_socket:Create();

    if not udp_socket:Bind(item.local_port) then
        printfnl("bind fail on port %d",item.local_port);
        return
    end
    
    printfnl("start forwarding: %s.",item_string(item));

    local remote_ip = CFunc.inet_addr(item.remote_server);
    local remote_port = item.remote_port;

    local from_ip, from_port;

    local mem = new_mem();
    while true do
        for i=1,1000,1 do
            mem:SetSize(0);
            if udp_socket:RecvMsg(mem) then
                local ip,port = udp_socket:GetSrcAddr(true);
                
                if ip == remote_ip and port == remote_port then
                    if from_ip and from_port then
                        udp_socket:SetDestIpAndPort(from_ip,from_port);
                    end
                else
                    from_ip = ip;
                    from_port = port;
                    udp_socket:SetDestIpAndPort(remote_ip,remote_port);
                end

                udp_socket:SendMsg(mem);
            else
                break;
            end
        end
        thread:Sleep(1);
    end
     
end

function create_forward_thread(item)
    local co = CoThread.new();
    co:Start(udp_forward_thread,item)
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
        create_forward_thread(item);
    end

    App.MainLoop();
end
