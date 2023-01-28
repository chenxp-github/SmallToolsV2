RES_TYPE_UNKNOWN = 0;
RES_TYPE_STRING = 1;
RES_TYPE_CSS = 2;
RES_TYPE_9_PNG = 3;
RES_TYPE_PACKED_IMAGE = 4;
RES_TYPE_RAW_IMAGE = 5;        
------------------------------------
g_resource_type_folders={
    is_resource_type_folder = 
        true,

    folders={
        "strings",
        "images",
        "styles",
    }
}

g_theme_folders = {
    is_theme_folder = 
        true,
    
    folders = {
        "NO_THEME",
        "theme1",
        "theme2",
        "theme3",
    }
}

g_language_folders = {
    is_language_folder = 
        true,

    folders = {
        "NO_LANG",
        "Chinese",
        "English",
        "Japanese",
        "German",
    }
}

------------------------------------
function is_9png_file(filename)
    return CFunc.wild_match("*.9.png",filename);
end

function is_string_file(filename)
    return CFunc.wild_match("*.string.lua",filename);
end

function is_style_file(filename)
    return CFunc.wild_match("*.css.lua",filename);
end

function is_normal_image_file(filename)
    if is_9png_file(filename) then
        return;
    end
        
    local patterns = {
        "*.png","*.jpg","*.bmp","*.ico","*.gif","*.jpeg",
    };
    
    for _,p in pairs(patterns) do
        if CFunc.wild_match(p,filename) then
            return true;
        end
    end
end

function res_type_to_prefix(res_type)
    if  res_type == RES_TYPE_9_PNG 
        or res_type == RES_TYPE_RAW_IMAGE 
        or res_type == RES_TYPE_PACKED_IMAGE 
    then
        return "IMAGE_";
    end

    if res_type == RES_TYPE_STRING then
        return "STRING_";
    end
    
    if res_type == RES_TYPE_CSS then
        return "CSS_";
    end
    
    return "";
end


function filename_to_id(res_type,filename)
    return string.upper(
        res_type_to_prefix(res_type)..
        string.gsub(filename,"[^%a%d]","_")
    );
end

function raw_nine_patches_to_slice_array(png_width,png_height,nine_patches)

    local function get_nine_patches(index)
        if not nine_patches[index] then
            return 0;
        else
            return nine_patches[index];
        end
    end

    local top_left = get_nine_patches(3+1);
    local top_right = get_nine_patches(4+1);
    local bottom_left = get_nine_patches(9+1);
    local bottom_right = get_nine_patches(10+1);

    local left = math.max(top_left, bottom_left);
    local right;
    
    if top_right > 0 and bottom_right > 0 then
        right = math.min(top_right, bottom_right);
    else
        right = math.max(top_right, bottom_right);
    end

    local left_top = get_nine_patches(0+1);
    local left_bottom = get_nine_patches(1+1);
    local right_top = get_nine_patches(6+1);
    local right_bottom = get_nine_patches(7+1);
    
    local top = math.max(left_top, right_top);
    local bottom;
    
    if left_bottom > 0 and right_bottom > 0 then
        bottom = math.min(left_bottom, right_bottom);
    else
        bottom = math.max(left_bottom, right_bottom);
    end

    if left > right or top > bottom then
        return "";
    end

    return {left,top, (png_width - right - 1),(png_height - bottom - 1)};
end
