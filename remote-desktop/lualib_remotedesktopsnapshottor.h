#ifndef __LUALIB_REMOTEDESKTOPSNAPSHOTTOR_H
#define __LUALIB_REMOTEDESKTOPSNAPSHOTTOR_H

#include "remote_desktop_snapshottor.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR "lua_userdata_remotedesktopsnapshottor"

CRemoteDesktopSnapshottor* get_remotedesktopsnapshottor(lua_State *L, int idx);
lua_userdata *remotedesktopsnapshottor_new_userdata(lua_State *L,CRemoteDesktopSnapshottor *pobj,int is_weak);
int luaopen_remotedesktopsnapshottor(lua_State *L);
bool is_remotedesktopsnapshottor(lua_State *L, int idx);
const luaL_Reg* get_remotedesktopsnapshottor_funcs();

#endif
