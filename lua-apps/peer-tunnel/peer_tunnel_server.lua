require("common")
require("peer_service_base")
require("linkrpc_pt_types")

PeerTunnelServer = class(PeerServiceBase);

function PeerTunnelServer:ctor()    
end

function PeerTunnelServer:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
--##End OnRequest ##--
end

--@@ Insert Method Here @@--
