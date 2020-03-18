#ifndef __LUALIB_FIVPROCESSMANAGER_H
#define __LUALIB_FIVPROCESSMANAGER_H

#include "nativeprocessmanager.h"
#include "nativeprocess.h"
#include "luavm.h"

#define LUA_USERDATA_NATIVEPROCESSMANAGER "lua_userdata_nativeprocessmanager"

CNativeProcessManager *get_nativeprocessmanager(lua_State *L, int idx);
lua_userdata *nativeprocessmanager_new_userdata(lua_State *L,CNativeProcessManager *pt,int is_weak);
int luaopen_nativeprocessmanager(lua_State *L);
bool is_nativeprocessmanager(lua_State *L, int idx);

#endif
