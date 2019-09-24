#ifndef __LUALIB_CMDENTRY_H
#define __LUALIB_CMDENTRY_H

#include "cmdentry.h"
#include "luavm.h"

#define LUA_USERDATA_CMDENTRY "lua_userdata_cmdentry"

CCmdEntry *get_cmdentry(lua_State *L, int idx);
lua_userdata *cmdentry_new_userdata(lua_State *L,CCmdEntry *pt,int is_weak);
int luaopen_cmdentry(lua_State *L);
bool is_cmdentry(lua_State *L, int idx);

#endif
