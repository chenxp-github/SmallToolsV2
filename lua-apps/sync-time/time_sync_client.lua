require("common")
require("peer_service_base")
require("linkrpc_tm_types")

TimeSyncClient = class(PeerServiceBase);

function TimeSyncClient:ctor()
end

function TimeSyncClient:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
--##End OnRequest ##--
end

--@@Begin Method GetHostTime @@--
function TimeSyncClient:GetHostTime(_callback)
    local _cbid = self:AddCallback(_callback);
    return self:SendRequest(nil,METHOD_TM_GETHOSTTIME,_cbid);
end
--@@End Method GetHostTime @@--

--@@Begin Method GetHostTime_Async @@--
function TimeSyncClient:GetHostTime_Async(thread)
    local ret = {};
    local done = false;
    
    self:GetHostTime(function(res,val)
        ret.result = res;
        ret.value = val;
        done = true;
    end);
    
    while not done and not thread:IsDead() do
        thread:Sleep(1);
    end
    
    return ret;
end
--@@End Method GetHostTime_Async @@--

--@@ Insert Method Here @@--
