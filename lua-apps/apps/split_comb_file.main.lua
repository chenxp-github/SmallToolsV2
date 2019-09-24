require("common");
require("utils");

function app_short_help()
    return "split a comb file.";
end

function app_name()
    return "split_comb_file";
end

function split_three_parts(mem_file)
    mem_file:SetSplitChars(" \t\r\n");
    mem_file:Seek(0);
    
    local buf,buf_file = new_mem();
    
    mem_file:ReadString(buf_file);    
    local part1 = buf:CStr();
    
    mem_file:ReadString(buf_file);    
    local part2 = buf:CStr();
    
    mem_file:ReadString(buf_file);    
    local part3 = buf:CStr();    
    
    return part1,part2,part3;
end

function is_splitor_commnets(mem_file)
    local ret = {};

    local part1,part2,part3 = split_three_parts(mem_file);
            
    if part1 == "/*$begin" and part3 == "$*/" then
        ret.tag1 = part1;
        ret.tag2 = part3;
        ret.filename = part2;
        ret.is_begin = true;
        return ret;
    end
    
    if part1 == "/*$end" and part3 == "$*/" then
        ret.tag1 = part1;
        ret.tag2 = part3;
        ret.filename = part2;
        ret.is_end = true;                        
        return ret;
    end
end


function split_comb_file(file,out_dir)        
    local line,line_file = new_mem(32*1024);    
    local eol,eol_file = new_mem();  
    local out,out_file = new_memfile();
    local list,list_file = new_memfile();
    
    list_file:Puts("AddFiles(\".\",{"..EOL);
    
    file:Seek(0);
  
    while file:ReadLineWithEol(line_file,eol_file) do
        local splitor = is_splitor_commnets(line_file);
        if splitor then
            if splitor.is_begin then
                list_file:Puts("\""..splitor.filename.."\","..EOL);
                out_file:SetSize(0);
            end
            
            if splitor.is_end then            
                FileManager.SaveFile(out_dir.."/"..splitor.filename,out_file);
                out_file:SetSize(0);
            end
        else
            out_file:Puts(line_file);
            out_file:Puts(eol_file);
        end
    end
    list_file:Puts("});"..EOL);    
    FileManager.CreateDir(out_dir);
    list_file:WriteToFile(out_dir.."/_list_.txt");
end

function app_main(args)
    local argc = #args;
    
    if argc < 3 then
        print(args[1].." <combfile> <out>");
        return;
    end
    
    local out_dir = FileManager.ToAbsPath(args[3]);
    local combfile = new_file(args[2],"rb");
    if not combfile then
        return;
    end
    
    split_comb_file(combfile:FileBase(),out_dir);    
end

