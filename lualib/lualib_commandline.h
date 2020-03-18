#ifndef __LUALIB_COMMANDLINE_H
#define __LUALIB_COMMANDLINE_H

#include "commandline.h"
#include "luavm.h"

#define LUA_USERDATA_COMMANDLINE "lua_userdata_commandline"

CCommandLine *get_commandline(lua_State *L, int idx);
lua_userdata *commandline_new_userdata(lua_State *L,CCommandLine *pt,int is_weak);
int luaopen_commandline(lua_State *L);
bool is_commandline(lua_State *L, int idx);

#endif
