#ifndef __LUALIB_SERVERSIDEPEER_H
#define __LUALIB_SERVERSIDEPEER_H

#include "serversidepeer.h"
#include "luavm.h"

#define LUA_USERDATA_SERVERSIDEPEER "lua_userdata_serversidepeer"

CServerSidePeer *get_serversidepeer(lua_State *L, int idx);
lua_userdata *serversidepeer_new_userdata(lua_State *L,CServerSidePeer *pt,int is_weak);
int luaopen_serversidepeer(lua_State *L);
bool is_serversidepeer(lua_State *L, int idx);

#endif
