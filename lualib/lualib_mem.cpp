#include "lualib_mem.h"
#include "lualib_filebase.h"
#include "lualib_stream.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CMem,mem)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CMem,mem)
LUA_NEW_USER_DATA_FUNC(CMem,mem,MEM)
LUA_GC_FUNC(CMem,mem)
LUA_IS_SAME_FUNC(CMem,mem)
LUA_TO_STRING_FUNC(CMem,mem)

bool is_mem(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_MEM,
		LUA_USERDATA_STREAM,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return mem_is_userdata_valid(ud);  
}


/****************************************/
static status_t mem_new(lua_State *L)
{
    CMem *pmem;
    NEW(pmem,CMem);
    pmem->Init();
    mem_new_userdata(L,pmem,0);
    return 1;
}

static int mem_filebase(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    filebase_new_userdata(L,pmem,1);
    return 1;
}
static int mem_ismalloc(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int _ret_0 = (int)pmem->IsMalloc();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_transfer(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    CMem *from = get_mem(L,2);
    ASSERT(from);
    int _ret_0 = (int)pmem->Transfer(from);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_realloc(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int newSize = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pmem->Realloc(newSize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_strequ(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    CMem *str = get_mem(L,2);
    ASSERT(str);
    int case_sensitive = (int)lua_toboolean(L,3);
    int _ret_0 = (int)pmem->StrEqu(str,case_sensitive!=0);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_c(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int index = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pmem->C(index);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int mem_malloc(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int asize = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pmem->Malloc(asize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_zero(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int _ret_0 = (int)pmem->Zero();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_copy(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pmem->Copy(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_cstr(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    lua_pushlstring(L,pmem->CStr(),pmem->StrLen());
    return 1;
}

static int mem_destroy_(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    SAVE_WEAK_REF_ID(*pmem,w);
    int _ret_0 = (int)pmem->Destroy();
    RESTORE_WEAK_REF_ID(*pmem,w);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mem_getrawbuf(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int_ptr_t _ret_0 = (int_ptr_t)pmem->GetRawBuf();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int mem_setrawbuf_v1(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    void* buf = (void*)lua_tointeger(L,2);
    int_ptr_t size = (int_ptr_t)lua_tointeger(L,3);
    int is_const = (int)lua_toboolean(L,4);
    int _ret_0 = (int)pmem->SetRawBuf(buf,size,is_const!=0);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int mem_setrawbuf_v2(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    
    size_t len = 0;
    const char *str = lua_tolstring(L,2,&len);
    ASSERT(str);
    int ret = pmem->SetRawBuf((void*)str,len,true);
    lua_pushboolean(L,ret);
    return 1;
}

static int mem_setrawbuf(lua_State *L)
{
    if(lua_isnumber(L,2))
        return mem_setrawbuf_v1(L);
    else
        return mem_setrawbuf_v2(L);
}

static int mem_setisreadonly(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    bool read_only = lua_toboolean(L,2)!=0;
    pmem->SetIsReadOnly(read_only);
    return OK;
}

static status_t mem_memcpy_v1(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int_ptr_t *addr = (int_ptr_t*)lua_tointeger(L,2);
    size_t size = (size_t)lua_tointeger(L,3);    
    if(size > (size_t)pmem->GetMaxSize())
        size = (size_t)pmem->GetMaxSize();
    ASSERT(addr != 0 && size > 0);        
    ASSERT(!pmem->mIsConst);
    memcpy(pmem->GetRawBuf(),addr,size); 
    pmem->SetSize(size);
    return 0;
}

static status_t mem_memcpy_v2(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    CMem *mem = get_mem(L,2);
    ASSERT(mem);

    void *addr = mem->GetRawBuf();
    size_t size = (size_t)lua_tointeger(L,3);

    if(size > (size_t)pmem->GetMaxSize())
        size = (size_t)pmem->GetMaxSize();
    ASSERT(addr != 0 && size > 0);                
    ASSERT(!pmem->mIsConst);
    
    memcpy(pmem->GetRawBuf(),addr,size);
    pmem->SetSize(size);
    return 0;
}

static status_t mem_memcpy(lua_State *L)
{
    if(lua_isinteger(L,2) && lua_isinteger(L,3))
    {
        return mem_memcpy_v1(L);
    }

    if(is_mem(L,2))
    {
        return mem_memcpy_v2(L);
    }

    return 0;
}

static status_t mem_slice(lua_State *L)
{
    CMem *pmem = get_mem(L,1);
    ASSERT(pmem);
    int start = (int)lua_tointeger(L,2);
    int size = (int)lua_tointeger(L,3);

    CMem *ret0;
    NEW(ret0,CMem);
    pmem->Slice(start,size,ret0);

    mem_new_userdata(L,ret0,0);
    return 1;
}

static const luaL_Reg mem_lib[] = {
    {"new",mem_new},
    {"__gc",mem_gc_},
    {"__tostring",mem_tostring_},
    {"__is_same",mem_issame_},
    {"FileBase",mem_filebase},
    {"IsMalloc",mem_ismalloc},
    {"Transfer",mem_transfer},
    {"Realloc",mem_realloc},
    {"StrEqu",mem_strequ},
    {"C",mem_c},
    {"Malloc",mem_malloc},
    {"Zero",mem_zero},
    {"Copy",mem_copy},
    {"CStr",mem_cstr},
    {"Destroy",mem_destroy_},
    {"GetRawBuf",mem_getrawbuf},
    {"SetRawBuf",mem_setrawbuf},
    {"SetIsReadOnly",mem_setisreadonly},
    {"MemCpy",mem_memcpy},    
    {"Slice",mem_slice},
    {NULL, NULL}
};

const luaL_Reg* get_mem_funcs()
{
    return mem_lib;
}

static int luaL_register_mem(lua_State *L)
{	
    static luaL_Reg _mem_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_mem_funcs_,&_index,get_filebase_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_mem_funcs_,&_index,get_mem_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_MEM);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_mem_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_mem_funcs_);
    return 1;
}        

int luaopen_mem(lua_State *L)
{
    luaL_requiref(L, "Mem",luaL_register_mem,1);
    lua_pop(L, 1);
    return 0;
}        

