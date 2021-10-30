#include "lualib_resource.h"
#include "lualib_minibson.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "memfile.h"

LUA_IS_VALID_USER_DATA_FUNC(CResource,resource)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CResource,resource)
LUA_NEW_USER_DATA_FUNC(CResource,resource,RESOURCE)
LUA_GC_FUNC(CResource,resource)
LUA_IS_SAME_FUNC(CResource,resource)
LUA_TO_STRING_FUNC(CResource,resource)

bool is_resource(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_RESOURCE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return resource_is_userdata_valid(ud);  
}

/****************************************/
static int resource_new(lua_State *L)
{
    CResource *pt;
    NEW(pt,CResource);
    pt->Init();
    resource_new_userdata(L,pt,0);
    return 1;
}

static int resource_copy(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    CResource *_p = get_resource(L,2);
    ASSERT(_p);
    int _ret_0 = (int)presource->Copy(_p);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resource_print(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    PRINT_OBJ(*presource);
    return 0;
}
static int resource_gettype(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    int _ret_0 = (int)presource->GetType();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int resource_getid(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    const char* _ret_0 = (const char*)presource->GetIdStr();
    lua_pushstring(L,_ret_0);
    return 1;
}
static int resource_gettheme(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    const char* _ret_0 = (const char*)presource->GetThemeStr();
    lua_pushstring(L,_ret_0);
    return 1;
}
static int resource_getlanguage(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    const char* _ret_0 = (const char*)presource->GetLanguageStr();
    lua_pushstring(L,_ret_0);
    return 1;
}
static int resource_getbsondata(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    CMiniBson *_ret_0 = presource->GetBsonData();
    if(_ret_0==NULL)return 0;
    minibson_new_userdata(L,_ret_0,1);
    return 1;
}
static int resource_settype(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    int _type = (int)lua_tointeger(L,2);
    int _ret_0 = (int)presource->SetType(_type);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resource_setid(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    const char* id = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)presource->SetId(id);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resource_settheme(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    const char* theme = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)presource->SetTheme(theme);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resource_setlanguage(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    const char* _language = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)presource->SetLanguage(_language);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int resource_setbsondata(lua_State *L)
{
    CResource *presource = get_resource(L,1);
    ASSERT(presource);
    CMiniBson *_bsondata = get_minibson(L,2);
    ASSERT(_bsondata);
    int _ret_0 = (int)presource->SetBsonData(_bsondata);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static const luaL_Reg resource_lib[] = {
    {"__gc",resource_gc_},
    {"__tostring",resource_tostring_},
    {"__is_same",resource_issame_},
    {"new",resource_new},
    {"Copy",resource_copy},
    {"Print",resource_print},
    {"GetType",resource_gettype},
    {"GetId",resource_getid},
    {"GetTheme",resource_gettheme},
    {"GetLanguage",resource_getlanguage},
    {"GetBsonData",resource_getbsondata},
    {"SetType",resource_settype},
    {"SetId",resource_setid},
    {"SetTheme",resource_settheme},
    {"SetLanguage",resource_setlanguage},
    {"SetBsonData",resource_setbsondata},
    {NULL, NULL}
};
static int luaL_register_resource(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_RESOURCE);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,resource_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,resource_lib);
    return 1;
}

int luaopen_resource(lua_State *L)
{
    luaL_requiref(L, "Resource",luaL_register_resource,1);
    lua_pop(L, 1);
    return 0;
}
