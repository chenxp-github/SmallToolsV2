#ifndef __LUALIB_LUAHDC_H
#define __LUALIB_LUAHDC_H

#include "lua_hdc.h"
#include "luavm.h"

#define LUA_USERDATA_LUAHDC "lua_userdata_luahdc"

CLuaHdc* get_luahdc(lua_State *L, int idx);
lua_userdata *luahdc_new_userdata(lua_State *L,CLuaHdc *pobj,int is_weak);
int luaopen_luahdc(lua_State *L);
bool is_luahdc(lua_State *L, int idx);
const luaL_Reg* get_luahdc_funcs();

#endif
