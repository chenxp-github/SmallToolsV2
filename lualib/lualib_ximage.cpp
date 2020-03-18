#include "lualib_ximage.h"
#include "lualib_ximagegif.h"
#include "mem_tool.h"
#include "syslog.h"

RGBQUAD int32_to_rgbquad(int32_t c)
{
    RGBQUAD q;
    q.rgbRed = (uint8_t)(c&0x000000ff);
    q.rgbGreen = (uint8_t)((c&0x0000ff00) >> 8);
    q.rgbBlue = (uint8_t)((c&0x00ff0000) >> 16);
    q.rgbReserved = (uint8_t)((c&0xff000000) >> 24);
    return q;
}

int32_t rgbquad_to_int32(RGBQUAD r)
{
    return  (r.rgbRed&0x000000ff) 
        | ((r.rgbGreen << 8)&0x0000ff00)
        | ((r.rgbBlue << 16)&0x00ff0000)
        | ((r.rgbReserved << 24)&0xff000000);
}

RECT int_array_to_rect(int *a)
{
    RECT r;
    r.left = a[0];
    r.top = a[1];
    r.right = a[2];
    r.bottom = a[3];
    return r;
}
static status_t resample_image(CxImage *img,int neww, int newh)
{
    ASSERT(img);

    if(img->GetWidth() == neww && img->GetHeight() == newh)
    {
        return OK;
    }
    
    if(img->GetWidth() > neww && img->GetHeight() > newh)
    {
        img->QIShrink(neww,newh,img,false);
    }
    else
    {
        img->Resample2(neww,newh,5,5,img,FALSE);
    }
    return OK;
}
////////////////////////////////////////////////
LUA_IS_VALID_USER_DATA_FUNC(CxImage,ximage)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CxImage,ximage)
LUA_NEW_USER_DATA_FUNC(CxImage,ximage,XIMAGE)
LUA_GC_FUNC(CxImage,ximage)
LUA_IS_SAME_FUNC(CxImage,ximage)
LUA_TO_STRING_FUNC(CxImage,ximage)

bool is_ximage(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XIMAGE,
        LUA_USERDATA_XIMAGEGIF,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return ximage_is_userdata_valid(ud);  
}

/****************************************/
static status_t ximage_new(lua_State *L)
{
    CxImage *pximage;
    NEW(pximage,CxImage);
    pximage->Init();
    ximage_new_userdata(L,pximage,0);
    return 1;
}

static int ximage_destroyall(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->DestroyAll();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_settranscolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int rgba = (int)lua_tointeger(L,2);
    pximage->SetTransColor(int32_to_rgbquad(rgba));
    return 0;
}
static int ximage_getcodecoption(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int imagetype = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->GetCodecOption(imagetype);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_threshold(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int level = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Threshold(level);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_splitrgb(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *r = get_ximage(L,2);
    ASSERT(r);
    CxImage *g = get_ximage(L,3);
    ASSERT(g);
    CxImage *b = get_ximage(L,4);
    ASSERT(b);
    int _ret_0 = (int)pximage->SplitRGB(r,g,b);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_splitcmyk(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *c = get_ximage(L,2);
    ASSERT(c);
    CxImage *m = get_ximage(L,3);
    ASSERT(m);
    CxImage *y = get_ximage(L,4);
    ASSERT(y);
    CxImage *k = get_ximage(L,5);
    ASSERT(k);
    int _ret_0 = (int)pximage->SplitCMYK(c,m,y,k);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_splityuv(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *y = get_ximage(L,2);
    ASSERT(y);
    CxImage *u = get_ximage(L,3);
    ASSERT(u);
    CxImage *v = get_ximage(L,4);
    ASSERT(v);
    int _ret_0 = (int)pximage->SplitYUV(y,u,v);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_splityiq(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *y = get_ximage(L,2);
    ASSERT(y);
    CxImage *i = get_ximage(L,3);
    ASSERT(i);
    CxImage *q = get_ximage(L,4);
    ASSERT(q);
    int _ret_0 = (int)pximage->SplitYIQ(y,i,q);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_splitxyz(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *x = get_ximage(L,2);
    ASSERT(x);
    CxImage *y = get_ximage(L,3);
    ASSERT(y);
    CxImage *z = get_ximage(L,4);
    ASSERT(z);
    int _ret_0 = (int)pximage->SplitXYZ(x,y,z);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_splithsl(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *h = get_ximage(L,2);
    ASSERT(h);
    CxImage *s = get_ximage(L,3);
    ASSERT(s);
    CxImage *l = get_ximage(L,4);
    ASSERT(l);
    int _ret_0 = (int)pximage->SplitHSL(h,s,l);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_huepalette(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float correction = (float)lua_tonumber(L,2);
    pximage->HuePalette(correction);
    return 0;
}
static int ximage_colorize(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int hue = (int)lua_tointeger(L,2);
    int sat = (int)lua_tointeger(L,3);
    float blend = (float)lua_tonumber(L,4);
    int _ret_0 = (int)pximage->Colorize(hue,sat,blend);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_light(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int brightness = (int)lua_tointeger(L,2);
    int contrast = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->Light(brightness,contrast);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_mean(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float _ret_0 = (float)pximage->Mean();
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_erode(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int Ksize = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Erode(Ksize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_dilate(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int Ksize = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Dilate(Ksize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_edge(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int Ksize = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Edge(Ksize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_mix(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *imgsrc2 = get_ximage(L,2);
    ASSERT(imgsrc2);
    int op = (int)lua_tointeger(L,3);
    int lXOffset = (int)lua_tointeger(L,4);
    int lYOffset = (int)lua_tointeger(L,5);
    int bMixAlpha = (int)lua_toboolean(L,6);
    pximage->Mix(*imgsrc2,(ImageOpType)op,lXOffset,lYOffset,bMixAlpha);
    return 0;
}
static int ximage_mixfrom(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *imagesrc2 = get_ximage(L,2);
    ASSERT(imagesrc2);
    int lXOffset = (int)lua_tointeger(L,3);
    int lYOffset = (int)lua_tointeger(L,4);
    pximage->MixFrom(*imagesrc2,lXOffset,lYOffset);
    return 0;
}
static int ximage_shiftrgb(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int r = (int)lua_tointeger(L,2);
    int g = (int)lua_tointeger(L,3);
    int b = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pximage->ShiftRGB(r,g,b);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_gamma(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float gamma = (float)lua_tonumber(L,2);
    int _ret_0 = (int)pximage->Gamma(gamma);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_gammargb(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float gammaR = (float)lua_tonumber(L,2);
    float gammaG = (float)lua_tonumber(L,3);
    float gammaB = (float)lua_tonumber(L,4);
    int _ret_0 = (int)pximage->GammaRGB(gammaR,gammaG,gammaB);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_median(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int Ksize = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Median(Ksize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_noise(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int level = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Noise(level);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_fft2(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *srcReal = get_ximage(L,2);
    ASSERT(srcReal);
    CxImage *srcImag = get_ximage(L,3);
    ASSERT(srcImag);
    CxImage *dstReal = get_ximage(L,4);
    ASSERT(dstReal);
    CxImage *dstImag = get_ximage(L,5);
    ASSERT(dstImag);
    int direction = (int)lua_tointeger(L,6);
    int bForceFFT = (int)lua_toboolean(L,7);
    int bMagnitude = (int)lua_toboolean(L,8);
    int _ret_0 = (int)pximage->FFT2(srcReal,srcImag,dstReal,dstImag,direction,bForceFFT,bMagnitude);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_combine(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *r = get_ximage(L,2);
    ASSERT(r);
    CxImage *g = get_ximage(L,3);
    ASSERT(g);
    CxImage *b = get_ximage(L,4);
    ASSERT(b);
    CxImage *a = get_ximage(L,5);
    ASSERT(a);
    int colorspace = (int)lua_tointeger(L,6);
    int _ret_0 = (int)pximage->Combine(r,g,b,a,colorspace);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_repair(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float radius = (float)lua_tonumber(L,2);
    int niterations = (int)lua_tointeger(L,3);
    int colorspace = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pximage->Repair(radius,niterations,colorspace);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_repairchannel(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *ch = get_ximage(L,2);
    ASSERT(ch);
    float radius = (float)lua_tonumber(L,3);
    int _ret_0 = (int)pximage->RepairChannel(ch,radius);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_contour(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->Contour();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_jitter(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int radius = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Jitter(radius);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_textblur(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int threshold = (int)lua_tointeger(L,2);
    int decay = (int)lua_tointeger(L,3);
    int max_depth = (int)lua_tointeger(L,4);
    int bBlurHorizontal = (int)lua_toboolean(L,5);
    int bBlurVertical = (int)lua_toboolean(L,6);
    CxImage *iDst = get_ximage(L,7);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->TextBlur(threshold,decay,max_depth,bBlurHorizontal,bBlurVertical,iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_gaussianblur(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float radius = (float)lua_tonumber(L,2);
    CxImage *iDst = get_ximage(L,3);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->GaussianBlur(radius,iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_selectiveblur(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float radius = (float)lua_tonumber(L,2);
    int threshold = (int)lua_tointeger(L,3);
    CxImage *iDst = get_ximage(L,4);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->SelectiveBlur(radius,threshold,iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_unsharpmask(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float radius = (float)lua_tonumber(L,2);
    float amount = (float)lua_tonumber(L,3);
    int threshold = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pximage->UnsharpMask(radius,amount,threshold);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_redeyeremove(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float strength = (float)lua_tonumber(L,2);
    int _ret_0 = (int)pximage->RedEyeRemove(strength);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_saturate(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int saturation = (int)lua_tointeger(L,2);
    int colorspace = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->Saturate(saturation,colorspace);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_solarize(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int level = (int)lua_tointeger(L,2);
    int bLinkedChannels = (int)lua_toboolean(L,3);
    int _ret_0 = (int)pximage->Solarize(level,bLinkedChannels);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_convertcolorspace(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int dstColorSpace = (int)lua_tointeger(L,2);
    int srcColorSpace = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->ConvertColorSpace(dstColorSpace,srcColorSpace);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_optimalthreshold(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int method = (int)lua_tointeger(L,2);
    int pBox[1024];
    int pBox_size = 1024;
    CLuaVm::ToIntArray(L,3,pBox,&pBox_size);
    
    CxImage *pContrastMask = get_ximage(L,4);
    ASSERT(pContrastMask);

    RECT r = int_array_to_rect(pBox);

    int _ret_0 = (int)pximage->OptimalThreshold(method,&r,pContrastMask);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_adaptivethreshold(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int method = (int)lua_tointeger(L,2);
    int nBoxSize = (int)lua_tointeger(L,3);
    CxImage *pContrastMask = get_ximage(L,4);
    ASSERT(pContrastMask);
    int nBias = (int)lua_tointeger(L,5);
    float fGlobalLocalBalance = (float)lua_tonumber(L,6);
    int _ret_0 = (int)pximage->AdaptiveThreshold(method,nBoxSize,pContrastMask,nBias,fGlobalLocalBalance);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_qishrink(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int newx = (int)lua_tointeger(L,2);
    int newy = (int)lua_tointeger(L,3);
    CxImage *iDst = get_ximage(L,4);
    ASSERT(iDst);
    int bChangeBpp = (int)lua_toboolean(L,5);
    int _ret_0 = (int)pximage->QIShrink(newx,newy,iDst,bChangeBpp);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_circletransform(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int type = (int)lua_tointeger(L,2);
    int rmax = (int)lua_tointeger(L,3);
    float Koeff = (float)lua_tonumber(L,4);
    int _ret_0 = (int)pximage->CircleTransform(type,rmax,Koeff);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_thumbnail(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int newx = (int)lua_tointeger(L,2);
    int newy = (int)lua_tointeger(L,3);
    int canvascolor = (int)lua_tointeger(L,4);
    CxImage *iDst = get_ximage(L,5);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->Thumbnail(newx,newy,int32_to_rgbquad(canvascolor),iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_expand(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int newx = (int)lua_tointeger(L,2);
    int newy = (int)lua_tointeger(L,3);
    int canvascolor = (int)lua_tointeger(L,4);
    CxImage *iDst = get_ximage(L,5);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->Expand(newx,newy,int32_to_rgbquad(canvascolor),iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int ximage_skew(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float xgain = (float)lua_tonumber(L,2);
    float ygain = (float)lua_tonumber(L,3);
    int xpivot = (int)lua_tointeger(L,4);
    int ypivot = (int)lua_tointeger(L,5);
    int bEnableInterpolation = (int)lua_toboolean(L,6);
    int _ret_0 = (int)pximage->Skew(xgain,ygain,xpivot,ypivot,bEnableInterpolation);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_crop(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int r[1024];
    int r_size = 1024;
    CLuaVm::ToIntArray(L,2,r,&r_size);
    CxImage *iDst = get_ximage(L,3);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->Crop(int_array_to_rect(r),iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_croprotatedrectangle(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int topx = (int)lua_tointeger(L,2);
    int topy = (int)lua_tointeger(L,3);
    int width = (int)lua_tointeger(L,4);
    int height = (int)lua_tointeger(L,5);
    float angle = (float)lua_tonumber(L,6);
    CxImage *iDst = get_ximage(L,7);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->CropRotatedRectangle(topx,topy,width,height,angle,iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_dither(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int method = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Dither(method);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_increasebpp(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int nbit = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->IncreaseBpp(nbit);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_decreasebpp(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int nbit = (int)lua_tointeger(L,2);
    int errordiffusion = (int)lua_toboolean(L,3);
    int clrimportant = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pximage->DecreaseBpp(nbit,errordiffusion,NULL,clrimportant);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_resample2(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int newx = (int)lua_tointeger(L,2);
    int newy = (int)lua_tointeger(L,3);
    int inMethod = (int)lua_tointeger(L,4);
    int ofMethod = (int)lua_tointeger(L,5);
    CxImage *iDst = get_ximage(L,6);
    ASSERT(iDst);
    int disableAveraging = (int)lua_toboolean(L,7);
    int _ret_0 = (int)pximage->Resample2(newx,newy,inMethod,ofMethod,iDst,disableAveraging);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_resample_v1(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int newx = (int)lua_tointeger(L,2);
    int newy = (int)lua_tointeger(L,3);
    int mode = (int)lua_tointeger(L,4);
    CxImage *iDst = get_ximage(L,5);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->Resample(newx,newy,mode,iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static status_t ximage_resample_v2(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int32_t newx = (int32_t)lua_tointeger(L,2);
    int32_t newy = (int32_t)lua_tointeger(L,3);
    bool ret0 = resample_image(pximage,newx,newy)!=0;
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ximage_resample(lua_State *L)
{
    if(is_ximage(L,5))
    {
        return ximage_resample_v1(L);
    }    
    return ximage_resample_v2(L); 
}
static int ximage_rotate180(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *iDst = get_ximage(L,2);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->Rotate180(iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_getpixelcolorinterpolated(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float y = (float)lua_tonumber(L,3);
    int inMethod = (int)lua_tointeger(L,4);
    int ofMethod = (int)lua_tointeger(L,5);
    int _ret_0 = rgbquad_to_int32(
        pximage->GetPixelColorInterpolated(x,y,inMethod,ofMethod,NULL)
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getareacolorinterpolated(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float xc = (float)lua_tonumber(L,2);
    float yc = (float)lua_tonumber(L,3);
    float w = (float)lua_tonumber(L,4);
    float h = (float)lua_tonumber(L,5);
    int inMethod = (int)lua_tointeger(L,6);
    int ofMethod = (int)lua_tointeger(L,7);
    int _ret_0 = rgbquad_to_int32(
        pximage->GetAreaColorInterpolated(xc,yc,w,h,inMethod,ofMethod,NULL)
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getpixelcolorwithoverflow(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int ofMethod = (int)lua_tointeger(L,4);
    int _ret_0 = rgbquad_to_int32(
        pximage->GetPixelColorWithOverflow(x,y,ofMethod,NULL)
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_kernelbspline(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBSpline(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernellinear(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float t = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelLinear(t);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelcubic(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float t = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelCubic(t);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelgeneralizedcubic(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float t = (float)lua_tonumber(L,2);
    float a = (float)lua_tonumber(L,3);
    float _ret_0 = (float)pximage->KernelGeneralizedCubic(t,a);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernellanczossinc(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float t = (float)lua_tonumber(L,2);
    float r = (float)lua_tonumber(L,3);
    float _ret_0 = (float)pximage->KernelLanczosSinc(t,r);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelbox(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBox(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelhermite(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelHermite(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelhanning(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelHanning(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelhamming(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelHamming(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelsinc(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelSinc(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelblackman(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBlackman(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelbessel_j1(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBessel_J1(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelbessel_p1(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBessel_P1(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelbessel_q1(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBessel_Q1(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelbessel_order1(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBessel_Order1(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelbessel(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelBessel(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelgaussian(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelGaussian(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelquadratic(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelQuadratic(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelmitchell(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelMitchell(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelcatrom(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float _ret_0 = (float)pximage->KernelCatrom(x);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_kernelpower(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float x = (float)lua_tonumber(L,2);
    float a = (float)lua_tonumber(L,3);
    float _ret_0 = (float)pximage->KernelPower(x,a);
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_rotate2(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float angle = (float)lua_tonumber(L,2);
    CxImage *iDst = get_ximage(L,3);
    ASSERT(iDst);
    int inMethod = (int)lua_tointeger(L,4);
    int ofMethod = (int)lua_tointeger(L,5);
    int optimizeRightAngles = (int)lua_toboolean(L,6);
    int bKeepOriginalSize = (int)lua_toboolean(L,7);
    int _ret_0 = (int)pximage->Rotate2(angle,iDst,inMethod,ofMethod,NULL,optimizeRightAngles,bKeepOriginalSize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_rotate(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float angle = (float)lua_tonumber(L,2);
    CxImage *iDst = get_ximage(L,3);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->Rotate(angle,iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_negative(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->Negative();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_rotateright(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *iDst = get_ximage(L,2);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->RotateRight(iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_rotateleft(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *iDst = get_ximage(L,2);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->RotateLeft(iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_mirror(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int bMirrorSelection = (int)lua_toboolean(L,2);
    int bMirrorAlpha = (int)lua_toboolean(L,3);
    int _ret_0 = (int)pximage->Mirror(bMirrorSelection,bMirrorAlpha);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_flip(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int bFlipSelection = (int)lua_toboolean(L,2);
    int bFlipAlpha = (int)lua_toboolean(L,3);
    int _ret_0 = (int)pximage->Flip(bFlipSelection,bFlipAlpha);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_layerdeleteall(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    pximage->LayerDeleteAll();
    return 0;
}
static int ximage_layerdelete(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int position = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->LayerDelete(position);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_getparent(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *_ret_0 = pximage->GetParent();
    if(_ret_0==NULL)return 0;
    ximage_new_userdata(L,_ret_0,1);
    return 1;
}
static int ximage_getlayer(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int position = (int)lua_tointeger(L,2);
    CxImage *_ret_0 = pximage->GetLayer(position);
    if(_ret_0==NULL)return 0;
    ximage_new_userdata(L,_ret_0,1);
    return 1;
}
static int ximage_getnumlayers(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetNumLayers();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_dumpsize(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->DumpSize();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getframe(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int nFrame = (int)lua_tointeger(L,2);
    CxImage *_ret_0 = pximage->GetFrame(nFrame);
    if(_ret_0==NULL)return 0;
    ximage_new_userdata(L,_ret_0,1);
    return 1;
}
static int ximage_setretreiveallframes(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int flag = (int)lua_toboolean(L,2);
    pximage->SetRetreiveAllFrames(flag);
    return 0;
}
static int ximage_getretreiveallframes(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetRetreiveAllFrames();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_getdisposalmethod(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetDisposalMethod();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_setdisposalmethod(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int dm = (int)lua_tointeger(L,2);
    pximage->SetDisposalMethod(dm);
    return 0;
}
static int ximage_setframe(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int nFrame = (int)lua_tointeger(L,2);
    pximage->SetFrame(nFrame);
    return 0;
}

static int ximage_getnumframes(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetNumFrames();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_setescape(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int i = (int)lua_tointeger(L,2);
    pximage->SetEscape(i);
    return 0;
}
static int ximage_setprogress(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int p = (int)lua_tointeger(L,2);
    pximage->SetProgress(p);
    return 0;
}
static int ximage_getescape(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetEscape();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getprogress(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetProgress();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_setjpegscale(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int q = (int)lua_tointeger(L,2);
    pximage->SetJpegScale(q);
    return 0;
}
static int ximage_getjpegscale(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetJpegScale();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_setjpegqualityf(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float q = (float)lua_tonumber(L,2);
    pximage->SetJpegQualityF(q);
    return 0;
}
static int ximage_getjpegqualityf(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    float _ret_0 = (float)pximage->GetJpegQualityF();
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int ximage_getjpegquality(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetJpegQuality();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_setoffset(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    pximage->SetOffset(x,y);
    return 0;
}
static int ximage_getoffset(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    long x = 0,y = 0;   
    pximage->GetOffset(&x,&y);
    lua_pushinteger(L,x);
    lua_pushinteger(L,y);
    return 2;
}
static int ximage_setframedelay(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int d = (int)lua_tointeger(L,2);
    pximage->SetFrameDelay(d);
    return 0;
}
static int ximage_gettranscolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = rgbquad_to_int32(
        pximage->GetTransColor()
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_gettransindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetTransIndex();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_settransindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int idx = (int)lua_tointeger(L,2);
    pximage->SetTransIndex(idx);
    return 0;
}
static int ximage_getframedelay(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetFrameDelay();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_gettypeindexfromid(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int id = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->GetTypeIndexFromId(id);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_gettypeidfromindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int index = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->GetTypeIdFromIndex(index);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_gettypeidfromname(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    const char* ext = (const char*)lua_tostring(L,2);
    ASSERT(ext);
    int _ret_0 = (int)pximage->GetTypeIdFromName(ext);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getnumtypes(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetNumTypes();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_settype(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int type = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->SetType(type);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_getnumcolors(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetNumColors();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_geteffwidth(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetEffWidth();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_grayscale(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GrayScale();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_blindselectionget(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->BlindSelectionGet(x,y);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_isindexed(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->IsIndexed();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_blindgetpixelpointer(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    pximage->BlindGetPixelPointer(x,y);
    return 0;
}
static int ximage_setclrimportant(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int ncolors = (int)lua_tointeger(L,2);
    pximage->SetClrImportant(ncolors);
    return 0;
}
static int ximage_getclrimportant(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetClrImportant();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_issamepalette(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *img = get_ximage(L,2);
    ASSERT(img);
    int bCheckAlpha = (int)lua_toboolean(L,3);
    int _ret_0 = (int)pximage->IsSamePalette(*img,bCheckAlpha);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_setstdpalette(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    pximage->SetStdPalette();
    return 0;
}
static int ximage_gettransparentmask(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *iDst = get_ximage(L,2);
    ASSERT(iDst);
    int _ret_0 = (int)pximage->GetTransparentMask(iDst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_blendpalette(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int cr = (int)lua_tointeger(L,2);
    int perc = (int)lua_tointeger(L,3);
    pximage->BlendPalette(cr,perc);
    return 0;
}

static int ximage_blendpixelcolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int c = (int)lua_tointeger(L,4);
    float blend = (float)lua_tonumber(L,5);
    int bSetAlpha = (int)lua_toboolean(L,6);
    pximage->BlendPixelColor(x,y,int32_to_rgbquad(c),blend,bSetAlpha);
    return 0;
}
static int ximage_setpixelcolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int c = (int)lua_tointeger(L,4);
    int bSetAlpha = (int)lua_toboolean(L,5);
    pximage->SetPixelColor(x,y,int32_to_rgbquad(c),bSetAlpha);
    return 0;
}
static int ximage_setpixelindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int i = (int)lua_tointeger(L,4);
    pximage->SetPixelIndex(x,y,i);
    return 0;
}
static int ximage_getpixelgray(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->GetPixelGray(x,y);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getpixelindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->GetPixelIndex(x,y);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getpixelcolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int bGetAlpha = (int)lua_toboolean(L,4);
    int _ret_0 = rgbquad_to_int32(
        pximage->GetPixelColor(x,y,bGetAlpha)
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_istransparent(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->IsTransparent(x,y);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphafromtransparency(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaFromTransparency();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphapalettesplit(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *dest = get_ximage(L,2);
    ASSERT(dest);
    int _ret_0 = (int)pximage->AlphaPaletteSplit(dest);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphasplit(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *dest = get_ximage(L,2);
    ASSERT(dest);
    int _ret_0 = (int)pximage->AlphaSplit(dest);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphamirror(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaMirror();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphaflip(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaFlip();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_getnearestindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int c = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->GetNearestIndex(int32_to_rgbquad(c));
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getlasterror(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    const char* _ret_0 = (const char*)pximage->GetLastError();
    if(_ret_0==NULL)return 0;
    lua_pushstring(L,_ret_0);
    return 1;
}
static int ximage_enable(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int enable = (int)lua_toboolean(L,2);
    pximage->Enable(enable);
    return 0;
}
static int ximage_isenabled(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->IsEnabled();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_isvalid(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->IsValid();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_blindsetpixelcolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int c = (int)lua_tointeger(L,4);
    int bSetAlpha = (int)lua_toboolean(L,5);
    pximage->BlindSetPixelColor(x,y,int32_to_rgbquad(c),bSetAlpha);
    return 0;
}
static int ximage_blindgetpixelcolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int bGetAlpha = (int)lua_toboolean(L,4);
    int _ret_0 = rgbquad_to_int32(
        pximage->BlindGetPixelColor(x,y,bGetAlpha)
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_alphastrip(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    pximage->AlphaStrip();
    return 0;
}
static int ximage_alphapaletteisvalid(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaPaletteIsValid();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_blindgetpixelindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->BlindGetPixelIndex(x,y);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_blindsetpixelindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int i = (int)lua_tointeger(L,4);
    pximage->BlindSetPixelIndex(x,y,i);
    return 0;
}
static int ximage_swapindex(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int idx1 = (int)lua_tointeger(L,2);
    int idx2 = (int)lua_tointeger(L,3);
    pximage->SwapIndex(idx1,idx2);
    return 0;
}
static int ximage_isgrayscale(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->IsGrayScale();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphaset(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *from = get_ximage(L,2);
    ASSERT(from);
    int _ret_0 = (int)pximage->AlphaSet(from);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphacopy(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *from = get_ximage(L,2);
    ASSERT(from);
    int _ret_0 = (int)pximage->AlphaCopy(from);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphainvert(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    pximage->AlphaInvert();
    return 0;
}
static int ximage_alphaclear(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    pximage->AlphaClear();
    return 0;
}
static int ximage_alphapaletteisenabled(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaPaletteIsEnabled();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphapaletteenable(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int enable = (int)lua_toboolean(L,2);
    pximage->AlphaPaletteEnable(enable);
    return 0;
}
static int ximage_alphaisvalid(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaIsValid();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphasetmax(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int nAlphaMax = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->AlphaSetMax(nAlphaMax);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_alphagetmax(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaGetMax();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_setpalettecolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int idx = (int)lua_tointeger(L,2);
    int cr = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->SetPaletteColor(idx,int32_to_rgbquad(cr));
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getpalettecolor(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int idx = (int)lua_tointeger(L,2);
    int _ret_0 = rgbquad_to_int32(
        pximage->GetPaletteColor(idx)
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_alphapaletteclear(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaPaletteClear();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_blindalphaget(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->BlindAlphaGet(x,y);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_alphagetpointer(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int_ptr_t _ret_0 = (int_ptr_t)pximage->AlphaGetPointer(x,y);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_alphaget(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->AlphaGet(x,y);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int ximage_alphacreate(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaCreate();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_setgraypalette(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->SetGrayPalette();
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int ximage_destroyframes(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->DestroyFrames();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_transfer(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *from = get_ximage(L,2);
    ASSERT(from);
    int bTransferFrames = (int)lua_toboolean(L,3);
    int _ret_0 = (int)pximage->Transfer(from,bTransferFrames);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_clear(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int bval = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pximage->Clear(bval);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_isinside(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pximage->IsInside(x,y);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_alphadelete(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->AlphaDelete();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_selectiondelete(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->SelectionDelete();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getbits(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int row = (int)lua_tointeger(L,2);
    int_ptr_t _ret_0 = (int_ptr_t)pximage->GetBits(row);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getpalettesize(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetPaletteSize();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getpalette(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int_ptr_t _ret_0 = (int_ptr_t)pximage->GetPalette();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getsize(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetSize();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_gettype(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetType();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getbpp(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetBpp();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getheight(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetHeight();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_getwidth(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->GetWidth();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_create(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int dwWidth = (int)lua_tointeger(L,2);
    int dwHeight = (int)lua_tointeger(L,3);
    int wBpp = (int)lua_tointeger(L,4);
    int imagetype = (int)lua_tointeger(L,5);
    pximage->Create(dwWidth,dwHeight,wBpp,imagetype);
    return 0;
}
static int ximage_ghost(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *from = get_ximage(L,2);
    ASSERT(from);
    int _ret_0 = (int)pximage->Ghost(from);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_destroy_(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int _ret_0 = (int)pximage->Destroy();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_copy(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    CxImage *p = get_ximage(L,2);
    ASSERT(p);
    int _ret_0 = (int)pximage->Copy(p);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_drawline(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int StartX = (int)lua_tointeger(L,2);
    int EndX = (int)lua_tointeger(L,3);
    int StartY = (int)lua_tointeger(L,4);
    int EndY = (int)lua_tointeger(L,5);
    int color = (int)lua_tointeger(L,6);
    int bSetAlpha = (int)lua_toboolean(L,7);
    int _ret_0 = (int)pximage->DrawLine(
        StartX,EndX,StartY,EndY,
        int32_to_rgbquad(color),
        bSetAlpha);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximage_fillrect(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int r[1024];
    int r_size = 1024;
    CLuaVm::ToIntArray(L,2,r,&r_size);
    int color = (int)lua_tointeger(L,3);
    int setAlpha = (int)lua_toboolean(L,4);
    int _ret_0 = (int)pximage->FillRect(int_array_to_rect(r),int32_to_rgbquad(color),setAlpha);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int ximage_drawrect(lua_State *L)
{
    CxImage *pximage = get_ximage(L,1);
    ASSERT(pximage);
    int r[1024];
    int r_size = 1024;
    CLuaVm::ToIntArray(L,2,r,&r_size);
    int color = (int)lua_tointeger(L,3);
    int setAlpha = (int)lua_toboolean(L,4);
    int _ret_0 = (int)pximage->DrawRect(int_array_to_rect(r),int32_to_rgbquad(color),setAlpha);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static const luaL_Reg ximage_lib[] = {
    {"new",ximage_new},
    {"__gc",ximage_gc_},
    {"__tostring",ximage_tostring_},
    {"IsSame",ximage_issame_},
    {"AdaptiveThreshold",ximage_adaptivethreshold},
    {"AlphaClear",ximage_alphaclear},
    {"AlphaCopy",ximage_alphacopy},
    {"AlphaCreate",ximage_alphacreate},
    {"AlphaDelete",ximage_alphadelete},
    {"AlphaFlip",ximage_alphaflip},
    {"AlphaFromTransparency",ximage_alphafromtransparency},
    {"AlphaGet",ximage_alphaget},
    {"AlphaGetMax",ximage_alphagetmax},
    {"AlphaGetPointer",ximage_alphagetpointer},
    {"AlphaInvert",ximage_alphainvert},
    {"AlphaIsValid",ximage_alphaisvalid},
    {"AlphaMirror",ximage_alphamirror},
    {"AlphaPaletteClear",ximage_alphapaletteclear},
    {"AlphaPaletteEnable",ximage_alphapaletteenable},
    {"AlphaPaletteIsEnabled",ximage_alphapaletteisenabled},
    {"AlphaPaletteIsValid",ximage_alphapaletteisvalid},
    {"AlphaPaletteSplit",ximage_alphapalettesplit},
    {"AlphaSet",ximage_alphaset},
    {"AlphaSetMax",ximage_alphasetmax},
    {"AlphaSplit",ximage_alphasplit},
    {"AlphaStrip",ximage_alphastrip},
    {"BlendPalette",ximage_blendpalette},
    {"BlendPixelColor",ximage_blendpixelcolor},
    {"BlindAlphaGet",ximage_blindalphaget},
    {"BlindGetPixelColor",ximage_blindgetpixelcolor},
    {"BlindGetPixelIndex",ximage_blindgetpixelindex},
    {"BlindGetPixelPointer",ximage_blindgetpixelpointer},
    {"BlindSelectionGet",ximage_blindselectionget},
    {"BlindSetPixelColor",ximage_blindsetpixelcolor},
    {"BlindSetPixelIndex",ximage_blindsetpixelindex},
    {"CircleTransform",ximage_circletransform},
    {"Clear",ximage_clear},
    {"Colorize",ximage_colorize},
    {"Combine",ximage_combine},
    {"Contour",ximage_contour},
    {"ConvertColorSpace",ximage_convertcolorspace},
    {"Copy",ximage_copy},
    {"Create",ximage_create},
    {"Crop",ximage_crop},
    {"CropRotatedRectangle",ximage_croprotatedrectangle},
    {"DecreaseBpp",ximage_decreasebpp},
    {"Destroy",ximage_destroy_},
    {"DestroyAll",ximage_destroyall},
    {"DestroyFrames",ximage_destroyframes},
    {"Dilate",ximage_dilate},
    {"Dither",ximage_dither},
    {"DumpSize",ximage_dumpsize},
    {"Edge",ximage_edge},
    {"Enable",ximage_enable},
    {"Erode",ximage_erode},
    {"Expand",ximage_expand},
    {"FFT2",ximage_fft2},
    {"Flip",ximage_flip},
    {"Gamma",ximage_gamma},
    {"GammaRGB",ximage_gammargb},
    {"GaussianBlur",ximage_gaussianblur},
    {"GetAreaColorInterpolated",ximage_getareacolorinterpolated},
    {"GetBits",ximage_getbits},
    {"GetBpp",ximage_getbpp},
    {"GetClrImportant",ximage_getclrimportant},
    {"GetCodecOption",ximage_getcodecoption},
    {"GetDisposalMethod",ximage_getdisposalmethod},
    {"GetEffWidth",ximage_geteffwidth},
    {"GetEscape",ximage_getescape},
    {"GetFrame",ximage_getframe},
    {"GetFrameDelay",ximage_getframedelay},
    {"GetHeight",ximage_getheight},
    {"GetJpegQuality",ximage_getjpegquality},
    {"GetJpegQualityF",ximage_getjpegqualityf},
    {"GetJpegScale",ximage_getjpegscale},
    {"GetLastError",ximage_getlasterror},
    {"GetLayer",ximage_getlayer},
    {"GetNearestIndex",ximage_getnearestindex},
    {"GetNumColors",ximage_getnumcolors},
    {"GetNumFrames",ximage_getnumframes},
    {"GetNumLayers",ximage_getnumlayers},
    {"GetNumTypes",ximage_getnumtypes},
    {"GetOffset",ximage_getoffset},
    {"GetPalette",ximage_getpalette},
    {"GetPaletteColor",ximage_getpalettecolor},
    {"GetPaletteSize",ximage_getpalettesize},
    {"GetParent",ximage_getparent},
    {"GetPixelColor",ximage_getpixelcolor},
    {"GetPixelColorInterpolated",ximage_getpixelcolorinterpolated},
    {"GetPixelColorWithOverflow",ximage_getpixelcolorwithoverflow},
    {"GetPixelGray",ximage_getpixelgray},
    {"GetPixelIndex",ximage_getpixelindex},
    {"GetProgress",ximage_getprogress},
    {"GetRetreiveAllFrames",ximage_getretreiveallframes},
    {"GetSize",ximage_getsize},
    {"GetTransColor",ximage_gettranscolor},
    {"GetTransIndex",ximage_gettransindex},
    {"GetTransparentMask",ximage_gettransparentmask},
    {"GetType",ximage_gettype},
    {"GetTypeIdFromIndex",ximage_gettypeidfromindex},
    {"GetTypeIdFromName",ximage_gettypeidfromname},
    {"GetTypeIndexFromId",ximage_gettypeindexfromid},
    {"GetWidth",ximage_getwidth},
    {"Ghost",ximage_ghost},
    {"GrayScale",ximage_grayscale},
    {"HuePalette",ximage_huepalette},
    {"IncreaseBpp",ximage_increasebpp},
    {"IsEnabled",ximage_isenabled},
    {"IsGrayScale",ximage_isgrayscale},
    {"IsIndexed",ximage_isindexed},
    {"IsInside",ximage_isinside},
    {"IsSamePalette",ximage_issamepalette},
    {"IsTransparent",ximage_istransparent},
    {"IsValid",ximage_isvalid},
    {"Jitter",ximage_jitter},
    {"KernelBSpline",ximage_kernelbspline},
    {"KernelBessel",ximage_kernelbessel},
    {"KernelBessel_J1",ximage_kernelbessel_j1},
    {"KernelBessel_Order1",ximage_kernelbessel_order1},
    {"KernelBessel_P1",ximage_kernelbessel_p1},
    {"KernelBessel_Q1",ximage_kernelbessel_q1},
    {"KernelBlackman",ximage_kernelblackman},
    {"KernelBox",ximage_kernelbox},
    {"KernelCatrom",ximage_kernelcatrom},
    {"KernelCubic",ximage_kernelcubic},
    {"KernelGaussian",ximage_kernelgaussian},
    {"KernelGeneralizedCubic",ximage_kernelgeneralizedcubic},
    {"KernelHamming",ximage_kernelhamming},
    {"KernelHanning",ximage_kernelhanning},
    {"KernelHermite",ximage_kernelhermite},
    {"KernelLanczosSinc",ximage_kernellanczossinc},
    {"KernelLinear",ximage_kernellinear},
    {"KernelMitchell",ximage_kernelmitchell},
    {"KernelPower",ximage_kernelpower},
    {"KernelQuadratic",ximage_kernelquadratic},
    {"KernelSinc",ximage_kernelsinc},
    {"LayerDelete",ximage_layerdelete},
    {"LayerDeleteAll",ximage_layerdeleteall},
    {"Light",ximage_light},
    {"Mean",ximage_mean},
    {"Median",ximage_median},
    {"Mirror",ximage_mirror},
    {"Mix",ximage_mix},
    {"MixFrom",ximage_mixfrom},
    {"Negative",ximage_negative},
    {"Noise",ximage_noise},
    {"OptimalThreshold",ximage_optimalthreshold},
    {"QIShrink",ximage_qishrink},
    {"RedEyeRemove",ximage_redeyeremove},
    {"Repair",ximage_repair},
    {"RepairChannel",ximage_repairchannel},
    {"Resample",ximage_resample},
    {"Resample2",ximage_resample2},
    {"Rotate",ximage_rotate},
    {"Rotate180",ximage_rotate180},
    {"Rotate2",ximage_rotate2},
    {"RotateLeft",ximage_rotateleft},
    {"RotateRight",ximage_rotateright},
    {"Saturate",ximage_saturate},
    {"SelectionDelete",ximage_selectiondelete},
    {"SelectiveBlur",ximage_selectiveblur},
    {"SetClrImportant",ximage_setclrimportant},
    {"SetDisposalMethod",ximage_setdisposalmethod},
    {"SetEscape",ximage_setescape},
    {"SetFrame",ximage_setframe},
    {"SetFrameDelay",ximage_setframedelay},
    {"SetGrayPalette",ximage_setgraypalette},
    {"SetJpegQualityF",ximage_setjpegqualityf},
    {"SetJpegScale",ximage_setjpegscale},
    {"SetOffset",ximage_setoffset},
    {"SetPaletteColor",ximage_setpalettecolor},
    {"SetPixelColor",ximage_setpixelcolor},
    {"SetPixelIndex",ximage_setpixelindex},
    {"SetProgress",ximage_setprogress},
    {"SetRetreiveAllFrames",ximage_setretreiveallframes},
    {"SetStdPalette",ximage_setstdpalette},
    {"SetTransColor",ximage_settranscolor},
    {"SetTransIndex",ximage_settransindex},
    {"SetType",ximage_settype},
    {"ShiftRGB",ximage_shiftrgb},
    {"Skew",ximage_skew},
    {"Solarize",ximage_solarize},
    {"SplitCMYK",ximage_splitcmyk},
    {"SplitHSL",ximage_splithsl},
    {"SplitRGB",ximage_splitrgb},
    {"SplitXYZ",ximage_splitxyz},
    {"SplitYIQ",ximage_splityiq},
    {"SplitYUV",ximage_splityuv},
    {"SwapIndex",ximage_swapindex},
    {"TextBlur",ximage_textblur},
    {"Threshold",ximage_threshold},
    {"Thumbnail",ximage_thumbnail},
    {"Transfer",ximage_transfer},
    {"UnsharpMask",ximage_unsharpmask},
    {"DrawLine",ximage_drawline},
    {"FillRect",ximage_fillrect},
    {"DrawRect",ximage_drawrect},
    {NULL, NULL}
};

const luaL_Reg* get_ximage_funcs()
{
    return ximage_lib;
}

static int luaL_register_ximage(lua_State *L)
{	
    static luaL_Reg _ximage_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_ximage_funcs_,&_index,get_ximage_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_XIMAGE);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_ximage_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_ximage_funcs_);
    return 1;
}        

int luaopen_ximage(lua_State *L)
{
    luaL_requiref(L, "XImage",luaL_register_ximage,1);
    lua_pop(L, 1);
    return 0;
}        
