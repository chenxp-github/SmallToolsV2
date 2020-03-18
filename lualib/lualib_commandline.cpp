#include "lualib_commandline.h"
#include "lualib_cmdentry.h"
#include "mem_tool.h"
#include "syslog.h"
#include "memfile.h"

LUA_IS_VALID_USER_DATA_FUNC(CCommandLine,commandline)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CCommandLine,commandline)
LUA_NEW_USER_DATA_FUNC(CCommandLine,commandline,COMMANDLINE)
LUA_GC_FUNC(CCommandLine,commandline)
LUA_IS_SAME_FUNC(CCommandLine,commandline)
LUA_TO_STRING_FUNC(CCommandLine,commandline)

bool is_commandline(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_COMMANDLINE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return commandline_is_userdata_valid(ud);  
}
/****************************************/
static int commandline_new(lua_State *L)
{
    CCommandLine *pt;
    NEW(pt,CCommandLine);
    pt->Init();
    commandline_new_userdata(L,pt,0);
    return 1;
}

static int commandline_printhelp(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    pcommandline->PrintHelp();
    return 0;
}
static int commandline_checkforerrors(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    int _ret_0 = (int)pcommandline->CheckForErrors();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int commandline_getvaluebykey(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    const char* key = (const char*)lua_tostring(L,2);
    CMem *value = pcommandline->GetValueByKey(key);
    ASSERT(value);
    lua_pushlstring(L,value->GetRawBuf(),value->StrLen());
    return 1;
}
static int commandline_haskey(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    const char* str = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pcommandline->HasKey(str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int commandline_addkeytype(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    const char* key = (const char*)lua_tostring(L,2);
    int type = (int)lua_tointeger(L,3);
    int option = (int)lua_tointeger(L,4);
    const char* help = (const char*)lua_tostring(L,5);
    int _ret_0 = (int)pcommandline->AddKeyType(key,type,option,help);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int commandline_loadfromargv(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);

    if(!lua_istable(L,2))
        return ERROR;
    
    int top = lua_gettop(L);
    lua_pushnil(L);
    
    const int _MAX = 1024;
    char *argv[_MAX];

    int argc = 0;
    while (lua_next(L, 2) != 0) 
    {   
        ASSERT(argc <= _MAX);
        argv[argc++] = strdup(lua_tostring(L,-1));
        lua_pop(L, 1);
    }
    lua_settop(L,top);

    pcommandline->LoadFromArgv(argc,argv);
    
    for(int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }

    return 0;
}
static int commandline_savetocmdline(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    LOCAL_MEM_WITH_SIZE(mem,32*1024);
    pcommandline->SaveToCmdLine(&mem);
    lua_pushlstring(L,mem.GetRawBuf(),mem.StrLen());
    return 1;
}
static int commandline_delbyindex(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    int index = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pcommandline->DelByIndex(index);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int commandline_delbykey(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    const char* key = (const char*)lua_tostring(L,2);
    ASSERT(key);
    CMem mem_key(key);
    int _ret_0 = (int)pcommandline->DelByKey(&mem_key);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int commandline_print(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    PRINT_OBJ(*pcommandline);
    return 0;
}
static int commandline_loadfromstring(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    const char* str = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pcommandline->LoadFromString(str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int commandline_getcmdentrieslen(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    int _ret_0 = (int)pcommandline->GetCmdEntriesLen();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int commandline_getcmdentry(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    int i = (int)lua_tointeger(L,2);
    CCmdEntry *_ret_0 = pcommandline->GetCmdEntry(i);
    if(_ret_0==NULL)return 0;
    cmdentry_new_userdata(L,_ret_0,1);
    return 1;
}

static status_t commandline_getallvaluesbykey(lua_State *L)
{
    CCommandLine *pcommandline = get_commandline(L,1);
    ASSERT(pcommandline);
    const char* key = (const char*)lua_tostring(L,2);
    ASSERT(key);
    
    CMemStk values;
    values.Init();    

    pcommandline->GetAllValuesByKey(key,&values);
    if(values.GetLen() <= 0)
        return 0;

    CLuaVm::PushStringArray(L,&values);
    return 1;
}

static const luaL_Reg commandline_lib[] = {
    {"__gc",commandline_gc_},
    {"__tostring",commandline_tostring_},
    {"__is_same",commandline_issame_},
    {"new",commandline_new},
    {"PrintHelp",commandline_printhelp},
    {"CheckForErrors",commandline_checkforerrors},
    {"GetValueByKey",commandline_getvaluebykey},
    {"HasKey",commandline_haskey},
    {"AddKeyType",commandline_addkeytype},
    {"LoadFromArgv",commandline_loadfromargv},
    {"SaveToCmdLine",commandline_savetocmdline},
    {"DelByIndex",commandline_delbyindex},
    {"DelByKey",commandline_delbykey},
    {"Print",commandline_print},
    {"LoadFromString",commandline_loadfromstring},
    {"GetCmdEntriesLen",commandline_getcmdentrieslen},
    {"GetCmdEntry",commandline_getcmdentry},
    {"GetAllValuesByKey",commandline_getallvaluesbykey},    
    {NULL, NULL}
};
static int luaL_register_commandline(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_COMMANDLINE);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,commandline_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,commandline_lib);
    return 1;
}

int luaopen_commandline(lua_State *L)
{
    luaL_requiref(L, "CommandLine",luaL_register_commandline,1);
    lua_pop(L, 1);
    return 0;
}
