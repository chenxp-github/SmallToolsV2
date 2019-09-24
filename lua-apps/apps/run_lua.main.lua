require("common");
require("utils");

function app_short_help()
    return "run lua";
end

function app_name()
    return "lua"
end

function app_main(args)
    App.LuaMain(args);
end

