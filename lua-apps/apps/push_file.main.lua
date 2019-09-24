require("common");
require("utils");
require("simple_file_client")

function app_short_help()
    return "push file to simple file server";
end

function app_name()
    return "push_file";
end

local kPeerName = "--peer-name";
local kAsServer = "--as-server";
local kServer = "--server";
local kPort = "--port";
local kLocalFile="--local";
local kRemoteFile="--remote";

function app_main(args)
    local cmd = CommandLine.new();

    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"peer name of the server,default: simple-file-client");
    
    cmd:AddKeyType(kAsServer,TYPE_KEY,OPTIONAL,"as tcp server");
    cmd:AddKeyType(kServer,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer server address,default: 127.0.0.1");
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer port, default: 2016")

    cmd:AddKeyType(kLocalFile,TYPE_KEY_EQUAL_VALUE,MUST,"local file to push");
    cmd:AddKeyType(kRemoteFile,TYPE_KEY_EQUAL_VALUE,MUST,"remote file to save");
    cmd:LoadFromArgv(args);            
    
    if cmd:CheckForErrors() then
        return
    end

    local as_tcp_server = cmd:HasKey(kAsServer);
    local peer_name = "simple-file-client";

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

    if as_tcp_server  then
        App.StartMessageCenter(port);
        printfnl("start message center on port %d",port);
    end

    local local_file =FileManager.ToAbsPath(cmd:GetValueByKey(kLocalFile));
    local remote_file = cmd:GetValueByKey(kRemoteFile);
    
    local local_dir = nil;
    local remote_dir = nil;

    if FileManager.IsDirExist(local_file) then
        local_dir = local_file;
        remote_dir = remote_file;
    end

    function main_thread(thread)
        local file_client = SimpleFileClient.new(thread);
        if as_tcp_server then
            file_client:InitServerSidePeer();        
        else
            file_client:InitClientSidePeer(server,port);
        end

        file_client:SetName(peer_name);
        file_client:SetDestPeerName("simple-file-server");
        file_client:Start();

        if not local_dir then
            if not file_client:PushBigFile(local_file,remote_file) then
                printfnl("push file fail %s",local_file);
            else
                printfnl("send %s ok",local_file);
            end
        else
            local file_list = {};
            FileManager.SearchDir(local_dir,true,function(info)
                if info.event == EVENT_SINGLE_FILE then
                    table.insert(file_list,info.full_name);
                end
            end)
            
            for _,v in ipairs(file_list) do
                local rfile = remote_dir.."/"..remove_path_prefix(v,local_dir);
                if not file_client:PushBigFile(v,rfile,true) then
                    printfnl("push file fail %s",v);
                else
                    printfnl("send %s ok",v);                    
                end
            end
        end

        while file_client:GetSendingQueueLength() > 0 do            
            thread:Sleep(1);
        end

        print("all done");
        App.QuitMainLoop();
    end
   
    co = CoThread.new(1);
    co:Start(main_thread);
    App.MainLoop();
end



