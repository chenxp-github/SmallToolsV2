#include "lualib_cmdentry.h"
#include "mem_tool.h"
#include "syslog.h"

static bool cmdentry_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CCmdEntry *p = (CCmdEntry*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}
bool is_cmdentry(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_testudata(L, idx, LUA_USERDATA_CMDENTRY);
    return cmdentry_is_userdata_valid(ud);
}
CCmdEntry *get_cmdentry(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_CMDENTRY);
    ASSERT(ud && ud->p);
    ASSERT(ud->__weak_ref_id != 0);
    CCmdEntry *p = (CCmdEntry*)ud->p;
    ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
    return p;
}
lua_userdata *cmdentry_new_userdata(lua_State *L,CCmdEntry *pt,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pt);
    ud->p = pt;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pt->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_CMDENTRY);
    lua_setmetatable(L,-2);
    return ud;
}

static int cmdentry_new(lua_State *L)
{
    CCmdEntry *pt;
    NEW(pt,CCmdEntry);
    pt->Init();
    cmdentry_new_userdata(L,pt,0);
    return 1;
}

static int cmdentry_destroy(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_CMDENTRY);
    if(!cmdentry_is_userdata_valid(ud))
        return 0;
    CCmdEntry *pcmdentry = (CCmdEntry*)ud->p;
    if(!(ud->is_attached))
    {
        DEL(pcmdentry);
    }
    return 0;
}
static int cmdentry_issame(lua_State *L)
{
    CCmdEntry *pcmdentry1 = get_cmdentry(L,1);
    ASSERT(pcmdentry1);
    CCmdEntry *pcmdentry2 = get_cmdentry(L,2);
    ASSERT(pcmdentry2);
    int is_same = (pcmdentry1==pcmdentry2);
    lua_pushboolean(L,is_same);
    return 1;
}
static int cmdentry_tostring(lua_State *L)
{
    CCmdEntry *pcmdentry = get_cmdentry(L,1);
    char buf[1024];
    sprintf(buf,"userdata:cmdentry:%p",pcmdentry);
    lua_pushstring(L,buf);
    return 1;
}
/****************************************/
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
    {"new",cmdentry_new},
    {"__gc",cmdentry_destroy},
    {"__tostring",cmdentry_tostring},
    {"IsSame",cmdentry_issame},
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
