#include "lualib_remotedesktoppixelbuffer.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_ximage.h"

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopPixelBuffer,remotedesktoppixelbuffer)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopPixelBuffer,remotedesktoppixelbuffer)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopPixelBuffer,remotedesktoppixelbuffer,REMOTEDESKTOPPIXELBUFFER)
LUA_GC_FUNC(CRemoteDesktopPixelBuffer,remotedesktoppixelbuffer)
LUA_IS_SAME_FUNC(CRemoteDesktopPixelBuffer,remotedesktoppixelbuffer)
LUA_TO_STRING_FUNC(CRemoteDesktopPixelBuffer,remotedesktoppixelbuffer)

bool is_remotedesktoppixelbuffer(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPPIXELBUFFER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktoppixelbuffer_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktoppixelbuffer_new(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer;
    NEW(premotedesktoppixelbuffer,CRemoteDesktopPixelBuffer);
    premotedesktoppixelbuffer->Init();
    remotedesktoppixelbuffer_new_userdata(L,premotedesktoppixelbuffer,0);
    return 1;
}

static status_t remotedesktoppixelbuffer_destroy(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    status_t ret0 = premotedesktoppixelbuffer->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_copy(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    CRemoteDesktopPixelBuffer *p = get_remotedesktoppixelbuffer(L,2);
    ASSERT(p);
    status_t ret0 = premotedesktoppixelbuffer->Copy(p);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getpixeldata(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int_ptr_t ret0 = (int_ptr_t)premotedesktoppixelbuffer->GetPixelData();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getpixeldatasize(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int ret0 = premotedesktoppixelbuffer->GetPixelDataSize();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getbyteperpixel(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    uint8_t ret0 = premotedesktoppixelbuffer->GetBytePerPixel();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getwidth(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int ret0 = premotedesktoppixelbuffer->GetWidth();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getheight(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int ret0 = premotedesktoppixelbuffer->GetHeight();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getoffsetx(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int ret0 = premotedesktoppixelbuffer->GetOffsetX();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getoffsety(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int ret0 = premotedesktoppixelbuffer->GetOffsetY();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getflags(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    uint32_t ret0 = premotedesktoppixelbuffer->GetFlags();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_allocpixeldata(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int _len = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktoppixelbuffer->AllocPixelData(_len);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_setwidth(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int _width = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktoppixelbuffer->SetWidth(_width);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_setheight(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int _height = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktoppixelbuffer->SetHeight(_height);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_setoffsetx(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int _offset_x = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktoppixelbuffer->SetOffsetX(_offset_x);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_setoffsety(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int _offset_y = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktoppixelbuffer->SetOffsetY(_offset_y);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_setflags(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    uint32_t _flags = (uint32_t)lua_tointeger(L,2);
    status_t ret0 = premotedesktoppixelbuffer->SetFlags(_flags);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_create(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int width = (int)lua_tointeger(L,2);
    int height = (int)lua_tointeger(L,3);
    uint8_t bpp = (uint8_t)lua_tointeger(L,4);
    status_t ret0 = premotedesktoppixelbuffer->Create(width,height,bpp);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_getpixelbits(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int_ptr_t ret0 = (int_ptr_t)premotedesktoppixelbuffer->GetPixelBits(x,y);
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_crop(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int w = (int)lua_tointeger(L,4);
    int h = (int)lua_tointeger(L,5);
    CRemoteDesktopPixelBuffer *out = get_remotedesktoppixelbuffer(L,6);
    ASSERT(out);
    status_t ret0 = premotedesktoppixelbuffer->Crop(x,y,w,h,out);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_combine(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    CRemoteDesktopPixelBuffer *pixelbuf = get_remotedesktoppixelbuffer(L,2);
    ASSERT(pixelbuf);
    status_t ret0 = premotedesktoppixelbuffer->Combine(pixelbuf);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_changebpp_v1(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    uint8_t bpp = (uint8_t)lua_tointeger(L,2);
    CRemoteDesktopPixelBuffer *out = get_remotedesktoppixelbuffer(L,3);
    ASSERT(out);
    status_t ret0 = premotedesktoppixelbuffer->ChangeBpp(bpp,out);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_changebpp_v2(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    uint8_t bpp = (uint8_t)lua_tointeger(L,2);
    status_t ret0 = premotedesktoppixelbuffer->ChangeBpp(bpp);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_savetoximage_v1(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    CxImage *img = get_ximage(L,2);
    ASSERT(img);
    status_t ret0 = premotedesktoppixelbuffer->SaveToXimage(img);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_savetoximage_v2(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    const char* fn = (const char*)lua_tostring(L,2);
    ASSERT(fn);
    status_t ret0 = premotedesktoppixelbuffer->SaveToXimage(fn);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_loadfromximage_v1(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    CxImage *img = get_ximage(L,2);
    ASSERT(img);
    status_t ret0 = premotedesktoppixelbuffer->LoadFromXimage(img);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_loadfromximage_v2(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    const char* fn = (const char*)lua_tostring(L,2);
    ASSERT(fn);
    status_t ret0 = premotedesktoppixelbuffer->LoadFromXimage(fn);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_issame(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    CRemoteDesktopPixelBuffer *pixelbuf = get_remotedesktoppixelbuffer(L,2);
    ASSERT(pixelbuf);
    bool ret0 = premotedesktoppixelbuffer->IsSame(pixelbuf);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_compressdata(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    status_t ret0 = premotedesktoppixelbuffer->CompressData();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_decompressdata(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    status_t ret0 = premotedesktoppixelbuffer->DecompressData();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_transfer(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    CRemoteDesktopPixelBuffer *from = get_remotedesktoppixelbuffer(L,2);
    ASSERT(from);
    status_t ret0 = premotedesktoppixelbuffer->Transfer(from);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_ghost(lua_State *L)
{
    CRemoteDesktopPixelBuffer *premotedesktoppixelbuffer = get_remotedesktoppixelbuffer(L,1);
    ASSERT(premotedesktoppixelbuffer);
    CRemoteDesktopPixelBuffer *from = get_remotedesktoppixelbuffer(L,2);
    ASSERT(from);
    status_t ret0 = premotedesktoppixelbuffer->Ghost(from);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktoppixelbuffer_changebpp(lua_State *L)
{
    if(lua_isinteger(L,2) && is_remotedesktoppixelbuffer(L,3))
    {
        return remotedesktoppixelbuffer_changebpp_v1(L);
    }

    if(lua_isinteger(L,2))
    {
        return remotedesktoppixelbuffer_changebpp_v2(L);
    }

    return 0;
}

static status_t remotedesktoppixelbuffer_savetoximage(lua_State *L)
{
    if(lua_isstring(L,2))
    {
        return remotedesktoppixelbuffer_savetoximage_v2(L);
    }

    if(is_ximage(L,2))
    {
        return remotedesktoppixelbuffer_savetoximage_v1(L);
    }

    return 0;
}

static status_t remotedesktoppixelbuffer_loadfromximage(lua_State *L)
{
    if(lua_isstring(L,2))
    {
        return remotedesktoppixelbuffer_loadfromximage_v2(L);
    }

    if(is_ximage(L,2))
    {
        return remotedesktoppixelbuffer_loadfromximage_v1(L);
    }

    return 0;
}

/****************************************************/
static const luaL_Reg remotedesktoppixelbuffer_funcs_[] = {
    {"__gc",remotedesktoppixelbuffer_gc_},
    {"__tostring",remotedesktoppixelbuffer_tostring_},
    {"__is_same",remotedesktoppixelbuffer_issame_},
    {"Crop",remotedesktoppixelbuffer_crop},
    {"Create",remotedesktoppixelbuffer_create},
    {"ChangeBpp",remotedesktoppixelbuffer_changebpp},
    {"GetPixelBits",remotedesktoppixelbuffer_getpixelbits},
    {"IsSame",remotedesktoppixelbuffer_issame},
    {"SetFlags",remotedesktoppixelbuffer_setflags},
    {"Transfer",remotedesktoppixelbuffer_transfer},
    {"Ghost",remotedesktoppixelbuffer_ghost},
    {"SetOffsetY",remotedesktoppixelbuffer_setoffsety},
    {"Combine",remotedesktoppixelbuffer_combine},
    {"SaveToXimage",remotedesktoppixelbuffer_savetoximage},
    {"LoadFromXimage",remotedesktoppixelbuffer_loadfromximage},
    {"Copy",remotedesktoppixelbuffer_copy},
    {"AllocPixelData",remotedesktoppixelbuffer_allocpixeldata},
    {"SetWidth",remotedesktoppixelbuffer_setwidth},
    {"SetOffsetX",remotedesktoppixelbuffer_setoffsetx},
    {"SetHeight",remotedesktoppixelbuffer_setheight},
    {"CompressData",remotedesktoppixelbuffer_compressdata},
    {"GetOffsetX",remotedesktoppixelbuffer_getoffsetx},
    {"GetPixelData",remotedesktoppixelbuffer_getpixeldata},
    {"GetPixelDataSize",remotedesktoppixelbuffer_getpixeldatasize},
    {"DecompressData",remotedesktoppixelbuffer_decompressdata},
    {"GetOffsetY",remotedesktoppixelbuffer_getoffsety},
    {"GetWidth",remotedesktoppixelbuffer_getwidth},
    {"Destroy",remotedesktoppixelbuffer_destroy},
    {"GetBytePerPixel",remotedesktoppixelbuffer_getbyteperpixel},
    {"GetHeight",remotedesktoppixelbuffer_getheight},
    {"GetFlags",remotedesktoppixelbuffer_getflags},
    {"new",remotedesktoppixelbuffer_new},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktoppixelbuffer_funcs()
{
    return remotedesktoppixelbuffer_funcs_;
}

static int luaL_register_remotedesktoppixelbuffer(lua_State *L)
{	
    static luaL_Reg _remotedesktoppixelbuffer_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktoppixelbuffer_funcs_,&_index,get_remotedesktoppixelbuffer_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPPIXELBUFFER);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktoppixelbuffer_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktoppixelbuffer_funcs_);
    return 1;
}        

int luaopen_remotedesktoppixelbuffer(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopPixelBuffer",luaL_register_remotedesktoppixelbuffer,1);
    lua_pop(L, 1);
    return 0;
}        

