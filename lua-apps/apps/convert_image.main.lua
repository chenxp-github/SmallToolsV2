require("common");
require("utils");

g_in = nil;
g_out = nil;
g_crop = nil;
g_width = nil;
g_height = nil;

function split_comma_string(str)
    local tab = {};
    for k in string.gmatch(str,"([%a%d]+)") do
        table.insert(tab,k);
    end
    return tab;
end

function app_short_help()
    return "covert image format";
end

function app_name()
    return "convert_image";
end

local kIn="--in";
local kOut="--out";
local kWidth="--width";
local kHeight="--height";
local kCrop="--crop";

function image_resample(img, neww,newh)
    if img:GetWidth() > neww and img:GetHeight() > newh then
        img:QIShrink(neww,newh,img,false);
    else
        img:Resample2(neww,newh,5,5,img);
    end
end

function app_main(args)
    local argc = #args;
    
    local cmd = CommandLine.new();
    cmd:AddKeyType(kIn,TYPE_KEY_EQUAL_VALUE,MUST,"input image file");
    cmd:AddKeyType(kOut,TYPE_KEY_EQUAL_VALUE,MUST,"output image file");
    cmd:AddKeyType(kWidth,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"output image width, default is input image width");
    cmd:AddKeyType(kHeight,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"output image height, default is input image height");    
    cmd:AddKeyType(kCrop,TYPE_KEY_EQUAL_VALUE,OPTIONAL,"input image crop rect (left,top,width,height)");        
    cmd:LoadFromArgv(args);
  
    if cmd:CheckForErrors() then
        return;
    end
    
    g_in = FileManager.ToAbsPath(cmd:GetValueByKey(kIn));
    g_out = FileManager.ToAbsPath(cmd:GetValueByKey(kOut));
    
    if cmd:HasKey(kWidth) then
        g_width = tonumber(cmd:GetValueByKey(kWidth));
    end
    
    if cmd:HasKey(kHeight) then
        g_height = tonumber(cmd:GetValueByKey(kHeight));
    end
    
    if cmd:HasKey(kCrop) then
        g_crop = split_comma_string(cmd:GetValueByKey(kCrop));
    end

    local in_image = XImage.new();
    if not XImageHelper.LoadImage(g_in,in_image) then
        exit("load image "..g_in.." fail");
    end
    
    --type 1
    if not g_crop 
       and not g_width
       and not g_height
    then        
       XImageHelper.SaveImage(in_image,g_out);
    end
   
    --type 2
    if not g_crop 
       and (g_width or g_height)
    then
       image_resample(in_image,g_width,g_height);
       XImageHelper.SaveImage(in_image,g_out);
    end
   
    --type 3
    if g_crop then
        g_crop[3] = g_crop[1] + g_crop[3];
        g_crop[4] = g_crop[2] + g_crop[4];
        in_image:Crop(g_crop,in_image);        
        if g_width or g_height then
            image_resample(in_image,g_width,g_height);
        end               
        XImageHelper.SaveImage(in_image,g_out);        
    end      
end
