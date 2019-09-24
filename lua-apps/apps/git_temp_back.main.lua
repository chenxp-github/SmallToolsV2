require("common");
require("utils");
require("make_file_list");

function app_short_help()
    return "back git temp modified files";
end

function app_name()
    return "git_temp_back";
end


function split_git_files()
    local ret = execute_command("git status");
    if not ret then return end
    
    local _,mf_file = new_memfile();
    mf_file:Puts(ret);
    
    
    mf_file:Seek(0);
    
    local mem,mem_file = new_mem();
    local buf,buf_file = new_mem();
    
    while mf_file:ReadLine(mem_file) do
        if mem:C(0) == 9 then --tab
            mem_file:Trim();
            local pos = 0;
            
            if mem_file:StartWith("modified:",true,true) then
                pos = string.len("modified:");
            end
            
            if mem_file:StartWith("deleted:",true,true) then
                pos = string.len("deleted:");
            end
            
            if mem_file:StartWith("new file:",true,true) then
                pos = string.len("new file:");
            end
         
            if pos > 0 then
                mem_file:Seek(pos);
                mem_file:ReadLeftStr(buf_file,true);
                AddSingleFile(buf:CStr());
            else                
                AddSingleFile(mem:CStr());
            end            
        end
    end
    
    return tab;
end


function app_main(args)
    local argc = #args;    
    if argc < 2 then
        return print(args[1].." <dest>");        
    end    
    local dest = args[2];    
    split_git_files();    
    CopyFilesTo(dest,nil,"");
end
