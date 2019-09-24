#ifndef __LUALIB_TCPSOCKET_H
#define __LUALIB_TCPSOCKET_H

#include "tcp.h"
#include "luavm.h"

#define LUA_USERDATA_TCPSOCKET "lua_userdata_tcpsocket"

typedef CSocket CTcpSocket;

CTcpSocket* get_tcpsocket(lua_State *L, int idx);
lua_userdata *tcpsocket_new_userdata(lua_State *L,CTcpSocket *pobj,int is_weak);
int luaopen_tcpsocket(lua_State *L);
bool is_tcpsocket(lua_State *L, int idx);
const luaL_Reg* get_tcpsocket_funcs();

#endif
