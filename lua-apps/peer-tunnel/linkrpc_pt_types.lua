require ("peer_tunnel_server_messages")
require ("peer_tunnel_client_messages")

--override default method id
--because server and client has same method names
METHOD_PT_CONNECTREMOTE = 800001;
METHOD_PT_WRITEDATA = 800002;
METHOD_PT_CLOSECONNECTION = 800003;
