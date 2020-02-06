require("common")
require("callback_map")

local TYPE_UNKNOWN = 0;
local TYPE_BSON = 1;
local TYPE_STRING = 2;

local REQUEST =  0x01;
local RESPONSE  = 0x02;
local SINGAL = 0x03;
local PART_RESPONSE = 0x04;

WebSocketServiceBase = class();

function WebSocketServiceBase:ctor()
    self.m_ws_server = WebSocketServer.new();
    self.m_callback_map = CallbackMap.new();
    self.m_callback_map:StartAutoClear();
end

function WebSocketServiceBase:SendRequest(obj, method,callback)
    local bson;
    if type(obj) == "table" then
        bson = ObjectToBson(obj);
    end
    
    local msg = WebSocketMessage.new();	
	msg:SetMethod(method);
	msg:SetCallbackId(callback);
	msg:SetMessageType(REQUEST);
	msg:SetDataType(TYPE_BSON);
    
    if bson then
        msg:TransferData(bson:GetRawData());
    end
    
    self.m_ws_server:SendMessage(msg);
end

function WebSocketServiceBase:SendReturnValue(context,ret)
    local bson;
    if type(ret) == "table" then
        bson = ObjectToBson(ret);
    end

    local msg = WebSocketMessage.new();	
	msg:SetMethod(context.method);
	msg:SetCallbackId(context.callback);
	msg:SetMessageType(RESPONSE);
	msg:SetDataType(TYPE_BSON);
    
    if bson then
        msg:TransferData(bson:GetRawData());
    end
    
    self.m_ws_server:SendMessage(msg);
end

function WebSocketServiceBase:SendPartReturnValue(context,ret)
    local bson;
    if type(obj) == "table" then
        bson = ObjectToBson(obj);
    end
    
    local msg = WebSocketMessage.new();	
	msg:SetMethod(cntext.method);
	msg:SetCallbackId(context.callback);
	msg:SetMessageType(PART_RESPONSE);
	msg:SetDataType(TYPE_BSON);
    
    if bson then
        msg:TransferData(bson:GetRawData());
    end
    
    self.m_ws_server:SendMessage(msg);
end

function WebSocketServiceBase:OnNewClientArrive(tcp_socket)
    self.m_ws_server:Reset();
    self.m_ws_server:OnNewClientArrive(tcp_socket,true);    
    self.m_ws_server:SetOnMessage(function(mem)
        local msg = WebSocketMessage.new();
        msg:AttachPayload(mem); 
        
        local obj;
        if msg:GetDataType() == TYPE_BSON then
            local bson = Bson.new();
            bson:LoadBson(msg:GetData());
            obj = BsonToObject(bson);
        end

        local context = {
            method = msg:GetMethod(),
            callback = msg:GetCallbackId();
        };
        
        if msg:GetMessageType() == REQUEST then
            self:OnRequest(context,obj);
        elseif msg:GetMessageType() == RESPONSE then
            self:OnResponse(context,obj);
        elseif msg:GetMessageType() == PART_RESPONSE then
            self:OnPartResponse(context,obj);
        end
    end);
end

function WebSocketServiceBase:OnResponse(context,obj)
    self.m_callback_map:Run(context.method,E_OK,obj);
    self.m_callback_map:Delete(context.callback);
end

function WebSocketServiceBase:OnPartResponse(context,obj)
    self.m_callback_map:Run(context.method,E_OK,obj);
end

function WebSocketServiceBase:OnRequest(context,obj)
    printnl("not expected here");
end
