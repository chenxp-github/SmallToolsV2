require("common");
require("utils");
require("print_buffer");

local g_root_path = nil;

function app_short_help()
    return "combine multiple txt files to one big txt file";
end

function app_name()
    return "comb_files"
end

local all_files_index = 0;
local all_files_list={};

function AddSingleFile(fullname,mem_file)    
    all_files_index = all_files_index + 1;    
    table.insert(all_files_list,{
        index = all_files_index,
        full_name = fullname,
        mem_file = mem_file,
    });
end

function AddFiles(path,files_table)
    for k,filename in ipairs(files_table) do
        local fullname = FileManager.ToAbsPath(path.."/"..filename);
        AddSingleFile(fullname);
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
    local can_add = false;
    
    local ext = FileManager.SliceFileName(info.full_name,FN_EXT);    
    for _,v in pairs(exts) do    
        if ext == v then
            can_add = true;
            break;
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

local codegen_id = 0;
function AddCodeGen(filename)
    code = PrintBuffer.new();
    AddConfig(filename);
    if code.mf_text:GetSize() > 0 then
        AddSingleFile(
            FileManager.ToAbsPath(filename.."."..codegen_id),
            code.mf_text
        );
        codegen_id = codegen_id + 1;
    end
    code = nil;
end

function SetRootPath(root_path)
    if g_root_path then
        exit("SetRootPath can only be called once");
    end
    g_root_path = FileManager.ToAbsPath(root_path);
end

local make_begin_comments = nil;
local make_end_comments = nil;

function SetComments(_begin,_end)
    make_begin_comments = _begin;
    make_end_comments = _end;
end

function set_stock_comments_style(style)
    if style == "c" or style == "js" then
        begin_str1="/*$begin ";
        begin_str2=" $*/";
        end_str1="/*$end ";
        end_str2=" $*/";
    end
    
    if style == "cpp" then
        begin_str1="//$begin ";
        begin_str2=" $//";
        end_str1="//$end ";
        end_str2=" $//";        
    end

    if style == "lua" then
        begin_str1="--$begin ";
        begin_str2=" $--";
        end_str1="--$end ";
        end_str2=" $--";        
    end
    
    if style == "html" then
        begin_str1="<!--$begin ";
        begin_str2=" $-->";
        end_str1="<!--$end ";
        end_str2=" $-->";        
    end

    SetComments(
        function(filename)
            return begin_str1..filename..begin_str2;
        end,

        function(filename)
            return end_str1..filename..end_str2;
        end
    );
    
end

function combine_files(sorted_list)
    local mf,mf_file = new_memfile(1024*1024,1024);

    for _,v in ipairs(sorted_list) do       
        local filename = v.full_name;
        local related_filename = filename;     
        if g_root_path then
            related_filename = remove_path_prefix(filename,g_root_path);
            if not related_filename then
                exit("wrong root path:"..g_root_path);
            end
        end
        
        if make_begin_comments then
            local comments = make_begin_comments(related_filename);
            if comments then
                mf_file:Puts(comments);
                mf_file:Puts("\r\n");
            end
        end
        
        local mem_file = v.mem_file;
        if not mem_file then
            mem_file = new_mem(filename);
        end
        
        mf_file:Puts(mem_file);
        mf_file:Puts("\r\n");            
        if make_end_comments then
            local comments = make_end_comments(related_filename);
            if comments then
                mf_file:Puts(comments);
                mf_file:Puts("\r\n\r\n");
            end
        end        

    end
    
    return mf_file;
end

function print_help(args)
    print(args[1].." <list> <out> [commnets style]");
    print("available functions for list script:");
    print("  AddFiles(path,files_table) : add one or more files in path to the list");
    print("  AddFolder(folder,exts_table,[headers],[footers]) : add one folder to the list,recursively");
    print("  AddConfig(config_file) : add another config file/");
    print("  AddCodeGen(script_file) : add a codegen lua file, the script will has a global code as PrintBuffer");
    print("  SetComments(func) : set splitor comments format");
    print("  SetRootPath(root) : set root path of source files, / is default");
    print("comments styles are:");
    print("  c:  /*@begin ??? */");
    print("  js: /*@begin ??? */");
    print("  cpp: //@begin ??? //");
    print("  lua: --@begin ??? --");
    print("  html: <!--@begin ??? -->");
end

function app_main(args)
    local argc = #args;
    if argc < 3 then
        return print_help(args);        
    end
    
    local list_file = FileManager.ToAbsPath(args[2]);
    local output_file = FileManager.ToAbsPath(args[3]);
    local comments_style = args[4];
    
    if not comments_style then
        comments_style = "js";
    end
    
    local work_dir = FileManager.SliceFileName(list_file,FN_PATH);
    FileManager.ChangeDir(work_dir);
    
    set_stock_comments_style(comments_style);
    
    if not FileManager.IsFileExist(list_file) then
        exit("list file "..list_file.." not exist");
    end
    
    print("use list file: "..list_file);
    
    if not exec_file(list_file) then
        exit("load list file fail.");        
    end

    local sorted_list = all_files_list;

    -- for k,v in pairs(all_files_list) do
    --     sorted_list[v.index] = v;
    -- end

    if #sorted_list <= 0 then
        return;
    end

    local mf = combine_files(sorted_list);
    assert(mf);
    
    if mf:WriteToFile(output_file) <= 0 then
        exit("save file "..output_file.." fail");
    end

    print("save file to "..output_file.." ok");
end

