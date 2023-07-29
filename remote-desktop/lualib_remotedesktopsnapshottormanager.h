#ifndef __LUALIB_REMOTEDESKTOPSNAPSHOTTORMANAGER_H
#define __LUALIB_REMOTEDESKTOPSNAPSHOTTORMANAGER_H

#include "remote_desktop_snapshottor_manager.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTORMANAGER "lua_userdata_remotedesktopsnapshottormanager"

CRemoteDesktopSnapshottorManager* get_remotedesktopsnapshottormanager(lua_State *L, int idx);
lua_userdata *remotedesktopsnapshottormanager_new_userdata(lua_State *L,CRemoteDesktopSnapshottorManager *pobj,int is_weak);
int luaopen_remotedesktopsnapshottormanager(lua_State *L);
bool is_remotedesktopsnapshottormanager(lua_State *L, int idx);
const luaL_Reg* get_remotedesktopsnapshottormanager_funcs();

#endif
