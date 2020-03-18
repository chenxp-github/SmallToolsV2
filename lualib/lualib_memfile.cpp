#include "lualib_memfile.h"
#include "lualib_filebase.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CMemFile,memfile)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CMemFile,memfile)
LUA_NEW_USER_DATA_FUNC(CMemFile,memfile,MEMFILE)
LUA_GC_FUNC(CMemFile,memfile)
LUA_IS_SAME_FUNC(CMemFile,memfile)
LUA_TO_STRING_FUNC(CMemFile,memfile)


bool is_memfile(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_MEMFILE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return memfile_is_userdata_valid(ud);  
}

/****************************************/
static status_t memfile_new(lua_State *L)
{
    CMemFile *pmemfile;
    NEW(pmemfile,CMemFile);
    pmemfile->Init();
    memfile_new_userdata(L,pmemfile,0);
    return 1;
}

static int memfile_transfer(lua_State *L)
{
    CMemFile *pmemfile = get_memfile(L,1);
    ASSERT(pmemfile);
    CMemFile *from = get_memfile(L,2);
    ASSERT(from);
    int _ret_0 = (int)pmemfile->Transfer(from);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int memfile_init_v1(lua_State *L)
{
    CMemFile *pmemfile = get_memfile(L,1);
    ASSERT(pmemfile);
    
    SAVE_WEAK_REF_ID(*pmemfile,w);
    int _ret_0 = (int)pmemfile->Init();
    RESTORE_WEAK_REF_ID(*pmemfile,w);

    lua_pushboolean(L,_ret_0);
    return 1;
}
static int memfile_init_v2(lua_State *L)
{
    CMemFile *pmemfile = get_memfile(L,1);
    ASSERT(pmemfile);
    int page_size = (int)lua_tointeger(L,2);
    int max_pages = (int)lua_tointeger(L,3);
    SAVE_WEAK_REF_ID(*pmemfile,w);
    int _ret_0 = (int)pmemfile->Init(page_size,max_pages);
    RESTORE_WEAK_REF_ID(*pmemfile,w);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int memfile_init(lua_State *L)
{
    if(lua_isinteger(L,2) && lua_isinteger(L,3))
        return memfile_init_v2(L);
    else
        return memfile_init_v1(L);
}
static int memfile_destroy_(lua_State *L)
{
    CMemFile *pmemfile = get_memfile(L,1);
    ASSERT(pmemfile);
    SAVE_WEAK_REF_ID(*pmemfile,w);
    int _ret_0 = (int)pmemfile->Destroy();
    RESTORE_WEAK_REF_ID(*pmemfile,w);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int memfile_filebase(lua_State *L)
{
    CMemFile *pmemfile = get_memfile(L,1);
    ASSERT(pmemfile);
    filebase_new_userdata(L,pmemfile,1);
    return 1;
}
static const luaL_Reg memfile_lib[] = {
    {"new",memfile_new},
    {"__gc",memfile_gc_},
    {"__tostring",memfile_tostring_},
    {"IsSame",memfile_issame_},
    {"Transfer",memfile_transfer},
    {"Init",memfile_init},
    {"Destroy",memfile_destroy_},
    {"FileBase",memfile_filebase},
    {NULL, NULL}
};

const luaL_Reg* get_memfile_funcs()
{
    return memfile_lib;
}

static int luaL_register_memfile(lua_State *L)
{	
    static luaL_Reg _memfile_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_memfile_funcs_,&_index,get_filebase_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_memfile_funcs_,&_index,get_memfile_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_MEMFILE);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_memfile_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_memfile_funcs_);
    return 1;
}        

int luaopen_memfile(lua_State *L)
{
    luaL_requiref(L, "MemFile",luaL_register_memfile,1);
    lua_pop(L, 1);
    return 0;
}        
