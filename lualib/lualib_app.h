#ifndef __LUALIB_APP_H
#define __LUALIB_APP_H

#include "luavm.h"

#define LUA_USERDATA_APP "lua_userdata_app"

int luaopen_app(lua_State *L);

#endif
