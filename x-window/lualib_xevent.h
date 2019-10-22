#ifndef __LUALIB_XEVENT_H
#define __LUALIB_XEVENT_H

#include "xevent.h"
#include "luavm.h"

#define LUA_USERDATA_XEVENT "lua_userdata_xevent"

CXEvent *get_xevent(lua_State *L, int idx);
lua_userdata *xevent_new_userdata(lua_State *L,CXEvent *pt,int is_weak);
int luaopen_xevent(lua_State *L);

#endif
