#ifndef __LUALIB_PEERPROXY_H
#define __LUALIB_PEERPROXY_H

#include "peerproxy.h"
#include "luavm.h"

#define LUA_USERDATA_PEERPROXY "lua_userdata_peerproxy"

CPeerProxy* get_peerproxy(lua_State *L, int idx);
lua_userdata *peerproxy_new_userdata(lua_State *L,CPeerProxy *pobj,int is_weak);
int luaopen_peerproxy(lua_State *L);
bool is_peerproxy(lua_State *L, int idx);
const luaL_Reg* get_peerproxy_funcs();

#endif
