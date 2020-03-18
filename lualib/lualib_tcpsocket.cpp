#include "lualib_tcpsocket.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_filebase.h"
#include "lualib_mem.h"
#include "tasktcpacceptor.h"
#include "tasktcpconnector.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CTcpSocket,tcpsocket)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CTcpSocket,tcpsocket)
LUA_NEW_USER_DATA_FUNC(CTcpSocket,tcpsocket,TCPSOCKET)
LUA_GC_FUNC(CTcpSocket,tcpsocket)
LUA_IS_SAME_FUNC(CTcpSocket,tcpsocket)
LUA_TO_STRING_FUNC(CTcpSocket,tcpsocket)

bool is_tcpsocket(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_TCPSOCKET,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return tcpsocket_is_userdata_valid(ud);  
}
/****************************************************/
static status_t tcpsocket_new(lua_State *L)
{
    CTcpSocket *ptcpsocket;
    NEW(ptcpsocket,CTcpSocket);
    ptcpsocket->Init();
    tcpsocket_new_userdata(L,ptcpsocket,0);
    return 1;
}

static status_t tcpsocket_getsocketfd(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    int32_t ret0 = ptcpsocket->GetSocketFd();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t tcpsocket_isipaddress(lua_State *L)
{
    CMem *str = get_mem(L,1);
    ASSERT(str);
    bool ret0 = CTcpSocket::IsIpAddress(str);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_canread(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    bool ret0 = ptcpsocket->CanRead();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_canwrite(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    bool ret0 = ptcpsocket->CanWrite();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_transfersocketfd(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    CTcpSocket *from = get_tcpsocket(L,2);
    ASSERT(from);
    status_t ret0 = ptcpsocket->TransferSocketFd(from);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_attachsocketfd(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    int32_t fd = (int32_t)lua_tointeger(L,2);
    status_t ret0 = ptcpsocket->AttachSocketFd(fd);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_getallip(lua_State *L)
{
    CFileBase *file = get_filebase(L,1);
    ASSERT(file);
    status_t ret0 = CTcpSocket::GetAllIP(file);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_isconnected(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    bool ret0 = ptcpsocket->IsConnected();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_init(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    status_t ret0 = ptcpsocket->Init();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t tcpsocket_destroy(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    status_t ret0 = ptcpsocket->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}


static status_t tcpsocket_callback_ontcpacceptorevent(lua_State *L, int _funcid, bool _once, int event, CTcpSocket *tcp_socket)
{
    if(_funcid == LUA_REFNIL)
    {
        return ERROR;
    }

    CLuaVm vm;
    vm.Init(L);
    lua_rawgeti(L,LUA_REGISTRYINDEX,_funcid);
    if(_once)
    {
        luaL_unref(L,LUA_REGISTRYINDEX,_funcid);
    }
    lua_pushinteger(L,event);
	if(tcp_socket)
	{
		tcpsocket_new_userdata(L,tcp_socket,0);
		vm.Run(2,0);
	}
	else
	{
		vm.Run(1,0);
	}
    vm.ClearStack();
    return OK;
}

static status_t tcpsocket_newtcpacceptor(lua_State *L)
{
    int listen_port = (int)lua_tointeger(L,1);
    int callback = CLuaVm::ToFunction(L,2);	
			
    BEGIN_CLOSURE_FUNC(on_accept)
    {
        CLOSURE_PARAM_INT(event,0);
		CLOSURE_PARAM_INT(callback,10);
		CLOSURE_PARAM_PTR(lua_State*,L,11);
		
        if(event == CTaskTcpAcceptor::EVENT_NEW_CLIENT)
        {
            CLOSURE_PARAM_INT(fd,1);

			CTcpSocket *tcp_socket;
			NEW(tcp_socket,CTcpSocket);
			tcp_socket->Init();
			tcp_socket->AttachSocketFd(fd);
			tcp_socket->SetBlocking(false);
			
			tcpsocket_callback_ontcpacceptorevent(L,callback,false,event,tcp_socket);

        }
        else if(event == CTaskTcpAcceptor::EVENT_STOP)
        {
			tcpsocket_callback_ontcpacceptorevent(L,callback,false,event,NULL);
        }
        return OK;
    }
    END_CLOSURE_FUNC(on_accept);
	
	CTaskTcpAcceptor *ptask;
	NEW(ptask,CTaskTcpAcceptor);
	ptask->Init(how_to_get_global_taskmgr());
	ptask->SetListenPort(listen_port);
	ptask->Start();
	ptask->Callback()->SetFunc(on_accept);
	ptask->Callback()->SetParamInt(10,callback);
	ptask->Callback()->SetParamPointer(11,L);
	
    lua_pushinteger(L,ptask->GetId());
    return 1;
}

static status_t tcpsocket_callback_ontcpconnectorevent(lua_State *L, int _funcid, bool _once, int event, CTcpSocket *tcp_socket)
{
    if(_funcid == LUA_REFNIL)
    {
        return ERROR;
    }
	
    CLuaVm vm;
    vm.Init(L);
    lua_rawgeti(L,LUA_REGISTRYINDEX,_funcid);
    if(_once)
    {
        luaL_unref(L,LUA_REGISTRYINDEX,_funcid);
    }
    lua_pushinteger(L,event);
	if(tcp_socket)
    {
		tcpsocket_new_userdata(L,tcp_socket,0);
		vm.Run(2,0);
	}
	else
	{
		vm.Run(1,0);
	}
    vm.ClearStack();
    return OK;
}

static status_t tcpsocket_newtcpconnector(lua_State *L)
{
    const char* server = (const char*)lua_tostring(L,1);
    ASSERT(server);
    int port = (int)lua_tointeger(L,2);
    int ontcpconnectorevent = CLuaVm::ToFunction(L,3);

    BEGIN_CLOSURE_FUNC(on_connect)
    {
        CLOSURE_PARAM_INT(event,0);
		CLOSURE_PARAM_INT(callback,10);
		CLOSURE_PARAM_PTR(lua_State*,L,11);
		
        if(event == CTaskTcpConnector::EVENT_CONNECTED)
        {
            CLOSURE_PARAM_PTR(CTcpClient*,tcp_client,2);		
			CTcpSocket *tcp_socket;
			NEW(tcp_socket,CTcpSocket);
			tcp_socket->Init();
			tcp_socket->TransferSocketFd(tcp_client);
			tcp_socket->SetBlocking(false);			
			tcpsocket_callback_ontcpconnectorevent(L,callback,false,event,tcp_socket);
        }
        else
        {
			tcpsocket_callback_ontcpconnectorevent(L,callback,true,event,NULL);
        }
        return OK;
    }
    END_CLOSURE_FUNC(on_connect);
	
	CTaskTcpConnector *ptask;
	NEW(ptask,CTaskTcpConnector);
	ptask->Init(how_to_get_global_taskmgr());
	ptask->SetServerName(server);
	ptask->SetPort(port);
	ptask->Start();
	ptask->Callback()->SetFunc(on_connect);
	ptask->Callback()->SetParamInt(10,ontcpconnectorevent);
	ptask->Callback()->SetParamPointer(11,L);
    lua_pushinteger(L,ptask->GetId());
    return 1;
}

static status_t tcpsocket_setblocking(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    ASSERT(ptcpsocket);
    bool b = (lua_toboolean(L,2)!=0);
    ptcpsocket->SetBlocking(b);
    return 0;
}

/****************************************************/
static const luaL_Reg tcpsocket_funcs_[] = {
    {"__gc",tcpsocket_gc_},
    {"__tostring",tcpsocket_tostring_},
    {"__is_same",tcpsocket_issame_},
    {"new",tcpsocket_new},
    {"GetSocketFd",tcpsocket_getsocketfd},
    {"IsIpAddress",tcpsocket_isipaddress},
    {"CanRead",tcpsocket_canread},
    {"CanWrite",tcpsocket_canwrite},
    {"TransferSocketFd",tcpsocket_transfersocketfd},
    {"AttachSocketFd",tcpsocket_attachsocketfd},
    {"GetAllIP",tcpsocket_getallip},
    {"IsConnected",tcpsocket_isconnected},
    {"Init",tcpsocket_init},
    {"Destroy",tcpsocket_destroy},
    {"NewTcpAcceptor",tcpsocket_newtcpacceptor},
    {"NewTcpConnector",tcpsocket_newtcpconnector},
    {"SetBlocking",tcpsocket_setblocking},
    {NULL,NULL},
};

const luaL_Reg* get_tcpsocket_funcs()
{
    return tcpsocket_funcs_;
}

static int luaL_register_tcpsocket(lua_State *L)
{	
    static luaL_Reg _tcpsocket_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_tcpsocket_funcs_,&_index,get_filebase_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_tcpsocket_funcs_,&_index,get_tcpsocket_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_TCPSOCKET);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_tcpsocket_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_tcpsocket_funcs_);
    return 1;
}        

int luaopen_tcpsocket(lua_State *L)
{
    luaL_requiref(L, "TcpSocket",luaL_register_tcpsocket,1);
    lua_pop(L, 1);
    return 0;
}        

