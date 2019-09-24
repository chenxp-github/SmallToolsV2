#ifndef __LUALIB_FILE_H
#define __LUALIB_FILE_H

#include "file.h"
#include "filebase.h"
#include "luavm.h"

#define LUA_USERDATA_FILE "lua_userdata_file"

CFile* get_file(lua_State *L, int idx);
lua_userdata *file_new_userdata(lua_State *L,CFile *pobj,int is_weak);
int luaopen_file(lua_State *L);
bool is_file(lua_State *L, int idx);
const luaL_Reg* get_file_funcs();

#endif
