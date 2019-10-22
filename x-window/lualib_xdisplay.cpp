#include "mem_tool.h"
#include "syslog.h"
#include "lualib_xevent.h"
#include "lualib_xdisplay.h"
#include "lualib_xwindow.h"

CxDisplay *get_xdisplay(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_XDISPLAY);
    ASSERT(ud && ud->p);
    ASSERT(ud->__weak_ref_id != 0);
    CxDisplay *p = (CxDisplay*)ud->p;
    ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
    return p;
}
lua_userdata *xdisplay_new_userdata(lua_State *L,CxDisplay *pt,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pt);
    ud->p = pt;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pt->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_XDISPLAY);
    lua_setmetatable(L,-2);
    return ud;
}

static int xdisplay_new(lua_State *L)
{
    CxDisplay *pt;
    NEW(pt,CxDisplay);
    pt->Init();
    xdisplay_new_userdata(L,pt,0);
    return 1;
}

static bool xdisplay_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CxDisplay *p = (CxDisplay*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}
static int xdisplay_destroy(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_XDISPLAY);
    if(!xdisplay_is_userdata_valid(ud))
        return 0;
    CxDisplay *pxdisplay = (CxDisplay*)ud->p;
    if(!(ud->is_attached))
    {
        DEL(pxdisplay);
    }
    return 0;
}
static int xdisplay_issame(lua_State *L)
{
    CxDisplay *pxdisplay1 = get_xdisplay(L,1);
    ASSERT(pxdisplay1);
    CxDisplay *pxdisplay2 = get_xdisplay(L,2);
    ASSERT(pxdisplay2);
    int is_same = (pxdisplay1==pxdisplay2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int xdisplay_tostring(lua_State *L)
{
    lua_pushstring(L,"userdata:xdisplay");
    return 1;
}
/****************************************/
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
    CXEvent *event = get_xevent(L,2);
    ASSERT(event);
    int _ret_0 = (int)pxdisplay->PeekEvent(event->GetNativeXEvent());
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xdisplay_nextevent(lua_State *L)
{
    CxDisplay *pxdisplay = get_xdisplay(L,1);
    ASSERT(pxdisplay);
    CXEvent *event = get_xevent(L,2);
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

static const luaL_Reg xdisplay_lib[] = {
    {"new",xdisplay_new},
    {"__gc",xdisplay_destroy},
    {"__tostring",xdisplay_tostring},
    {"IsSame",xdisplay_issame},
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
