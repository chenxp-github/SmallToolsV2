require("common")
require("peer_service_base")
require("linkrpc_tm_types")

TimeSyncServer = class(PeerServiceBase);

function TimeSyncServer:ctor()
    self.inner_tm = CommonTime.new();
end

function TimeSyncServer:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
    if method == METHOD_TM_GETHOSTTIME then
        self:OnGetHostTime(_context,_param);
    end
--##End OnRequest ##--
end

--@@Begin Method OnGetHostTime @@--
function TimeSyncServer:OnGetHostTime(_context,_param)
    self.inner_tm:SetNow();
    local _ret={
        now = {_int64_ = self.inner_tm:GetLong()}
    };
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnGetHostTime @@--

--@@ Insert Method Here @@--
