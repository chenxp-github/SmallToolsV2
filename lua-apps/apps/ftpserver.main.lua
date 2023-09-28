require("common");
require("utils");

function app_short_help()
    return "a simple ftp server.";
end

function app_name()
    return "ftpserver";
end

local kPort = "--port";
local kRoot = "--root";
local kFixClientIp = "--fix-client-ip";
local kFixServerIp = "--fix-server-ip";
local kUserName = "--username";
local kPassword = "--password";
local kUtf8 = "--utf8";

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kPort,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"listening port, default is 2121");
    cmd:AddKeyType(kRoot,TYPE_KEY_EQUAL_VALUE,MUST,"root path of ftpserver");
    cmd:AddKeyType(kFixClientIp,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"fix client ip address");
    cmd:AddKeyType(kFixServerIp,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"fix server ip address");
    cmd:AddKeyType(kUserName,TYPE_KEY_EQUAL_VALUE,MUST,"user name");
    cmd:AddKeyType(kPassword,TYPE_KEY_EQUAL_VALUE,MUST,"password");
    cmd:AddKeyType(kUtf8,TYPE_KEY,OPTIONAL,"use utf8 encoding");
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end
    
    local port = 2121;
    if cmd:HasKey(kPort) then
        port = tonumber(cmd:GetValueByKey(kPort));
    end

    local root_path =  FileManager.ToAbsPath(cmd:GetValueByKey(kRoot));
    printf("root is %s",root_path);
    printf("port is %d",port);

    local username = cmd:GetValueByKey(kUserName);
    local password = cmd:GetValueByKey(kPassword);
    
    server = FtpServer.new();
    server:SetAllowAnonymouse(true);
    server:AddConfig(username,password,root_path,utf8);

    if cmd:HasKey(kFixClientIp) then
        local client_ip  = cmd:GetValueByKey(kFixClientIp);
        printf("fix client ip to %s",client_ip);
        server:SetFixClientIp(client_ip);
    end

    if cmd:HasKey(kFixServerIp) then
        local server_ip  = cmd:GetValueByKey(kFixServerIp);
        printf("fix server ip to %s",server_ip);
        server:SetFixServerIp(server_ip);
    end

    if cmd:HasKey(kUtf8) then
        printfnl("use utf8 encoding.");
        local ENCODING_UTF8 = 2;
        server:SetEncoding(ENCODING_UTF8);
    end
    
    server:Start(port);

    App.MainLoop();
end
