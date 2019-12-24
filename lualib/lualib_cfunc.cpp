#include "lualib_cfunc.h"
#include "mem_tool.h"
#include "syslog.h"
#include "misc.h"

/****************************************/
static int cfunc_memcmp(lua_State *L)
{
    int_ptr_t s1 = (int_ptr_t)lua_tointeger(L,1);
    int_ptr_t s2 = (int_ptr_t)lua_tointeger(L,2);
    int_ptr_t n = (int_ptr_t)lua_tointeger(L,3);
    int _ret_0 = (int)memcmp((const void*)s1,(const void*)s2,(size_t)n);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int cfunc_wild_match(lua_State *L)
{
    const char* pattern = (const char*)lua_tostring(L,1);
    ASSERT(pattern);
    const char* str = (const char*)lua_tostring(L,2);
    ASSERT(str);
    int _ret_0 = (int)wild_match(pattern,str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int cfunc_memcpy(lua_State *L)
{
    int_ptr_t s1 = (int_ptr_t)lua_tointeger(L,1);
    int_ptr_t s2 = (int_ptr_t)lua_tointeger(L,2);
    int_ptr_t n = (int_ptr_t)lua_tointeger(L,3);
    memcpy((void*)s1,(const void*)s2,(size_t)n);
    return 0;
}

#if _IS_LINUX_
static int cfunc_getpid(lua_State *L)
{
    lua_pushinteger(L,getpid());
    return 1;
}
#endif

static const luaL_Reg cfunc_lib[] = {
    {"memcmp",cfunc_memcmp},
    {"memcpy",cfunc_memcpy},
    {"wild_match",cfunc_wild_match},
#if _IS_LINUX_
    {"getpid",cfunc_getpid},
#endif
    {NULL, NULL}
};
static int luaL_register_cfunc(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_CFUNC);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,cfunc_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,cfunc_lib);
    return 1;
}

int luaopen_cfunc(lua_State *L)
{
    luaL_requiref(L, "CFunc",luaL_register_cfunc,1);
    lua_pop(L, 1);
    return 0;
}
