#ifndef __LUALIB_XIMAGEGIF_H
#define __LUALIB_XIMAGEGIF_H

#include "ximagegif.h"
#include "luavm.h"

#define LUA_USERDATA_XIMAGEGIF "lua_userdata_ximagegif"

CxImageGIF* get_ximagegif(lua_State *L, int idx);
lua_userdata *ximagegif_new_userdata(lua_State *L,CxImageGIF *pobj,int is_weak);
int luaopen_ximagegif(lua_State *L);
bool is_ximagegif(lua_State *L, int idx);
const luaL_Reg* get_ximagegif_funcs();

#endif
