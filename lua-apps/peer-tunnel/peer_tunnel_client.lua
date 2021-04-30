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
    self.auto_clear_thread = nil;
    self.auto_id = 0;
end

function PeerTunnelClient:AllocId()
    self.auto_id = self.auto_id + 1;
    return self.auto_id; 
 end

function PeerTunnelClient:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
    if method == METHOD_PT_WRITEDATA then
        self:OnWriteData(_context,_param);
    end
    if method == METHOD_PT_CLOSECONNECTION then
        self:OnCloseConnection(_context,_param);
    end
--##End OnRequest ##--
end


--@@Begin Method ConnectRemote @@--
function PeerTunnelClient:ConnectRemote(_server, _port, _callback)
    local _cbid = self:AddCallback(_callback,15000);
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



--@@Begin Method WriteData @@--
function PeerTunnelClient:WriteData(_handle, _data, _callback)
    local _cbid = self:AddCallback(_callback,-1);
    local _param={
        handle = _handle,
        data = {_binary_=_data},
    };
    return self:SendRequest(_param,METHOD_PT_WRITEDATA,_cbid);
end
--@@End Method WriteData @@--


--@@Begin Method WriteData_Async @@--
function PeerTunnelClient:WriteData_Async(thread, _handle, _data)
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


--@@Begin Method OnWriteData @@--
function PeerTunnelClient:OnWriteData(_context,_param)
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


--@@Begin Method CloseConnection @@--
function PeerTunnelClient:CloseConnection(_handle, _callback)
    local _cbid = self:AddCallback(_callback);
    local _param={
        handle = _handle,
    };
    return self:SendRequest(_param,METHOD_PT_CLOSECONNECTION,_cbid);
end
--@@End Method CloseConnection @@--


--@@Begin Method CloseConnection_Async @@--
function PeerTunnelClient:CloseConnection_Async(thread, _handle)
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


--@@Begin Method OnCloseConnection @@--
function PeerTunnelClient:OnCloseConnection(_context,_param)
    local con = self:GetLocalConnection(_param.handle);
    if con then 
        printfnl("close client side connetion %d.",_param.handle);
        con:Close(); 
    end
    local _ret={ret = 1};
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnCloseConnection @@--

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
        local ret;
        local retry = 0;
        while new_socket:IsConnected() do
            ret = self:ConnectRemote_Async(thread,self.remote_server,self.remote_port);
            if ret.result ~= OK then
                --new_socket:Destroy();
                printfnl("call ConnectRemote_Async timeout, %s:%d, retry=%d",
                    self.remote_server,self.remote_port,retry);
            else
                if ret.value.handle <= 0 then
                    --new_socket:Destroy();
                    printfnl("connect remote rejected %s,retry=%d",ret.value.errStr,retry);
                else
                    break;
                end
            end
            retry = retry + 1;
            if retry >= 5 then
                new_socket:Destroy();
                printfnl("exceed max retries,connect remote fail.");
                return;
            end            
            thread:Sleep(2000);
        end

        printfnl("connect remote %s:%d success.", self.remote_server,self.remote_port);
        local connection = LocalConnection.new(self,new_socket,ret.value.handle);
        self.local_connections[ret.value.handle] = connection;
        connection:StartForwarding();
    end

    local co = CoThread.new();
    co:Start(connect_thread);
end

function PeerTunnelClient:GetLocalConnection(handle)
    return self.local_connections[handle];
end

function PeerTunnelClient:StartAutoClearThread()
    if self.auto_clear_thread then
        return
    end

    function auto_clear(thread)        
        while true do
            thread:Sleep(5000);
            for _,con in pairs(self.local_connections) do
                if not con:IsConnected() then
                    self:CloseConnection(con.handle); --notity server
                    printfnl("auto delete local connection %d",con.handle);
                    self.local_connections[con.handle] = nil;
                end
            end            
        end
    end
    self.auto_clear_thread = CoThread.new();
    self.auto_clear_thread:Start(auto_clear);
end
