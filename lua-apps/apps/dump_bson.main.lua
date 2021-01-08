require("common");
require("utils");

function app_short_help()
    return "dump a bson file.";
end

function app_name()
    return "dump_bson";
end

function app_main(args)
    if not args[2] then
        printf("%s <bson>",args[1]);
        return;
    end

    local bson_file = args[2];

    local bson = Bson.new();

    if not bson:LoadBson(bson_file) then
        printfnl("load bson %s fail.",bson_file);
        return
    end

    print(bson:ToJson());
end
