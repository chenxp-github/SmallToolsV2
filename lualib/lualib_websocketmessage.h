#ifndef __LUALIB_WEBSOCKETMESSAGE_H
#define __LUALIB_WEBSOCKETMESSAGE_H

#include "websocketmessage.h"
#include "luavm.h"

#define LUA_USERDATA_WEBSOCKETMESSAGE "lua_userdata_websocketmessage"

CWebSocketMessage* get_websocketmessage(lua_State *L, int idx);
lua_userdata *websocketmessage_new_userdata(lua_State *L,CWebSocketMessage *pobj,int is_weak);
int luaopen_websocketmessage(lua_State *L);
bool is_websocketmessage(lua_State *L, int idx);
const luaL_Reg* get_websocketmessage_funcs();

#endif
