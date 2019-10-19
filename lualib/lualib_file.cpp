#include "lualib_file.h"
#include "lualib_filebase.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"


static bool file_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CFile *p = (CFile*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}    

bool is_file(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_FILE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return file_is_userdata_valid(ud);  
}

CFile *get_file(lua_State *L, int idx)
{
    lua_userdata *ud = NULL;
    if(is_file(L,idx))
    {
        ud = (lua_userdata*)lua_touserdata(L,idx);		
    }
    ASSERT(ud);
    return (CFile *)ud->p;
} 

lua_userdata *file_new_userdata(lua_State *L,CFile *pobj,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pobj);
    ud->p = pobj;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pobj->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_FILE);
    lua_setmetatable(L,-2);
    return ud;
}

static int file_gc_(lua_State *L)
{
    if(!is_file(L,1)) return 0;

    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);		
    ASSERT(ud);

    if(!(ud->is_attached))
    {
        CFile *pfile = (CFile*)ud->p;
        DEL(pfile);
    }
    return 0;
}

static int file_issame_(lua_State *L)
{
    CFile *pfile1 = get_file(L,1);
    ASSERT(pfile1);
    CFile *pfile2 = get_file(L,2);
    ASSERT(pfile2);
    int is_same = (pfile1==pfile2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int file_tostring_(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    char buf[1024];
    sprintf(buf,"userdata:file:%p",pfile);
    lua_pushstring(L,buf);
    return 1;
}

/****************************************/
static status_t file_new(lua_State *L)
{
    CFile *pfile;
    NEW(pfile,CFile);
    pfile->Init();
    file_new_userdata(L,pfile,0);
    return 1;
}

static int file_filebase(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    filebase_new_userdata(L,pfile,1);
    return 1;
}
static int file_isopened(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    int _ret_0 = (int)pfile->IsOpened();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int file_init(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    int weak_ref_id = pfile->__weak_ref_id;
    int _ret_0 = (int)pfile->Init();
    pfile->__weak_ref_id = weak_ref_id;
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int file_destroy_(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    int weak_ref_id = pfile->__weak_ref_id;
    int _ret_0 = (int)pfile->Destroy();
    pfile->__weak_ref_id = weak_ref_id;
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int file_openfile(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
	LOCAL_MEM(_buf_);
    const char* fn = (const char*)lua_to_local_string(L,2,&_buf_);
    const char* mode = (const char*)lua_tostring(L,3);
    int _ret_0 = (int)pfile->OpenFile(fn,mode);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int file_closefile(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    int _ret_0 = (int)pfile->CloseFile();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int file_getsizereal(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    int _ret_0 = (int)pfile->GetSizeReal();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int file_hasbuffer(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    int _ret_0 = (int)pfile->HasBuffer();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int file_setbufsize(lua_State *L)
{
    CFile *pfile = get_file(L,1);
    ASSERT(pfile);
    int size = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pfile->SetBufSize(size);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static const luaL_Reg file_lib[] = {
    {"new",file_new},
    {"__gc",file_gc_},
    {"__tostring",file_tostring_},
    {"IsSame",file_issame_},
    {"FileBase",file_filebase},
    {"IsOpened",file_isopened},
    {"Init",file_init},
    {"Destroy",file_destroy_},
    {"OpenFile",file_openfile},
    {"CloseFile",file_closefile},
    {"GetSizeReal",file_getsizereal},
    {"HasBuffer",file_hasbuffer},
    {"SetBufSize",file_setbufsize},
    {NULL, NULL}
};

const luaL_Reg* get_file_funcs()
{
    return file_lib;
}

static int luaL_register_file(lua_State *L)
{	
    static luaL_Reg _file_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_file_funcs_,&_index,get_filebase_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_file_funcs_,&_index,get_file_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_FILE);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_file_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_file_funcs_);
    return 1;
}        

int luaopen_file(lua_State *L)
{
    luaL_requiref(L, "File",luaL_register_file,1);
    lua_pop(L, 1);
    return 0;
}        
