#include "lualib_remotedesktopserver.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_remotedesktopsnapshottormanager.h"
#include "globals.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopServer,remotedesktopserver)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopServer,remotedesktopserver)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopServer,remotedesktopserver,REMOTEDESKTOPSERVER)
LUA_GC_FUNC(CRemoteDesktopServer,remotedesktopserver)
LUA_IS_SAME_FUNC(CRemoteDesktopServer,remotedesktopserver)
LUA_TO_STRING_FUNC(CRemoteDesktopServer,remotedesktopserver)

bool is_remotedesktopserver(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPSERVER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktopserver_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktopserver_new(lua_State *L)
{
    CRemoteDesktopServer *premotedesktopserver;
    NEW(premotedesktopserver,CRemoteDesktopServer);
	ASSERT(how_to_get_global_taskmgr);
    premotedesktopserver->Init(how_to_get_global_taskmgr(L));
    remotedesktopserver_new_userdata(L,premotedesktopserver,0);
    return 1;
}

static status_t remotedesktopserver_setremotedesktopsnapshottormanager(lua_State *L)
{
    CRemoteDesktopServer *premotedesktopserver = get_remotedesktopserver(L,1);
    ASSERT(premotedesktopserver);
    CRemoteDesktopSnapshottorManager *mgr = get_remotedesktopsnapshottormanager(L,2);
    ASSERT(mgr);
    status_t ret0 = premotedesktopserver->SetRemoteDesktopSnapshottorManager(mgr);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopserver_destroy(lua_State *L)
{
    CRemoteDesktopServer *premotedesktopserver = get_remotedesktopserver(L,1);
    ASSERT(premotedesktopserver);
    status_t ret0 = premotedesktopserver->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopserver_initservicesidepeer(lua_State *L)
{
    CRemoteDesktopServer *premotedesktopserver = get_remotedesktopserver(L,1);
    ASSERT(premotedesktopserver);
	ASSERT(how_to_get_peer_globals);
    status_t ret0 = premotedesktopserver->InitServiceSidePeer(how_to_get_peer_globals(L));
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopserver_initclientsidepeer(lua_State *L)
{
    CRemoteDesktopServer *premotedesktopserver = get_remotedesktopserver(L,1);
    ASSERT(premotedesktopserver);
    const char* server = (const char*)lua_tostring(L,2);
    ASSERT(server);
    int port = (int)lua_tointeger(L,3);
	ASSERT(how_to_get_peer_globals);
    status_t ret0 = premotedesktopserver->InitClientSidePeer(how_to_get_peer_globals(L),server,port);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopserver_setname(lua_State *L)
{
    CRemoteDesktopServer *premotedesktopserver = get_remotedesktopserver(L,1);
    ASSERT(premotedesktopserver);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = premotedesktopserver->SetName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopserver_start(lua_State *L)
{
    CRemoteDesktopServer *premotedesktopserver = get_remotedesktopserver(L,1);
    ASSERT(premotedesktopserver);
    status_t ret0 = premotedesktopserver->Start();
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg remotedesktopserver_funcs_[] = {
    {"__gc",remotedesktopserver_gc_},
    {"__tostring",remotedesktopserver_tostring_},
    {"__is_same",remotedesktopserver_issame_},
    {"new",remotedesktopserver_new},
    {"SetRemoteDesktopSnapshottorManager",remotedesktopserver_setremotedesktopsnapshottormanager},
    {"Destroy",remotedesktopserver_destroy},
    {"InitServiceSidePeer",remotedesktopserver_initservicesidepeer},
    {"InitClientSidePeer",remotedesktopserver_initclientsidepeer},
    {"SetName",remotedesktopserver_setname},
	{"Start",remotedesktopserver_start},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktopserver_funcs()
{
    return remotedesktopserver_funcs_;
}

static int luaL_register_remotedesktopserver(lua_State *L)
{	
    static luaL_Reg _remotedesktopserver_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktopserver_funcs_,&_index,get_remotedesktopserver_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPSERVER);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktopserver_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktopserver_funcs_);
    return 1;
}        

int luaopen_remotedesktopserver(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopServer",luaL_register_remotedesktopserver,1);
    lua_pop(L, 1);
    return 0;
}        

