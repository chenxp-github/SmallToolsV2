#ifndef __LUALIB_MEM_H
#define __LUALIB_MEM_H

#include "mem.h"
#include "filebase.h"
#include "luavm.h"

#define LUA_USERDATA_MEM "lua_userdata_mem"

CMem* get_mem(lua_State *L, int idx);
lua_userdata *mem_new_userdata(lua_State *L,CMem *pobj,int is_weak);
int luaopen_mem(lua_State *L);
bool is_mem(lua_State *L, int idx);
const luaL_Reg* get_mem_funcs();


#endif
