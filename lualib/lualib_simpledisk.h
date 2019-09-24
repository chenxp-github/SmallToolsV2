#ifndef __LUALIB_SIMPLEDISK_H
#define __LUALIB_SIMPLEDISK_H

#include "simpledisk.h"
#include "filebase.h"
#include "luavm.h"

#define LUA_USERDATA_SIMPLEDISK "lua_userdata_simpledisk"

CSimpleDisk *get_simpledisk(lua_State *L, int idx);
lua_userdata *simpledisk_new_userdata(lua_State *L,CSimpleDisk *pt,int is_weak);
int luaopen_simpledisk(lua_State *L);
bool is_simpledisk(lua_State *L, int idx);

#endif
