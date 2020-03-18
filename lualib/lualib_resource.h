#ifndef __LUALIB_RESOURCE_H
#define __LUALIB_RESOURCE_H

#include "resource.h"
#include "minibson.h"
#include "luavm.h"

#define LUA_USERDATA_RESOURCE "lua_userdata_resource"

CResource *get_resource(lua_State *L, int idx);
lua_userdata *resource_new_userdata(lua_State *L,CResource *pt,int is_weak);
int luaopen_resource(lua_State *L);
bool is_resource(lua_State *L, int idx);

#endif
