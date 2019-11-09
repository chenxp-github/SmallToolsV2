require("common");
require("utils");
require("simple_file_client")

function app_short_help()
    return "pull file from simple file server";
end

function app_name()
    return "pull_file";
end

function parallel_pull_files(thread,file_client,all_pull_files)
    local pending = 0;

    while #all_pull_files > 0 do
        if pending < 20 then
            local head = all_pull_files[1];
            table.remove(all_pull_files,1);
            pending = pending + 1;
                        
            file_client:PullFile(head.remote_file,head.local_file,function(error)
                if error then
                    printfnl("~~ pull file %s fail.~~",head.remote_file);
                else
                    printfnl("pull file %s ok.",head.remote_file);
                end                
                pending = pending - 1;
            end);
        else
            thread:Sleep(1);
        end
    end

    while pending > 0 and not file_client:IsClosedPermanently() do
        thread:Sleep(1);
    end
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

        local local_dir = local_file;
        local remote_dir = nil;

        local list = file_client:List(remote_file);
        if list then
            remote_dir = remote_file;
        end

        local all_pull_files = {};

        function will_pull_file(_remote_file,_local_file)
            table.insert(all_pull_files,{
                remote_file = _remote_file,
                local_file = _local_file,
            });
        end

        function pull_folder(thread,_local_dir,_remote_dir)            
            local file_list = file_client:List(_remote_dir);
            if not file_list then
                return
            end
            for _, info in ipairs(file_list) do
                local next_file = _remote_dir .."/"..info.name;

                if info.is_dir then          
                    pull_folder(thread,_local_dir,next_file);                    
                else
                    local _local_file = FileManager.ToAbsPath(
                        _local_dir.."/"..remove_path_prefix(next_file,remote_dir)
                    );
                    will_pull_file(next_file,_local_file);
                end
            end
        end

        if not remote_dir then
            will_pull_file(remote_file,local_file);
        else
            printfnl("listing folder %s",remote_dir);
            pull_folder(thread,local_dir,remote_dir);
        end
        
        printfnl("total %d files.",#all_pull_files);
        parallel_pull_files(thread,file_client,all_pull_files);
        printnl("all done");
        App.QuitMainLoop();
    end
   
    co = CoThread.new(1);
    co:Start(main_thread);
    App.MainLoop();
end



