#ifndef __LUALIB_XIMAGE_H
#define __LUALIB_XIMAGE_H

#include "ximage.h"
#include "luavm.h"

#define LUA_USERDATA_XIMAGE "lua_userdata_ximage"

CxImage* get_ximage(lua_State *L, int idx);
lua_userdata *ximage_new_userdata(lua_State *L,CxImage *pobj,int is_weak);
int luaopen_ximage(lua_State *L);
bool is_ximage(lua_State *L, int idx);
const luaL_Reg* get_ximage_funcs();

#endif
