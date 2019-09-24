require("common");
require("utils");

local g_list_file_name = nil;

function app_short_help()
    return "fast change directory.";
end

function app_name()
    return "fast_change_dir";
end

function print_help()
    print(app_name().." <listfile> <cmd> [...]");
    print("commands are:");
    print("    clear_all: clear all items.");
    print("    add: add current directory.");
    print("    list: list all items.");
    print("    del <n> : delete the item at index.");
    print("    get <n> : get the item at index.");
    print("    refresh: auto delete items which not exist.");
    print("    sort: sort all items alphabetically.");
end

function get_all_items()
    local array = {};
    split_file_lines(g_list_file_name,function(line)
        table.insert(array,line);
    end);
    return array;
end

function save_all_items(all)
    local _,mf = new_memfile();
    for k,v in ipairs(all) do
        mf:Puts(v);
        mf:Puts(EOL);
    end
    if mf:WriteToFile(g_list_file_name) > 0 then
        print("save to "..g_list_file_name);
    else
        if #all > 0 then
            print("fail to save "..g_list_file_name);
        end
    end
end

function is_item_exist(all,item)
    for k,v in ipairs(all) do
        if v == item then
            return true;
        end
    end
end

function cmd_list()
    local all = get_all_items();
    for k,v in ipairs(all) do
        print(k..": "..v);
    end
end

function cmd_add()
    local cur_dir = FileManager.GetCurDir();
    local all = get_all_items();
    if is_item_exist(all,cur_dir) then
        return
    end
    table.insert(all,cur_dir);
    save_all_items(all);
end

function cmd_clear_all()
    local all={};
    save_all_items(all);
end

function cmd_get(arg_n)
    if not arg_n then
        return
    end

    local n = tonumber(arg_n);

    local all = get_all_items();

    if all[n] then
        print(all[n]);
    end
end

function cmd_del(args)
    if not args[4] then
        return
    end

    local n = tonumber(args[4]);

    local all = get_all_items();

    if all[n] then
        table.remove(all,n);
        save_all_items(all);
    end
end

function cmd_refresh()
    local all = get_all_items();
    local len = #all;
    for i=1,len,1 do
        if all[i] then
            if not FileManager.IsDirExist(all[i]) then
                table.remove(all,i);
            end
        end
    end
    save_all_items(all);
end

function cmd_sort()
    local all = get_all_items();
    table.sort(all,function(e1,e2)
        return string.lower(e1) < string.lower(e2);
    end);
    save_all_items(all);
end

function app_main(args)
    local argc = #args;

    if argc <= 1 then
        print_help();
        return;
    end

    local file_name = FileManager.ToAbsPath(args[2]);

    if not FileManager.IsFileExist(file_name) then
        print("create new file "..file_name);
        local file = new_file(file_name,"w+");
        if file then file:Destroy() end
    end

    g_list_file_name = file_name;

    local cmd = "list";
    if args[3] then  cmd = args[3] end

    if cmd == "list" then  return cmd_list(args) end
    if cmd == "add" then return cmd_add(args) end
    if cmd == "clear_all" then return cmd_clear_all(args) end
    if cmd == "get" then return cmd_get(args[4]) end
    if cmd == "del" then return cmd_del(args) end
    if cmd == "refresh" then return cmd_refresh(args) end
    if cmd == "sort" then return cmd_sort(args) end

    return cmd_get(cmd);
end
