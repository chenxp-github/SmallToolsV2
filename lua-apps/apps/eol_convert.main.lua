require("common");
require("utils");

function app_short_help()
    return "convert eol of file or folder.";
end

function app_name()
    return "eol_convert";
end

function eol_convert(file_name,in_file,out_file,is_window_style)
    if not in_file then return end

    if not FileManager.IsTextFile(in_file) then
        print(file_name.." is a binary file.");
        return
    end
  
    in_file:Seek(0);
    
    local tmp,tmp_file = new_mem(32*1024);    
    while in_file:ReadLine(tmp_file) do   
        out_file:Puts(tmp_file);
        if is_window_style then
            out_file:Puts("\r\n");
        else
            out_file:Puts("\n");
        end    
    end
    
    return true;
end

function convert_a_file(file_name,is_window_style)
    local _,in_file = new_mem(file_name);
    local _,out_file = new_memfile(1024*1024,1024);        
    
    if not eol_convert(file_name,in_file,out_file,is_window_style) then
        return
    end
    
    if not FileManager.IsFileSame(in_file, out_file) then
        print("convert "..file_name);
        out_file:WriteToFile(file_name);
    end
end


local kFile = "--file";
local kFolder = "--folder";
local kWindowsStyle = "--windows-style";
local kExts = "--exts";

function app_main(args)
    local cmd = CommandLine.new();
    cmd:AddKeyType(kFile,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"file to convert");
    cmd:AddKeyType(kFolder,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"folder to convert");    
    cmd:AddKeyType(kWindowsStyle,TYPE_KEY,OPTIONAL,"is windows style eol");    
    cmd:AddKeyType(kExts,TYPE_KEY_EQUAL_VALUE,OPTIONAL,
        "only files with special ext will be converted"..EOL..
        "        e.g. --exts=c,cpp,js,lua"
    );

    cmd:LoadFromArgv(args);            
    
    if cmd:CheckForErrors() then
        return
    end
    
    if not cmd:HasKey(kFile) and not cmd:HasKey(kFolder) then        
        print("you must spacify at least one of "..kFile.." or "..kFolder);
        cmd:PrintHelp();
        return;
    end
   
    local file_name = nil;
    local folder_name = nil;
    
    if cmd:HasKey(kFile) then
        file_name = FileManager.ToAbsPath(cmd:GetValueByKey(kFile));
    end
    
    if cmd:HasKey(kFolder) then
        folder_name = FileManager.ToAbsPath(cmd:GetValueByKey(kFolder));
    end

    local is_window_style = false;
    if cmd:HasKey(kWindowsStyle) then
        is_window_style = true;
    end
        
    if file_name then 
        convert_a_file(file_name,is_window_style);
        return
    end
    
    local exts_table = nil;
    
    if cmd:HasKey(kExts) then
        exts_table = split_file_exts_list(cmd:GetValueByKey(kExts));
    end

    if folder_name then
         FileManager.SearchDir(folder_name,true,function(info)
            if info.event == EVENT_SINGLE_FILE then
                local ext = FileManager.SliceFileName(info.file_name,FN_EXT);

                if not exts_table then
                    convert_a_file(info.full_name,is_window_style);
                end
                
                if exts_table and exts_table[ext] then
                    convert_a_file(info.full_name,is_window_style);
                end                
            end
        end);   
    end    
end

