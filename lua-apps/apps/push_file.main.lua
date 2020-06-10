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
local kLocalFile = "--local";
local kRemoteFile = "--remote";
local kIsFileList = "--is-file-list"
local kStrip = "--strip";
local kPathPrefix = "--path-prefix";

function app_main(args)
    local cmd = CommandLine.new();

    cmd:AddKeyType(kPeerName,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"peer name of the server,default: simple-file-client");    
    cmd:AddKeyType(kAsServer,TYPE_KEY,OPTIONAL,"as tcp server");
    cmd:AddKeyType(kServer,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer server address,default: 127.0.0.1");
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"message peer port, default: 2016")
    cmd:AddKeyType(kLocalFile,TYPE_KEY_EQUAL_VALUE,MUST,"local file to push");
    cmd:AddKeyType(kRemoteFile,TYPE_KEY_EQUAL_VALUE,MUST,"remote file to save");
    cmd:AddKeyType(kIsFileList,TYPE_KEY,OPTIONAL,"local file is a file list actually.");
    cmd:AddKeyType(kStrip,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"path strip when use file list mode.");
    cmd:AddKeyType(kPathPrefix,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"path prefix when use file list mode.");
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

    local path_strip,path_prefix;
    if cmd:HasKey(kStrip) then
        path_strip = cmd:GetValueByKey(kStrip);
    end

    if cmd:HasKey(kPathPrefix) then
        path_prefix = cmd:GetValueByKey(kPathPrefix);
    end

    local is_file_list = cmd:HasKey(kIsFileList);

    local local_file =FileManager.ToAbsPath(cmd:GetValueByKey(kLocalFile));
    local remote_file = cmd:GetValueByKey(kRemoteFile);
    
    local local_dir = nil;
    local remote_dir = nil;

    if FileManager.IsDirExist(local_file) then
        local_dir = local_file;
        remote_dir = remote_file;
    end

    local error_occur = false;

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

        if is_file_list then
            for_each_line(local_file,function(line)  
                line:Trim(); 
                if line:C(0) == 0 then return end
                         
                local rpath = line:CStr();

                if path_strip then
                    rpath = remove_path_prefix(rpath,path_strip);
                end

                local remote_path = remote_file.."/"..rpath;

                if path_prefix then
                    rpath = path_prefix.."/"..rpath;
                end

                local local_path=FileManager.ToAbsPath(rpath);                
                if not file_client:PushBigFile(local_path,remote_path) then
                    printfnl("push file fail %s",local_path);
                    error_occur = true;
                else
                    printfnl("-->send %s ok",local_path);
                end    
            end);
        elseif not local_dir then
            if not file_client:PushBigFile(local_file,remote_file) then
                printfnl("push file fail %s",local_file);
                error_occur = true;
            else
                printfnl("-->send %s ok",local_file);
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
                if not file_client:PushBigFile(v,rfile) then
                    printfnl("push file fail %s",v);
                    error_occur = true;
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

    if error_occur then
        os.exit(-1);
    end
end



