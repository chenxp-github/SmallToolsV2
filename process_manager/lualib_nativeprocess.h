#ifndef __LUALIB_NATIVEPROCESS_H
#define __LUALIB_NATIVEPROCESS_H

#include "nativeprocess.h"
#include "luavm.h"

#define LUA_USERDATA_NATIVEPROCESS "lua_userdata_nativeprocess"

CNativeProcess *get_nativeprocess(lua_State *L, int idx);
lua_userdata *nativeprocess_new_userdata(lua_State *L,CNativeProcess *pt,int is_weak);
int luaopen_nativeprocess(lua_State *L);

#endif
