#ifndef __LUALIB_XDISPLAY_H
#define __LUALIB_XDISPLAY_H

#include "xdisplay.h"
#include "xwindow.h"
#include "luavm.h"

#define LUA_USERDATA_XDISPLAY "lua_userdata_xdisplay"

CxDisplay *get_xdisplay(lua_State *L, int idx);
lua_userdata *xdisplay_new_userdata(lua_State *L,CxDisplay *pt,int is_weak);
int luaopen_xdisplay(lua_State *L);
bool is_xdisplay(lua_State *L, int idx);

#endif
