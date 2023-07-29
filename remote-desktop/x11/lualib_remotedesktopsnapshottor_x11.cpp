#include "lualib_remotedesktopsnapshottor_x11.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_remotedesktopsnapshottor.h"
#include "lualib_xdisplay.h"
#include "lualib_xwindow.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopSnapshottor_X11,remotedesktopsnapshottor_x11)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopSnapshottor_X11,remotedesktopsnapshottor_x11)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopSnapshottor_X11,remotedesktopsnapshottor_x11,REMOTEDESKTOPSNAPSHOTTOR_X11)
LUA_GC_FUNC(CRemoteDesktopSnapshottor_X11,remotedesktopsnapshottor_x11)
LUA_IS_SAME_FUNC(CRemoteDesktopSnapshottor_X11,remotedesktopsnapshottor_x11)
LUA_TO_STRING_FUNC(CRemoteDesktopSnapshottor_X11,remotedesktopsnapshottor_x11)

bool is_remotedesktopsnapshottor_x11(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_X11,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktopsnapshottor_x11_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktopsnapshottor_x11_new(lua_State *L)
{
    CRemoteDesktopSnapshottor_X11 *premotedesktopsnapshottor_x11;
    NEW(premotedesktopsnapshottor_x11,CRemoteDesktopSnapshottor_X11);
    ASSERT(how_to_get_global_taskmgr);
    premotedesktopsnapshottor_x11->Init(how_to_get_global_taskmgr(L));
    remotedesktopsnapshottor_x11_new_userdata(L,premotedesktopsnapshottor_x11,0);
    return 1;
}

static status_t remotedesktopsnapshottor_x11_setxdisplay(lua_State *L)
{
    CRemoteDesktopSnapshottor_X11 *premotedesktopsnapshottor_x11 = get_remotedesktopsnapshottor_x11(L,1);
    ASSERT(premotedesktopsnapshottor_x11);
    CxDisplay *xdisplay = get_xdisplay(L,2);
    ASSERT(xdisplay);
    status_t ret0 = premotedesktopsnapshottor_x11->SetXDisplay(xdisplay);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_x11_setxwindow(lua_State *L)
{
    CRemoteDesktopSnapshottor_X11 *premotedesktopsnapshottor_x11 = get_remotedesktopsnapshottor_x11(L,1);
    ASSERT(premotedesktopsnapshottor_x11);
    CxWindow *xwindow = get_xwindow(L,2);
    ASSERT(xwindow);
    status_t ret0 = premotedesktopsnapshottor_x11->SetXWindow(xwindow);
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg remotedesktopsnapshottor_x11_funcs_[] = {
    {"__gc",remotedesktopsnapshottor_x11_gc_},
    {"__tostring",remotedesktopsnapshottor_x11_tostring_},
    {"__is_same",remotedesktopsnapshottor_x11_issame_},
    {"new",remotedesktopsnapshottor_x11_new},
    {"SetXDisplay",remotedesktopsnapshottor_x11_setxdisplay},
    {"SetXWindow",remotedesktopsnapshottor_x11_setxwindow},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktopsnapshottor_x11_funcs()
{
    return remotedesktopsnapshottor_x11_funcs_;
}

static int luaL_register_remotedesktopsnapshottor_x11(lua_State *L)
{	
    static luaL_Reg _remotedesktopsnapshottor_x11_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktopsnapshottor_x11_funcs_,&_index,get_remotedesktopsnapshottor_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_remotedesktopsnapshottor_x11_funcs_,&_index,get_remotedesktopsnapshottor_x11_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_X11);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktopsnapshottor_x11_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktopsnapshottor_x11_funcs_);
    return 1;
}        

int luaopen_remotedesktopsnapshottor_x11(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopSnapshottor_X11",luaL_register_remotedesktopsnapshottor_x11,1);
    lua_pop(L, 1);
    return 0;
}        

