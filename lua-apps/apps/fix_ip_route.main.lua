require("common");
require("utils");

function app_short_help()
    return "fix wrong ip route.";
end

function app_name()
    return "fix_ip_route";
end

local kWant="--want"

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kWant,TYPE_KEY_EQUAL_VALUE,MUST,
        "wanted pattern you want to keep,"..EOL..
        "  e.g *via 192.168.43.* "
    );
    cmd:LoadFromArgv(args);
    
    if cmd:CheckForErrors() then
        return;
    end

    local wanted = cmd:GetValueByKey(kWant);
    print("wanted = "..wanted);

    local mf = new_memfile();
    local mem = new_mem();

    while true do        
        local str = execute_command("ip route");
        
        mf:SetSize(0);
        mf:Puts(str);
        mf:Seek(0);
        
        while mf:ReadLine(mem) do
            if CFunc.wild_match("default via*",mem:CStr()) then
                if CFunc.wild_match(wanted,mem:CStr()) then
                    break;
                else
                    local cmd = "ip route del "..mem:CStr();
                    print("exec:"..cmd)
                    os.execute(cmd);
                end
            end
        end

        App.Sleep(3000);
    end       
end
