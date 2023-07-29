#include "lualib_remotedesktopclient.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopClient,remotedesktopclient)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopClient,remotedesktopclient)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopClient,remotedesktopclient,REMOTEDESKTOPCLIENT)
LUA_GC_FUNC(CRemoteDesktopClient,remotedesktopclient)
LUA_IS_SAME_FUNC(CRemoteDesktopClient,remotedesktopclient)
LUA_TO_STRING_FUNC(CRemoteDesktopClient,remotedesktopclient)

bool is_remotedesktopclient(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPCLIENT,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktopclient_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktopclient_new(lua_State *L)
{
    CRemoteDesktopClient *premotedesktopclient;
    NEW(premotedesktopclient,CRemoteDesktopClient);
	ASSERT(how_to_get_global_taskmgr);
    premotedesktopclient->Init(how_to_get_global_taskmgr(L));
    remotedesktopclient_new_userdata(L,premotedesktopclient,0);
    return 1;
}

static status_t remotedesktopclient_destroy(lua_State *L)
{
    CRemoteDesktopClient *premotedesktopclient = get_remotedesktopclient(L,1);
    ASSERT(premotedesktopclient);
    status_t ret0 = premotedesktopclient->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclient_initservicesidepeer(lua_State *L)
{
    CRemoteDesktopClient *premotedesktopclient = get_remotedesktopclient(L,1);
    ASSERT(premotedesktopclient);
	ASSERT(how_to_get_peer_globals);
    status_t ret0 = premotedesktopclient->InitServiceSidePeer(how_to_get_peer_globals(L));
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclient_initclientsidepeer(lua_State *L)
{
    CRemoteDesktopClient *premotedesktopclient = get_remotedesktopclient(L,1);
    ASSERT(premotedesktopclient);
    const char* server = (const char*)lua_tostring(L,2);
    ASSERT(server);
    int port = (int)lua_tointeger(L,3);
	ASSERT(how_to_get_peer_globals);
    status_t ret0 = premotedesktopclient->InitClientSidePeer(how_to_get_peer_globals(L),server,port);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclient_setname(lua_State *L)
{
    CRemoteDesktopClient *premotedesktopclient = get_remotedesktopclient(L,1);
    ASSERT(premotedesktopclient);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = premotedesktopclient->SetName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclient_setdestpeername(lua_State *L)
{
    CRemoteDesktopClient *premotedesktopclient = get_remotedesktopclient(L,1);
    ASSERT(premotedesktopclient);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = premotedesktopclient->SetDestPeerName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclient_start(lua_State *L)
{
    CRemoteDesktopClient *premotedesktopclient = get_remotedesktopclient(L,1);
    ASSERT(premotedesktopclient);
    status_t ret0 = premotedesktopclient->Start();
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg remotedesktopclient_funcs_[] = {
    {"__gc",remotedesktopclient_gc_},
    {"__tostring",remotedesktopclient_tostring_},
    {"__is_same",remotedesktopclient_issame_},
    {"new",remotedesktopclient_new},
    {"Destroy",remotedesktopclient_destroy},
    {"InitServiceSidePeer",remotedesktopclient_initservicesidepeer},
    {"InitClientSidePeer",remotedesktopclient_initclientsidepeer},
    {"SetName",remotedesktopclient_setname},
	{"SetDestPeerName",remotedesktopclient_setdestpeername},
	{"Start",remotedesktopclient_start},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktopclient_funcs()
{
    return remotedesktopclient_funcs_;
}

static int luaL_register_remotedesktopclient(lua_State *L)
{	
    static luaL_Reg _remotedesktopclient_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktopclient_funcs_,&_index,get_remotedesktopclient_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPCLIENT);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktopclient_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktopclient_funcs_);
    return 1;
}        

int luaopen_remotedesktopclient(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopClient",luaL_register_remotedesktopclient,1);
    lua_pop(L, 1);
    return 0;
}        

