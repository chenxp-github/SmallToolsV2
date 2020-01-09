require("common");
require("utils");

function app_short_help()
    return "deploy binary files for fiv-3359";
end

function app_name()
    return "deploy_fiv_3359"
end

function print_help(args)
    print(args[1].." <dest>");
    print("    <dest> is the destination path to deploy");
end

function app_main(args)
    local n = #args;
    if n < 2 then
        return print_help(args);
    end
    local dest_path = FileManager.ToAbsPath(args[2]);
    local src_path = FileManager.GetCurDir();
    
    if not FileManager.IsDirExist(dest_path) then 
        print("dest path "..dest_path.." doesn't exist.");
        create_folder(dest_path,true);
    end
    
    local function fiv_copy_files(name)
        checked_copy_files(src_path.."/"..name,dest_path,true);
    end

    fiv_copy_files("fiv");
    fiv_copy_files("v8_context_snapshot.bin");
    fiv_copy_files("cef_100_percent.pak");
    fiv_copy_files("cef_200_percent.pak");
    fiv_copy_files("icudtl.dat");
    fiv_copy_files("natives_blob.bin");
    fiv_copy_files("locales");
    fiv_copy_files("cef.pak");
    fiv_copy_files("cef_extensions.pak");
    fiv_copy_files("resources");
    fiv_copy_files("devtools_resources.pak");
end


