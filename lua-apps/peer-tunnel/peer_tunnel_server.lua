require("common")
require("peer_service_base")
require("linkrpc_pt_types")
require("local_connection")

PeerTunnelServer = class(PeerServiceBase);

function PeerTunnelServer:ctor()    
    self.local_connections = {};
end

function PeerTunnelServer:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
    if method == METHOD_PT_CONNECTREMOTE then
        self:OnConnectRemote(_context,_param);
    end
--##End OnRequest ##--
end

--@@Begin Method OnConnectRemote @@--
function PeerTunnelServer:OnConnectRemote(_context,_param)
    printfnl("try to connect to %s:%d",_param.server,_param.port);

    TcpSocket.NewTcpConnector(_param.server,_param.port,
        function(event,new_socket)
            local _ret;
            if event == EVENT_CONNECTED then                
                _ret={
                    handle = new_socket:GetSocketFd(),
                    errStr = "",
                };
                printf("new tunnel client arrive %d",_ret.handle);
                local connection = LocalConnection.new(self,new_socket,_ret.handle);
                self.local_connections[_ret.handle] = connection;
            else
                _ret={
                    handle = -1,
                    errStr = "connect error",
                };                           
            end
            self:SendReturnValue(_context,_ret);     
        end
    );   
end
--@@End Method OnConnectRemote @@--

--@@ Insert Method Here @@--
