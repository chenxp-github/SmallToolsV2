require("common");
require("utils");
require("peer_service_base");

METHOD_MC_GETALLPEERNAMES = 8000001;
MessageCenterServer = class(PeerServiceBase);

function MessageCenterServer:ctor()
end

function MessageCenterServer:OnRequest(_context,_param)
--##Begin OnRequest ##--
    local method = _context.method;
    if method == METHOD_MC_GETALLPEERNAMES then
        self:OnGetAllPeerNames(_context,_param);
    end
--##End OnRequest ##--
end

--@@Begin Method OnGetAllPeerNames @@--
function MessageCenterServer:OnGetAllPeerNames(_context,_param)
    local names = App.GetAllPeerNames();    
    print_table(names);
    if not names then names = {}; end
    local _ret={
        RetVal0 = names,
    };
    self:SendReturnValue(_context,_ret);
end
--@@End Method OnGetAllPeerNames @@--
----------------------------------------------------------------
function app_short_help()
    return "a message peer server";
end

function app_name()
    return "message_center";
end

local kTrustMode = "--trust-mode";
local kPeerName = "--peer-name";
local kPort = "--port";

function app_main(args)
    local cmd = CommandLine.new();
    cmd:AddKeyType(kTrustMode,TYPE_KEY,OPTIONAL,"use trust mode");
    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"peer name of message center query API, default is 'message-center-query'");
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,MUST,"port number to listen");    
    cmd:LoadFromArgv(args);                
    if cmd:CheckForErrors() then
        return
    end
   
    local trust_mode = cmd:HasKey(kTrustMode);
    local port = tonumber(cmd:GetValueByKey(kPort));

    local peer_name="message-center-query";
    if cmd:HasKey(kPeerName) then
        peer_name = cmd:GetValueByKey(kPeerName);
    end
   
    if port <= 0 then
        return exit("invalid port number:"..port);
    end
    
    print("start message center on port "..port);
    print("trust",trust_mode);

    App.StartMessageCenter(port,trust_mode);

    g_server = MessageCenterServer.new();
    g_server:InitServerSidePeer();
    g_server:SetName(peer_name);
    g_server:Start();

    App.MainLoop();
end

