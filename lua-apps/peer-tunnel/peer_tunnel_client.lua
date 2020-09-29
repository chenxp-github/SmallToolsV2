require("common")
require("peer_service_base")
require("linkrpc_pt_types")
require("local_connection")

local EVENT_NEW_CLIENT = 1;
local EVENT_STOP = 2;
local EVENT_CONNECTED = 1;

PeerTunnelClient = class(PeerServiceBase);

function PeerTunnelClient:ctor()
    self.local_connections = {};
end

function PeerTunnelClient:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
--##End OnRequest ##--
end

--@@ Insert Method Here @@--

function PeerTunnelClient:StartListeningLocalPort(local_port)
    TcpSocket.NewTcpAcceptor(local_port,function(event,new_socket)
        if event == EVENT_STOP then
            printnl("create tcp acceptor fail on port "..item.local_port);
            App.QuitMainLoop();
            return
        end

        if event == EVENT_NEW_CLIENT then
            printfnl("new client %d arrive on port %d",
                new_socket:GetSocketFd(),
                local_port
            );
            self:OnNewLocalConnection(new_socket);
        end
    end)
end

function PeerTunnelClient:OnNewLocalConnection(new_socket)
    local connection = LocalConnection.new(self,new_socket);
    table.insert(self.local_connections,connection);
end

