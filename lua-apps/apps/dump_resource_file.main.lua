require("common");
require("utils");

function app_short_help()
    return "dump a resource file.";
end

function app_name()
    return "dump_resource_file";
end

function app_main(args)
    local argc = #args;
    
    if argc < 3 then
        print(args[1].." <resource_file> <out>");
        return;
    end

    local r = ResourceManager.new();

    if not r:LoadResourceFile(args[2]) then
        print("load resource file "..args[2].." error");
        return;
    end

    r:DumpResourceFile(args[3]);
end

