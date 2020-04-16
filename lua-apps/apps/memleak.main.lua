require("common");
require("utils");

function app_short_help()
    return "analyze memleak via log file.";
end

function app_name()
    return "memleak";
end

function parse_item(line)
    local item = {};

    line:Seek(0);
    local mem = new_mem();
    local str = line:NextWord();
    item.name = str;
    
    str = line:NextString();
    str = line:NextString();
    item.addr =str;

    line:ReadLeftStr(mem,true);
    item.disp = mem:CStr();

    mem:Destroy();
    return item;
end

local alloc_table = {};
local free_table = {};

function build_alloc_free_table(item)
    if item.name == "alloc" then
        if not alloc_table[item.addr] then
            alloc_table[item.addr] = 1;
        else
            alloc_table[item.addr] = alloc_table[item.addr] + 1;
        end    
    elseif item.name == "free" then
        if not free_table[item.addr] then
            free_table[item.addr] = 1;
        else
            free_table[item.addr] = free_table[item.addr] + 1;
        end
    end
end

function app_main(args)
    if #args < 2 then
        printf("%s <logfile>",args[1]);
        return
    end

    local f = new_file(args[2]);
    if not f then
        return
    end

    f:Seek(0);
    local mem = new_mem();
    mem:SetSplitChars(":= \t\r\n");

    local all_items = {};    
    local line = 0;
    while f:ReadLine(mem) do        
        local item = parse_item(mem);
        build_alloc_free_table(item);
        table.insert(all_items,item);
        line = line+1;
    end

    for k,v in ipairs(all_items) do
        if v.name == "alloc" then            
            local leak = false;

            if not free_table[v.addr] then
                leak = true;
            elseif free_table[v.addr] then
                free_table[v.addr] = free_table[v.addr] - 1;
                leak = free_table[v.addr] < 0;
            end

            if leak then
                printf("leak: addr=0x%x %s",v.addr,v.disp);
            end

        elseif v.name == "free" then
            local dbfree = false;

            if not alloc_table[v.addr] then
                dbfree = true;
            elseif alloc_table[v.addr] then
                alloc_table[v.addr] = alloc_table[v.addr] - 1;
                dbfree = alloc_table[v.addr] < 0;
            end

            if dbfree then
                printf("dbfree: addr=0x%x %s",v.addr,v.disp);
            end
        end
    end
end

