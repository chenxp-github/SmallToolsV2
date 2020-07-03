#include "lualib_luahdc.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CLuaHdc,luahdc)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CLuaHdc,luahdc)
LUA_NEW_USER_DATA_FUNC(CLuaHdc,luahdc,LUAHDC)
LUA_GC_FUNC(CLuaHdc,luahdc)
LUA_IS_SAME_FUNC(CLuaHdc,luahdc)
LUA_TO_STRING_FUNC(CLuaHdc,luahdc)

bool is_luahdc(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_LUAHDC,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return luahdc_is_userdata_valid(ud);  
}

/****************************************************/
static status_t luahdc_destroy(lua_State *L)
{
    CLuaHdc *pluahdc = get_luahdc(L,1);
    ASSERT(pluahdc);
    status_t ret0 = pluahdc->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg luahdc_funcs_[] = {
    {"__gc",luahdc_gc_},
    {"__tostring",luahdc_tostring_},
    {"__is_same",luahdc_issame_},
    {"Destroy",luahdc_destroy},
    {NULL,NULL},
};

const luaL_Reg* get_luahdc_funcs()
{
    return luahdc_funcs_;
}

static int luaL_register_luahdc(lua_State *L)
{	
    static luaL_Reg _luahdc_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_luahdc_funcs_,&_index,get_luahdc_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_LUAHDC);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_luahdc_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_luahdc_funcs_);
    return 1;
}        

int luaopen_luahdc(lua_State *L)
{
    luaL_requiref(L, "Hdc",luaL_register_luahdc,1);
    lua_pop(L, 1);
    return 0;
}        

