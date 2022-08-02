#ifndef __LUALIB_ICMPSOCKET_H
#define __LUALIB_ICMPSOCKET_H

#include "icmp_socket.h"
#include "luavm.h"

#define LUA_USERDATA_ICMPSOCKET "lua_userdata_icmpsocket"

CIcmpSocket* get_icmpsocket(lua_State *L, int idx);
lua_userdata *icmpsocket_new_userdata(lua_State *L,CIcmpSocket *pobj,int is_weak);
int luaopen_icmpsocket(lua_State *L);
bool is_icmpsocket(lua_State *L, int idx);
const luaL_Reg* get_icmpsocket_funcs();

#endif
