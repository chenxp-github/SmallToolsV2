#ifndef __LUALIB_XIMAGEHELPER_H
#define __LUALIB_XIMAGEHELPER_H

#include "filebase.h"
#include "mem.h"
#include "ximage.h"
#include "luavm.h"

#define LUA_USERDATA_XIMAGEHELPER "lua_userdata_ximagehelper"

int luaopen_ximagehelper(lua_State *L);

#endif
