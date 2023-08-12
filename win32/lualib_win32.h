#ifndef __LUALIB_WIN32_H
#define __LUALIB_WIN32_H

#include "luavm.h"

#define LUA_USERDATA_WIN32 "lua_userdata_win32"

int luaopen_win32(lua_State *L);
const luaL_Reg* get_win32_funcs();
status_t win32_callback_onwindowmessage(lua_State *L, int _funcid, bool _once, uint32_t hwnd, uint32_t msg, uint32_t wparam, uint32_t lparam);

#endif
