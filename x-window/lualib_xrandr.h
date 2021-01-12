#ifndef __LUALIB_XRANDR_H
#define __LUALIB_XRANDR_H

#include "xrandr.h"
#include "luavm.h"

#define LUA_USERDATA_XRANDR "lua_userdata_xrandr"

int luaopen_xrandr(lua_State *L);


#endif
