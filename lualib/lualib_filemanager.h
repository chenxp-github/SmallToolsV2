#ifndef __LUALIB_FILEMANAGER_H
#define __LUALIB_FILEMANAGER_H

#include "luavm.h"

#define LUA_USERDATA_FILEMANAGER "lua_userdata_filemanager"

int luaopen_filemanager(lua_State *L);

#endif
