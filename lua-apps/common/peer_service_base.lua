require("common")
require("callback_map")
require("utils")

local BODY_TYPE_STRING = 0x01<<4;
local BODY_TYPE_BSON = 0x02<<4;
local BODY_TYPE_RAW = 0x03<<4;
local BODY_TYPE_JSON = 0x04<<4;
local MSG_TYPE_REQUEST =  0x01;
local MSG_TYPE_RESPONSE  = 0x02;
local MSG_TYPE_SIGNAL = 0x03;
local MSG_TYPE_PART_RESPONSE  = 0x04;
local BODY_TYPE_MASK = 0x000000f0;
local MSG_TYPE_MASK = 0x0000000f;

local PEER_EVENT_GOT_MESSAGE = 9000;
local PEER_EVENT_CONNECTED = 9001;
local PEER_EVENT_DISCONNECTED = 9002;
local PEER_EVENT_STOPPED= 9003;
local PEER_EVENT_CAN_FETCH_MESSAGE = 9004;

PeerServiceBase = class();

function PeerServiceBase:ctor()
    self.m_server_side_peer = nil;
    self.m_client_side_peer = nil;
    self.m_is_server_side = false;
    self.m_callback_map = CallbackMap.new();
    self.m_callback_map:StartAutoClear();
    self.m_dest_peer_name = "";
end

function PeerServiceBase:InitServerSidePeer()
    self.m_server_side_peer = ServerSidePeer.new();
    self.m_is_server_side = true;
end

function PeerServiceBase:InitClientSidePeer(server, port)
    self.m_client_side_peer = MessagePeer.new();
    self.m_client_side_peer:SetServer(server);
    self.m_client_side_peer:SetPort(port);
    self.m_is_server_side = false;
end

function PeerServiceBase:Start()
    local on_message = function(msg)       
        if msg.event == PEER_EVENT_DISCONNECTED then
            printfnl("socket of peer '%s' disconnected",self:GetName());
            self.m_callback_map:CancelAllWhenPeerDisconnected(self);
        elseif msg.event == PEER_EVENT_GOT_MESSAGE then
            self:OnMessage(msg);
        end
    end

    if self.m_is_server_side then
        self.m_server_side_peer:Start();
        self.m_server_side_peer:StartFetchMessageTask();
        self.m_server_side_peer:SetCanFetchMessage(true);
        self.m_server_side_peer:SetOnMessage(on_message);
    else
        self.m_client_side_peer:Start();
        self.m_client_side_peer:SetOnMessage(on_message);
    end    
end

function PeerServiceBase:SetName(name)
    if self.m_is_server_side then
        self.m_server_side_peer:SetName(name);
    else
        self.m_client_side_peer:SetName(name);
    end
end

function PeerServiceBase:SendMessage(msg)
    if self.m_is_server_side then
        self.m_server_side_peer:SendMessage(
            msg.to,msg.method,msg.callback_id,msg.flags,msg.body
        );
    else
        self.m_client_side_peer:SendMessage(
            msg.to,msg.method,msg.callback_id,msg.flags,msg.body
        );
    end
end

function PeerServiceBase:Connect(name)
    if self.m_is_server_side then
        self.m_server_side_peer:Connect(name);
    else
        self.m_client_side_peer:Connect(name);
    end
end

function PeerServiceBase:GetName()
    if self.m_is_server_side then
        return self.m_server_side_peer:GetName();
    else
        return self.m_client_side_peer:GetName();
    end
end

function PeerServiceBase:IsConnected()
    if self.m_is_server_side then
        return true;
    else
        return self.m_client_side_peer:IsConnected();
    end
end

function PeerServiceBase:GetSendingQueueLength()
    if self.m_is_server_side then
        return self.m_server_side_peer:GetSendingQueueLength();
    else
        return self.m_client_side_peer:GetSendingQueueLength();
    end
end

function PeerServiceBase:SetMaxRetries(max)
    if not self.m_is_server_side then
        self.m_client_side_peer:SetMaxRetries(max);
    end
end

function PeerServiceBase:IsClosedPermanently()
    if not self.m_is_server_side then
        return self.m_client_side_peer:IsClosedPermanently();
    end
end

function PeerServiceBase:GetRetries()
    if not self.m_is_server_side then
        return self.m_client_side_peer:GetRetries();
    end
end

function PeerServiceBase:SetRetries(retry)
    if not self.m_is_server_side then
        return self.m_client_side_peer:SetRetries(retry);
    end
end

function PeerServiceBase:ClearSendingQueue()
    if self.m_is_server_side then
        self.m_server_side_peer:ClearSendingQueue();
    else
        self.m_client_side_peer:ClearSendingQueue();
    end
end

function PeerServiceBase:OnMessage(msg)    
    local context = {
        from = msg.from,
        method = msg.method,
        callback_id = msg.callback_id,
    }

    if msg.msg_type == MSG_TYPE_REQUEST then
        self:OnRequest(context,BsonToObject(msg.body));
    elseif msg.msg_type == MSG_TYPE_RESPONSE then
        self:OnResponse(msg)
    elseif msg.msg_type == MSG_TYPE_PART_RESPONSE then
        self:OnPartResponse(msg)
    end
end

function PeerServiceBase:OnRequest(context,params)
    --override--
    printnl("unexpected come here");
end

function PeerServiceBase:OnResponse(msg)
    local obj = BsonToObject(msg.body);
    self.m_callback_map:Run(msg.callback_id,E_OK,obj);
    self.m_callback_map:Delete(msg.callback_id);
end

function PeerServiceBase:OnPartResponse(msg)
    local obj = BsonToObject(msg.body);
    self.m_callback_map:Run(msg.callback_id,E_PART_OK,obj);
end

function PeerServiceBase:AddCallback(func,timeout)
    local id = self.m_callback_map:Put(func,timeout);
    self.m_callback_map:SetPeer(id,self);
    return id;
end

--never timeout, but emit error when socket disconncted
function PeerServiceBase:AddPeerCallback(func)
    local id = self.m_callback_map:Put(func,-1);
    self.m_callback_map:SetPeer(id,self);
    return id;
end

function PeerServiceBase:CancelCallback(id)
    self.m_callback_map:Delete(id);
end

function PeerServiceBase:SetDestPeerName(name)
    self.m_dest_peer_name = name;
end

function PeerServiceBase:SendRequest(obj, method,callback_id)
    local bson = ObjectToBson(obj);   
    local msg={
        to = self.m_dest_peer_name,
        method = method,
        callback_id = callback_id,
        body = bson,
        flags = BODY_TYPE_BSON | MSG_TYPE_REQUEST,
    };

    self:SendMessage(msg);
end

function PeerServiceBase:SendReturnValue(context,obj)
    local bson = ObjectToBson(obj);       
    local msg={
        to = context.from,
        method = context.method,
        callback_id = context.callback_id,
        body = bson,
        flags = BODY_TYPE_BSON | MSG_TYPE_RESPONSE,
    };
    self:SendMessage(msg);
end

function PeerServiceBase:SendPartReturnValue(context,obj)
    local bson = ObjectToBson(obj);       
    local msg={
        to = context.from,
        method = context.method,
        callback_id = context.callback_id,
        body = bson,
        flags = BODY_TYPE_BSON | MSG_TYPE_PART_RESPONSE,
    };
    self:SendMessage(msg);
end

function PeerServiceBase:GetAliveClientNumber()
    if self.m_is_server_side then
        return self.m_server_side_peer:GetAliveClientNumber();
    end
    --client side do not know server status
    --so return a positive number
    return 1000000;  
end

