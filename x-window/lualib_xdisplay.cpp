#include "mem_tool.h"
#include "syslog.h"
#include "lualib_xevent.h"
#include "lualib_xdisplay.h"
#include "lualib_xwindow.h"

LUA_IS_VALID_USER_DATA_FUNC(CxDisplay,xdisplay)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CxDisplay,xdisplay)
LUA_NEW_USER_DATA_FUNC(CxDisplay,xdisplay,XDISPLAY)
LUA_GC_FUNC(CxDisplay,xdisplay)
LUA_IS_SAME_FUNC(CxDisplay,xdisplay)
LUA_TO_STRING_FUNC(CxDisplay,xdisplay)

bool is_xdisplay(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XDISPLAY,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return xdisplay_is_userdata_valid(ud);  
}

/****************************************/
static int xdisplay_new(lua_State *L)
{
    CxDisplay *pt;
    NEW(pt,CxDisplay);
    pt->Init();
    xdisplay_new_userdata(L,pt,0);
    return 1;
}

static int xdisplay_defaultscreennumber(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int _ret_0 = (int)pxdisplay->DefaultScreenNumber();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xdisplay_installdefaulterrorhandler(lua_State *L)
{
    int _ret_0 = (int)CxDisplay::InstallDefaultErrorHandler();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_flush(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int _ret_0 = (int)pxdisplay->Flush();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_getatombyname(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    const char* name = (const char*)lua_tostring(L,2);
    int only_if_exist = (int)lua_toboolean(L,3);
    uint32_t _ret_0 = (uint32_t)pxdisplay->GetAtomByName(name,only_if_exist);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xdisplay_getdefaultrootwindow(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);

    CxWindow *win;
    NEW(win,CxWindow);
    win->Init();

    if(pxdisplay->GetDefaultRootWindow(win))
    {
        xwindow_new_userdata(L,win,0);
        return 1;
    }
    else
    {
        DEL(win);
        return 0;
    }
}
static int xdisplay_closedisplay(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int _ret_0 = (int)pxdisplay->CloseDisplay();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_opendisplay(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    const char* name = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pxdisplay->OpenDisplay(name);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_getnativexdisplay(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    NativeXDisplay _ret_0 = pxdisplay->GetNativeXDisplay();
    lua_pushinteger(L,(int_ptr_t)_ret_0);
    return 1;
}
static int xdisplay_setnativexdisplay(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int_ptr_t _nativexdisplay = (int_ptr_t)lua_tointeger(L,2);
    int _ret_0 = (int)pxdisplay->SetNativeXDisplay((NativeXDisplay)_nativexdisplay);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_attach(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    NativeXDisplay _nativexdisplay = (NativeXDisplay)lua_tointeger(L,2);
    int _ret_0 = (int)pxdisplay->Attach(_nativexdisplay);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_isattached(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int _ret_0 = (int)pxdisplay->IsAttached();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_getactivewindow(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);

    CxWindow *pwin;
    NEW(pwin,CxWindow);
    pwin->Init();

    if(pxdisplay->GetActiveWindow(pwin))
    {
        xwindow_new_userdata(L,pwin,0);
        return 1;
    }

    return 0;
}
static int xdisplay_peekevent(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    CxEvent *event = get_xevent(L,2);
    ASSERT(event);
    int _ret_0 = (int)pxdisplay->PeekEvent(event->GetNativeXEvent());
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_nextevent(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    CxEvent *event = get_xevent(L,2);
    ASSERT(event);
    int _ret_0 = (int)pxdisplay->NextEvent(event->GetNativeXEvent());
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_pending(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int _ret_0 = (int)pxdisplay->Pending();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static status_t xdisplay_getscreencount(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int ret0 = pxdisplay->GetScreenCount();
    lua_pushinteger(L,ret0);
    return 1;
}
static status_t xdisplay_getwidth(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int sceen_number = (int)lua_tointeger(L,2);
    int ret0 = pxdisplay->GetWidth(sceen_number);
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t xdisplay_getheight(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    int screen_number = (int)lua_tointeger(L,2);
    int ret0 = pxdisplay->GetHeight(screen_number);
    lua_pushinteger(L,ret0);
    return 1;
}

static const luaL_Reg xdisplay_lib[] = {
    {"__gc",xdisplay_gc_},
    {"__tostring",xdisplay_tostring_},
    {"__is_same",xdisplay_issame_},
    {"new",xdisplay_new},
    {"DefaultScreenNumber",xdisplay_defaultscreennumber},
    {"InstallDefaultErrorHandler",xdisplay_installdefaulterrorhandler},
    {"Flush",xdisplay_flush},
    {"GetAtomByName",xdisplay_getatombyname},
    {"GetDefaultRootWindow",xdisplay_getdefaultrootwindow},
    {"CloseDisplay",xdisplay_closedisplay},
    {"OpenDisplay",xdisplay_opendisplay},
    {"GetNativeXDisplay",xdisplay_getnativexdisplay},
    {"SetNativeXDisplay",xdisplay_setnativexdisplay},
    {"Attach",xdisplay_attach},
    {"IsAttached",xdisplay_isattached},
    {"GetActiveWindow",xdisplay_getactivewindow},
    {"PeekEvent",xdisplay_peekevent},
    {"NextEvent",xdisplay_nextevent},
    {"Pending",xdisplay_pending},
    {"GetScreenCount",xdisplay_getscreencount},    
    {"GetWidth",xdisplay_getwidth},
    {"GetHeight",xdisplay_getheight},    
    {NULL, NULL}
};
static int luaL_register_xdisplay(lua_State *L)
{
    luaL_newmetatable(L, LUA_USERDATA_XDISPLAY);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L,xdisplay_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,xdisplay_lib);
    return 1;
}

int luaopen_xdisplay(lua_State *L)
{
    luaL_requiref(L, "XDisplay",luaL_register_xdisplay,1);
    lua_pop(L, 1);
    return 0;
}
