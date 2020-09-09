#ifndef __LUA_HELPER_H
#define __LUA_HELPER_H

#include "luavm.h"
#include "taskmgr.h"

#define LUA_TO_LOCAL_STRING(var,L,index)\
LOCAL_MEM_WITH_SIZE(_buf_##var,32*1024);\
const char *var = lua_to_local_string(L,index,&_buf_##var);\
ASSERT(var)\

#define LUA_TO_UNICODE_STRING(var,L,index)\
LOCAL_MEM_WITH_SIZE(_buf_##var,32*1024);\
const wchar_t *var = lua_to_unicode_string(L,index,&_buf_##var);\
ASSERT(var)\

const char* lua_push_local_string(lua_State *L, const char *local_string);
const char* lua_to_local_string(lua_State *L, int index,CMem *out);
const wchar_t* lua_to_unicode_string(lua_State *L, int index,CMem *out);

typedef CTaskMgr* (*HOW_TO_GET_GLOBAL_TASKMGR)(lua_State *L);
typedef CLuaVm* (*HOW_TO_GET_GLOBAL_LUAVM)(lua_State *L);
typedef int* (*HOW_TO_GET_LUA_RUNNING_FLAG)(lua_State *L);
typedef const void* (*HOW_TO_GET_PEER_GLOBALS)(lua_State *L);

extern HOW_TO_GET_GLOBAL_TASKMGR how_to_get_global_taskmgr;
extern HOW_TO_GET_GLOBAL_LUAVM how_to_get_global_luavm;
extern HOW_TO_GET_LUA_RUNNING_FLAG how_to_get_lua_running_flag;
extern HOW_TO_GET_PEER_GLOBALS how_to_get_peer_globals;

#endif
