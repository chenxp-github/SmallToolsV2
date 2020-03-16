#include "lualib_messagepeer.h"
#include "lualib_minibson.h"
#include "lua_helper.h"
#include "closure.h"
#include "mem_tool.h"
#include "syslog.h"

#define CALLBACK_INDEX 15

CMessagePeer *get_messagepeer(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_MESSAGEPEER);
    ASSERT(ud && ud->p);
    ASSERT(ud->__weak_ref_id != 0);
    CHECK_IS_UD_READABLE(CMessagePeer,ud);
    CMessagePeer *p = (CMessagePeer*)ud->p;
    ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
    return p;
}
lua_userdata *messagepeer_new_userdata(lua_State *L,CMessagePeer *pt,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pt);
    ud->p = pt;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pt->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_MESSAGEPEER);
    lua_setmetatable(L,-2);
    return ud;
}

static int messagepeer_new(lua_State *L)
{
    CMessagePeer *pt;
    NEW(pt,CMessagePeer);
    pt->Init(how_to_get_global_taskmgr());
    pt->SetServer("127.0.0.1");
    pt->SetPort(2086);

    pt->Callback()->SetParamInt(CALLBACK_INDEX,LUA_REFNIL);
    messagepeer_new_userdata(L,pt,0);
    return 1;
}

static int messagepeer_destroy(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_MESSAGEPEER);
    ASSERT(ud);
    CMessagePeer *pmessagepeer = (CMessagePeer*)ud->p;
    if(!(ud->is_attached))
    {
        DEL(pmessagepeer);
    }
    return 0;
}

static int messagepeer_tostring(lua_State *L)
{
    lua_pushstring(L,"userdata:messagepeer");
    return 1;
}

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
        msg->SetBodyString(str);
        
    }
    else if(msg->GetBodyType() == CPeerMessage::BSON)
    {
        CMiniBson *bson = get_minibson(L,6);
        ASSERT(bson);
        msg->TransferBody(bson->GetRawData());  
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
    pmessagepeer->Destroy();
	return 0;
}

static const luaL_Reg messagepeer_lib[] = {
    {"new",messagepeer_new},
    {"__gc",messagepeer_destroy},
    {"__tostring",messagepeer_tostring},
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
