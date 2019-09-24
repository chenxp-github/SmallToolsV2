require("common");
require("utils");

function app_short_help()
    return "pkg-config wrapper, can set user sysroot";
end

function app_name()
    return "pkgconfig"
end

local kSysRoot="--sysroot";
local kInclude="-I"
local kLib = "-L";
local kIncludeSystem= "-isystem";

function convert_params(cmdline,key,sysroot)
    local l = cmdline:GetCmdEntriesLen();    
    for i=0,l-1,1 do
        local entry = cmdline:GetCmdEntry(i);        
        if entry:GetKey() == key then
            local value = entry:GetValue();
            entry:SetValue(sysroot..value);
        end
    end
end

function parse_pkgconfig_result(sysroot,result)
    if not result then return end
    
    if sysroot=="" then 
        print(result) 
        return
    end
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kInclude,TYPE_KEY_VALUE,OPTIONAL,"include");
    cmd:AddKeyType(kIncludeSystem,TYPE_KEY_VALUE,OPTIONAL,"include system");
    cmd:AddKeyType(kLib,TYPE_KEY_VALUE,OPTIONAL,"lib");
        
    cmd:LoadFromString(result);
    
    convert_params(cmd,kInclude,sysroot);
    convert_params(cmd,kIncludeSystem,sysroot);
    convert_params(cmd,kLib,sysroot);
    print(cmd:SaveToCmdLine());
end

function app_main(args)
    local cmd = CommandLine.new();
    cmd:AddKeyType(kSysRoot,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"sysroot");
    cmd:LoadFromArgv(args);        
    
    local sysroot = "";
    
    if cmd:HasKey(kSysRoot) then
        sysroot = cmd:GetValueByKey(kSysRoot);
    end
    
    cmd:DelByIndex(0);
    cmd:DelByKey(kSysRoot);
    
    local ori_cmd = "pkg-config "..cmd:SaveToCmdLine();

    local file = io.popen(ori_cmd);
    if not file then
        print("run command "..ori_cmd.." fail.");
        exit(-1);
    end
    
    local ret = file:read("*all");
    file:close();    
    
    parse_pkgconfig_result(sysroot,ret);
end

