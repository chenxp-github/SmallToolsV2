#ifndef __LUALIB_WEBSOCKETSERVER_H
#define __LUALIB_WEBSOCKETSERVER_H

#include "websocketserver.h"
#include "luavm.h"

#define LUA_USERDATA_WEBSOCKETSERVER "lua_userdata_websocketserver"

CWebSocketServer* get_websocketserver(lua_State *L, int idx);
lua_userdata *websocketserver_new_userdata(lua_State *L,CWebSocketServer *pobj,int is_weak);
int luaopen_websocketserver(lua_State *L);
bool is_websocketserver(lua_State *L, int idx);
const luaL_Reg* get_websocketserver_funcs();

#endif
