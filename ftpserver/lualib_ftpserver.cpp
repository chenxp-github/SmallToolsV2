#include "lualib_ftpserver.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CFtpServer,ftpserver)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CFtpServer,ftpserver)
LUA_NEW_USER_DATA_FUNC(CFtpServer,ftpserver,FTPSERVER)
LUA_GC_FUNC(CFtpServer,ftpserver)
LUA_IS_SAME_FUNC(CFtpServer,ftpserver)
LUA_TO_STRING_FUNC(CFtpServer,ftpserver)

bool is_ftpserver(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_FTPSERVER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return ftpserver_is_userdata_valid(ud);  
}

/****************************************************/
static status_t ftpserver_new(lua_State *L)
{
	ASSERT(how_to_get_global_taskmgr);
    CFtpServer *pftpserver;
    NEW(pftpserver,CFtpServer);
    pftpserver->Init(how_to_get_global_taskmgr(L));
    ftpserver_new_userdata(L,pftpserver,0);
    return 1;
}

static status_t ftpserver_setallowanonymouse(lua_State *L)
{
    CFtpServer *pftpserver = get_ftpserver(L,1);
    ASSERT(pftpserver);
    bool allow = lua_toboolean(L,2)!=0;
    status_t ret0 = pftpserver->SetAllowAnonymouse(allow);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ftpserver_getfixclientip(lua_State *L)
{
    CFtpServer *pftpserver = get_ftpserver(L,1);
    ASSERT(pftpserver);
    const char* ret0 = pftpserver->GetFixClientIpStr();
    ASSERT(ret0);
    lua_pushstring(L,ret0);
    return 1;
}

static status_t ftpserver_setfixclientip(lua_State *L)
{
    CFtpServer *pftpserver = get_ftpserver(L,1);
    ASSERT(pftpserver);
    const char* fix_client_ip = (const char*)lua_tostring(L,2);
    ASSERT(fix_client_ip);
    status_t ret0 = pftpserver->SetFixClientIp(fix_client_ip);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ftpserver_addconfig(lua_State *L)
{
    CFtpServer *pftpserver = get_ftpserver(L,1);
    ASSERT(pftpserver);
    const char* user = (const char*)lua_tostring(L,2);
    ASSERT(user);
    const char* pass = (const char*)lua_tostring(L,3);
    ASSERT(pass);
    const char* root = (const char*)lua_tostring(L,4);
    ASSERT(root);
    status_t ret0 = pftpserver->AddConfig(user,pass,root);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ftpserver_start(lua_State *L)
{
    CFtpServer *pftpserver = get_ftpserver(L,1);
    ASSERT(pftpserver);
    int port = (int)lua_tointeger(L,2);
    status_t ret0 = pftpserver->Start(port);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ftpserver_setencoding(lua_State *L)
{
    CFtpServer *pftpserver = get_ftpserver(L,1);
    ASSERT(pftpserver);
    int encoding = (int)lua_tointeger(L,2);
    status_t ret0 = pftpserver->SetEncoding(encoding);
    lua_pushboolean(L,ret0);
    return 1;
}
static status_t ftpserver_setfixserverip(lua_State *L)
{
    CFtpServer *pftpserver = get_ftpserver(L,1);
    ASSERT(pftpserver);
    const char* fix_server_ip = (const char*)lua_tostring(L,2);
    ASSERT(fix_server_ip);
    status_t ret0 = pftpserver->SetFixServerIp(fix_server_ip);
    lua_pushboolean(L,ret0);
    return 1;
}
/****************************************************/
static const luaL_Reg ftpserver_funcs_[] = {
    {"__gc",ftpserver_gc_},
    {"__tostring",ftpserver_tostring_},
    {"__is_same",ftpserver_issame_},
    {"new",ftpserver_new},
    {"SetAllowAnonymouse",ftpserver_setallowanonymouse},
    {"GetFixClientIp",ftpserver_getfixclientip},
    {"SetFixClientIp",ftpserver_setfixclientip},
    {"SetFixServerIp",ftpserver_setfixserverip},
    {"AddConfig",ftpserver_addconfig},
    {"Start",ftpserver_start},
    {"SetEncoding",ftpserver_setencoding},
	{NULL,NULL},
};

const luaL_Reg* get_ftpserver_funcs()
{
    return ftpserver_funcs_;
}

static int luaL_register_ftpserver(lua_State *L)
{	
    static luaL_Reg _ftpserver_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_ftpserver_funcs_,&_index,get_ftpserver_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_FTPSERVER);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_ftpserver_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_ftpserver_funcs_);
    return 1;
}        

int luaopen_ftpserver(lua_State *L)
{
    luaL_requiref(L, "FtpServer",luaL_register_ftpserver,1);
    lua_pop(L, 1);
    return 0;
}        

