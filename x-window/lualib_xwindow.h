#ifndef __LUALIB_XWINDOW_H
#define __LUALIB_XWINDOW_H

#include "xwindow.h"
#include "xdisplay.h"
#include "luavm.h"

#define LUA_USERDATA_XWINDOW "lua_userdata_xwindow"

CxWindow *get_xwindow(lua_State *L, int idx);
lua_userdata *xwindow_new_userdata(lua_State *L,CxWindow *pt,int is_weak);
int luaopen_xwindow(lua_State *L);
bool is_xwindow(lua_State *L, int idx);

#endif
