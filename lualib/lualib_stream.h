#ifndef __LUALIB_STREAM_H
#define __LUALIB_STREAM_H

#include "stream.h"
#include "luavm.h"

#define LUA_USERDATA_STREAM "lua_userdata_stream"

CStream* get_stream(lua_State *L, int idx);
lua_userdata *stream_new_userdata(lua_State *L,CStream *pobj,int is_weak);
int luaopen_stream(lua_State *L);
bool is_stream(lua_State *L, int idx);
const luaL_Reg* get_stream_funcs();

#endif
