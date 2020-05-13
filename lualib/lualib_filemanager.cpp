#include "lualib_filemanager.h"
#include "lualib_filebase.h"
#include "syslog.h"
#include "mem_tool.h"
#include "dirmgr.h"
#include "fileutils.h"
#include "lua_helper.h"


static int filemanager_toabspath_v1(lua_State *L)
{
	LUA_TO_LOCAL_STRING(relatedPath,L,1);
    LOCAL_MEM(abs_path);
    abs_path.Puts(relatedPath);
    CDirMgr::ToAbsPath(&abs_path);
    lua_push_local_string(L,abs_path.CStr());
    return 1;
}

static int filemanager_toabspath_v2(lua_State *L)
{
    const char *relatedPath = lua_tostring(L,1);
    ASSERT(relatedPath);
	const char *prefix = lua_tostring(L,2);
	ASSERT(prefix);
    LOCAL_MEM(abs_path);
    abs_path.Puts(relatedPath);
    CDirMgr::ToAbsPath(prefix,&abs_path);
    lua_pushstring(L,abs_path.CStr());
    return 1;
}

static int filemanager_toabspath(lua_State *L)
{
	if(lua_isstring(L,2))
		return filemanager_toabspath_v2(L);
	else
		return filemanager_toabspath_v1(L);
}


static int filemanager_getcurdir(lua_State *L)
{
    LOCAL_MEM(dir);
    CDirMgr::GetCurDir(&dir);
    lua_push_local_string(L,dir.CStr());
    return 1;
}

static int filemanager_slicefilename(lua_State *L)
{
    const char *full = lua_tostring(L,1);
    ASSERT(full);
    uint32_t mask = (uint32_t)lua_tointeger(L,2);
    LOCAL_MEM(out);
    CDirMgr::GetFileName(full,&out,mask);
    lua_pushstring(L,out.CStr());
    return 1;
}

static int filemanager_changedir(lua_State *L)
{   
	LUA_TO_LOCAL_STRING(dir,L,1);
    if(dir[0])
    {
        crt_chdir(dir);
    }
    return 0;
}
static int filemanager_isdirexist(lua_State *L)
{
    LUA_TO_LOCAL_STRING(dir,L,1);
    ASSERT(dir);
    CMem mem(dir);
    int _r=(int)CDirMgr::IsDirExist(&mem);
    lua_pushboolean(L,_r);
    return 1;
}

static int filemanager_searchdir(lua_State *L)
{
	LUA_TO_LOCAL_STRING(dir,L,1);
    if(dir[0] == 0)dir="/";
    int recursive = (int)lua_toboolean(L,2);
    int callback = CLuaVm::ToFunction(L,3);

    BEGIN_CLOSURE(on_search_dir)
    {
        CLOSURE_PARAM_INT(callback,10);
        CLOSURE_PARAM_PTR(lua_State*,L,11);
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(char*,file_name,1);
        CLOSURE_PARAM_PTR(char*,full_name,2);
        CLOSURE_PARAM_INT(is_dir,3);
        CLOSURE_PARAM_INT64(size,4);
        CLOSURE_PARAM_INT64(last_write_time,5);

        lua_rawgeti(L,LUA_REGISTRYINDEX,callback);

        lua_newtable(L);
        int top = lua_gettop(L);

        lua_pushstring(L,"event");
        lua_pushinteger(L,event);
        lua_settable(L, top);

        lua_pushstring(L,"file_name");
        lua_push_local_string(L,file_name);
        lua_settable(L, top);

        lua_pushstring(L,"full_name");
        lua_push_local_string(L,full_name);
        lua_settable(L, top);

        lua_pushstring(L,"is_dir");
        lua_pushboolean(L,is_dir);
        lua_settable(L, top);

        lua_pushstring(L,"size");
        lua_pushinteger(L,size);
        lua_settable(L, top);

        lua_pushstring(L,"last_write_time");
        lua_pushinteger(L,last_write_time);
        lua_settable(L, top);

        CLuaVm vm;
        vm.Init(L);
        vm.Run(1,0);
        vm.ClearStack();
        return OK;
    }
    END_CLOSURE(on_search_dir);

    on_search_dir.SetParamInt(10,callback);
    on_search_dir.SetParamPointer(11,L);
	
	CMem mem_dir(dir);
    CDirMgr::SearchDir_Interruptable(
        &mem_dir,recursive!=0,&on_search_dir,
        how_to_get_lua_running_flag()
    );
	luaL_unref(L,LUA_REGISTRYINDEX,callback);

    return 0;
}
static int filemanager_deletefile(lua_State *L)
{
	LUA_TO_LOCAL_STRING(name,L,1);
    CMem mem(name);
    int _ret_0 = (int)CDirMgr::DeleteFile(&mem);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filemanager_isfileexist(lua_State *L)
{
	LUA_TO_LOCAL_STRING(fn,L,1);
    CMem mem_fn(fn);
    int _ret_0 = (int)CDirMgr::IsFileExist(&mem_fn);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filemanager_savefile(lua_State *L)
{
	LUA_TO_LOCAL_STRING(path,L,1);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);

    CMem mem_path(path);
    int _ret_0 = (int)CDirMgr::SaveFileSuper(&mem_path,file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filemanager_createdir(lua_State *L)
{
	LUA_TO_LOCAL_STRING(path,L,1);
    CMem mem_path(path);
    int _ret_0 = (int)CDirMgr::CreateDirSuper(&mem_path);
    lua_pushboolean(L,_ret_0);
    return 1;
}

#if _IS_LINUX_

static int filemanager_readlink(lua_State *L)
{
    const char* link_name = (const char*)lua_tostring(L,1);
    ASSERT(link_name);

    char buf[LBUF_SIZE];

    int n = readlink(link_name,buf,sizeof(buf));
    if(n <= 0) return 0;

    buf[n] = 0; 
    lua_pushstring(L,buf);

    return 1;
}

#else

static int filemanager_readlink(lua_State *L)
{
	return 0;
}

#endif

static int filemanager_copysinglefile_v0(lua_State *L)
{
	LUA_TO_LOCAL_STRING(from,L,1);
	LUA_TO_LOCAL_STRING(to,L,2);
	
    CMem mem_from(from);
    CMem mem_to(to);

	fsize_t start = (fsize_t)lua_tointeger(L,3);
	fsize_t size = (fsize_t)lua_tointeger(L,4);
	fsize_t to_start = (fsize_t)lua_tointeger(L,5);
	int mode = (int)lua_tointeger(L,6);

	int _ret_0 = (int)CopySingleFile_Interruptable(
        &mem_from,&mem_to,start,size,to_start,mode,
        how_to_get_lua_running_flag()
    );

	lua_pushinteger(L,_ret_0);
	return 1;
}

static int filemanager_copysinglefile_v1(lua_State *L)
{
	LUA_TO_LOCAL_STRING(from,L,1);
	LUA_TO_LOCAL_STRING(to,L,2);
	
    CMem mem_from(from);
    CMem mem_to(to);

    fsize_t start = (fsize_t)lua_tointeger(L,3);
    fsize_t size = (fsize_t)lua_tointeger(L,4);
    uint32_t mode = (uint32_t)lua_tointeger(L,5);

    int _ret_0 = (int)CopySingleFile_Interruptable(
        &mem_from,&mem_to,start,size,mode,
        how_to_get_lua_running_flag()
    );
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filemanager_copysinglefile_v2(lua_State *L)
{
	LUA_TO_LOCAL_STRING(from,L,1);
	LUA_TO_LOCAL_STRING(to,L,2);
    int mode = (int)lua_tointeger(L,3);

    int _ret_0 = (int)CopySingleFile_Interruptable(
        from,to,mode,how_to_get_lua_running_flag()
    );

    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filemanager_copysinglefile(lua_State *L)
{
	if(lua_isinteger(L,6))
		return filemanager_copysinglefile_v0(L);

    if(lua_isinteger(L,5))
        return filemanager_copysinglefile_v1(L);
    else
        return filemanager_copysinglefile_v2(L);
}

static int filemanager_istextfile_v1(lua_State *L)
{
    CFileBase *file = get_filebase(L,1);
    ASSERT(file);
    int _ret_0 = (int)IsTextFile(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int filemanager_istextfile_v2(lua_State *L)
{
	LUA_TO_LOCAL_STRING(fn,L,1);
    int _ret_0 = (int)IsTextFile(fn);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int filemanager_istextfile(lua_State *L)
{
    if(lua_isstring(L,1))
        return filemanager_istextfile_v2(L);
    else
        return filemanager_istextfile_v1(L);
}

static int filemanager_converteol_v1(lua_State *L)
{
    CFileBase *in = get_filebase(L,1);
    ASSERT(in);
    CFileBase *out = get_filebase(L,2);
    ASSERT(out);
    int mode = (int)lua_tointeger(L,3);
    int _ret_0 = (int)ConvertEol(in,out,mode);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filemanager_converteol_v2(lua_State *L)
{
	LUA_TO_LOCAL_STRING(in,L,1);
	LUA_TO_LOCAL_STRING(out,L,2);
    int mode = (int)lua_tointeger(L,3);
    int _ret_0 = (int)ConvertEol(in,out,mode);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int filemanager_converteol(lua_State *L)
{
    if(lua_isstring(L,1))
        return filemanager_converteol_v2(L);
    else
        return filemanager_converteol_v1(L);
}


static int filemanager_isfilesame_v1(lua_State *L)
{
    CFileBase *src1 = get_filebase(L,1);
    ASSERT(src1);
    CFileBase *src2 = get_filebase(L,2);
    ASSERT(src2);
    int _ret_0 = (int)IsFileSame(src1,src2);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filemanager_isfilesame_v2(lua_State *L)
{
	LUA_TO_LOCAL_STRING(fn1,L,1);
	LUA_TO_LOCAL_STRING(fn2,L,2);
    int _ret_0 = (int)IsFileSame(fn1,fn2);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int filemanager_isfilesame(lua_State *L)
{
    if(lua_isstring(L,1))
        return filemanager_isfilesame_v2(L);
    else
        return filemanager_isfilesame_v1(L);
}
static int filemanager_deletedir(lua_State *L)
{
	LUA_TO_LOCAL_STRING(dir,L,1);
	int _ret_0 = (int)CDirMgr::DeleteDir(dir);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static status_t filemanager_encodingconvert(lua_State *L)
{
    int from = (int)lua_tointeger(L,1);
    int to = (int)lua_tointeger(L,2);
    CFileBase *infile = get_filebase(L,3);
    ASSERT(infile);
    CFileBase *outfile = get_filebase(L,4);
    ASSERT(outfile);
	status_t ret0 = CEncoder::EncodingConvert(from,to,infile,outfile);
    lua_pushboolean(L,ret0);
    return 1;
}


static status_t filemanager_md5sum_v1(lua_State *L)
{
    CFileBase *file = get_filebase(L,1);
    ASSERT(file);

	LOCAL_MEM(mem);
    CEncoder::Md5Encode_Interruptable(file,&mem,
        how_to_get_lua_running_flag());
    lua_pushstring(L,mem.CStr());
    return 1;
}

static status_t filemanager_md5sum_v2(lua_State *L)
{
    LUA_TO_LOCAL_STRING(fn,L,1);
	
	CFile file;
	file.Init();
	file.SetBufSize(1024*4096);
	if(file.OpenFile(fn,"rb"))
	{
		LOCAL_MEM(mem);
		CEncoder::Md5Encode_Interruptable(&file,&mem,
            how_to_get_lua_running_flag());
		lua_pushstring(L,mem.CStr());
		return 1;
	}
	else
	{
		XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "open file %s fail.",fn
        );
	}
	return 0;
}

static status_t filemanager_md5sum(lua_State *L)
{
    if(is_filebase(L,1))
    {
        return filemanager_md5sum_v1(L);
    }
	
    if(lua_isstring(L,1))
    {
        return filemanager_md5sum_v2(L);
    }
	
    return 0;
}
static int filemanager_isabspath(lua_State *L)
{
    LUA_TO_LOCAL_STRING(fn,L,1);
    bool is = CDirMgr::IsAbsPath(fn);
    lua_pushboolean(L,is);
    return 1;
}

static const luaL_Reg filemanager_lib[] = {
    {"ToAbsPath",filemanager_toabspath},
    {"GetCurDir",filemanager_getcurdir},
    {"SliceFileName",filemanager_slicefilename},
    {"ChangeDir",filemanager_changedir},
    {"IsDirExist",filemanager_isdirexist},
    {"SearchDir",filemanager_searchdir},
    {"DeleteFile",filemanager_deletefile},
    {"IsFileExist",filemanager_isfileexist},        
    {"SaveFile",filemanager_savefile},
    {"CreateDir",filemanager_createdir},
    {"ReadLink",filemanager_readlink},
    {"CopySingleFile",filemanager_copysinglefile},
    {"IsTextFile",filemanager_istextfile},
    {"ConvertEol",filemanager_converteol},
    {"IsFileSame",filemanager_isfilesame},
	{"DeleteDir",filemanager_deletedir},	
    {"EncodingConvert",filemanager_encodingconvert},
    {"Md5Sum",filemanager_md5sum},	
    {"IsAbsPath",filemanager_isabspath},	
    {NULL, NULL}
};

static int luaL_register_filemanager(lua_State *L)
{
    luaL_newmetatable(L, LUA_USERDATA_FILEMANAGER);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L,filemanager_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,filemanager_lib);
    return 1;
}

int luaopen_filemanager(lua_State *L)
{
    luaL_requiref(L, "FileManager",luaL_register_filemanager,1);
    lua_pop(L, 1);
    return 0;
}
