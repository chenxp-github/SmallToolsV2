#ifndef __LUALIB_FIVPROCESSMANAGER_H
#define __LUALIB_FIVPROCESSMANAGER_H

#include "nativeprocessmanager.h"
#include "nativeprocess.h"
#include "luavm.h"

#define LUA_USERDATA_FIVPROCESSMANAGER "lua_userdata_nativeprocessmanager"

CNativeProcessManager *get_fivprocessmanager(lua_State *L, int idx);
lua_userdata *fivprocessmanager_new_userdata(lua_State *L,CNativeProcessManager *pt,int is_weak);
int luaopen_fivprocessmanager(lua_State *L);

#endif
