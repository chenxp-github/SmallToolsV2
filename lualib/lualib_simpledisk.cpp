#include "lualib_simpledisk.h"
#include "lualib_filebase.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CSimpleDisk,simpledisk)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CSimpleDisk,simpledisk)
LUA_NEW_USER_DATA_FUNC(CSimpleDisk,simpledisk,SIMPLEDISK)
LUA_GC_FUNC(CSimpleDisk,simpledisk)
LUA_IS_SAME_FUNC(CSimpleDisk,simpledisk)
LUA_TO_STRING_FUNC(CSimpleDisk,simpledisk)

bool is_simpledisk(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_SIMPLEDISK,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return simpledisk_is_userdata_valid(ud);  
}
/****************************************/
static int simpledisk_new(lua_State *L)
{
    CSimpleDisk *pt;
    NEW(pt,CSimpleDisk);
    pt->Init();
    simpledisk_new_userdata(L,pt,0);
    return 1;
}

static int simpledisk_buildfastfsindex(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    CFileBase *out = get_filebase(L,2);
    ASSERT(out);
    int _ret_0 = (int)psimpledisk->BuildFastFsIndex(out);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_rebuildfastindex(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int _ret_0 = (int)psimpledisk->RebuildFastIndex();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_addfastfsindex(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int _ret_0 = (int)psimpledisk->AddFastFsIndex();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_loadfastfsindex(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int index = (int)lua_tointeger(L,2);
    int _ret_0 = (int)psimpledisk->LoadFastFsIndex(index);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_getfileoffsetandsize(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int index = (int)lua_tointeger(L,2);
    fsize_t offset,size;
    psimpledisk->GetFileOffsetAndSize(index,&offset,&size);
    lua_pushinteger(L,offset);
    lua_pushinteger(L,size);
    return 2;
}
static int simpledisk_setdstfile(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    CFileBase *iFile = get_filebase(L,2);
    ASSERT(iFile);
    int _ret_0 = (int)psimpledisk->SetDstFile(iFile);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_addfile(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    CFileBase *file = get_filebase(L,3);
    ASSERT(file);
    int _ret_0 = (int)psimpledisk->AddFile(name,file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_save(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int _ret_0 = (int)psimpledisk->Save();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_addfolder(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    const char* dir = (const char*)lua_tostring(L,2);
    ASSERT(dir);
    int _ret_0 = (int)psimpledisk->AddFolder(dir);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_loaddiskimage(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)psimpledisk->LoadDiskImage(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_getfilesize(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int index = (int)lua_tointeger(L,2);
    int _ret_0 = (int)psimpledisk->GetFileSize(index);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int simpledisk_getfile(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int index = (int)lua_tointeger(L,2);
    CFileBase *file = get_filebase(L,3);
    ASSERT(file);
    int _ret_0 = (int)psimpledisk->GetFile(index,file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_extractallfiles(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    const char* destDir = (const char*)lua_tostring(L,2);
    ASSERT(destDir);
    int _ret_0 = (int)psimpledisk->ExtractAllFiles(destDir);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_reset(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int _ret_0 = (int)psimpledisk->Reset();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_endfolder(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    int offset = (int)lua_tointeger(L,2);
    int _ret_0 = (int)psimpledisk->EndFolder(offset);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int simpledisk_beginfolder(lua_State *L)
{
    CSimpleDisk *psimpledisk = get_simpledisk(L,1);
    ASSERT(psimpledisk);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    fsize_t offset = 0;
    psimpledisk->BeginFolder(name,&offset);
    lua_pushinteger(L,offset);
    return 1;
}
static const luaL_Reg simpledisk_lib[] = {
    {"__gc",simpledisk_gc_},
    {"__tostring",simpledisk_tostring_},
    {"__is_same",simpledisk_issame_},
    {"new",simpledisk_new},
    {"BuildFastFsIndex",simpledisk_buildfastfsindex},
    {"RebuildFastIndex",simpledisk_rebuildfastindex},
    {"AddFastFsIndex",simpledisk_addfastfsindex},
    {"LoadFastFsIndex",simpledisk_loadfastfsindex},
    {"GetFileOffsetAndSize",simpledisk_getfileoffsetandsize},
    {"SetDstFile",simpledisk_setdstfile},
    {"AddFile",simpledisk_addfile},
    {"Save",simpledisk_save},
    {"AddFolder",simpledisk_addfolder},
    {"LoadDiskImage",simpledisk_loaddiskimage},
    {"GetFileSize",simpledisk_getfilesize},
    {"GetFile",simpledisk_getfile},
    {"ExtractAllFiles",simpledisk_extractallfiles},
    {"Reset",simpledisk_reset},
    {"EndFolder",simpledisk_endfolder},
    {"BeginFolder",simpledisk_beginfolder},
    {NULL, NULL}
};
static int luaL_register_simpledisk(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_SIMPLEDISK);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,simpledisk_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,simpledisk_lib);
    return 1;
}

int luaopen_simpledisk(lua_State *L)
{
    luaL_requiref(L, "SimpleDisk",luaL_register_simpledisk,1);
    lua_pop(L, 1);
    return 0;
}
