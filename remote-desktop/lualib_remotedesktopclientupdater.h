#ifndef __LUALIB_REMOTEDESKTOPCLIENTUPDATER_H
#define __LUALIB_REMOTEDESKTOPCLIENTUPDATER_H

#include "remote_desktop_client_updater.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPCLIENTUPDATER "lua_userdata_remotedesktopclientupdater"

CRemoteDesktopClientUpdater* get_remotedesktopclientupdater(lua_State *L, int idx);
lua_userdata *remotedesktopclientupdater_new_userdata(lua_State *L,CRemoteDesktopClientUpdater *pobj,int is_weak);
int luaopen_remotedesktopclientupdater(lua_State *L);
bool is_remotedesktopclientupdater(lua_State *L, int idx);
const luaL_Reg* get_remotedesktopclientupdater_funcs();

#endif
