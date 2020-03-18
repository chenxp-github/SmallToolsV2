#include "lualib_mmapfile.h"
#include "lualib_stream.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CMMapFile,mmapfile)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CMMapFile,mmapfile)
LUA_NEW_USER_DATA_FUNC(CMMapFile,mmapfile,MMAPFILE)
LUA_GC_FUNC(CMMapFile,mmapfile)
LUA_IS_SAME_FUNC(CMMapFile,mmapfile)
LUA_TO_STRING_FUNC(CMMapFile,mmapfile)

bool is_mmapfile(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_MMAPFILE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return mmapfile_is_userdata_valid(ud);  
}

/****************************************/
static int mmapfile_new(lua_State *L)
{
    CMMapFile *pt;
    NEW(pt,CMMapFile);
    pt->Init();
    mmapfile_new_userdata(L,pt,0);
    return 1;
}

static status_t mmapfile_destroy(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    SAVE_WEAK_REF_ID(*pmmapfile,w);
    pmmapfile->Destroy();
    RESTORE_WEAK_REF_ID(*pmmapfile,w);
    return 0;
}
static int mmapfile_unlink(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    int _ret_0 = (int)pmmapfile->Unlink();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mmapfile_sync(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    int _ret_0 = (int)pmmapfile->Sync();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mmapfile_getsize(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    int _ret_0 = (int)pmmapfile->GetSize();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int mmapfile_openreadwrite(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    const char* filename = (const char*)lua_tostring(L,2);
    ASSERT(filename);
    int _ret_0 = (int)pmmapfile->OpenReadWrite(filename);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mmapfile_opencreate(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
	LUA_TO_LOCAL_STRING(filename,L,2);
    int size = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pmmapfile->OpenCreate(filename,size);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mmapfile_openreadonly(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
	LUA_TO_LOCAL_STRING(filename,L,2);
    int _ret_0 = (int)pmmapfile->OpenReadOnly(filename);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mmapfile_close(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    int _ret_0 = (int)pmmapfile->Close();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mmapfile_setfilename(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    const char* filename = (const char*)lua_tostring(L,2);
    ASSERT(filename);
    int _ret_0 = (int)pmmapfile->SetFileName(filename);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int mmapfile_stream(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    ASSERT(pmmapfile);
    
    if(pmmapfile->GetData() == NULL)
        return 0;
    if(pmmapfile->GetSize() == 0)
        return 0;

    CStream *stream;
    NEW(stream,CStream);
    stream->Init();
    stream->SetRawBuf(pmmapfile->GetData(),pmmapfile->GetSize(),false);
    stream_new_userdata(L,stream,0);
    return 1;
}
static const luaL_Reg mmapfile_lib[] = {
    {"__gc",mmapfile_gc_},
    {"__tostring",mmapfile_tostring_},
    {"__is_same",mmapfile_issame_},
    {"new",mmapfile_new},
    {"Destroy",mmapfile_destroy},
    {"Unlink",mmapfile_unlink},
    {"Sync",mmapfile_sync},
    {"GetSize",mmapfile_getsize},
    {"OpenReadWrite",mmapfile_openreadwrite},
    {"OpenCreate",mmapfile_opencreate},
    {"OpenReadOnly",mmapfile_openreadonly},
    {"Close",mmapfile_close},
    {"SetFileName",mmapfile_setfilename},
    {"Stream",mmapfile_stream},
    {NULL, NULL}
};
static int luaL_register_mmapfile(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_MMAPFILE);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,mmapfile_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,mmapfile_lib);
    return 1;
}

int luaopen_mmapfile(lua_State *L)
{
    luaL_requiref(L, "MMapFile",luaL_register_mmapfile,1);
    lua_pop(L, 1);
    return 0;
}
