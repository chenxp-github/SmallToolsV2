require("common");
require("utils");

local g_folder = nil;
local g_last_config = nil;
local g_changed = false;
local g_last_config_name = nil;
local g_dummy_upload = false;
local g_run_path = ".";
local g_tmp_ftp_cmd_file = "_ftp_.txt";
local g_tmp_ftp_log_file = "_ftp_log_.txt";

function app_short_help()
    return "batch upload files to server";
end

function app_name()
    return "batch_upload";
end

--default
function get_server_list()
end

--default
function upload_file(server,info)
    if server then print_table(server); end
    if info then print_table(info); end
    return true;
end

--default
function server_to_id(server)
    return server.ip;
end

function get_last_config()
    local fn = g_last_config_name;
    
    if not FileManager.IsFileExist(fn) then
        return
    end

    local r,tab = exec_file(fn);

    if not r then
        return exit("exec file fail: "..fn);
    end

    return tab;
end

function is_ftp_upload_success()
    if not FileManager.IsFileExist(g_tmp_ftp_log_file) then
        return false;
    end

    local log = new_mem(g_tmp_ftp_log_file);

    log:Seek(0);
    
    if log:SearchStr("226 Transfer complete",1,0) > 0 then
        return true;
    end

    log:Seek(0);
    if log:SearchStr("directory created",1,0)  > 0 then
        return true;
    end

    log:Seek(0);
    if log:SearchStr("Can't create directory",1,0)  > 0 then
        return true;
    end
end

function my_ftp_win_upload(server,info)
    if info.event == EVENT_END_DIR then
        return
    end

    local pbuf = PrintBuffer.new();

    pbuf:Log(
        "open %s %d",
        server.ip,server.port
    );
    pbuf:Log("cxp");
    pbuf:Log("cxp");
    pbuf:Log("binary");
    
    if info.event == EVENT_BEGIN_DIR then
        local remote_path = FileManager.ToAbsPath(
            server.root_path.."/"..info.rpath
        );        
        pbuf:Log("mkdir %s",remote_path);
    elseif info.event == EVENT_SINGLE_FILE then
        local path = FileManager.ToAbsPath(server.root_path.."/"..    
            FileManager.SliceFileName(info.rpath,FN_PATH)
        ,"");
        pbuf:Log("cd %s",path);
        pbuf:Log("put %s",info.rpath);
    end
    
    pbuf:Log("bye");
    pbuf:Log("bye");

    pbuf:SaveToFile(g_tmp_ftp_cmd_file);
    os.execute(string.format(
        "ftp -s:%s >%s 2>&1",
        g_tmp_ftp_cmd_file,
        g_tmp_ftp_log_file
    ));
    
    local success = false;

    if is_ftp_upload_success() then
        success = true;
    end

    FileManager.DeleteFile(g_tmp_ftp_cmd_file);
    FileManager.DeleteFile(g_tmp_ftp_log_file);
    return success;
end

function my_ftp_linux_upload(server,info)
    if info.event == EVENT_END_DIR then
        return
    end

    local pbuf = PrintBuffer.new();
    pbuf:SetEol("\n");

    pbuf:Log(
        "open %s %d",
        server.ip,server.port
    );

    pbuf:Log("user cxp cxp");
    pbuf:Log("binary");
    pbuf:Log("passive");
    
    if info.event == EVENT_BEGIN_DIR then        
        local remote_path = FileManager.ToAbsPath(
            server.root_path.."/"..info.rpath
        );        
        pbuf:Log("mkdir %s",remote_path);
    elseif info.event == EVENT_SINGLE_FILE then        
        pbuf:Log("cd "..server.root_path);
        pbuf:Log("put %s",info.rpath);
    end
    
    pbuf:Log("bye");

    pbuf:SaveToFile(g_tmp_ftp_cmd_file);
    os.execute(string.format(
        "ftp -vn<%s>%s",
        g_tmp_ftp_cmd_file,
        g_tmp_ftp_log_file
    ));

    local success = false;
    if is_ftp_upload_success() then
        success = true;
    end

    FileManager.DeleteFile(g_tmp_ftp_cmd_file);
    FileManager.DeleteFile(g_tmp_ftp_log_file);
    return success;
end

function my_ftp_upload(server,info)
    if App.OS() == "win32" then
        return my_ftp_win_upload(server,info);
    else
        return my_ftp_linux_upload(server,info);
    end
end

function save_config()
    if not g_changed then
        print("not changed");
        return
    end
    local fn = g_last_config_name;
    local pbuf = PrintBuffer.new();
    pbuf:Log("return {");
    pbuf:IncLogLevel(1);
    object_to_lua(g_last_config,pbuf);
    pbuf:IncLogLevel(-1);
    pbuf:Log("}");
    pbuf:SaveToFile(fn);
end

function upload_all_files_each_server(server)
    local server_id = server_to_id(server);
    local server_tab = g_last_config[server_id];

    if not server_tab then
        server_tab = {};
        g_last_config[server_id] = server_tab;        
    end

    function need_upload(info)
        if info.full_name == g_last_config_name then
            return false;
        end

        local v = server_tab[info.rpath];
        if not v then return true end

        if not info.is_dir then
            if v.last_write_time ~= info.last_write_time then
                return true;
            end

            if v.size ~= info.size then
                return true;
            end
        end
    end

    function do_upload(server,info)
        if g_dummy_upload then
            return true;
        end
        return upload_file(server,info)
    end

    FileManager.SearchDir(g_folder,true,function(info)           
        local rpath = remove_path_prefix(info.full_name,g_folder);
        info.rpath = string.gsub(rpath,"\\","/",1,true);

        if need_upload(info) then
            print(string.format(
                "start upload %s:%s",
                 server_id,rpath
            ));
            
            local old_pwd = FileManager.GetCurDir();
            FileManager.ChangeDir(g_folder);

            if do_upload(server,info) then
                print("success.");
                server_tab[info.rpath] = {
                    size = info.size,
                    last_write_time = info.last_write_time,
                };
                g_changed = true;
                save_config();
            else
                print("fail.")
            end

            FileManager.ChangeDir(old_pwd);
        end
    end);   
end

function ssh_make_dir(server,path)
    local cmd;
    if server.pass then
        cmd = "sshpass -p "..server.pass.." "..
            "ssh "..server.user.."@"..server.ip.." \""..
            "mkdir -p "..path.."\"";        
    else
        cmd = "ssh "..server.user.."@"..server.ip.." \""..
            "mkdir -p "..path.."\"";            
    end
    print(cmd);
    os.execute(cmd);
end

function scp_copy_file(server,info)
    local remote_file = FileManager.ToAbsPath(server.root_path.."/"..info.rpath);
    local local_file = info.full_name;
        
    local cmd;
    
    if server.pass then
        cmd = "sshpass -p "..server.pass.." "..
            "scp "..local_file.." "..server.user.."@"..server.ip..":"..
            remote_file;
    else
        cmd ="scp "..local_file.." "..server.user.."@"..server.ip..":"..
            remote_file;    
    end
    
    print(cmd);
    return os.execute(cmd);
end

function ssh_upload_file(server,info) 
    if info.event == EVENT_BEGIN_DIR then                
        local remote_path = FileManager.ToAbsPath(server.root_path.."/"..info.rpath);        
        ssh_make_dir(server,remote_path);
        return true;
    end
    
    if info.event == EVENT_SINGLE_FILE then
        return scp_copy_file(server,info);
    end
end

function adb_push_file(server,info)
    local device_name = server.name;
    local remote_file = FileManager.ToAbsPath(server.root_path.."/"..info.rpath);
    local local_file = info.full_name;
	local cmd =string.format(
        "adb -s %s push %s %s",
        device_name,local_file,remote_file
    );  

	print(cmd);
	return os.execute(cmd);
end

function adb_push_upload_file(server,info)
	if info.event == EVENT_BEGIN_DIR then
		return true;
	end
	
	if info.event == EVENT_SINGLE_FILE then
        return adb_push_file(server,info);
    end
end


local kLuaConfig="--lua-config";
local kFolder="--folder";
local kDummyUpload="--dummy"

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kFolder,TYPE_KEY_EQUAL_VALUE,MUST,"the source folder");
    cmd:AddKeyType(kDummyUpload,TYPE_KEY,OPTIONAL,"dummy upload, just update _last_.lua");
    cmd:AddKeyType(kLuaConfig,TYPE_KEY_EQUAL_VALUE,MUST,[[
    use a lua as config file,the following functions shoule be exist:

    function get_server_list() 
        --return server list.
    end
    
    function server_to_id(server) 
        --convert server object to a id string
    end
    
    function upload_file(server,info) 
        --will tell how to upload one file,
        --return true to indicate the uploading is succeed.
    end
    ]]);

    cmd:LoadFromArgv(args);    
    if cmd:CheckForErrors() then return end

    local lua_config = FileManager.ToAbsPath(cmd:GetValueByKey(kLuaConfig));

    if not exec_file(lua_config) then
        return exit("exec lua config file error!");
    end

    g_run_path = FileManager.ToAbsPath(".");
    g_tmp_ftp_cmd_file = FileManager.ToAbsPath(g_tmp_ftp_cmd_file);
    g_tmp_ftp_log_file = FileManager.ToAbsPath(g_tmp_ftp_log_file);

    g_folder = FileManager.ToAbsPath(cmd:GetValueByKey(kFolder));
    g_last_config_name = FileManager.ToAbsPath(g_folder.."/_last_.lua");
    g_last_config = get_last_config();
    g_dummy_upload = cmd:HasKey(kDummyUpload);

    if not g_last_config then
        g_last_config = {};
    end

    local server_list = get_server_list();
    if not server_list then
        server_list={{ip="localhost"}};
    end

    FileManager.ChangeDir(g_folder);

    for _,server in ipairs(server_list) do
        upload_all_files_each_server(server);        
    end

    save_config();
end
