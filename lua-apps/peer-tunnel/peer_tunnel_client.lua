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
    self.remote_server = "";
    self.remote_port = 0;
end

function PeerTunnelClient:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
--##End OnRequest ##--
end


--@@Begin Method ConnectRemote @@--
function PeerTunnelClient:ConnectRemote(_server, _port, _callback)
    local _cbid = self:AddCallback(_callback);
    local _param={
        server = _server,
        port = _port,
    };
    return self:SendRequest(_param,METHOD_PT_CONNECTREMOTE,_cbid);
end
--@@End Method ConnectRemote @@--


--@@Begin Method ConnectRemote_Async @@--
function PeerTunnelClient:ConnectRemote_Async(thread, _server, _port)
    local ret = {};
    local done = false;
    
    self:ConnectRemote(_server,_port,function(res,val)
        ret.result = res;
        ret.value = val;
        done = true;
    end);
    
    while not done and not thread:IsDead() do
        thread:Sleep(1);
    end
    
    return ret;
end
--@@End Method ConnectRemote_Async @@--

--@@ Insert Method Here @@--

function PeerTunnelClient:SetRemote(server, port)
    self.remote_server = server;
    self.remote_port = port;
end

function PeerTunnelClient:StartListeningLocalPort(local_port)
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
            self:OnNewLocalClient(new_socket);
        end
    end)
end

function PeerTunnelClient:OnNewLocalClient(new_socket)    
    function connect_thread(thread)
        local ret = self:ConnectRemote_Async(thread,self.remote_server,self.remote_port);
        if ret.result ~= OK then
            printfnl("call ConnectRemote_Async timeout.");
            return;
        end

        if ret.value.handle <= 0 then
            printfnl("connect remote rejected %s",ret.value.errStr);
            return;
        end

        printfnl("connect remote %s:%d success.", self.remote_server,self.remote_port);
        local connection = LocalConnection.new(self,new_socket,ret.value.handle);
        self.local_connections[ret.value.handle] = connection;
    end

    local co = CoThread.new();
    co:Start(connect_thread);
end

