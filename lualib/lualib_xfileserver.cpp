#include "lualib_xfileserver.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CxFileServer,xfileserver)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CxFileServer,xfileserver)
LUA_NEW_USER_DATA_FUNC(CxFileServer,xfileserver,XFILESERVER)
LUA_GC_FUNC(CxFileServer,xfileserver)
LUA_IS_SAME_FUNC(CxFileServer,xfileserver)
LUA_TO_STRING_FUNC(CxFileServer,xfileserver)

bool is_xfileserver(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XFILESERVER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return xfileserver_is_userdata_valid(ud);  
}

/****************************************************/
static status_t xfileserver_new(lua_State *L)
{
    CxFileServer *pxfileserver;
    NEW(pxfileserver,CxFileServer);
    pxfileserver->Init(how_to_get_global_taskmgr(L));
    xfileserver_new_userdata(L,pxfileserver,0);
    return 1;
}

static status_t xfileserver_initservicesidepeer(lua_State *L)
{
    CxFileServer *pxfileserver = get_xfileserver(L,1);
    ASSERT(pxfileserver);
    status_t ret0 = pxfileserver->InitServiceSidePeer(how_to_get_peer_globals(L));
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xfileserver_initclientsidepeer(lua_State *L)
{
    CxFileServer *pxfileserver = get_xfileserver(L,1);
    ASSERT(pxfileserver);
    const char* server = (const char*)lua_tostring(L,2);
    ASSERT(server);
    int port = (int)lua_tointeger(L,3);
    status_t ret0 = pxfileserver->InitClientSidePeer(how_to_get_peer_globals(L),server,port);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xfileserver_setpeername(lua_State *L)
{
    CxFileServer *pxfileserver = get_xfileserver(L,1);
    ASSERT(pxfileserver);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = pxfileserver->SetName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xfileserver_setrootpath(lua_State *L)
{
    CxFileServer *pxfileserver = get_xfileserver(L,1);
    ASSERT(pxfileserver);
    const char* root = (const char*)lua_tostring(L,2);
    ASSERT(root);
    status_t ret0 = pxfileserver->SetRootPath(root);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xfileserver_start(lua_State *L)
{
    CxFileServer *pxfileserver = get_xfileserver(L,1);
    ASSERT(pxfileserver);
    status_t ret0 = pxfileserver->Start();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xfileserver_destroy(lua_State *L)
{
    CxFileServer *pxfileserver = get_xfileserver(L,1);
    ASSERT(pxfileserver);
    status_t ret0 = pxfileserver->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg xfileserver_funcs_[] = {
    {"__gc",xfileserver_gc_},
    {"__tostring",xfileserver_tostring_},
    {"__is_same",xfileserver_issame_},
    {"new",xfileserver_new},
    {"InitServiceSidePeer",xfileserver_initservicesidepeer},
    {"InitClientSidePeer",xfileserver_initclientsidepeer},
    {"SetPeerName",xfileserver_setpeername},
    {"SetRootPath",xfileserver_setrootpath},
    {"Start",xfileserver_start},
    {"Destroy",xfileserver_destroy},
    {NULL,NULL},
};

const luaL_Reg* get_xfileserver_funcs()
{
    return xfileserver_funcs_;
}

static int luaL_register_xfileserver(lua_State *L)
{	
    static luaL_Reg _xfileserver_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_xfileserver_funcs_,&_index,get_xfileserver_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_XFILESERVER);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_xfileserver_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_xfileserver_funcs_);
    return 1;
}        

int luaopen_xfileserver(lua_State *L)
{
    luaL_requiref(L, "XFileServer",luaL_register_xfileserver,1);
    lua_pop(L, 1);
    return 0;
}        

