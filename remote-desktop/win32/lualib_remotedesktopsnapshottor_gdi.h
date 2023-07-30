#ifndef __LUALIB_REMOTEDESKTOPSNAPSHOTTOR_GDI_H
#define __LUALIB_REMOTEDESKTOPSNAPSHOTTOR_GDI_H

#include "remote_desktop_snapshottor_gdi.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_GDI "lua_userdata_remotedesktopsnapshottor_gdi"

CRemoteDesktopSnapshottor_GDI* get_remotedesktopsnapshottor_gdi(lua_State *L, int idx);
lua_userdata *remotedesktopsnapshottor_gdi_new_userdata(lua_State *L,CRemoteDesktopSnapshottor_GDI *pobj,int is_weak);
int luaopen_remotedesktopsnapshottor_gdi(lua_State *L);
bool is_remotedesktopsnapshottor_gdi(lua_State *L, int idx);
const luaL_Reg* get_remotedesktopsnapshottor_gdi_funcs();

#endif
