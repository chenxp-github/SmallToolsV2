#include "lualib_sharedmemory.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_stream.h"

LUA_IS_VALID_USER_DATA_FUNC(CSharedMemory,sharedmemory)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CSharedMemory,sharedmemory)
LUA_NEW_USER_DATA_FUNC(CSharedMemory,sharedmemory,SHAREDMEMORY)
LUA_GC_FUNC(CSharedMemory,sharedmemory)
LUA_IS_SAME_FUNC(CSharedMemory,sharedmemory)
LUA_TO_STRING_FUNC(CSharedMemory,sharedmemory)

bool is_sharedmemory(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_SHAREDMEMORY,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return sharedmemory_is_userdata_valid(ud);  
}

/****************************************************/
static status_t sharedmemory_new(lua_State *L)
{
    CSharedMemory *psharedmemory;
    NEW(psharedmemory,CSharedMemory);
    psharedmemory->Init();
    sharedmemory_new_userdata(L,psharedmemory,0);
    return 1;
}

static status_t sharedmemory_zero(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    status_t ret0 = psharedmemory->Zero();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_getsize(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    int ret0 = psharedmemory->GetSize();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t sharedmemory_openreadwrite(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    status_t ret0 = psharedmemory->OpenReadWrite();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_openreadonly(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    status_t ret0 = psharedmemory->OpenReadOnly();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_opencreate(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    int size = (int)lua_tointeger(L,2);
    status_t ret0 = psharedmemory->OpenCreate(size);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_setname(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    int name = (int)lua_tointeger(L,2);
    status_t ret0 = psharedmemory->SetName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_unlink(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    status_t ret0 = psharedmemory->Unlink();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_close(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    status_t ret0 = psharedmemory->Close();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_destroy(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
    status_t ret0 = psharedmemory->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t sharedmemory_stream(lua_State *L)
{
    CSharedMemory *psharedmemory = get_sharedmemory(L,1);
    ASSERT(psharedmemory);
   
    if(psharedmemory->GetData() == NULL)
        return 0;
    if(psharedmemory->GetSize() == 0)
        return 0;

    CStream *stream;
    NEW(stream,CStream);
    stream->Init();
    stream->SetRawBuf(psharedmemory->GetData(),psharedmemory->GetSize(),false);
    stream_new_userdata(L,stream,0);
    return 1;    
}

/****************************************************/
static const luaL_Reg sharedmemory_funcs_[] = {
    {"__gc",sharedmemory_gc_},
    {"__tostring",sharedmemory_tostring_},
    {"__is_same",sharedmemory_issame_},
    {"new",sharedmemory_new},
    {"Zero",sharedmemory_zero},
    {"GetSize",sharedmemory_getsize},
    {"OpenReadWrite",sharedmemory_openreadwrite},
    {"OpenReadOnly",sharedmemory_openreadonly},
    {"OpenCreate",sharedmemory_opencreate},
    {"SetName",sharedmemory_setname},
    {"Unlink",sharedmemory_unlink},
    {"Close",sharedmemory_close},
    {"Destroy",sharedmemory_destroy},
    {"Stream",sharedmemory_stream},
    {NULL,NULL},
};

const luaL_Reg* get_sharedmemory_funcs()
{
    return sharedmemory_funcs_;
}

static int luaL_register_sharedmemory(lua_State *L)
{	
    static luaL_Reg _sharedmemory_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_sharedmemory_funcs_,&_index,get_sharedmemory_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_SHAREDMEMORY);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_sharedmemory_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_sharedmemory_funcs_);
    return 1;
}        

int luaopen_sharedmemory(lua_State *L)
{
    luaL_requiref(L, "SharedMemory",luaL_register_sharedmemory,1);
    lua_pop(L, 1);
    return 0;
}        

