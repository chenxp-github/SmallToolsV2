require("common")
require("http_proxy_connection")

HttpProxyServer = class();

function HttpProxyServer:ctor()
    self.all_clients = {};
end

function HttpProxyServer:StartListening(local_port)
    printfnl("start listening on port %d",local_port);
    TcpSocket.NewTcpAcceptor(local_port,function(event,new_socket)
        if event == EVENT_STOP then
            exit("listening tcp port %d fail",local_port);
            return
        end

        if event == EVENT_NEW_CLIENT then
            printfnl("new client %d arrive on port %d",
                new_socket:GetSocketFd(),
                local_port
            );
            self:OnNewClient(new_socket);
        end
    end)
end

function HttpProxyServer:OnNewClient(new_socket)
    local con = HttpProxyConnection.new(new_socket);
    con:Start();
    table.insert(self.all_clients,con);
end
