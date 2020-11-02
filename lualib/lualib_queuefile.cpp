#include "lualib_queuefile.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_mem.h"
#include "lualib_filebase.h"

LUA_IS_VALID_USER_DATA_FUNC(CQueueFile,queuefile)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CQueueFile,queuefile)
LUA_NEW_USER_DATA_FUNC(CQueueFile,queuefile,QUEUEFILE)
LUA_GC_FUNC(CQueueFile,queuefile)
LUA_IS_SAME_FUNC(CQueueFile,queuefile)
LUA_TO_STRING_FUNC(CQueueFile,queuefile)

bool is_queuefile(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_QUEUEFILE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return queuefile_is_userdata_valid(ud);  
}
/****************************************************/
static status_t queuefile_new(lua_State *L)
{
    int_ptr_t bytes = (int_ptr_t)lua_tointeger(L,1);
    CQueueFile *pqueuefile;
    NEW(pqueuefile,CQueueFile);
    pqueuefile->Init(bytes);
    queuefile_new_userdata(L,pqueuefile,0);
    return 1;
}

static status_t queuefile_findtag(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    CMem *tag = get_mem(L,2);
    ASSERT(tag);
    int size = (int)lua_tointeger(L,3);
    int_ptr_t ret0 = pqueuefile->FindTag(tag->GetRawBuf(),size);
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t queuefile_peekdata(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    CMem *data = get_mem(L,2);
    ASSERT(data);
    int_ptr_t size = (int_ptr_t)lua_tointeger(L,3);
    if(size > (int_ptr_t)data->GetMaxSize())
        size = (int_ptr_t)data->GetMaxSize();

    int_ptr_t ret0 = pqueuefile->PeekData(data->GetRawBuf(),size);
    data->SetSize(ret0);

    lua_pushinteger(L,ret0);
    return 1;
}

static status_t queuefile_clear(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    int ret0 = pqueuefile->Clear();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t queuefile_destroy(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    status_t ret0 = pqueuefile->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t queuefile_getfreesize(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    fsize_t ret0 = pqueuefile->GetFreeSize();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t queuefile_hasemptyline(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    bool ret0 = pqueuefile->HasEmptyLine();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t queuefile_haswholeline(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    bool ret0 = pqueuefile->HasWholeLine();
    lua_pushboolean(L,ret0);
    return 1;
}
static status_t queuefile_skip(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    int_ptr_t size = (int_ptr_t)lua_tointeger(L,2);
    int_ptr_t rs = pqueuefile->Read(NULL,size);
    lua_pushinteger(L,rs);
    return 1;
}
static status_t queuefile_savecontext(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    CMem *context = get_mem(L,2);
    ASSERT(context);
    status_t ret0 = pqueuefile->SaveContext(context->GetRawBuf());
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t queuefile_restorecontext(lua_State *L)
{
    CQueueFile *pqueuefile = get_queuefile(L,1);
    ASSERT(pqueuefile);
    CMem *context = get_mem(L,2);
    ASSERT(context);
    status_t ret0 = pqueuefile->RestoreContext(context->GetRawBuf());
    lua_pushboolean(L,ret0);
    return 1;
}
/****************************************************/
static const luaL_Reg queuefile_funcs_[] = {
    {"__gc",queuefile_gc_},
    {"__tostring",queuefile_tostring_},
    {"__is_same",queuefile_issame_},
    {"new",queuefile_new},
    {"FindTag",queuefile_findtag},
    {"PeekData",queuefile_peekdata},
    {"Clear",queuefile_clear},
    {"Destroy",queuefile_destroy},
    {"GetFreeSize",queuefile_getfreesize},
    {"HasEmptyLine",queuefile_hasemptyline},
    {"HasWholeLine",queuefile_haswholeline},
    {"Skip",queuefile_skip},
    {"SaveContext",queuefile_savecontext},
    {"RestoreContext",queuefile_restorecontext},    
    {NULL,NULL},
};

const luaL_Reg* get_queuefile_funcs()
{
    return queuefile_funcs_;
}

static int luaL_register_queuefile(lua_State *L)
{	
    static luaL_Reg _queuefile_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_queuefile_funcs_,&_index,get_filebase_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_queuefile_funcs_,&_index,get_queuefile_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_QUEUEFILE);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_queuefile_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_queuefile_funcs_);
    return 1;
}        

int luaopen_queuefile(lua_State *L)
{
    luaL_requiref(L, "QueueFile",luaL_register_queuefile,1);
    lua_pop(L, 1);
    return 0;
}        

