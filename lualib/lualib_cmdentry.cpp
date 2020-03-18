#include "lualib_cmdentry.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CCmdEntry,cmdentry)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CCmdEntry,cmdentry)
LUA_NEW_USER_DATA_FUNC(CCmdEntry,cmdentry,CMDENTRY)
LUA_GC_FUNC(CCmdEntry,cmdentry)
LUA_IS_SAME_FUNC(CCmdEntry,cmdentry)
LUA_TO_STRING_FUNC(CCmdEntry,cmdentry)

bool is_cmdentry(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_CMDENTRY,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return cmdentry_is_userdata_valid(ud);  
}

/****************************************/
static int cmdentry_new(lua_State *L)
{
    CCmdEntry *pt;
    NEW(pt,CCmdEntry);
    pt->Init();
    cmdentry_new_userdata(L,pt,0);
    return 1;
}

static int cmdentry_setoption(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    int option = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pcmdentry->SetOption(option);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int cmdentry_getoption(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    int _ret_0 = (int)pcmdentry->GetOption();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int cmdentry_getkeytype(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    int _ret_0 = (int)pcmdentry->GetKeyType();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int cmdentry_getkey(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    if(pcmdentry->GetKey()->StrLen() > 0)
    {
        const char* _ret_0 = (const char*)pcmdentry->GetKeyStr();
        lua_pushstring(L,_ret_0);
        return 1;
    }
    return 0;
}
static int cmdentry_getvalue(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    if(pcmdentry->GetValue()->StrLen() > 0)
    {
        const char* _ret_0 = (const char*)pcmdentry->GetValueStr();
        lua_pushstring(L,_ret_0);
        return 1;
    }
    return 0;
}
static int cmdentry_setkeytype(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    int _keytype = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pcmdentry->SetKeyType(_keytype);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int cmdentry_setkey(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    const char* _key = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pcmdentry->SetKey(_key);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int cmdentry_setvalue(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    const char* _value = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pcmdentry->SetValue(_value);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int cmdentry_gethelp(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    if(pcmdentry->GetHelp()->StrLen() > 0)
    {
        const char* _ret_0 = (const char*)pcmdentry->GetHelpStr();
        lua_pushstring(L,_ret_0);
        return 1;
    }
    return 0;
}
static int cmdentry_sethelp(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    ASSERT(pcmdentry);
    const char* _help = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pcmdentry->SetHelp(_help);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static const luaL_Reg cmdentry_lib[] = {
    {"__gc",cmdentry_gc_},
    {"__tostring",cmdentry_tostring_},
    {"__is_same",cmdentry_issame_},
    {"new",cmdentry_new},
    {"SetOption",cmdentry_setoption},
    {"GetOption",cmdentry_getoption},
    {"GetKeyType",cmdentry_getkeytype},
    {"GetKey",cmdentry_getkey},
    {"GetValue",cmdentry_getvalue},
    {"SetKeyType",cmdentry_setkeytype},
    {"SetKey",cmdentry_setkey},
    {"SetValue",cmdentry_setvalue},
    {"GetHelp",cmdentry_gethelp},
    {"SetHelp",cmdentry_sethelp},
    {NULL, NULL}
};
static int luaL_register_cmdentry(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_CMDENTRY);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,cmdentry_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,cmdentry_lib);
    return 1;
}

int luaopen_cmdentry(lua_State *L)
{
    luaL_requiref(L, "CmdEntry",luaL_register_cmdentry,1);
    lua_pop(L, 1);
    return 0;
}
