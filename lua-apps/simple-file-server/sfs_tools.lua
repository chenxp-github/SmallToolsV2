require("common")

local g_server_ip=os.getenv("SERVER_IP");
if not g_server_ip then
    g_server_ip = "127.0.0.1"
end

local g_work_dir = os.getenv("WORK_DIR");
if not g_work_dir then
    g_work_dir = ".";
end

FileManager.ChangeDir(g_work_dir);

function set_work_dir(work_dir)
    g_work_dir = work_dir;
    FileManager.ChangeDir(work_dir);
end

function set_server_ip(ip)
    g_server_ip = ip;
end

function pull_all(folder,names)
    local name = 0;
    
	for _,v in pairs(names) do
		local remote_file = folder.."/"..v;		
		local local_file = FileManager.ToAbsPath(v);
		
		if not FileManager.IsFileExist(local_file) and
           not FileManager.IsDirExist(local_file) 
        then    
            local args = App.GetArgs();
            local exe = args[1].." "..args[2];
            local cmd = string.format("%s pull_file \"--server=%s\" \"--remote=%s\" \"--local=%s\" \"--peer-name=client-%d\"",
                exe,g_server_ip,remote_file,v,name
            );            
            print(cmd);
            os.execute(cmd);            
            name = name + 1;       
        end
	end
end
