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
local MMAP_FILE_NAME="/tmp/my_tools_wget.bin";
local g_mmap_file = nil;
local g_mmap_stream = nil;
local g_threads = 10;

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
{"http://www.163.com","163.html"},
{"http://www.baidu.com","baidu.html"},
};
]];


local kList = "--list";
local kThreads = "--threads";
local kChild = "--child";
local kUrl = "--url";
local kSaveFile = "--save-file";


function child_mode_main(cmd)

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
    end
    return true;
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

function find_first_not_start_url(list)
    for _,item in ipairs(list) do
        if item.status == STATUS_NOT_START then
           return item;
        end
    end
end

function host_mode_main(cmd)
    if not cmd:HasKey(kList) then
        printnl("%s is missing.",kList);
        print(usage);
        return
    end

    local threads = 10;

    local list_file = cmd:GetValueByKey(kList);
    exec_file(list_file);
    if not all_urls then
        return exit("can not find all_urls, maybe lua syntax is wrong.");
    end

    for _,item in ipairs(all_urls) do
        item.status = STATUS_NOT_START;
    end

    if cmd:HasKey(kThreads) then
        threads = tonumber(cmd:GetValueByKey(kThreads));
    end

    printf("threads = %d",threads);
    if threads <= 0 or threads > 200 then
        return exit("threads must between 1-200");
    end

    g_threads = threads;
    create_shared_mmap_file(threads);

    while not is_all_complete(all_urls) do
        local slot = alloc_empty_slot();
        if slot then
            local item = find_first_not_start_url(all_urls);
            if item then
                item.status = STATUS_DOWNLOADING;
                item.slot = slot;
                printf("start download %s at slot %d",item[1],slot);
                
            end
        end

        App.Sleep(1000);
    end

    g_mmap_file:Close();
    g_mmap_file:Unlink();
end

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kList,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"url list file");
    cmd:AddKeyType(kThreads,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"max number of download process, default is 10");
    cmd:AddKeyType(kChild,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"child index.inner use only");
    cmd:LoadFromArgv(args);
    
    local child_index = 0; 
    local threads = 10;

    if cmd:HasKey(kChild) then
        child_index = tonumber(cmd:GetValueByKey(kChild));
    end
    
    if child_index > 0 then --child mode
        return child_mode_main(cmd);
    else
        return host_mode_main(cmd);
    end
end
