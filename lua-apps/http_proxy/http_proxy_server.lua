require("common")
require("http_proxy_connection")

HttpProxyServer = class();

function HttpProxyServer:ctor()
    self.all_clients = {};
    self.auto_clear_thread = nil;
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

function HttpProxyServer:StartAutoClear()
    if self.auto_clear_thread then
        return
    end

    function _auto_clear(thread)
        while true do
            thread:Sleep(5000);
            local i = 1;
            local len = #self.all_clients;
            while i < len do                
                local con = self.all_clients[i];
                if con and not con:IsConnected() then
                    printfnl("auto remove a proxy connection %d.",con.socket_fd);
                    con:Close();                    
                    table.remove(self.all_clients,i);
                    i = i - 1;
                end
                i = i + 1;
            end

        end
    end

    self.auto_clear_thread = CoThread.new();
    self.auto_clear_thread:Start(_auto_clear);
end