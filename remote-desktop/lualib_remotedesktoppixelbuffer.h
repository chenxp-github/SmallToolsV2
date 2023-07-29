#ifndef __LUALIB_REMOTEDESKTOPPIXELBUFFER_H
#define __LUALIB_REMOTEDESKTOPPIXELBUFFER_H

#include "remote_desktop_pixel_buffer.h"
#include "luavm.h"

#define LUA_USERDATA_REMOTEDESKTOPPIXELBUFFER "lua_userdata_remotedesktoppixelbuffer"

CRemoteDesktopPixelBuffer* get_remotedesktoppixelbuffer(lua_State *L, int idx);
lua_userdata *remotedesktoppixelbuffer_new_userdata(lua_State *L,CRemoteDesktopPixelBuffer *pobj,int is_weak);
int luaopen_remotedesktoppixelbuffer(lua_State *L);
bool is_remotedesktoppixelbuffer(lua_State *L, int idx);
const luaL_Reg* get_remotedesktoppixelbuffer_funcs();

#endif
