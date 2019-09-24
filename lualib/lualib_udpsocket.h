#ifndef __LUALIB_UDPSOCKET_H
#define __LUALIB_UDPSOCKET_H

#include "udpsocket.h"
#include "luavm.h"

#define LUA_USERDATA_UDPSOCKET "lua_userdata_udpsocket"

CUdpSocket* get_udpsocket(lua_State *L, int idx);
lua_userdata *udpsocket_new_userdata(lua_State *L,CUdpSocket *pobj,int is_weak);
int luaopen_udpsocket(lua_State *L);
bool is_udpsocket(lua_State *L, int idx);
const luaL_Reg* get_udpsocket_funcs();

#endif
