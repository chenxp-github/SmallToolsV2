#ifndef __LUALIB_REMOTEDESKTOPCLIENT_H
#define __LUALIB_REMOTEDESKTOPCLIENT_H

#include "remote_desktop_client.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPCLIENT "lua_userdata_remotedesktopclient"

CRemoteDesktopClient* get_remotedesktopclient(lua_State *L, int idx);
lua_userdata *remotedesktopclient_new_userdata(lua_State *L,CRemoteDesktopClient *pobj,int is_weak);
int luaopen_remotedesktopclient(lua_State *L);
bool is_remotedesktopclient(lua_State *L, int idx);
const luaL_Reg* get_remotedesktopclient_funcs();

#endif
