require("common")
require("peer_service_base")
require("linkrpc_pu_types")
require("local_udp_connection")

UdpPeerTunnelClient = class(PeerServiceBase);

function UdpPeerTunnelClient:ctor()
    self.from_ip = nil;
    self.from_port = nil;
    self.local_connection = nil;
    self.total_recv = 0;
end

function UdpPeerTunnelClient:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
    if method == METHOD_PU_SENDDATA then
        self:OnSendData(_context,_param);
    end
--##End OnRequest ##--
end

--@@Begin Method BindRemote @@--
function UdpPeerTunnelClient:BindRemote(_port, _callback)
    local _cbid = self:AddCallback(_callback);
    local _param={
        port = _port,
    };
    return self:SendRequest(_param,METHOD_PU_BINDREMOTE,_cbid);
end
--@@End Method BindRemote @@--

--@@Begin Method BindRemote_Async @@--
function UdpPeerTunnelClient:BindRemote_Async(thread, _port)
    local ret = {};
    local done = false;
    
    self:BindRemote(_port,function(res,val)
        ret.result = res;
        ret.value = val;
        done = true;
    end);
    
    while not done and not thread:IsDead() do
        thread:Sleep(1);
    end
    
    return ret;
end
--@@End Method BindRemote_Async @@--

--@@Begin Method SendData @@--
function UdpPeerTunnelClient:SendData(_handle, _ip, _port, _data, _callback)
    local _cbid = self:AddCallback(_callback);
    local _param={
        handle = _handle,
        ip = _ip,
        port = _port,
        data = {_binary_=_data},
    };
    return self:SendRequest(_param,METHOD_PU_SENDDATA,_cbid);
end
--@@End Method SendData @@--

--@@Begin Method SendData_Async @@--
function UdpPeerTunnelClient:SendData_Async(thread, _handle, _ip, _port, _data)
    local ret = {};
    local done = false;
    
    self:SendData(_handle,_ip,_port,_data,function(res,val)
        ret.result = res;
        ret.value = val;
        done = true;
    end);
    
    while not done and not thread:IsDead() do
        thread:Sleep(1);
    end
    
    return ret;
end
--@@End Method SendData_Async @@--

--@@Begin Method CloseHandle @@--
function UdpPeerTunnelClient:CloseHandle(_handle, _callback)
    local _cbid = self:AddCallback(_callback);
    local _param={
        handle = _handle,
    };
    return self:SendRequest(_param,METHOD_PU_CLOSEHANDLE,_cbid);
end
--@@End Method CloseHandle @@--

--@@Begin Method CloseHandle_Async @@--
function UdpPeerTunnelClient:CloseHandle_Async(thread, _handle)
    local ret = {};
    local done = false;
    
    self:CloseHandle(_handle,function(res,val)
        ret.result = res;
        ret.value = val;
        done = true;
    end);
    
    while not done and not thread:IsDead() do
        thread:Sleep(1);
    end
    
    return ret;
end
--@@End Method CloseHandle_Async @@--

--@@Begin Method OnSendData @@--
function UdpPeerTunnelClient:OnSendData(_context,_param)
    local dest_ip = self.local_dest_ip;
    local dest_port = self.local_dest_port;
    if dest_port == 0 then
        dest_ip = self.from_ip;
        dest_port = self.from_port;
    end

    if not dest_ip or not dest_port then
        local _ret={
            ws = -1
        };
        self:SendReturnValue(_context,_ret);
    else
        local data = _param.data._binary_;
        self.total_recv = self.total_recv + data:GetSize();

        local ws = self.local_connection:SendLocalData(dest_ip,dest_port,data);

        local _ret={
            ws = ws
        };
        self:SendReturnValue(_context,_ret);
    end
end
--@@End Method OnSendData @@--

--@@ Insert Method Here @@--
function UdpPeerTunnelClient:CreateLocalConnection( local_dest_ip,local_bind_port,local_dest_port, 
                                                    remote_dest_ip,remote_bind_port,remote_dest_port)
    self.local_connection = LocalUdpConnection.new(self);    
    self.local_connection:StartClientForwarding(
        local_dest_ip,local_bind_port,local_dest_port, 
        remote_dest_ip,remote_bind_port,remote_dest_port
    );
    self.local_dest_port = local_dest_port;
    self.local_dest_ip = local_dest_ip;
end

