require("common");
require("utils");

function app_short_help()
    return "unpack a widget.";
end

function app_name()
    return "unpack_widget";
end

function unpack_widget(widget,folder)
    local img,img_file = new_file(widget,"rb");
    if not img then
        exit("open file "..widget.." error.");
    end
    local disk = SimpleDisk.new();
    disk:LoadDiskImage(img_file);
    disk:ExtractAllFiles(folder);
    print("extract to "..folder);
end

function app_main(args)
    local argc = #args;
        
    if argc < 3 then
        print(args[1].." <widget> <folder>");
        return;
    end

    local widget = FileManager.ToAbsPath(args[2]);
    local folder = FileManager.ToAbsPath(args[3]);

    unpack_widget(widget,folder);    
end

