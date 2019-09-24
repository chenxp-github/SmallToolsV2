require("common");
require("utils");

function app_short_help()
    return "pack a widget.";
end

function app_name()
    return "pack_widget";
end

function pack_widget(folder,widget)
    local out,out_file = new_file(widget,"wb+");
    if not out then
        exit("create file "..widget.." error.");
    end

    local disk = SimpleDisk.new();
    disk:SetDstFile(out_file);
    if App.MakeOrderSimpleDiskFromDir(folder,disk) then
        disk:Save();
        out:CloseFile();
        print("save to "..widget);
    else
        exit("add folder "..folder.." fail.");
    end
end

function app_main(args)
    local argc = #args;
        
    if argc < 3 then
        print(args[1].." <folder> <widget>");
        return;
    end

    local folder = FileManager.ToAbsPath(args[2]);
    local widget = FileManager.ToAbsPath(args[3]);
    
    pack_widget(folder,widget);
    
end

