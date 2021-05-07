require("common");
require("utils");

function app_short_help()
    return "multi process wget.";
end

function app_name()
    return "wget";
end

-----------------------------------------------------------
-----------------------------------------------------------
local MMAP_FILE_NAME="/tmp/.my_tools_wget.bin";
local g_mmap_file = nil;
local g_mmap_stream = nil;
local g_threads = 20;
local g_retry = -1;

local FLAG_UNUSED = 0;
local FLAG_USED = 1;
local FLAG_SUCCESS = 2;
local FLAG_FAIL = 3;

local STATUS_NOT_START = 0;
local STATUS_DOWNLOADING = 1;
local STATUS_FAILED = 2;
local STATUS_SUCCESS = 3;

local usage=[[

list file is a lua table, like this:

all_urls={
{"wget http://www.baidu.com -O baidu.html"},
{"wget http://www.csdn.net -O csdn.html"},
};
]];


local kList = "--list";
local kThreads = "--threads";
local kChild = "--child";
local kCommand = "--cmd";
local kRetry = "--retry";

function child_mode_main(cmd)
    if not cmd:HasKey(kCommand) then
        printfnl("error: %s is missing.",kCommand);
        return
    end
    
    if not cmd:HasKey(kChild) then
        printfnl("error: %s is missing.",kChild);
        return
    end

    local command = cmd:GetValueByKey(kCommand);
    local slot =  tonumber(cmd:GetValueByKey(kChild));

    g_mmap_file = MMapFile.new();
    if not g_mmap_file:OpenReadWrite(MMAP_FILE_NAME) then
        return exit("open mmap file %s fail.",MMAP_FILE_NAME);
    end
    g_mmap_stream = g_mmap_file:Stream();

    local ret = os.execute(command);

    g_mmap_stream:Seek(slot);
    g_mmap_stream:PutInt8(ret and FLAG_SUCCESS or FLAG_FAIL);
end

function create_shared_mmap_file(bytes)
    FileManager.DeleteFile(MMAP_FILE_NAME); --delete first
    g_mmap_file = MMapFile.new();
    if not g_mmap_file:OpenCreate(MMAP_FILE_NAME,bytes) then
        return exit("create mmap file %s fail.",MMAP_FILE_NAME);
    end
    g_mmap_stream = g_mmap_file:Stream();
    g_mmap_stream:Zero();
end

function is_all_complete(list)
    for _,item in ipairs(list) do
        if item.status == STATUS_NOT_START then
            return false;
        end
        if item.status == STATUS_DOWNLOADING then
            return false;
        end
        if item.status == STATUS_FAILED then
            if g_retry <= 0 then
                return false;
            elseif item.retries < g_retry then
                return false;
            end
        end
    end
    return true;
end

function update_progress(list)
    for i=0,g_threads-1,1 do
        local flag = g_mmap_stream:CharAt(i);
        local item = find_url_by_slot(list,i);
        if item then
            if flag == FLAG_SUCCESS then
                printf("slot %d download success, cmd=%s",i,item[1]);                
                item.status = STATUS_SUCCESS;
                item.slot = -1;
                g_mmap_stream:SetChar(i,FLAG_UNUSED);
            elseif flag == FLAG_FAIL then
                item.status = STATUS_FAILED;
                item.slot = -1;
                printf("slot %d fail to download, cmd=%s",i,item[1]);                
                g_mmap_stream:SetChar(i,FLAG_UNUSED);
            end            
        end
    end
end

function alloc_empty_slot()
    for i=0,g_threads-1,1 do
        local flags = g_mmap_stream:CharAt(i);
        if flags == FLAG_UNUSED then
            g_mmap_stream:SetChar(i,FLAG_USED);
            return i;
        end
    end
end

function find_first_pending_url(list)
    for _,item in ipairs(list) do
        if item.status == STATUS_NOT_START then
           return item;
        end

        if item.status == STATUS_FAILED then
            item.retries = item.retries+1;
            if g_retry > 0 and item.retries > g_retry then
                return
            end
            return item;
        end
    end
end

function find_url_by_slot(list, slot)
    for _,item in ipairs(list) do
        if item.slot == slot then
           return item;
        end
    end
end

function run_child_process(item,slot)
    local command = item[1];
    printf("start '%s' at slot %d",command,slot);

    local child_cmd = string.format(
        "wget --child=%d --cmd=\"%s\" &",
        slot,command
    );
    local args = App.GetArgs();
	return os.execute(args[1].." "..args[2].." "..child_cmd);
end

function host_mode_main(cmd)
    if not cmd:HasKey(kList) then
        printfnl("error: %s is missing.",kList);
        printnl("usage:");
        cmd:PrintHelp();
        print(usage);
        return
    end

    local list_file = FileManager.ToAbsPath(cmd:GetValueByKey(kList));
    exec_file(list_file);
    if not all_urls then
        return exit("can not find all_urls, maybe lua syntax is wrong.");
    end

    for _,item in ipairs(all_urls) do
        item.status = STATUS_NOT_START;
        item.retries = 0;
    end

    if cmd:HasKey(kThreads) then
        g_threads = tonumber(cmd:GetValueByKey(kThreads));
    end

    if cmd:HasKey(kRetry) then
        g_retry = tonumber(cmd:GetValueByKey(kRetry));
    end
    
    printf("threads = %d",g_threads);
    if g_threads <= 0 or g_threads > 200 then
        return exit("threads must between 1-200");
    end

    create_shared_mmap_file(g_threads);

    while not is_all_complete(all_urls) do
        local find = false;

        local slot = alloc_empty_slot();
        if slot then
            local item = find_first_pending_url(all_urls);
            if item then
                item.status = STATUS_DOWNLOADING;
                item.slot = slot;
                run_child_process(item,slot);
                find = true;
            end
        end

        if not find then
            update_progress(all_urls);
            App.Sleep(1000);
        end
    end

    g_mmap_file:Close();
    g_mmap_file:Unlink();
end

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kList,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"url list file");
    cmd:AddKeyType(kThreads,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"max number of download process, default is 10");
    cmd:AddKeyType(kChild,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"child index, child process only");
    cmd:AddKeyType(kCommand,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"command to run, child process only");
    cmd:AddKeyType(kRetry,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"maxmium retries, default is -1");
    cmd:LoadFromArgv(args);
    
    local child_index = -1; 
    local g_threads = 10;

    if cmd:HasKey(kChild) then
        child_index = tonumber(cmd:GetValueByKey(kChild));
    end
    
    if child_index >= 0 then --child mode
        return child_mode_main(cmd);
    else
        return host_mode_main(cmd);
    end
end
