require("common");
require("utils");

function app_short_help()
    return "convert tab to space in text file.";
end

function app_name()
    return "tab2space";
end

function tab2space(in_mem, length, tab, out_file)
    local c = 0;    
    for i=0,length-1,1 do
        local ch = in_mem:C(i);
        if ch == 0x09 then
            for j=0,tab-(c % tab)-1,1 do
                out_file:Putc(32);
                c = 0;            
            end    
        else
            out_file:Putc(ch);
            c = c + 1;
        end
    end
end

function tab2space_file(file_name,in_file,tab, out_file)
    if not in_file then return end

    if not FileManager.IsTextFile(in_file) then
        print(file_name.." is a binary file.");
        return
    end
  
    in_file:Seek(0);
    
    local tmp,tmp_file = new_mem(32*1024);
    local eol,eol_file = new_mem();
    
    while in_file:ReadLineWithEol(tmp_file,eol_file) do    
        tab2space(tmp,tmp_file:StrLen(),tab,out_file);
        out_file:Puts(eol_file);
    end
    
    return true;
end

function convert_a_file(file_name,tab)
    local _,in_file = new_mem(file_name);
    local _,out_file = new_memfile(1024*1024,1024);        
    
    if not tab2space_file(file_name,in_file,tab,out_file) then
        return
    end
    
    if not FileManager.IsFileSame(in_file, out_file) then
        print("convert "..file_name);
        out_file:WriteToFile(file_name);
    end
end

function split_exts(exts)
   local tab = {};
   for k in string.gmatch(exts, "(%a+)") do
        tab[k] = true;
   end
   return tab;
end

local kFile = "--file";
local kFolder = "--folder";
local kTab = "--tab";
local kExts = "--exts";

function app_main(args)
    local cmd = CommandLine.new();
    cmd:AddKeyType(kFile,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"file to convert");
    cmd:AddKeyType(kFolder,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"folder to convert");    
    cmd:AddKeyType(kTab,TYPE_KEY_EQUAL_VALUE,MUST,"space number for a tab character");    
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

   
    local tab = tonumber(cmd:GetValueByKey(kTab));
    
    if type(tab) ~= "number" or (tab <= 0 or tab > 64) then
        print("invalidate tab number, ",tab);
        return 
    end
    
    if file_name then 
        convert_a_file(file_name,tab);
        return
    end
    
    local exts_table = nil;
    
    if cmd:HasKey(kExts) then
        exts_table = split_exts(cmd:GetValueByKey(kExts));
    end

    if folder_name then
         FileManager.SearchDir(folder_name,true,function(info)
            if info.event == EVENT_SINGLE_FILE then
                local ext = FileManager.SliceFileName(info.file_name,FN_EXT);

                if not exts_table then
                    convert_a_file(info.full_name,tab);
                end
                
                if exts_table and exts_table[ext] then
                    convert_a_file(info.full_name,tab);
                end
                
            end
        end);   
    end    
end

