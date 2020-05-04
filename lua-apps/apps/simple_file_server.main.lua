require("common");
require("utils");
require("simple_file_server")

function app_short_help()
    return "simple file server use message peer.";
end

function app_name()
    return "simple_file_server";
end

local kPeerName = "--peer-name";
local kRoot = "--root";
local kAsClient = "--as-client";
local kServer = "--server";
local kPort = "--port";

function app_main(args)
    local cmd = CommandLine.new();

    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"peer name of the server,default: simple-file-server");
    cmd:AddKeyType(kRoot,TYPE_KEY_EQUAL_VALUE,MUST,"root dir of the file server");    
    cmd:AddKeyType(kAsClient,TYPE_KEY,OPTIONAL,"serve as tcp client");
    cmd:AddKeyType(kServer,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer server address,default: 127.0.0.1");
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer port, default: 2016")

    cmd:LoadFromArgv(args);            
    
    if cmd:CheckForErrors() then
        return
    end

    local root_dir = FileManager.ToAbsPath(cmd:GetValueByKey(kRoot));    
    if root_dir == "" then root_dir="/" end
    if not FileManager.IsDirExist(root_dir) then
        printfnl("root path %s is not exist.",root_dir);
    end
    
    if is_windows() then
        root_dir = root_dir.."\\";
    end

    FileManager.ChangeDir(root_dir);

    printfnl("root dir is: %s",root_dir);

    local as_tcp_client = cmd:HasKey(kAsClient);
    local peer_name = "simple-file-server";

    if cmd:HasKey(kPeerName) then
        peer_name = cmd:GetValueByKey(kPeerName);
    end

    local server = "127.0.0.1";
    local port = 2016;

    if cmd:HasKey(kServer) then
        server = cmd:GetValueByKey(kServer);
    end

    if cmd:HasKey(kPort) then
        port = tonumber(cmd:GetValueByKey(kPort));
    end

    if not as_tcp_client then
        App.StartMessageCenter(port,true);
        printfnl("start message center on port %d",port);
    end

    local file_server = SimpleFileServer.new();
    if as_tcp_client then
        file_server:InitClientSidePeer(server,port);
    else
        file_server:InitServerSidePeer();
    end

    file_server:SetName(peer_name);
    file_server:SetRootDir(root_dir);
    file_server:Start();
   
    App.MainLoop();
end


