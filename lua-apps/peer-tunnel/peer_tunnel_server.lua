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
    if method == METHOD_PT_WRITEDATA then
        self:OnWriteData(_context,_param);
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
                connection:StartForwarding();
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

--@@Begin Method OnWriteData @@--
function PeerTunnelServer:OnWriteData(_context,_param)
    local handle = _param.handle;
    local data = _param.data._binary_;

    local ws = -1;
    local local_connection = self:GetLocalConnection(handle);
    if local_connection then
        ws = local_connection:Write(data,data:GetSize());
    end

    local _ret={
        ws = ws
    };
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnWriteData @@--


--@@Begin Method WriteData @@--
function PeerTunnelServer:WriteData(_handle, _data, _callback)
    local _cbid = self:AddCallback(_callback,-1);
    local _param={
        handle = _handle,
        data = {_binary_=_data},
    };
    return self:SendRequest(_param,METHOD_PT_WRITEDATA,_cbid);
end
--@@End Method WriteData @@--

--@@Begin Method WriteData_Async @@--
function PeerTunnelServer:WriteData_Async(thread, _handle, _data)
    local ret = {};
    local done = false;
    
    self:WriteData(_handle,_data,function(res,val)
        ret.result = res;
        ret.value = val;
        done = true;
    end);
    
    while not done and not thread:IsDead() do
        thread:Sleep(1);
    end
    
    return ret;
end
--@@End Method WriteData_Async @@--

--@@ Insert Method Here @@--

function PeerTunnelServer:GetLocalConnection(handle)
    return self.local_connections[handle];
end

