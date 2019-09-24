#ifndef __LUALIB_MMAPFILE_H
#define __LUALIB_MMAPFILE_H

#include "mmapfile.h"
#include "stream.h"
#include "luavm.h"

#define LUA_USERDATA_MMAPFILE "lua_userdata_mmapfile"

CMMapFile *get_mmapfile(lua_State *L, int idx);
lua_userdata *mmapfile_new_userdata(lua_State *L,CMMapFile *pt,int is_weak);
int luaopen_mmapfile(lua_State *L);
bool is_mmapfile(lua_State *L, int idx);

#endif
