#include "lualib_remotedesktopsnapshottor_gdi.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_remotedesktoppixelbuffer.h"
#include "lualib_luahdc.h"
#include "lualib_remotedesktopsnapshottor.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopSnapshottor_GDI,remotedesktopsnapshottor_gdi)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopSnapshottor_GDI,remotedesktopsnapshottor_gdi)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopSnapshottor_GDI,remotedesktopsnapshottor_gdi,REMOTEDESKTOPSNAPSHOTTOR_GDI)
LUA_GC_FUNC(CRemoteDesktopSnapshottor_GDI,remotedesktopsnapshottor_gdi)
LUA_IS_SAME_FUNC(CRemoteDesktopSnapshottor_GDI,remotedesktopsnapshottor_gdi)
LUA_TO_STRING_FUNC(CRemoteDesktopSnapshottor_GDI,remotedesktopsnapshottor_gdi)

bool is_remotedesktopsnapshottor_gdi(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_GDI,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktopsnapshottor_gdi_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktopsnapshottor_gdi_new(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi;
    NEW(premotedesktopsnapshottor_gdi,CRemoteDesktopSnapshottor_GDI);
	ASSERT(how_to_get_global_taskmgr);
    premotedesktopsnapshottor_gdi->Init(how_to_get_global_taskmgr(L));
    remotedesktopsnapshottor_gdi_new_userdata(L,premotedesktopsnapshottor_gdi,0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_getsrcoriginx(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int ret0 = premotedesktopsnapshottor_gdi->GetSrcOriginX();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_getsrcoriginy(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int ret0 = premotedesktopsnapshottor_gdi->GetSrcOriginY();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_getsrcwidth(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int ret0 = premotedesktopsnapshottor_gdi->GetSrcWidth();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_getsrcheight(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int ret0 = premotedesktopsnapshottor_gdi->GetSrcHeight();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_getdstwidth(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int ret0 = premotedesktopsnapshottor_gdi->GetDstWidth();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_getdstheight(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int ret0 = premotedesktopsnapshottor_gdi->GetDstHeight();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_setsrcoriginx(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int _src_origin_x = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor_gdi->SetSrcOriginX(_src_origin_x);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_setsrcoriginy(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int _src_origin_y = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor_gdi->SetSrcOriginY(_src_origin_y);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_setsrcwidth(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int _src_width = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor_gdi->SetSrcWidth(_src_width);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_setsrcheight(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int _src_height = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor_gdi->SetSrcHeight(_src_height);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_setdstwidth(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int _dst_width = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor_gdi->SetDstWidth(_dst_width);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_setdstheight(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    int _dst_height = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor_gdi->SetDstHeight(_dst_height);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_createbitmapbuffer(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    status_t ret0 = premotedesktopsnapshottor_gdi->CreateBitmapBuffer();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_releasebitmapbuffer(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    status_t ret0 = premotedesktopsnapshottor_gdi->ReleaseBitmapBuffer();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_takesnapshot(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    CRemoteDesktopPixelBuffer *pixelbuf = get_remotedesktoppixelbuffer(L,2);
    ASSERT(pixelbuf);
    status_t ret0 = premotedesktopsnapshottor_gdi->TakeSnapshot(pixelbuf);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_gdi_setsrchdc(lua_State *L)
{
    CRemoteDesktopSnapshottor_GDI *premotedesktopsnapshottor_gdi = get_remotedesktopsnapshottor_gdi(L,1);
    ASSERT(premotedesktopsnapshottor_gdi);
    CLuaHdc *hdc = get_luahdc(L,2);
    ASSERT(hdc);
    status_t ret0 = premotedesktopsnapshottor_gdi->SetSrcHdc(hdc->hdc);
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg remotedesktopsnapshottor_gdi_funcs_[] = {
    {"__gc",remotedesktopsnapshottor_gdi_gc_},
    {"__tostring",remotedesktopsnapshottor_gdi_tostring_},
    {"__is_same",remotedesktopsnapshottor_gdi_issame_},
	{"new",remotedesktopsnapshottor_gdi_new},
    {"GetSrcOriginX",remotedesktopsnapshottor_gdi_getsrcoriginx},
    {"GetSrcOriginY",remotedesktopsnapshottor_gdi_getsrcoriginy},
    {"GetSrcWidth",remotedesktopsnapshottor_gdi_getsrcwidth},
    {"GetSrcHeight",remotedesktopsnapshottor_gdi_getsrcheight},
    {"GetDstWidth",remotedesktopsnapshottor_gdi_getdstwidth},
    {"GetDstHeight",remotedesktopsnapshottor_gdi_getdstheight},
    {"SetSrcOriginX",remotedesktopsnapshottor_gdi_setsrcoriginx},
    {"SetSrcOriginY",remotedesktopsnapshottor_gdi_setsrcoriginy},
    {"SetSrcWidth",remotedesktopsnapshottor_gdi_setsrcwidth},
    {"SetSrcHeight",remotedesktopsnapshottor_gdi_setsrcheight},
    {"SetDstWidth",remotedesktopsnapshottor_gdi_setdstwidth},
    {"SetDstHeight",remotedesktopsnapshottor_gdi_setdstheight},
    {"CreateBitmapBuffer",remotedesktopsnapshottor_gdi_createbitmapbuffer},
    {"ReleaseBitmapBuffer",remotedesktopsnapshottor_gdi_releasebitmapbuffer},
    {"TakeSnapshot",remotedesktopsnapshottor_gdi_takesnapshot},
    {"SetSrcHdc",remotedesktopsnapshottor_gdi_setsrchdc},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktopsnapshottor_gdi_funcs()
{
    return remotedesktopsnapshottor_gdi_funcs_;
}

static int luaL_register_remotedesktopsnapshottor_gdi(lua_State *L)
{	
    static luaL_Reg _remotedesktopsnapshottor_gdi_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktopsnapshottor_gdi_funcs_,&_index,get_remotedesktopsnapshottor_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_remotedesktopsnapshottor_gdi_funcs_,&_index,get_remotedesktopsnapshottor_gdi_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_GDI);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktopsnapshottor_gdi_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktopsnapshottor_gdi_funcs_);
    return 1;
}        

int luaopen_remotedesktopsnapshottor_gdi(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopSnapshottor_GDI",luaL_register_remotedesktopsnapshottor_gdi,1);
    lua_pop(L, 1);
    return 0;
}        

