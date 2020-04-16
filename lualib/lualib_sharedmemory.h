#ifndef __LUALIB_SHAREDMEMORY_H
#define __LUALIB_SHAREDMEMORY_H

#include "sharedmemory.h"
#include "luavm.h"

#define LUA_USERDATA_SHAREDMEMORY "lua_userdata_sharedmemory"

CSharedMemory* get_sharedmemory(lua_State *L, int idx);
lua_userdata *sharedmemory_new_userdata(lua_State *L,CSharedMemory *pobj,int is_weak);
int luaopen_sharedmemory(lua_State *L);
bool is_sharedmemory(lua_State *L, int idx);
const luaL_Reg* get_sharedmemory_funcs();

#endif
