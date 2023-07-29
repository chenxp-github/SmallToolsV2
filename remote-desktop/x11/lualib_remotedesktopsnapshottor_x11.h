#ifndef __LUALIB_REMOTEDESKTOPSNAPSHOTTOR_X11_H
#define __LUALIB_REMOTEDESKTOPSNAPSHOTTOR_X11_H

#include "remote_desktop_snapshottor_x11.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_X11 "lua_userdata_remotedesktopsnapshottor_x11"

CRemoteDesktopSnapshottor_X11* get_remotedesktopsnapshottor_x11(lua_State *L, int idx);
lua_userdata *remotedesktopsnapshottor_x11_new_userdata(lua_State *L,CRemoteDesktopSnapshottor_X11 *pobj,int is_weak);
int luaopen_remotedesktopsnapshottor_x11(lua_State *L);
bool is_remotedesktopsnapshottor_x11(lua_State *L, int idx);
const luaL_Reg* get_remotedesktopsnapshottor_x11_funcs();

#endif
