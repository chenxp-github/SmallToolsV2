#include "lualib_tcpsocket.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_filebase.h"
#include "lualib_mem.h"
#include "tasktcpacceptor.h"
#include "tasktcpconnector.h"
#include "lua_helper.h"

static bool tcpsocket_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CTcpSocket *p = (CTcpSocket*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}    

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

CTcpSocket *get_tcpsocket(lua_State *L, int idx)
{
    lua_userdata *ud = NULL;
    if(is_tcpsocket(L,idx))
    {
        ud = (lua_userdata*)lua_touserdata(L,idx);		
    }
    ASSERT(ud);
    return (CTcpSocket *)ud->p;
} 

lua_userdata *tcpsocket_new_userdata(lua_State *L,CTcpSocket *pobj,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pobj);
    ud->p = pobj;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pobj->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_TCPSOCKET);
    lua_setmetatable(L,-2);
    return ud;
}

static int tcpsocket_gc_(lua_State *L)
{
    if(!is_tcpsocket(L,1)) return 0;

    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);		
    ASSERT(ud);

    if(!(ud->is_attached))
    {
        CTcpSocket *ptcpsocket = (CTcpSocket*)ud->p;
        DEL(ptcpsocket);
    }
    return 0;
}

static int tcpsocket_issame_(lua_State *L)
{
    CTcpSocket *ptcpsocket1 = get_tcpsocket(L,1);
    ASSERT(ptcpsocket1);
    CTcpSocket *ptcpsocket2 = get_tcpsocket(L,2);
    ASSERT(ptcpsocket2);
    int is_same = (ptcpsocket1==ptcpsocket2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int tcpsocket_tostring_(lua_State *L)
{
    CTcpSocket *ptcpsocket = get_tcpsocket(L,1);
    char buf[1024];
    sprintf(buf,"userdata:tcpsocket:%p",ptcpsocket);
    lua_pushstring(L,buf);
    return 1;
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
			tcpsocket_callback_ontcpconnectorevent(L,callback,false,event,NULL);
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

