#include "lualib_ximagehelper.h"
#include "lualib_filebase.h"
#include "lualib_mem.h"
#include "lualib_ximage.h"
#include "ximagehelper.h"
#include "lua_helper.h"
#include "mem_tool.h"
#include "syslog.h"

#if CXIMAGE_SUPPORT_WINDOWS
#include "lualib_luahdc.h"
#endif

static int ximagehelper_imagetorawrgba(lua_State *L)
{
    CxImage *img = get_ximage(L,1);
    ASSERT(img);
    CMem *rgba = get_mem(L,2);
    ASSERT(rgba);
    int _ret_0 = (int)CxImageHelper::ImageToRawRgba(img,rgba);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximagehelper_saveimage_v1(lua_State *L)
{
    CxImage *img = get_ximage(L,1);
    ASSERT(img);
	LUA_TO_LOCAL_STRING(filename,L,2);
    int _ret_0 = (int)CxImageHelper::SaveImage(img,filename);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximagehelper_saveimage_v2(lua_State *L)
{
    CxImage *img = get_ximage(L,1);
    ASSERT(img);
    const char* type = (const char*)lua_tostring(L,2);
    ASSERT(type);
    CFileBase *dst = get_filebase(L,3);
    ASSERT(dst);
    int _ret_0 = (int)CxImageHelper::SaveImage(img,type,dst);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximagehelper_saveimage(lua_State *L)
{
    if(is_filebase(L,3))
        return ximagehelper_saveimage_v2(L);
    else
        return ximagehelper_saveimage_v1(L);
}
static int ximagehelper_loadimage_v1(lua_State *L)
{
	LUA_TO_LOCAL_STRING(filename,L,1);
    CxImage *out = get_ximage(L,2);
    ASSERT(out);
    int _ret_0 = (int)CxImageHelper::LoadImage(filename,out);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximagehelper_loadimage_v2(lua_State *L)
{
    CFileBase *file = get_filebase(L,1);
    ASSERT(file);
    CxImage *out = get_ximage(L,2);
    ASSERT(out);
    int _ret_0 = (int)CxImageHelper::LoadImage(file,out);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int ximagehelper_loadimage(lua_State *L)
{
    if(lua_isstring(L,1))
        return ximagehelper_loadimage_v1(L);
    else
        return ximagehelper_loadimage_v2(L);
}
static int ximagehelper_convert9png(lua_State *L)
{
    CxImage *nineImg = get_ximage(L,1);
    ASSERT(nineImg);
    CxImage *outImg = get_ximage(L,2);
    ASSERT(outImg);

    int info[256];
    int info_size = 256;
    memset(info,0,sizeof(info));

    CxImageHelper::Convert9Png(nineImg,outImg,info,&info_size);
        
    if(info_size == 0)
        return 0;

    lua_newtable(L);
    int top = lua_gettop(L);

    for(int i = 0; i < info_size; i++)
    {
        lua_pushinteger(L,i+1);
        lua_pushinteger(L,info[i]);
        lua_settable(L, top);
    }

    return 1;
}
static int ximagehelper_calcusimilarity(lua_State *L)
{
    CxImage *img1 = get_ximage(L,1);
    ASSERT(img1);
    CxImage *img2 = get_ximage(L,2);
    ASSERT(img2);
    int threshold = (int)lua_tointeger(L,3);
    CxImage *diff_img = get_ximage(L,4);
    ASSERT(diff_img);
    float _ret_0 = (float)CxImageHelper::CalcuSimilarity(img1,img2,threshold,diff_img);
    lua_pushnumber(L,_ret_0);
    return 1;
}

#if CXIMAGE_SUPPORT_WINDOWS
static status_t ximagehelper_createimagefromhdc(lua_State *L)
{
    CxImage *img = get_ximage(L,1);
    ASSERT(img);
    int nxorigindest = (int)lua_tointeger(L,2);
    int nyorigindest = (int)lua_tointeger(L,3);
    int nwidthdest = (int)lua_tointeger(L,4);
    int nheightdest = (int)lua_tointeger(L,5);
    CLuaHdc *hdcsrc = get_luahdc(L,6);
    ASSERT(hdcsrc);
    int nxoriginsrc = (int)lua_tointeger(L,7);
    int nyoriginsrc = (int)lua_tointeger(L,8);
    int nwidthsrc = (int)lua_tointeger(L,9);
    int nheightsrc = (int)lua_tointeger(L,10);
    status_t ret0 = CxImageHelper::CreateImageFromHdc(img,nxorigindest,nyorigindest,nwidthdest,nheightdest,hdcsrc->hdc,nxoriginsrc,nyoriginsrc,nwidthsrc,nheightsrc);
    lua_pushboolean(L,ret0);
    return 1;
}
#endif

static const luaL_Reg ximagehelper_lib[] = {
    {"ImageToRawRgba",ximagehelper_imagetorawrgba},
    {"SaveImage",ximagehelper_saveimage},
    {"LoadImage",ximagehelper_loadimage},
    {"Convert9Png",ximagehelper_convert9png},    
	{"CalcuSimilarity",ximagehelper_calcusimilarity},

#if CXIMAGE_SUPPORT_WINDOWS
	{"CreateImageFromHdc",ximagehelper_createimagefromhdc},
#endif

    {NULL, NULL}
};
static int luaL_register_ximagehelper(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_XIMAGEHELPER);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,ximagehelper_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,ximagehelper_lib);
    return 1;
}

int luaopen_ximagehelper(lua_State *L)
{
    luaL_requiref(L, "XImageHelper",luaL_register_ximagehelper,1);
    lua_pop(L, 1);
    return 0;
}
