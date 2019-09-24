require("common");
require("utils");

g_all_files_index = 0;
g_all_files_list={};

function AddSingleFile(fullname)    
    if not g_all_files_list[fullname] then
        g_all_files_index = g_all_files_index + 1;
        g_all_files_list[fullname] = g_all_files_index;
    end
end

function AddFiles(path,files_table)
    for k,filename in ipairs(files_table) do
        local fullname = FileManager.ToAbsPath(path.."/"..filename);
        AddSingleFile(fullname);
    end
end

function RemoveFiles(path,files_table)
    for k,filename in ipairs(files_table) do
        local fullname = FileManager.ToAbsPath(path.."/"..filename);
        g_all_files_list[fullname] = nil;
    end
end

function RemoveFilesByFolder(source_prefix, folder)
    folder = FileManager.ToAbsPath(folder);
    
    if not FileManager.IsDirExist(folder) then
        return
    end

    FileManager.SearchDir(folder,true,function(info)        
        local rpath = remove_path_prefix(info.full_name,folder);
        local full_source_path = FileManager.ToAbsPath(source_prefix.."/"..rpath);        
        g_all_files_list[full_source_path] = nil;
    end);    
end

function KeepFilesByFolder(source_prefix,folder)
    folder = FileManager.ToAbsPath(folder);
    
    if not FileManager.IsDirExist(folder) then
        return
    end
    
    local tmp_list = {};
    FileManager.SearchDir(folder,true,function(info)        
        local rpath = remove_path_prefix(info.full_name,folder);
        tmp_list[rpath] = true;
    end);
    
    for filename,index in pairs(g_all_files_list) do
        
        local rpath = remove_path_prefix(filename,source_prefix);
        if not tmp_list[rpath] then
            g_all_files_list[filename] = nil;
        end        
    end
end

function is_interesting_ext(interesting_exts,ext)    
    if type(interesting_exts) == "string" then
        interesting_exts = {interesting_exts};
    end
    
    for k,v in ipairs(interesting_exts) do
        if v == ext then
            return true;
        end
    end    
end

function default_can_be_added(root,info,exts,header_table,footer_table)
    local can_add = true;

    local ext = FileManager.SliceFileName(info.full_name,FN_EXT);
    
    if exts then
        can_add = false;
        --two kinds of exts, one is array , another is hashmap
        if #exts > 0 then
            for _,v in ipairs(exts) do    
                if ext == v then
                    can_add = true;
                    break;
                end
            end
        else
            if exts[ext] then
                can_add = true;
            end
        end
    end

    if not can_add then
        return false;
    end
    
    local rpath = remove_path_prefix(info.full_name,root);
   
    if header_table then
        for _,fn in pairs(header_table) do
            if rpath == fn then
                return false;
            end
        end
    end
   
    if footer_table then
        for _,fn in pairs(footer_table) do        
            if rpath == fn then
                return false;
            end
        end
    end

    return true;
end

function AddFolder(folder,exts,param1, param2)        
    local fullname = FileManager.ToAbsPath(folder);

    if not FileManager.IsDirExist(fullname) then
        return
    end

    if type(exts) == "string" then
        exts = {exts};
    end
    
    local can_be_added = default_can_be_added;
    local header_table = nil;
    local footer_table = nil;
    
    if type(param1) == "function" then
        can_be_added = param1;
    end
    
    local header_footer_mode = false;
    
    if type(param1) == "table" then
        header_footer_mode = true;
        header_table = param1;
        footer_table = param2;        
        AddFiles(folder,header_table);
    end
        
    if not can_be_added then
        can_be_added = default_can_be_added;
    end
       
    local tmp_list = {};    
    FileManager.SearchDir(fullname,true,function(info)    
        if info.event == EVENT_SINGLE_FILE then        
            if can_be_added(fullname,info,exts,header_table,footer_table) then
                table.insert(tmp_list,info.full_name);
            end
        end
    end);
       
    table.sort(tmp_list);

    for k,v in ipairs(tmp_list) do
        AddSingleFile(v);
    end               
    
    if header_footer_mode then
        AddFiles(folder,footer_table);
    end
end

function AddConfig(config_name)
    local fullpath = FileManager.ToAbsPath(config_name);
    local path = FileManager.SliceFileName(fullpath,FN_PATH);    
    local old_cur_dir = FileManager.GetCurDir();
    FileManager.ChangeDir(path);
    if not exec_file(fullpath) then
        print("load config "..config_name.." fail");
        exit(-1);
    end
    FileManager.ChangeDir(old_cur_dir);
end

function ClearFilesList()
    g_all_files_index = 0;
    g_all_files_list={};
end

function CurrentFilesList()
    return g_all_files_list;
end

function CopyFilesTo(dest,mode,path_prefix)
    if not mode then
        mode = MODE_USE_LOG | MODE_CHECKSAME | MODE_OVERWRITE;
    end
    
    for filename in pairs(g_all_files_list) do       
        local rpath;
        
        if path_prefix then
            rpath = remove_path_prefix(filename,path_prefix);
        else
            rpath = FileManager.SliceFileName(filename,FN_FILENAME);
        end
        
        local dest_path = FileManager.ToAbsPath(dest.."/"..rpath);        
        FileManager.CopySingleFile(filename,dest_path,mode);        
    end
end
