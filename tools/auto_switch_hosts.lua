require("common")

local url = "https://cdn.jsdelivr.net/gh/521xueweihan/GitHub520@main/hosts";
local tmp_hosts_file = "/tmp/tmp_hosts_github520.txt";
local etc_hosts_file = "/etc/hosts";
local key_line = "#switch-hosts-chenxp";

function switch_hosts()
    execf("rm %s",tmp_hosts_file);
    execf("wget %s -O %s",url,tmp_hosts_file);
    if not FileManager.IsFileExist(tmp_hosts_file) then
        return
    end

    local new_file = new_mem(tmp_hosts_file);
    local etc_file = new_mem(etc_hosts_file);

    local out_file = new_memfile();
    etc_file:Seek(0);
    local mem = new_mem();

    while etc_file:ReadLine(mem) do
        mem:Trim();
        if mem:CStr() == key_line then
            break;
        end
        out_file:Puts(mem);
        out_file:Puts("\n");
    end

    out_file:Puts(key_line);
    out_file:Puts("\n");
    out_file:Puts(new_file);

    out_file:Dump();

    out_file:WriteToFile(etc_hosts_file);

end

if arg[1] == "loop" then
    while true do
        switch_hosts();
        App.Sleep(600*1000);
    end
else
    switch_hosts();
end





