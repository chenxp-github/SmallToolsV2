#ifndef __LUALIB_MESSAGEPEER_H
#define __LUALIB_MESSAGEPEER_H

#include "messagepeer.h"
#include "luavm.h"

#define LUA_USERDATA_MESSAGEPEER "lua_userdata_messagepeer"

CMessagePeer *get_messagepeer(lua_State *L, int idx);
lua_userdata *messagepeer_new_userdata(lua_State *L,CMessagePeer *pt,int is_weak);
void luaopen_messagepeer(lua_State *L);
bool is_messagepeer(lua_State *L, int idx);

#define PEER_SET_ON_MESSAGE_FUNC(func_name, peer_type, get_peer)\
static int func_name(lua_State *L)\
{\
    peer_type *pmessagepeer = get_peer(L,1);\
    ASSERT(pmessagepeer);\
\
    int OnMessage = CLuaVm::ToFunction(L,2);\
\
    int callback_on_message = LUA_REFNIL;\
	if(pmessagepeer->Callback()->GetParamType(CALLBACK_INDEX) == PARAM_TYPE_INT)\
		callback_on_message = pmessagepeer->Callback()->GetParamInt(CALLBACK_INDEX);\
    CLuaVm::ReleaseFunction(L,callback_on_message);\
\
    BEGIN_CLOSURE_FUNC(on_message)\
    {\
        CLOSURE_PARAM_INT(event,0);\
		CLOSURE_PARAM_INT(callback_on_message,CALLBACK_INDEX);\
		CLOSURE_PARAM_PTR(lua_State*,LL,11);\
\
		CLuaVm *vm = how_to_get_global_luavm(LL);\
		ASSERT(vm);\
		lua_State *L = vm->GetLuaState();\
\
		vm->PushFunction(callback_on_message);\
\
		lua_newtable(L);\
		int top = lua_gettop(L);\
\
		lua_pushstring(L,"event");\
		lua_pushinteger(L,event);\
		lua_settable(L, top);\
\
        if(event == PEER_EVENT_GOT_MESSAGE)\
        {\
            CLOSURE_PARAM_PTR(CMessagePeer*,peer,10);\
            ASSERT(peer);\
            CLOSURE_PARAM_PTR(CPeerMessage*,msg,1);\
            ASSERT(msg);\
            ASSERT(msg->GetFrom());\
\
			lua_pushstring(L,"method");\
			lua_pushinteger(L,msg->GetFunc());\
			lua_settable(L, top);\
\
			lua_pushstring(L,"from");\
			lua_pushstring(L,msg->GetFrom()->CStr());\
			lua_settable(L, top);\
\
			lua_pushstring(L,"callback_id");\
			lua_pushinteger(L,msg->GetCallbackId());\
			lua_settable(L, top);\
\
			lua_pushstring(L,"flags");\
			lua_pushinteger(L,msg->GetFlags());\
			lua_settable(L, top);\
\
			lua_pushstring(L,"body_type");\
			lua_pushinteger(L,msg->GetBodyType());\
			lua_settable(L, top);\
\
			lua_pushstring(L,"msg_type");\
			lua_pushinteger(L,msg->GetMessageType());\
			lua_settable(L, top);\
\
			lua_pushstring(L,"body");\
            if(msg->GetBodyType() == CPeerMessage::STRING)\
            {\
                CMem *str = msg->GetBody();\
                ASSERT(str);\
                lua_pushlstring(L,str->CStr(),str->StrLen());\
				lua_settable(L, top);\
				vm->Run(1,0);\
            }\
            else if(msg->GetBodyType() == CPeerMessage::BSON)\
            {\
				CMiniBson tmp_bson;\
                tmp_bson.Init();\
                tmp_bson.LoadRawBuf(msg->GetBody());\
                minibson_new_userdata(L,&tmp_bson,true);\
				lua_settable(L, top);\
				vm->Run(1,0);\
            }\
			else\
			{\
				ASSERT(0);\
			}\
        }\
		else\
		{\
			vm->Run(1,0);\
		}\
		vm->ClearStack();\
        return OK;\
    }\
    END_CLOSURE_FUNC(on_message);\
\
    pmessagepeer->Callback()->SetFunc(on_message);\
    pmessagepeer->Callback()->SetParamPointer(10,pmessagepeer);\
	pmessagepeer->Callback()->SetParamPointer(11,L);\
    pmessagepeer->Callback()->SetParamInt(CALLBACK_INDEX,OnMessage);\
    return 1;\
}\


#endif

