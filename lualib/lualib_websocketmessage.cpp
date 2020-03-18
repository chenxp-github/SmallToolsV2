#include "lualib_websocketmessage.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_mem.h"
#include "lualib_filebase.h"

LUA_IS_VALID_USER_DATA_FUNC(CWebSocketMessage,websocketmessage)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CWebSocketMessage,websocketmessage)
LUA_NEW_USER_DATA_FUNC(CWebSocketMessage,websocketmessage,WEBSOCKETMESSAGE)
LUA_GC_FUNC(CWebSocketMessage,websocketmessage)
LUA_IS_SAME_FUNC(CWebSocketMessage,websocketmessage)
LUA_TO_STRING_FUNC(CWebSocketMessage,websocketmessage)

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

