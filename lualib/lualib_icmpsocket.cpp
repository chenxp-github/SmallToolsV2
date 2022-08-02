#include "lualib_icmpsocket.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_mem.h"

LUA_IS_VALID_USER_DATA_FUNC(CIcmpSocket,icmpsocket)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CIcmpSocket,icmpsocket)
LUA_NEW_USER_DATA_FUNC(CIcmpSocket,icmpsocket,ICMPSOCKET)
LUA_GC_FUNC(CIcmpSocket,icmpsocket)
LUA_IS_SAME_FUNC(CIcmpSocket,icmpsocket)
LUA_TO_STRING_FUNC(CIcmpSocket,icmpsocket)

bool is_icmpsocket(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_ICMPSOCKET,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return icmpsocket_is_userdata_valid(ud);  
}

/****************************************************/
static status_t icmpsocket_new(lua_State *L)
{
    CIcmpSocket *picmpsocket;
    NEW(picmpsocket,CIcmpSocket);
    picmpsocket->Init();
    icmpsocket_new_userdata(L,picmpsocket,0);
    return 1;
}

static status_t icmpsocket_destroy(lua_State *L)
{
    CIcmpSocket *picmpsocket = get_icmpsocket(L,1);
    ASSERT(picmpsocket);
    status_t ret0 = picmpsocket->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t icmpsocket_setdestip(lua_State *L)
{
    CIcmpSocket *picmpsocket = get_icmpsocket(L,1);
    ASSERT(picmpsocket);
    const char* ip = (const char*)lua_tostring(L,2);
    ASSERT(ip);
    status_t ret0 = picmpsocket->SetDestIp(ip);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t icmpsocket_create(lua_State *L)
{
    CIcmpSocket *picmpsocket = get_icmpsocket(L,1);
    ASSERT(picmpsocket);
    status_t ret0 = picmpsocket->Create();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t icmpsocket_sendmsg(lua_State *L)
{
    CIcmpSocket *picmpsocket = get_icmpsocket(L,1);
    ASSERT(picmpsocket);
    uint8_t type = (uint8_t)lua_tointeger(L,2);
    uint8_t code = (uint8_t)lua_tointeger(L,3);
    CMem *data = get_mem(L,4);
    ASSERT(data);
    status_t ret0 = picmpsocket->SendMsg(type,code,data->GetRawBuf(),(int)data->GetSize());
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t icmpsocket_recvmsg(lua_State *L)
{
    CIcmpSocket *picmpsocket = get_icmpsocket(L,1);
    ASSERT(picmpsocket);
    CMem *out = get_mem(L,2);
    ASSERT(out);
    status_t ret0 = picmpsocket->RecvMsg(out);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t icmpsocket_sendechomsg(lua_State *L)
{
    CIcmpSocket *picmpsocket = get_icmpsocket(L,1);
    ASSERT(picmpsocket);
    uint16_t id = (uint16_t)lua_tointeger(L,2);
    uint16_t seq = (uint16_t)lua_tointeger(L,3);
	
	if(is_mem(L,4))
	{
		CMem *data = get_mem(L,4);
		ASSERT(data);
		status_t ret0 = picmpsocket->SendEchoMsg(
			id,seq,
			(const uint8_t*)data->GetRawBuf(),
			(int)data->GetSize()
		);
		lua_pushboolean(L,ret0);
		return 1;
	}
	else if(lua_isstring(L,4))
	{
		size_t len = 0;
		const char *str = lua_tolstring(L,4,&len);
		ASSERT(str);

		status_t ret0 = picmpsocket->SendEchoMsg(
			id,seq,
			(const uint8_t*)str,
			(int)len
		);
		lua_pushboolean(L,ret0);
		return 1;
	}

    return 0;
}

static status_t icmpsocket_recvicmpmsg(lua_State *L)
{
    CIcmpSocket *picmpsocket = get_icmpsocket(L,1);
    ASSERT(picmpsocket);
    CMem *out = get_mem(L,2);
    ASSERT(out);
	uint8_t ret1,ret2;
    status_t ret0 = picmpsocket->RecvIcmpMsg(out,&ret1,&ret2);
    lua_pushboolean(L,ret0);
    lua_pushinteger(L,ret1);
    lua_pushinteger(L,ret2);
    return 3;
}

static status_t icmpsocket_ipheadersize(lua_State *L)
{
    int ret0 = CIcmpSocket::IpHeaderSize();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t icmpsocket_calculateicmpchecksum(lua_State *L)
{
    CMem *data = get_mem(L,1);
    ASSERT(data);
    uint16_t ret0 = CIcmpSocket::CalculateIcmpChecksum(
		(uint8_t*)data->GetRawBuf(),
		(int)data->GetSize()
	);
    lua_pushinteger(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg icmpsocket_funcs_[] = {
    {"__gc",icmpsocket_gc_},
    {"__tostring",icmpsocket_tostring_},
    {"__is_same",icmpsocket_issame_},
    {"new",icmpsocket_new},
    {"Destroy",icmpsocket_destroy},
    {"SetDestIp",icmpsocket_setdestip},
    {"Create",icmpsocket_create},
    {"SendMsg",icmpsocket_sendmsg},
    {"RecvMsg",icmpsocket_recvmsg},
    {"SendEchoMsg",icmpsocket_sendechomsg},
    {"RecvIcmpMsg",icmpsocket_recvicmpmsg},
    {"IpHeaderSize",icmpsocket_ipheadersize},
    {"CalculateIcmpChecksum",icmpsocket_calculateicmpchecksum},
    {NULL,NULL},
};

const luaL_Reg* get_icmpsocket_funcs()
{
    return icmpsocket_funcs_;
}

static int luaL_register_icmpsocket(lua_State *L)
{	
    static luaL_Reg _icmpsocket_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_icmpsocket_funcs_,&_index,get_icmpsocket_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_ICMPSOCKET);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_icmpsocket_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_icmpsocket_funcs_);
    return 1;
}        

int luaopen_icmpsocket(lua_State *L)
{
    luaL_requiref(L, "IcmpSocket",luaL_register_icmpsocket,1);
    lua_pop(L, 1);
    return 0;
}        

