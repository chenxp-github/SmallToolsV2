#include "lualib_messagepeer.h"
#include "lualib_minibson.h"
#include "lua_helper.h"
#include "closure.h"
#include "mem_tool.h"
#include "syslog.h"


#define CALLBACK_INDEX 15

#define RELEASE_CALLBACK(p) do{\
ASSERT(p);\
if(p->Callback()->GetParamType(CALLBACK_INDEX) == PARAM_TYPE_INT)\
{\
    int old_cbid = p->Callback()->GetParamInt(CALLBACK_INDEX);\
    CLuaVm::ReleaseFunction(L,old_cbid);\
    p->Callback()->SetParamInt(CALLBACK_INDEX,LUA_REFNIL);\
}\
}while(0)\


LUA_IS_VALID_USER_DATA_FUNC(CMessagePeer,messagepeer)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CMessagePeer,messagepeer)
LUA_NEW_USER_DATA_FUNC(CMessagePeer,messagepeer,MESSAGEPEER)
LUA_IS_SAME_FUNC(CMessagePeer,messagepeer)
LUA_TO_STRING_FUNC(CMessagePeer,messagepeer)

static int messagepeer_gc_(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);
    ASSERT(ud);
    if(!is_messagepeer(L,1))
	{
		ud->weak_ptr.Destroy();
		return 0;
	}
    if(!(ud->is_attached))
	{		
		CMessagePeer *p = (CMessagePeer*)ud->weak_ptr.GetRawPtr();	
		RELEASE_CALLBACK(p);
		DEL(p);
	}
    ud->weak_ptr.Destroy();
    return 0;
}

bool is_messagepeer(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_MESSAGEPEER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return messagepeer_is_userdata_valid(ud);  
}

/*********************************************************/
PEER_SET_ON_MESSAGE_FUNC(messagepeer_setonmessage,CMessagePeer,get_messagepeer)

static int messagepeer_sendmessage(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
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
    ASSERT(pmessagepeer->SendMessage(msg));

    lua_pushboolean(L,true);
    return 1;
}
static int messagepeer_connect(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    const char* peer_name = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pmessagepeer->Connect(peer_name);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_setname(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    const char* name = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pmessagepeer->SetName(name);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_setserver(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    const char* server = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pmessagepeer->SetServer(server);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_setport(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int port = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pmessagepeer->SetPort(port);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_start(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int _ret_0 = (int)pmessagepeer->Start();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_isconnected(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int _ret_0 = (int)pmessagepeer->IsConnected();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_getsendingqueuelength(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int _ret_0 = (int)pmessagepeer->GetSendingQueueLength();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int messagepeer_setmaxretries(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int max_retries = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pmessagepeer->SetMaxRetries(max_retries);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_isclosedpermanently(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int _ret_0 = (int)pmessagepeer->IsClosedPermanently();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int messagepeer_getretries(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int _ret_0 = (int)pmessagepeer->GetRetries();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int messagepeer_setretries(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int retry = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pmessagepeer->SetRetries(retry);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int messagepeer_clearsendingqueue(lua_State *L)
{
    CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    int _ret_0 = (int)pmessagepeer->ClearSendingQueue();
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int messagepeer_getname(lua_State *L)
{
	CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
    const char *name = pmessagepeer->GetNameStr();
	ASSERT(name);
	lua_pushstring(L,name);
	return 1;
}

static int messagepeer_destroy_(lua_State *L)
{
	CMessagePeer *pmessagepeer = get_messagepeer(L,1);
    ASSERT(pmessagepeer);
	SAVE_WEAK_REF_ID(*pmessagepeer,w);
    pmessagepeer->Destroy();
	RESTORE_WEAK_REF_ID(*pmessagepeer,w);
	return 0;
}

static status_t messagepeer_new(lua_State *L)
{
    CMessagePeer *pmessagepeer;
    NEW(pmessagepeer,CMessagePeer);
    pmessagepeer->Init(how_to_get_global_taskmgr());
    messagepeer_new_userdata(L,pmessagepeer,0);
    return 1;
}

static const luaL_Reg messagepeer_lib[] = {
    {"__gc",messagepeer_gc_},
    {"__tostring",messagepeer_tostring_},
    {"__is_same",messagepeer_issame_},
    {"new",messagepeer_new},
    {"SetOnMessage",messagepeer_setonmessage},
    {"SendMessage",messagepeer_sendmessage},
    {"Connect",messagepeer_connect},
    {"SetName",messagepeer_setname},
	{"GetName",messagepeer_getname},
    {"SetServer",messagepeer_setserver},
    {"SetPort",messagepeer_setport},
    {"Start",messagepeer_start},
    {"IsConnected",messagepeer_isconnected},
    {"GetSendingQueueLength",messagepeer_getsendingqueuelength},
    {"SetMaxRetries",messagepeer_setmaxretries},
    {"IsClosedPermanently",messagepeer_isclosedpermanently},
    {"GetRetries",messagepeer_getretries},
    {"SetRetries",messagepeer_setretries},
    {"ClearSendingQueue",messagepeer_clearsendingqueue},    
    {"Destroy",messagepeer_destroy_},    
    {NULL, NULL}
};
static int luaL_register_messagepeer(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_MESSAGEPEER);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,messagepeer_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,messagepeer_lib);
    return 1;
}

void luaopen_messagepeer(lua_State *L)
{
    luaL_requiref(L, "MessagePeer",luaL_register_messagepeer,1);
    lua_pop(L, 1);
}
