require("common");
require("utils");

function app_short_help()
    return "compare two folder snapshots.";
end

function app_name()
    return "comp_folder_snapshot";
end

function read_single_file(bson)
    local doc = bson:GetDocument("f");
    
    doc:ResetPointer();
    
    local name = doc:GetString("n");
    if not name then 
        return exit("no name entry found.");
    end
    
    local size = doc:GetInt64("s");
    if not size then
        return exit("no size entry found");
    end
    
    local tm = doc:GetInt64("t");
    if not tm then
        return exit("not time entry found");
    end

    local md5 = doc:GetString("m");

    return {
        name=name,
        size=size,
        last_write_time = tm,
        md5 = md5,
    };
end

function read_begin_folder(bson)
    local doc = bson:GetDocument("b");    
    doc:ResetPointer();
    local name = doc:GetString("n");
    if not name then 
        return exit("no name entry found.");
    end
    return {name=name};
end

function read_end_folder(bson)
    local doc = bson:GetDocument("e");    
    doc:ResetPointer();
    local name = doc:GetString("n");
    if not name then 
        return exit("no name entry found.");
    end
    return {name=name};
end

local g_path={};

function path_to_string(path)
    local str="";
    for i,v in ipairs(path) do
        if i == 1 then
            str = v;
        else
            str = str.."/"..v;
        end
    end
    return str;
end

function add_path_to_filename(filename)
    local str = path_to_string(g_path);
    if str == "" then
        return filename;
    else
        return str.."/"..filename;
    end
end

function snapshot_to_tables(snapshot,no_exit)
    local files_list = {};  
    local folders_list = {};
    
    local bson = Bson.new();
    if not bson:LoadBson(snapshot) then
        print("load snapshot file"..snapshot.." fail.");                
        if no_exit then  goto out end
        return exit(-1);
    end    
    bson:ResetPointer();

    while not bson:IsEnd() do
        local type_,name = bson:PeekNext();        
        if name == "f" then
            local info = read_single_file(bson);            
            info.name = add_path_to_filename(info.name);
            files_list[info.name] = info;
        elseif name == "b" then
            local info = read_begin_folder(bson);
            table.insert(g_path,info.name);            
            folders_list[path_to_string(g_path)] = info;            
        elseif name == "e" then
            local info = read_end_folder(bson);
            table.remove(g_path);    
        else
            return exit("unknown entry "..name);
        end
    end

::out::    
    return {
        files_list = files_list,
        folders_list = folders_list
    };
end

function comp_folders(left_tables,right_tables,comp_time,comp_md5)
    local files_not_match={};
    local files_only_in_left={};
    local files_only_in_right={};
    local folders_only_in_left={};
    local folders_only_in_right={};
    
    for left_fn,left_info in pairs(left_tables.files_list) do        
        local right_info = right_tables.files_list[left_fn];       
        if right_info then
            if left_info.size ~= right_info.size then
                table.insert(files_not_match,left_fn);
            elseif comp_time and left_info.last_write_time ~= right_info.last_write_time then
                table.insert(files_not_match,left_fn);
            elseif comp_md5 and left_info.md5 ~= right_info.md5 then
                table.insert(files_not_match,left_fn);
            end
        else
            table.insert(files_only_in_left,left_fn);
        end        
    end
    
    for right_fn,right_info in pairs(right_tables.files_list) do        
        local left_info = left_tables.files_list[right_fn];       
        if not left_info then
            table.insert(files_only_in_right,right_fn);
        end    
    end
    
    
    for left_fn,left_info in pairs(left_tables.folders_list) do        
        local right_info = right_tables.folders_list[left_fn];       
        if not right_info then
            table.insert(folders_only_in_left,left_fn);
        end        
    end

    for right_fn,right_info in pairs(right_tables.folders_list) do        
        local left_info = left_tables.folders_list[right_fn];       
        if not left_info then
            table.insert(folders_only_in_right,right_fn);
        end        
    end
    
    return {
        files_not_match=files_not_match,
        files_only_in_left=files_only_in_left,
        files_only_in_right=files_only_in_right,
        folders_only_in_left=folders_only_in_left,
        folders_only_in_right=folders_only_in_right,
    };
end

local INDEX_FILES_NOT_MATCH = 1;
local INDEX_FILES_ONLY_IN_LEFT = 2;
local INDEX_FILES_ONLY_IN_RIGHT = 3;
local INDEX_FOLDERS_ONLY_IN_LEFT = 4;
local INDEX_FOLDERS_ONLY_IN_RIGHT = 5;

local g_output_option = nil;

function is_switch_on(index)
    if not g_output_option then
        return true;
    end
    
    local ch = string.char(
        string.byte(g_output_option,index)
    );

    return ch == "1"
end


function report_header(file,name)
    if g_output_option then return end
    if file:GetSize() > 0 then file:Puts(EOL) end
    file:Puts("**************************************************"..EOL);
    file:Puts("** "..name..EOL);
    file:Puts("**************************************************"..EOL);
end

function report_final_result(result,out_fn)
    local _,out = new_memfile();
    
    if #result.files_not_match > 0 and is_switch_on(INDEX_FILES_NOT_MATCH) then    
        report_header(out,"files not match"); 
        for _,fn in ipairs(result.files_not_match) do
            out:Puts(fn..EOL);
        end    
    end
    
    if #result.files_only_in_left > 0 and is_switch_on(INDEX_FILES_ONLY_IN_LEFT) then       
        report_header(out,"files only in left side"); 
        for _,fn in ipairs(result.files_only_in_left) do
            out:Puts(fn..EOL);
        end    
    end    

    if #result.folders_only_in_left > 0 and is_switch_on(INDEX_FOLDERS_ONLY_IN_LEFT) then    
        report_header(out,"folders only in left side"); 
        for _,fn in ipairs(result.folders_only_in_left) do
            out:Puts(fn..EOL);
        end    
    end
    
    if #result.folders_only_in_right > 0 and is_switch_on(INDEX_FOLDERS_ONLY_IN_RIGHT) then    
        report_header(out,"folders only in right side"); 
        for _,fn in ipairs(result.folders_only_in_right) do
            out:Puts(fn..EOL);
        end    
    end
    
    if #result.files_only_in_right > 0 and is_switch_on(INDEX_FILES_ONLY_IN_RIGHT) then       
        report_header(out,"files only in right side"); 
        for _,fn in ipairs(result.files_only_in_right) do
            out:Puts(fn..EOL);
        end    
    end    
    
    print("save report to "..out_fn);
    out:WriteToFile(out_fn);    
end

local kLeft = "--left";
local kRight = "--right";
local kOut="--out";
local kCompTime = "--comp-time"
local kCompMd5 = "--comp-md5"
local kReportOpt = "--opt"

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kLeft,TYPE_KEY_EQUAL_VALUE,MUST,"left snapshot file.");
    cmd:AddKeyType(kRight,TYPE_KEY_EQUAL_VALUE,MUST,"right snapshot file.");
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"report output file.");
    cmd:AddKeyType(kCompTime,TYPE_KEY,OPTIONAL,"compare last write time.");
    cmd:AddKeyType(kCompMd5,TYPE_KEY,OPTIONAL,"compare md5 of file.");
    cmd:AddKeyType(kReportOpt,TYPE_KEY_EQUAL_VALUE,OPTIONAL,
        [[report format (a string, each char represent a output switch):
      1 -- files not match.
      2 -- files only in left side
      3 -- files only in right side
      4 -- folders only in left side
      5 -- folders only in right side
    ie. "101" tells us "files not match" and "files only in right side"
    Will appear in the results.
    "0101" tells us "files only in left side" and "folders only in left side"
    Will appear in the results.
        ]]
    );
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then return end
    
    local left_snapshot = FileManager.ToAbsPath(cmd:GetValueByKey(kLeft));
    local right_snapshot = FileManager.ToAbsPath(cmd:GetValueByKey(kRight));
    local output = FileManager.ToAbsPath(cmd:GetValueByKey(kOut));   
    local comp_time = cmd:HasKey(kCompTime);
    local comp_md5 = cmd:HasKey(kCompMd5);

    if cmd:HasKey(kReportOpt) then
        g_output_option = cmd:GetValueByKey(kReportOpt);
    end

    local left_tables = snapshot_to_tables(left_snapshot); 
    local right_tables = snapshot_to_tables(right_snapshot); 
    
    local result = comp_folders(left_tables,right_tables,comp_time,comp_md5);
    report_final_result(result,output);        
end
