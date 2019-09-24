#ifndef __LUALIB_FILEBASE_H
#define __LUALIB_FILEBASE_H

#include "filebase.h"
#include "stream.h"
#include "luavm.h"

#define LUA_USERDATA_FILEBASE "lua_userdata_filebase"

CFileBase* get_filebase(lua_State *L, int idx);
lua_userdata *filebase_new_userdata(lua_State *L,CFileBase *pobj,int is_weak);
int luaopen_filebase(lua_State *L);
bool is_filebase(lua_State *L, int idx);
const luaL_Reg* get_filebase_funcs();

#endif
