#include "lualib_xevent.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CxEvent,xevent)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CxEvent,xevent)
LUA_NEW_USER_DATA_FUNC(CxEvent,xevent,XEVENT)
LUA_GC_FUNC(CxEvent,xevent)
LUA_IS_SAME_FUNC(CxEvent,xevent)
LUA_TO_STRING_FUNC(CxEvent,xevent)

bool is_xevent(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XEVENT,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return xevent_is_userdata_valid(ud);  
}
/****************************************/
static int xevent_new(lua_State *L)
{
    CxEvent *pt;
    NEW(pt,CxEvent);
    pt->Init();
    xevent_new_userdata(L,pt,0);
    return 1;
}

static int xevent_copy(lua_State *L)
{
    CxEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    CxEvent *e = get_xevent(L,2);
    ASSERT(e);
    int _ret_0 = (int)pxevent->Copy(e);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xevent_gettype(lua_State *L)
{
    CxEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    int _ret_0 = (int)pxevent->GetType();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getserial(lua_State *L)
{
    CxEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetSerial();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getsendevent(lua_State *L)
{
    CxEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetSendEvent();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getwindow(lua_State *L)
{
    CxEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetWindow();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getrootwindow(lua_State *L)
{
    CxEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetRootWindow();
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int xevent_getraw(lua_State *L)
{
    CxEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    lua_pushlstring(L,
        (const char*)pxevent->GetNativeXEvent(),
        pxevent->Size()
    );    
    return 1;
}

static const luaL_Reg xevent_lib[] = {
    {"__gc",xevent_gc_},
    {"__tostring",xevent_tostring_},
    {"__is_same",xevent_issame_},
    {"new",xevent_new},
    {"Copy",xevent_copy},
    {"GetType",xevent_gettype},
    {"GetSerial",xevent_getserial},
    {"GetSendEvent",xevent_getsendevent},
    {"GetWindow",xevent_getwindow},
    {"GetRootWindow",xevent_getrootwindow},
    {"GetRaw",xevent_getraw},
    {NULL, NULL}
};
static int luaL_register_xevent(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_XEVENT);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,xevent_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,xevent_lib);
    return 1;
}

int luaopen_xevent(lua_State *L)
{
    luaL_requiref(L, "XEvent",luaL_register_xevent,1);
    lua_pop(L, 1);
    return 0;
}
