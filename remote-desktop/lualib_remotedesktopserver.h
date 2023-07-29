#ifndef __LUALIB_REMOTEDESKTOPSERVER_H
#define __LUALIB_REMOTEDESKTOPSERVER_H

#include "remote_desktop_server.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPSERVER "lua_userdata_remotedesktopserver"

CRemoteDesktopServer* get_remotedesktopserver(lua_State *L, int idx);
lua_userdata *remotedesktopserver_new_userdata(lua_State *L,CRemoteDesktopServer *pobj,int is_weak);
int luaopen_remotedesktopserver(lua_State *L);
bool is_remotedesktopserver(lua_State *L, int idx);
const luaL_Reg* get_remotedesktopserver_funcs();

#endif
