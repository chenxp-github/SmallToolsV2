#include "lualib_serversidepeer.h"
#include "lualib_minibson.h"
#include "lualib_messagepeer.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"

#define CALLBACK_INDEX 15

LUA_IS_VALID_USER_DATA_FUNC(CServerSidePeer,serversidepeer)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CServerSidePeer,serversidepeer)
LUA_NEW_USER_DATA_FUNC(CServerSidePeer,serversidepeer,SERVERSIDEPEER)
LUA_GC_FUNC(CServerSidePeer,serversidepeer)
LUA_IS_SAME_FUNC(CServerSidePeer,serversidepeer)
LUA_TO_STRING_FUNC(CServerSidePeer,serversidepeer)

bool is_serversidepeer(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_SERVERSIDEPEER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return serversidepeer_is_userdata_valid(ud);  
}
/****************************************/
static int serversidepeer_new(lua_State *L)
{
    CServerSidePeer *pt;
    NEW(pt,CServerSidePeer);
    
	ASSERT(how_to_get_peer_globals);
	ASSERT(how_to_get_global_taskmgr);

	pt->Init(
		how_to_get_global_taskmgr(L),
		how_to_get_peer_globals(L)
	);

    serversidepeer_new_userdata(L,pt,0);
    return 1;
}

static int serversidepeer_clearsendingqueue(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->ClearSendingQueue();
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_getsendingqueuelength(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->GetSendingQueueLength();
	lua_pushinteger(L,_ret_0);
	return 1;
}
static int serversidepeer_resumefetchmessagetask(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->ResumeFetchMessageTask();
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_suspendfetchmessagetask(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->SuspendFetchMessageTask();
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_startfetchmessagetask(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->StartFetchMessageTask();
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_setcanfetchmessage(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int can = (int)lua_toboolean(L,2);
	int _ret_0 = (int)pserversidepeer->SetCanFetchMessage(can!=0);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_connect(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	const char* peer_name = (const char*)lua_tostring(L,2);
	ASSERT(peer_name);
	int _ret_0 = (int)pserversidepeer->Connect(peer_name);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_start(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->Start();
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_getname(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	const char* _ret_0 = (const char*)pserversidepeer->GetNameStr();
	if(_ret_0==NULL)return 0;
	lua_pushstring(L,_ret_0);
	return 1;
}
static int serversidepeer_setname(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	const char* _name = (const char*)lua_tostring(L,2);
	ASSERT(_name);
	int _ret_0 = (int)pserversidepeer->SetName(_name);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_ismarkquit(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->IsMarkQuit();
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int serversidepeer_destroy(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	ASSERT(pserversidepeer);
	int _ret_0 = (int)pserversidepeer->Destroy();
	lua_pushboolean(L,_ret_0);
	return 1;
}

PEER_SET_ON_MESSAGE_FUNC(serversidepeer_setonmessage,CServerSidePeer,get_serversidepeer)

static int serversidepeer_sendmessage(lua_State *L)
{
    CServerSidePeer *peer = get_serversidepeer(L,1);
    ASSERT(peer);
    const char* to = (const char*)lua_tostring(L,2);
    int func = (int)lua_tointeger(L,3);
    int cbid = (int)lua_tointeger(L,4);
    int flags = (int)lua_tointeger(L,5);

    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->SetTo(to);
    msg->SetFunc(func);
    msg->SetCallbackId(cbid);
    msg->SetFlags(flags);

    int body_type = msg->GetBodyType();

    if(body_type == CPeerMessage::JSON || body_type == CPeerMessage::STRING)
    {
        const char* str = (const char*)lua_tostring(L,6);
		if(str)
		{
			msg->SetBodyString(str);
		}        
    }
    else if(msg->GetBodyType() == CPeerMessage::BSON)
    {
		if(is_minibson(L,6))
		{
			CMiniBson *bson = get_minibson(L,6);
			ASSERT(bson);
			msg->TransferBody(bson->GetRawData());  
		}
    }

    msg->SetBodyType(body_type);
    ASSERT(peer->SendMessage(msg));

    lua_pushboolean(L,true);
    return 1;
}
static status_t serversidepeer_getaliveclientnumber(lua_State *L)
{
    CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
    ASSERT(pserversidepeer);
    int ret0 = pserversidepeer->GetAliveClientNumber();
    lua_pushinteger(L,ret0);
    return 1;
}
static const luaL_Reg serversidepeer_lib[] = {
    {"__gc",serversidepeer_gc_},
    {"__tostring",serversidepeer_tostring_},
    {"__is_same",serversidepeer_issame_},
    {"new",serversidepeer_new},
	{"ClearSendingQueue",serversidepeer_clearsendingqueue},
	{"GetSendingQueueLength",serversidepeer_getsendingqueuelength},
	{"ResumeFetchMessageTask",serversidepeer_resumefetchmessagetask},
	{"SuspendFetchMessageTask",serversidepeer_suspendfetchmessagetask},
	{"StartFetchMessageTask",serversidepeer_startfetchmessagetask},
	{"SetCanFetchMessage",serversidepeer_setcanfetchmessage},
	{"Connect",serversidepeer_connect},
	{"Start",serversidepeer_start},
	{"GetName",serversidepeer_getname},
	{"SetName",serversidepeer_setname},
	{"IsMarkQuit",serversidepeer_ismarkquit},
	{"Destroy",serversidepeer_destroy},
    {"SetOnMessage",serversidepeer_setonmessage},
    {"SendMessage",serversidepeer_sendmessage},
    {"GetAliveClientNumber",serversidepeer_getaliveclientnumber},	
	{NULL, NULL}
};
static int luaL_register_serversidepeer(lua_State *L)
{	
	luaL_newmetatable(L, LUA_USERDATA_SERVERSIDEPEER);
	lua_pushvalue(L, -1);	
	lua_setfield(L, -2, "__index");	
	luaL_setfuncs(L,serversidepeer_lib,0);
	lua_pop(L, 1);
	luaL_newlib(L,serversidepeer_lib);
	return 1;
}

int luaopen_serversidepeer(lua_State *L)
{
	luaL_requiref(L, "ServerSidePeer",luaL_register_serversidepeer,1);
	lua_pop(L, 1);
	return 0;
}
