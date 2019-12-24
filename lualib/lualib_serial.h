#ifndef __LUALIB_SERIAL_H
#define __LUALIB_SERIAL_H

#include "serial.h"
#include "luavm.h"

#define LUA_USERDATA_SERIAL "lua_userdata_serial"

CSerial* get_serial(lua_State *L, int idx);
lua_userdata *serial_new_userdata(lua_State *L,CSerial *pobj,int is_weak);
int luaopen_serial(lua_State *L);
bool is_serial(lua_State *L, int idx);
const luaL_Reg* get_serial_funcs();

#endif
