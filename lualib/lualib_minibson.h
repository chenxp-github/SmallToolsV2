#ifndef __LUALIB_MINIBSON_H
#define __LUALIB_MINIBSON_H

#include "minibson.h"
#include "luavm.h"

#define LUA_USERDATA_MINIBSON "lua_userdata_minibson"

bool lua_is_minibson(lua_State *L, int idx);
CMiniBson *get_minibson(lua_State *L, int idx);
lua_userdata *minibson_new_userdata(lua_State *L,CMiniBson *pt,int is_weak);
void luaopen_minibson(lua_State *L);
bool is_minibson(lua_State *L, int idx);

#endif

