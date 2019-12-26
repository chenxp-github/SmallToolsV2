require("common");
require("utils");

function app_short_help()
    return "create a build time H file";
end

function app_name()
    return "build_time";
end

function app_main(args)
    

    
    local mf = new_memfile();

    mf:Puts("#ifndef __BUILD_TIME__H"..EOL);
    mf:Puts("#define __BUILD_TIME__H"..EOL);
    mf:Puts(EOL);
    
    mf:Puts(string.format("#define BUILD_DATE \"%s\"",
        os.date("%Y-%m-%d")
    ));
    mf:Puts(EOL);

    mf:Puts(string.format("#define BUILD_TIME \"%s\"",
        os.date("%H:%M:%S")
    ));
    mf:Puts(EOL);

    mf:Puts(EOL);
    mf:Puts("#endif"..EOL);
    mf:Puts(EOL);

    local fn = "_build_time_.h";
    if mf:WriteToFile(fn) > 0 then
        printf("save to %s",fn);
    end
end
