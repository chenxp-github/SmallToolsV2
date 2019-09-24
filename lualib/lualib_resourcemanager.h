#ifndef __LUALIB_RESOURCEMANAGER_H
#define __LUALIB_RESOURCEMANAGER_H

#include "resourcemanager.h"
#include "resource.h"
#include "luavm.h"

#define LUA_USERDATA_RESOURCEMANAGER "lua_userdata_resourcemanager"

CResourceManager *get_resourcemanager(lua_State *L, int idx);
lua_userdata *resourcemanager_new_userdata(lua_State *L,CResourceManager *pt,int is_weak);
int luaopen_resourcemanager(lua_State *L);

#endif
