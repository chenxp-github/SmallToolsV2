#ifndef __LUALIB_QUEUEFILE_H
#define __LUALIB_QUEUEFILE_H

#include "queuefile.h"
#include "luavm.h"

#define LUA_USERDATA_QUEUEFILE "lua_userdata_queuefile"

CQueueFile* get_queuefile(lua_State *L, int idx);
lua_userdata *queuefile_new_userdata(lua_State *L,CQueueFile *pobj,int is_weak);
int luaopen_queuefile(lua_State *L);
bool is_queuefile(lua_State *L, int idx);
const luaL_Reg* get_queuefile_funcs();

#endif
