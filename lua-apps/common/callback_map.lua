require("common");
require("cothread");
require("bson");

CallbackMap = class();

function CallbackMap:ctor()
    self.m_map = {};
    self.m_alloc_id = 0;
    self.m_auto_clear_thread = nil;
    self:StartAutoClear();
    self.m_callback_count = 0;
end

function CallbackMap:alloc_id()
    self.m_alloc_id = self.m_alloc_id + 1;
    return self.m_alloc_id;
end

function CallbackMap:Put(func_,timeout_)
    if type(func_) ~= "function" then
        return;
    end
    
    if timeout_ == nil then
        timeout_ = 10*1000;
    end
    
    local id_ = self:alloc_id();
    local item = {
        id = id_,
        func = func_,
        life_time=0,
        timeout=timeout_,
        flags=0,
    };
    self.m_map[id_] = item;
    self.m_auto_clear_thread:Resume();
    self.m_callback_count = self.m_callback_count + 1;
    return id_;
end

function CallbackMap:Get(id)
    return self.m_map[id];
end

function CallbackMap:Delete(id)
    if self.m_map[id] then
        self.m_callback_count = self.m_callback_count - 1;
    end
    self.m_map[id] = nil;
end

function CallbackMap:Run(id,...)
    local item = self:Get(id);
    if not item then return end
    item.func(...);
    item.life_time = 0;
end

local function _callbackmap_auto_clear_thread(thread,self)
    local interval = 3000;
    while true do
        local len = 0;
        for _,item in pairs(self.m_map) do
            len = len + 1;
            item.life_time = item.life_time + interval; 
            if item.life_time > item.timeout then
                if item.timeout > 0 then
                    self:Run(item.id,E_TIMEOUT);
                    self:Delete(item.id);
                end
            end 
        end     
        if len == 0 then 
            thread:Suspend(true);
        end
        thread:Sleep(interval);     
    end
end

function CallbackMap:StartAutoClear()
    if self.m_auto_clear_thread then
        return;
    end
    self.m_auto_clear_thread = CoThread.new();
    self.m_auto_clear_thread:Start(_callbackmap_auto_clear_thread,self);
end

function CallbackMap:GetCallbackCount()
    return self.m_callback_count;
end

function CallbackMap:SetPeer(id,peer_service)
    local item = self:Get(id);
    if not item then return end
    item.peer_service = peer_service;
end

function CallbackMap:CancelAllWhenPeerDisconnected(peer_service)
    while true do
        local old_count = self.m_callback_count;
        for _,item in pairs(self.m_map) do
            if item.peer_service == peer_service then
                self:Run(item.id,E_INNER_ERROR);
                self:Delete(item.id);                
            end
        end     
        local new_count = self.m_callback_count;
        --if not any deleted
        if old_count == new_count then
            break;
        end
    end
end
