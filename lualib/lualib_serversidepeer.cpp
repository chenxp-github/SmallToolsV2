#include "lualib_serversidepeer.h"
#include "lualib_minibson.h"
#include "lualib_messagepeer.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"


#define CALLBACK_INDEX 15

static bool serversidepeer_is_userdata_valid(lua_userdata *ud)
{
	if(ud == NULL)return false;
	if(ud->p == NULL)return false;
	if(ud->__weak_ref_id == 0) return false;
	CServerSidePeer *p = (CServerSidePeer*)ud->p;
	return p->__weak_ref_id == ud->__weak_ref_id;
}
bool is_serversidepeer(lua_State *L, int idx)
{
	lua_userdata *ud = (lua_userdata*)luaL_testudata(L, idx, LUA_USERDATA_SERVERSIDEPEER);
	return serversidepeer_is_userdata_valid(ud);
}
CServerSidePeer *get_serversidepeer(lua_State *L, int idx)
{
	lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_SERVERSIDEPEER);
	ASSERT(ud && ud->p);
	ASSERT(ud->__weak_ref_id != 0);
	CServerSidePeer *p = (CServerSidePeer*)ud->p;
	ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
	return p;
}
lua_userdata *serversidepeer_new_userdata(lua_State *L,CServerSidePeer *pt,int is_weak)
{
	lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
	ASSERT(ud && pt);
	ud->p = pt;
	ud->is_attached = is_weak;
	ud->__weak_ref_id = pt->__weak_ref_id;
	luaL_getmetatable(L,LUA_USERDATA_SERVERSIDEPEER);
	lua_setmetatable(L,-2);
	return ud;
}

static int serversidepeer_new(lua_State *L)
{	
	CServerSidePeer *pt;
	NEW(pt,CServerSidePeer);
	pt->Init(how_to_get_global_taskmgr());
	serversidepeer_new_userdata(L,pt,0);
	return 1;
}

static int serversidepeer_destroy__(lua_State *L)
{
	lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_SERVERSIDEPEER);
	if(!serversidepeer_is_userdata_valid(ud))
		return 0;
	CServerSidePeer *pserversidepeer = (CServerSidePeer*)ud->p;
	if(!(ud->is_attached))
	{
		DEL(pserversidepeer);
	}
	return 0;
}
static int serversidepeer_issame(lua_State *L)
{
	CServerSidePeer *pserversidepeer1 = get_serversidepeer(L,1);
	ASSERT(pserversidepeer1);
	CServerSidePeer *pserversidepeer2 = get_serversidepeer(L,2);
	ASSERT(pserversidepeer2);
	int is_same = (pserversidepeer1==pserversidepeer2);
	lua_pushboolean(L,is_same);
	return 1;
}
static int serversidepeer_tostring(lua_State *L)
{
	CServerSidePeer *pserversidepeer = get_serversidepeer(L,1);
	char buf[1024];
	sprintf(buf,"userdata:serversidepeer:%p",pserversidepeer);
	lua_pushstring(L,buf);
	return 1;
}
/****************************************/
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
        msg->SetBodyString(str);
        
    }
    else if(msg->GetBodyType() == CPeerMessage::BSON)
    {
        CMiniBson *bson = get_minibson(L,6);
        ASSERT(bson);
        msg->TransferBody(bson->GetRawData());  
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
	{"new",serversidepeer_new},
	{"__gc",serversidepeer_destroy__},
	{"__tostring",serversidepeer_tostring},
	{"IsSame",serversidepeer_issame},
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
