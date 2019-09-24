require("common");
require("utils");

function app_short_help()
    return "a message peer server";
end

function app_name()
    return "message_center";
end


function app_main(args)
    local argc = #args;
    
    if argc < 2 then
        print(args[1].." <port>");
        return -1;
    end
    
    local port = tonumber(args[2]);
    if port <= 0 then
        return exit("invalid port number:"..port);
    end
    
    print("start message center on port:"..port);
    App.StartMessageCenter(port);
    App.MainLoop();
end

