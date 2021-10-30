#include "lualib_resourcemanager.h"
#include "lualib_resource.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "memfile.h"

LUA_IS_VALID_USER_DATA_FUNC(CResourceManager,resourcemanager)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CResourceManager,resourcemanager)
LUA_NEW_USER_DATA_FUNC(CResourceManager,resourcemanager,RESOURCEMANAGER)
LUA_GC_FUNC(CResourceManager,resourcemanager)
LUA_IS_SAME_FUNC(CResourceManager,resourcemanager)
LUA_TO_STRING_FUNC(CResourceManager,resourcemanager)

bool is_resourcemanager(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_RESOURCEMANAGER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return resourcemanager_is_userdata_valid(ud);  
}

/****************************************/
static int resourcemanager_new(lua_State *L)
{
    CResourceManager *pt;
    NEW(pt,CResourceManager);
    pt->Init();
    resourcemanager_new_userdata(L,pt,0);
    return 1;
}

static int resourcemanager_loadresourcefile(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    const char* filename = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)presourcemanager->LoadResourceFile(filename);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resourcemanager_searchbystring(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    const char* theme = (const char*)lua_tostring(L,2);
    ASSERT(theme);
    const char* language = (const char*)lua_tostring(L,3);
    ASSERT(language);
    const char* id = (const char*)lua_tostring(L,4);
    ASSERT(id);
    CResource *value = get_resource(L,5);
    ASSERT(value);
    int _ret_0 = (int)presourcemanager->SearchByString(theme,language,id,value);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resourcemanager_searchresource(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    CResource *key = get_resource(L,2);
    ASSERT(key);
    CResource *value = get_resource(L,3);
    ASSERT(value);
    int _ret_0 = (int)presourcemanager->SearchResource(key,value);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resourcemanager_saveresourcefile(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    const char* filename = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)presourcemanager->SaveResourceFile(filename);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resourcemanager_addresource(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    CResource *res = get_resource(L,2);
    ASSERT(res);
    int _ret_0 = (int)presourcemanager->AddResource(res);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resourcemanager_print(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    PRINT_OBJ(*presourcemanager);
    return 0;
}
static int resourcemanager_setpathprefix(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    const char* _pathprefix = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)presourcemanager->SetPathPrefix(_pathprefix);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resourcemanager_getpathprefix(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    const char* _ret_0 = (const char*)presourcemanager->GetPathPrefixStr();
    lua_pushstring(L,_ret_0);
    return 1;
}
static int resourcemanager_dumpresourcefile(lua_State *L)
{
    CResourceManager *presourcemanager = get_resourcemanager(L,1);
    ASSERT(presourcemanager);
    const char* filename = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)presourcemanager->DumpResourceFile(filename);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static const luaL_Reg resourcemanager_lib[] = {
    {"__gc",resourcemanager_gc_},
    {"__tostring",resourcemanager_tostring_},
    {"__is_same",resourcemanager_issame_},
    {"new",resourcemanager_new},
    {"LoadResourceFile",resourcemanager_loadresourcefile},
    {"SearchByString",resourcemanager_searchbystring},
    {"SearchResource",resourcemanager_searchresource},
    {"SaveResourceFile",resourcemanager_saveresourcefile},
    {"AddResource",resourcemanager_addresource},
    {"Print",resourcemanager_print},
    {"SetPathPrefix",resourcemanager_setpathprefix},
    {"GetPathPrefix",resourcemanager_getpathprefix},
    {"DumpResourceFile",resourcemanager_dumpresourcefile},
    {NULL, NULL}
};
static int luaL_register_resourcemanager(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_RESOURCEMANAGER);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,resourcemanager_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,resourcemanager_lib);
    return 1;
}

int luaopen_resourcemanager(lua_State *L)
{
    luaL_requiref(L, "ResourceManager",luaL_register_resourcemanager,1);
    lua_pop(L, 1);
    return 0;
}
