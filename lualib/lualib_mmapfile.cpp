#include "lualib_mmapfile.h"
#include "lualib_stream.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"

static bool mmapfile_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CHECK_IS_UD_READABLE(CMMapFile,ud);
    CMMapFile *p = (CMMapFile*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}
bool is_mmapfile(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_testudata(L, idx, LUA_USERDATA_MMAPFILE);
    return mmapfile_is_userdata_valid(ud);
}
CMMapFile *get_mmapfile(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_MMAPFILE);
    ASSERT(ud && ud->p);
    ASSERT(ud->__weak_ref_id != 0);
    CMMapFile *p = (CMMapFile*)ud->p;
    ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
    return p;
}
lua_userdata *mmapfile_new_userdata(lua_State *L,CMMapFile *pt,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pt);
    ud->p = pt;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pt->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_MMAPFILE);
    lua_setmetatable(L,-2);
    return ud;
}

static int mmapfile_new(lua_State *L)
{
    CMMapFile *pt;
    NEW(pt,CMMapFile);
    pt->Init();
    mmapfile_new_userdata(L,pt,0);
    return 1;
}

static int mmapfile_destroy(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_MMAPFILE);
    if(!mmapfile_is_userdata_valid(ud))
        return 0;
    CMMapFile *pmmapfile = (CMMapFile*)ud->p;
    if(!(ud->is_attached))
    {
        DEL(pmmapfile);
    }
    return 0;
}
static int mmapfile_issame(lua_State *L)
{
    CMMapFile *pmmapfile1 = get_mmapfile(L,1);
    ASSERT(pmmapfile1);
    CMMapFile *pmmapfile2 = get_mmapfile(L,2);
    ASSERT(pmmapfile2);
    int is_same = (pmmapfile1==pmmapfile2);
    lua_pushboolean(L,is_same);
    return 1;
}
static int mmapfile_tostring(lua_State *L)
{
    CMMapFile *pmmapfile = get_mmapfile(L,1);
    char buf[1024];
    sprintf(buf,"userdata:mmapfile:%p",pmmapfile);
    lua_pushstring(L,buf);
    return 1;
}
/****************************************/
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
    {"new",mmapfile_new},
    {"__gc",mmapfile_destroy},
    {"__tostring",mmapfile_tostring},
    {"IsSame",mmapfile_issame},
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
