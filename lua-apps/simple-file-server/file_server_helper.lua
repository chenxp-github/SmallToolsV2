require("utils")
require("cothread");

FsHelper = {};

FsHelper.ListFolder = function(dir)
    if not FileManager.IsDirExist(dir) then
        return
    end

    local list={
        _is_array_ = true,
    };

    FileManager.SearchDir(dir,false,function(info)
        
        local t = {
            is_dir  = false,
            size = 0,
            name = info.file_name,
        };
        
        if info.event == EVENT_BEGIN_DIR then
            t.is_dir = true;
            table.insert(list,t);
        end

        if info.event == EVENT_SINGLE_FILE then
            t.size = {_int64_=info.size};
            table.insert(list,t);
        end
    end);

    return list;
end

FsHelper.SaveSmallFile=function(filename,mem)
    return mem:WriteToFile(filename) > 0;
end


