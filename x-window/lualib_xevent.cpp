#include "lualib_xevent.h"
#include "lualib_stream.h"
#include "mem_tool.h"
#include "syslog.h"

CXEvent *get_xevent(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_XEVENT);
    ASSERT(ud && ud->p);
    ASSERT(ud->__weak_ref_id != 0);
    CXEvent *p = (CXEvent*)ud->p;
    ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
    return p;
}
lua_userdata *xevent_new_userdata(lua_State *L,CXEvent *pt,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pt);
    ud->p = pt;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pt->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_XEVENT);
    lua_setmetatable(L,-2);
    return ud;
}

static int xevent_new(lua_State *L)
{
    CXEvent *pt;
    NEW(pt,CXEvent);
    pt->Init();
    xevent_new_userdata(L,pt,0);
    return 1;
}

static bool xevent_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CXEvent *p = (CXEvent*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}
static int xevent_destroy(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_XEVENT);
    if(!xevent_is_userdata_valid(ud))
        return 0;
    CXEvent *pxevent = (CXEvent*)ud->p;
    if(!(ud->is_attached))
    {
        DEL(pxevent);
    }
    return 0;
}
static int xevent_issame(lua_State *L)
{
    CXEvent *pxevent1 = get_xevent(L,1);
    ASSERT(pxevent1);
    CXEvent *pxevent2 = get_xevent(L,2);
    ASSERT(pxevent2);
    int is_same = (pxevent1==pxevent2);
    lua_pushboolean(L,is_same);
    return 1;
}
static int xevent_tostring(lua_State *L)
{
    lua_pushstring(L,"userdata:xevent");
    return 1;
}
/****************************************/
static int xevent_copy(lua_State *L)
{
    CXEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    CXEvent *e = get_xevent(L,2);
    ASSERT(e);
    int _ret_0 = (int)pxevent->Copy(e);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xevent_gettype(lua_State *L)
{
    CXEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    int _ret_0 = (int)pxevent->GetType();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getserial(lua_State *L)
{
    CXEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetSerial();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getsendevent(lua_State *L)
{
    CXEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetSendEvent();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getwindow(lua_State *L)
{
    CXEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetWindow();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_getrootwindow(lua_State *L)
{
    CXEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    uint32_t _ret_0 = (uint32_t)pxevent->GetRootWindow();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xevent_tostream(lua_State *L)
{
    CXEvent *pxevent = get_xevent(L,1);
    ASSERT(pxevent);
    CStream *stream = get_stream(L,2);
    ASSERT(stream);

    if(stream->GetSize() < pxevent->Size())
    {
        stream->Realloc(pxevent->Size());
    }
    
    stream->Seek(0);
    stream->Write(pxevent->GetNativeXEvent(),pxevent->Size());
    stream->Seek(0);

    return 0;
}
static const luaL_Reg xevent_lib[] = {
    {"new",xevent_new},
    {"__gc",xevent_destroy},
    {"__tostring",xevent_tostring},
    {"IsSame",xevent_issame},
    {"Copy",xevent_copy},
    {"GetType",xevent_gettype},
    {"GetSerial",xevent_getserial},
    {"GetSendEvent",xevent_getsendevent},
    {"GetWindow",xevent_getwindow},
    {"GetRootWindow",xevent_getrootwindow},
    {"ToStream",xevent_tostream},
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
