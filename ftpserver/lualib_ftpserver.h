#ifndef __LUALIB_FTPSERVER_H
#define __LUALIB_FTPSERVER_H

#include "ftpserver.h"
#include "luavm.h"

#define LUA_USERDATA_FTPSERVER "lua_userdata_ftpserver"

CFtpServer* get_ftpserver(lua_State *L, int idx);
lua_userdata *ftpserver_new_userdata(lua_State *L,CFtpServer *pobj,int is_weak);
int luaopen_ftpserver(lua_State *L);
bool is_ftpserver(lua_State *L, int idx);
const luaL_Reg* get_ftpserver_funcs();

#endif
