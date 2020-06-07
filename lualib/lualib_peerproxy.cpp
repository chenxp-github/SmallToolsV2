#include "lualib_peerproxy.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CPeerProxy,peerproxy)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CPeerProxy,peerproxy)
LUA_NEW_USER_DATA_FUNC(CPeerProxy,peerproxy,PEERPROXY)
LUA_GC_FUNC(CPeerProxy,peerproxy)
LUA_IS_SAME_FUNC(CPeerProxy,peerproxy)
LUA_TO_STRING_FUNC(CPeerProxy,peerproxy)

bool is_peerproxy(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_PEERPROXY,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return peerproxy_is_userdata_valid(ud);  
}

/****************************************************/
static status_t peerproxy_isserverside(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    bool ret0 = ppeerproxy->IsServerSide();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t peerproxy_clearsendingqueue(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    status_t ret0 = ppeerproxy->ClearSendingQueue();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t peerproxy_getsendingqueuelength(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    int ret0 = ppeerproxy->GetSendingQueueLength();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t peerproxy_ispeerconnectedme(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    CPeerProxy *peer = get_peerproxy(L,2);
    ASSERT(peer);
    bool ret0 = ppeerproxy->IsPeerConnectedMe(peer);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t peerproxy_getallconnectedpeernames(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    ppeerproxy->GetAllConnectedPeerNames();
    return 0;
}

static status_t peerproxy_addpeer(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = ppeerproxy->AddPeer(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t peerproxy_setmaxconnectedpeers(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    int max = (int)lua_tointeger(L,2);
    status_t ret0 = ppeerproxy->SetMaxConnectedPeers(max);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t peerproxy_getname(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    const char* ret0 = ppeerproxy->GetNameStr();
    ASSERT(ret0);
    lua_pushstring(L,ret0);
    return 1;
}

static status_t peerproxy_isconnected(lua_State *L)
{
    CPeerProxy *ppeerproxy = get_peerproxy(L,1);
    ASSERT(ppeerproxy);
    bool ret0 = ppeerproxy->IsConnected();
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg peerproxy_funcs_[] = {
    {"__gc",peerproxy_gc_},
    {"__tostring",peerproxy_tostring_},
    {"__is_same",peerproxy_issame_},
    {"IsServerSide",peerproxy_isserverside},
    {"ClearSendingQueue",peerproxy_clearsendingqueue},
    {"GetSendingQueueLength",peerproxy_getsendingqueuelength},
    {"IsPeerConnectedMe",peerproxy_ispeerconnectedme},
    {"GetAllConnectedPeerNames",peerproxy_getallconnectedpeernames},
    {"AddPeer",peerproxy_addpeer},
    {"SetMaxConnectedPeers",peerproxy_setmaxconnectedpeers},
    {"GetName",peerproxy_getname},
    {"IsConnected",peerproxy_isconnected},
    {NULL,NULL},
};

const luaL_Reg* get_peerproxy_funcs()
{
    return peerproxy_funcs_;
}

static int luaL_register_peerproxy(lua_State *L)
{	
    static luaL_Reg _peerproxy_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_peerproxy_funcs_,&_index,get_peerproxy_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_PEERPROXY);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_peerproxy_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_peerproxy_funcs_);
    return 1;
}        

int luaopen_peerproxy(lua_State *L)
{
    luaL_requiref(L, "PeerProxy",luaL_register_peerproxy,1);
    lua_pop(L, 1);
    return 0;
}        

