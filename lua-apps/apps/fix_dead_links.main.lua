require("common");
require("utils");

local g_links_need_to_rebuild={};

function app_short_help()
    return "fix dead links of cross compile sysroot";
end

function app_name()
    return "fix_dead_links"
end

function is_file_or_dir_exist(name)
    if FileManager.IsDirExist(name) then
        return true;
    end
    
    if FileManager.IsFileExist(name) then
        return true;
    end
end

function make_dots(levels)
    
    local dots = "";
    for i=1,levels,1 do
        dots = dots.."../";
    end
    return dots
end

function levels_to_sysroot(full_name,sysroot)
    local levels = 0;
    for i=1,1000,1 do    
        if string.len(full_name) < string.len(sysroot) then
            return 0;
        end    
        if full_name == sysroot then
            return levels;
        end
        levels = levels + 1;
        full_name = FileManager.ToAbsPath(full_name.."/..");
    end    
    return levels;
end

function remove_sysroot_prefix(full_name,sysroot)
    if string.find(full_name,sysroot,1,true) ~= 1 then
        return
    end
    
    local len = string.len(sysroot);    
    local rpath = string.sub(full_name,len+2);
    return rpath;
end

function fix_dead_link(link,info,sysroot,just_wrong_link)
    local full_name = info.full_name;
    
    for i=1,1000,1 do 
        local test_name = FileManager.ToAbsPath(full_name.."/"..link);
        
        if is_file_or_dir_exist(test_name) then
            local levels = levels_to_sysroot(info.full_name,sysroot)
            local t = {
                full_name = info.full_name,
                link_name = make_dots(levels-1)..remove_sysroot_prefix(test_name,sysroot);
                is_wrong_link = just_wrong_link,
            }            
            table.insert(g_links_need_to_rebuild,t);
            return;
        end
        
        full_name = FileManager.ToAbsPath(full_name.."/..");
        if string.len(full_name) == 0 then
            break;
        end
    end
--[[    
    print("\""
        ..remove_sysroot_prefix(info.full_name,sysroot)
        .." -> "..link
        .."\" can not be fixed"
    );
--]]    
end

function rebuild_all_links(sysroot)
    for _,t in pairs(g_links_need_to_rebuild) do
    
        local path = FileManager.SliceFileName(t.full_name,FN_PATH);
        local file_name = FileManager.SliceFileName(t.full_name,FN_FILENAME);

        print(""
            ..remove_sysroot_prefix(t.full_name,sysroot)
            .." ==> "..t.link_name
        );
        
        FileManager.ChangeDir(path);
        os.execute("rm "..file_name);
        os.execute("ln -s "..t.link_name.." "..file_name);
    end
end

function app_main(args)
    local argc = #args;
    
    if argc < 3 then
        print(args[1].." <sysroot> <dir>");   
        return;
    end
    
    local sysroot = FileManager.ToAbsPath(args[2]);
    local dir = FileManager.ToAbsPath(args[3]);
    print("fix dead links of \""..dir.."\"");
    print("sysroot is \""..sysroot.."\"");
        
    FileManager.SearchDir(dir,true,function(info)
        if info.event == EVENT_SINGLE_FILE or info.event == EVENT_BEGIN_DIR then
            local link = FileManager.ReadLink(info.full_name);            
            if link then                
                if not is_file_or_dir_exist(info.full_name) then        
                    fix_dead_link(link,info,sysroot);
                elseif string.sub(link,1,1) == "/" then
                    fix_dead_link(link,info,sysroot,true);
                end
            end
        end
    end);
   
    rebuild_all_links(sysroot);
end

