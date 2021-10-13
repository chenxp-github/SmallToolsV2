#ifndef __LUALIB_COMMONTIME_H
#define __LUALIB_COMMONTIME_H

#include "common_time.h"
#include "luavm.h"

#define LUA_USERDATA_COMMONTIME "lua_userdata_commontime"

CCommonTime* get_commontime(lua_State *L, int idx);
lua_userdata *commontime_new_userdata(lua_State *L,CCommonTime *pobj,int is_weak);
int luaopen_commontime(lua_State *L);
bool is_commontime(lua_State *L, int idx);
const luaL_Reg* get_commontime_funcs();

#endif
