require("common")
require("peer_service_base")
require("linkrpc_pt_types")
require("local_connection")

PeerTunnelServer = class(PeerServiceBase);

function PeerTunnelServer:ctor()    
    self.local_connections = {};
    self.auto_clear_thread = nil;
    self.timeout = -1;
    self.auto_id = 0;
end

function PeerTunnelServer:AllocId()
   self.auto_id = self.auto_id + 1;
   return self.auto_id; 
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
    if method == METHOD_PT_CLOSECONNECTION then
        self:OnCloseConnection(_context,_param);
    end
--##End OnRequest ##--
end

--@@Begin Method OnConnectRemote @@--
function PeerTunnelServer:OnConnectRemote(_context,_param)
    printfnl("try to connect to %s:%d",_param.server,_param.port);
    local peer_name = _context.from;
    TcpSocket.NewTcpConnector(_param.server,_param.port,
        function(event,new_socket)
            local _ret;
            if event == EVENT_CONNECTED then                
                _ret={
                    handle = self:AllocId(),
                    errStr = "",
                };
                printfnl("new tunnel client from %s arrive %d",peer_name,_ret.handle);
                local connection = LocalConnection.new(self,new_socket,_ret.handle);
                connection.peer_name = peer_name;
                self.local_connections[_ret.handle] = connection;
                connection:StartForwarding();
                connection:SetTimeout(self.timeout);
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

    local ws = -2;
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


--@@Begin Method OnCloseConnection @@--
function PeerTunnelServer:OnCloseConnection(_context,_param)
    local con = self:GetLocalConnection(_param.handle);
    if con then 
        printfnl("close server side connetion %d.",_param.handle);
        con:Close(); 
    end
    local _ret={ret = 1};
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnCloseConnection @@--


--@@Begin Method CloseConnection @@--
function PeerTunnelServer:CloseConnection(_handle, _callback)
    local _cbid = self:AddCallback(_callback);
    local _param={
        handle = _handle,
    };
    return self:SendRequest(_param,METHOD_PT_CLOSECONNECTION,_cbid);
end
--@@End Method CloseConnection @@--


--@@Begin Method CloseConnection_Async @@--
function PeerTunnelServer:CloseConnection_Async(thread, _handle)
    local ret = {};
    local done = false;
    
    self:CloseConnection(_handle,function(res,val)
        ret.result = res;
        ret.value = val;
        done = true;
    end);
    
    while not done and not thread:IsDead() do
        thread:Sleep(1);
    end
    
    return ret;
end
--@@End Method CloseConnection_Async @@--

--@@ Insert Method Here @@--

function PeerTunnelServer:GetLocalConnection(handle)
    return self.local_connections[handle];
end

function PeerTunnelServer:StartAutoClearThread()
    if self.auto_clear_thread then
        return
    end

    function auto_clear(thread)        
        while true do
            thread:Sleep(5000);
            for _,con in pairs(self.local_connections) do
                if not con:IsConnected() then
                    self:CloseConnection(con.handle); --notity client
                    printfnl("auto delete local connection %d",con.handle);
                    self.local_connections[con.handle] = nil;
                end
            end            
        end
    end
    
    self.auto_clear_thread = CoThread.new();
    self.auto_clear_thread:Start(auto_clear);
end

function PeerTunnelServer:SetTimeout(to)
    self.timeout = to;
end