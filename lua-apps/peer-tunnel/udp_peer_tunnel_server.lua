require("common")
require("peer_service_base")
require("linkrpc_pu_types")

UdpPeerTunnelServer = class(PeerServiceBase);

function UdpPeerTunnelServer:ctor()
    self.auto_id = 0;
    self.local_connections = {};
end

function UdpPeerTunnelServer:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
    if method == METHOD_PU_BINDREMOTE then
        self:OnBindRemote(_context,_param);
    end
    if method == METHOD_PU_SENDDATA then
        self:OnSendData(_context,_param);
    end
    if method == METHOD_PU_CLOSEHANDLE then
        self:OnCloseHandle(_context,_param);
    end
--##End OnRequest ##--
end

function UdpPeerTunnelServer:AllocId()
   self.auto_id = self.auto_id + 1;
   return self.auto_id; 
end

--@@Begin Method OnBindRemote @@--
function UdpPeerTunnelServer:OnBindRemote(_context,_param)
    local port = _param.port;

    self:DeleteLocalConnectionByBindPort(port);
    
    local udp_socket = UdpSocket.new();
    udp_socket:Create();

    if port > 0 then
        if not udp_socket:Bind(port) then
            local _ret={
                handle = 0,
                errStr = string.format("bind fail on port %d",port);
            };
            return self:SendReturnValue(_context,_ret);
        end
    end

    local handle = self:AllocId();
    local connection = LocalUdpConnection.new(self);
    connection.from_peer_name = _context.from;
    self.local_connections[handle] = connection;
    connection:StartServerForwarding(udp_socket,handle,port);

    printfnl("create handle: %d",handle);
    local _ret={
        handle = handle,
        errStr = ""
    };
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnBindRemote @@--

--@@Begin Method OnSendData @@--
function UdpPeerTunnelServer:OnSendData(_context,_param)
    local handle = _param.handle;
    local ip = _param.ip;
    local port = _param.port;
    local data = _param.data._binary_;

    local ws = -1;
    local connection=self.local_connections[handle];

    if connection then
        ws = connection:SendLocalData(ip,port,data);
    end

    local _ret={  
        ws = ws,
    };
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnSendData @@--

--@@Begin Method OnCloseHandle @@--
function UdpPeerTunnelServer:OnCloseHandle(_context,_param)
    --local _ret={
    --    ret = 
    --};
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnCloseHandle @@--


--@@Begin Method SendData @@--
function UdpPeerTunnelServer:SendData(_handle, _data)
    local _cbid = 0
    local _param={
        handle = _handle,
        data = {_binary_=_data},
    };
    return self:SendRequest(_param,METHOD_PU_SENDDATA,_cbid);
end
--@@End Method SendData @@--

--@@ Insert Method Here @@--

function UdpPeerTunnelServer:GetHandleByBindPort(bind_port)
    for handle,conn in pairs(self.local_connections) do
        if conn.bind_port == bind_port then
            return handle;
        end
    end
end

function UdpPeerTunnelServer:DeleteLocalConnectionByBindPort(bind_port)
    local handle = self:GetHandleByBindPort(bind_port);
    if handle then
        printfnl("remove handle : %d",handle);
        local conn = self.local_connections[handle];
        conn:Close();
        self.local_connections[handle] =nil;
    end
end
