#ifndef __LUALIB_TASKTIMER_H
#define __LUALIB_TASKTIMER_H

#include "tasktimer.h"
#include "luavm.h"

#define LUA_USERDATA_TASKTIMER "lua_userdata_tasktimer"

CTaskTimer *get_tasktimer(lua_State *L, int idx);
lua_userdata *tasktimer_new_userdata(lua_State *L,CTaskTimer *pt,int is_weak);
int luaopen_tasktimer(lua_State *L);
bool is_tasktimer(lua_State *L, int idx);

#endif
