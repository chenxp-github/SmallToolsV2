#include "lualib_websocketmessage.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_mem.h"
#include "lualib_filebase.h"

static bool websocketmessage_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CWebSocketMessage *p = (CWebSocketMessage*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}    

bool is_websocketmessage(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_WEBSOCKETMESSAGE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return websocketmessage_is_userdata_valid(ud);  
}

CWebSocketMessage *get_websocketmessage(lua_State *L, int idx)
{
    lua_userdata *ud = NULL;
    if(is_websocketmessage(L,idx))
    {
        ud = (lua_userdata*)lua_touserdata(L,idx);		
    }
    ASSERT(ud);
    return (CWebSocketMessage *)ud->p;
} 

lua_userdata *websocketmessage_new_userdata(lua_State *L,CWebSocketMessage *pobj,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pobj);
    ud->p = pobj;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pobj->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_WEBSOCKETMESSAGE);
    lua_setmetatable(L,-2);
    return ud;
}

static int websocketmessage_gc_(lua_State *L)
{
    if(!is_websocketmessage(L,1)) return 0;

    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);		
    ASSERT(ud);

    if(!(ud->is_attached))
    {
        CWebSocketMessage *pwebsocketmessage = (CWebSocketMessage*)ud->p;
        DEL(pwebsocketmessage);
    }
    return 0;
}

static int websocketmessage_issame_(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage1 = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage1);
    CWebSocketMessage *pwebsocketmessage2 = get_websocketmessage(L,2);
    ASSERT(pwebsocketmessage2);
    int is_same = (pwebsocketmessage1==pwebsocketmessage2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int websocketmessage_tostring_(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    char buf[1024];
    sprintf(buf,"userdata:websocketmessage:%p",pwebsocketmessage);
    lua_pushstring(L,buf);
    return 1;
}

/****************************************************/
static status_t websocketmessage_new(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage;
    NEW(pwebsocketmessage,CWebSocketMessage);
    pwebsocketmessage->Init();
    websocketmessage_new_userdata(L,pwebsocketmessage,0);
    return 1;
}

static status_t websocketmessage_transferdata(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    CMem *from = get_mem(L,2);
    ASSERT(from);
    status_t ret0 = pwebsocketmessage->TransferData(from);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_getmessagetype(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int ret0 = pwebsocketmessage->GetMessageType();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t websocketmessage_setmessagetype(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int _type = (int)lua_tointeger(L,2);
    status_t ret0 = pwebsocketmessage->SetMessageType(_type);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_attachpayload(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    CMem *payload = get_mem(L,2);
    ASSERT(payload);
    status_t ret0 = pwebsocketmessage->AttachPayload(payload);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_headersize(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int ret0 = pwebsocketmessage->HeaderSize();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t websocketmessage_size(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int ret0 = pwebsocketmessage->Size();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t websocketmessage_deserializeheader(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    status_t ret0 = pwebsocketmessage->DeserializeHeader(file);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_serializeheader(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    status_t ret0 = pwebsocketmessage->SerializeHeader(file);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_copy(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    CWebSocketMessage *_p = get_websocketmessage(L,2);
    ASSERT(_p);
    status_t ret0 = pwebsocketmessage->Copy(_p);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_getmethod(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int ret0 = pwebsocketmessage->GetMethod();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t websocketmessage_getcallbackid(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int ret0 = pwebsocketmessage->GetCallbackId();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t websocketmessage_getdatatype(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int ret0 = pwebsocketmessage->GetDataType();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t websocketmessage_getdata(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    CMem *ret0 = pwebsocketmessage->GetData();
    ASSERT(ret0);
    mem_new_userdata(L,ret0,1);
    return 1;
}

static status_t websocketmessage_setmethod(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int _method = (int)lua_tointeger(L,2);
    status_t ret0 = pwebsocketmessage->SetMethod(_method);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_setcallbackid(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int _callbackid = (int)lua_tointeger(L,2);
    status_t ret0 = pwebsocketmessage->SetCallbackId(_callbackid);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_setdatatype(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    int _type = (int)lua_tointeger(L,2);
    status_t ret0 = pwebsocketmessage->SetDataType(_type);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketmessage_setdata(lua_State *L)
{
    CWebSocketMessage *pwebsocketmessage = get_websocketmessage(L,1);
    ASSERT(pwebsocketmessage);
    CMem *_data = get_mem(L,2);
    ASSERT(_data);
    status_t ret0 = pwebsocketmessage->SetData(_data);
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg websocketmessage_funcs_[] = {
    {"__gc",websocketmessage_gc_},
    {"__tostring",websocketmessage_tostring_},
    {"__is_same",websocketmessage_issame_},
    {"new",websocketmessage_new},
    {"TransferData",websocketmessage_transferdata},
    {"GetMessageType",websocketmessage_getmessagetype},
    {"SetMessageType",websocketmessage_setmessagetype},
    {"AttachPayload",websocketmessage_attachpayload},
    {"HeaderSize",websocketmessage_headersize},
    {"Size",websocketmessage_size},
    {"DeserializeHeader",websocketmessage_deserializeheader},
    {"SerializeHeader",websocketmessage_serializeheader},
    {"Copy",websocketmessage_copy},
    {"GetMethod",websocketmessage_getmethod},
    {"GetCallbackId",websocketmessage_getcallbackid},
    {"GetDataType",websocketmessage_getdatatype},
    {"GetData",websocketmessage_getdata},
    {"SetMethod",websocketmessage_setmethod},
    {"SetCallbackId",websocketmessage_setcallbackid},
    {"SetDataType",websocketmessage_setdatatype},
    {"SetData",websocketmessage_setdata},
    {NULL,NULL},
};

const luaL_Reg* get_websocketmessage_funcs()
{
    return websocketmessage_funcs_;
}

static int luaL_register_websocketmessage(lua_State *L)
{	
    static luaL_Reg _websocketmessage_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_websocketmessage_funcs_,&_index,get_websocketmessage_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_WEBSOCKETMESSAGE);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_websocketmessage_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_websocketmessage_funcs_);
    return 1;
}        

int luaopen_websocketmessage(lua_State *L)
{
    luaL_requiref(L, "WebSocketMessage",luaL_register_websocketmessage,1);
    lua_pop(L, 1);
    return 0;
}        

