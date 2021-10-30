#ifndef __LUALIB_XFILESERVER_H
#define __LUALIB_XFILESERVER_H

#include "x_file_server.h"
#include "luavm.h"

#define LUA_USERDATA_XFILESERVER "lua_userdata_xfileserver"

CxFileServer* get_xfileserver(lua_State *L, int idx);
lua_userdata *xfileserver_new_userdata(lua_State *L,CxFileServer *pobj,int is_weak);
int luaopen_xfileserver(lua_State *L);
bool is_xfileserver(lua_State *L, int idx);
const luaL_Reg* get_xfileserver_funcs();

#endif
