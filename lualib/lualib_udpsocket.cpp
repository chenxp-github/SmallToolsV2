#include "lualib_udpsocket.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_mem.h"

LUA_IS_VALID_USER_DATA_FUNC(CUdpSocket,udpsocket)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CUdpSocket,udpsocket)
LUA_NEW_USER_DATA_FUNC(CUdpSocket,udpsocket,UDPSOCKET)
LUA_GC_FUNC(CUdpSocket,udpsocket)
LUA_IS_SAME_FUNC(CUdpSocket,udpsocket)
LUA_TO_STRING_FUNC(CUdpSocket,udpsocket)

bool is_udpsocket(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_UDPSOCKET,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return udpsocket_is_userdata_valid(ud);  
}

/****************************************************/
static status_t udpsocket_new(lua_State *L)
{
    CUdpSocket *pudpsocket;
    NEW(pudpsocket,CUdpSocket);
    pudpsocket->Init();
    udpsocket_new_userdata(L,pudpsocket,0);
    return 1;
}

static status_t udpsocket_init(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    status_t ret0 = pudpsocket->Init();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t udpsocket_destroy(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    status_t ret0 = pudpsocket->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t udpsocket_create(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    status_t ret0 = pudpsocket->Create();
    pudpsocket->SetBlocking(false); //always use none block mode
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t udpsocket_bind(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    int port = (int)lua_tointeger(L,2);
    status_t ret0 = pudpsocket->Bind(port);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t udpsocket_setdestipandport(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    const char* ip = (const char*)lua_tostring(L,2);
    ASSERT(ip);
    int port = (int)lua_tointeger(L,3);
    status_t ret0 = pudpsocket->SetDestIpAndPort(ip,port);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t udpsocket_sendmsg(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    CMem *data = get_mem(L,2);
    ASSERT(data);
    status_t ret0 = pudpsocket->SendMsg(data);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t udpsocket_recvmsg(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    CMem *data = get_mem(L,2);
    ASSERT(data);
    status_t ret0 = pudpsocket->RecvMsg(data);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t udpsocket_getsrcaddr(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    LOCAL_MEM(ip);
    int port;
    pudpsocket->GetSrcAddr(&ip,&port);
    lua_pushstring(L,ip.CStr());
    lua_pushinteger(L,port);
    return 2;
}
static status_t udpsocket_setblocking(lua_State *L)
{
    CUdpSocket *pudpsocket = get_udpsocket(L,1);
    ASSERT(pudpsocket);
    bool b = (lua_toboolean(L,2)!=0);
    pudpsocket->SetBlocking(b);
    return 0;
}
/****************************************************/
static const luaL_Reg udpsocket_funcs_[] = {
    {"__gc",udpsocket_gc_},
    {"__tostring",udpsocket_tostring_},
    {"__is_same",udpsocket_issame_},
    {"new",udpsocket_new},
    {"Init",udpsocket_init},
    {"Destroy",udpsocket_destroy},
    {"Create",udpsocket_create},
    {"Bind",udpsocket_bind},
    {"SetDestIpAndPort",udpsocket_setdestipandport},
    {"SendMsg",udpsocket_sendmsg},
    {"RecvMsg",udpsocket_recvmsg},
    {"GetSrcAddr",udpsocket_getsrcaddr},
    {"SetBlocking",udpsocket_setblocking},
    {NULL,NULL},
};

const luaL_Reg* get_udpsocket_funcs()
{
    return udpsocket_funcs_;
}

static int luaL_register_udpsocket(lua_State *L)
{	
    static luaL_Reg _udpsocket_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_udpsocket_funcs_,&_index,get_udpsocket_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_UDPSOCKET);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_udpsocket_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_udpsocket_funcs_);
    return 1;
}        

int luaopen_udpsocket(lua_State *L)
{
    luaL_requiref(L, "UdpSocket",luaL_register_udpsocket,1);
    lua_pop(L, 1);
    return 0;
}        

