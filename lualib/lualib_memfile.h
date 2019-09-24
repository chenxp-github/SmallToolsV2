#ifndef __LUALIB_MEMFILE_H
#define __LUALIB_MEMFILE_H

#include "memfile.h"
#include "luavm.h"

#define LUA_USERDATA_MEMFILE "lua_userdata_memfile"

CMemFile* get_memfile(lua_State *L, int idx);
lua_userdata *memfile_new_userdata(lua_State *L,CMemFile *pobj,int is_weak);
int luaopen_memfile(lua_State *L);
bool is_memfile(lua_State *L, int idx);
const luaL_Reg* get_memfile_funcs();

#endif
