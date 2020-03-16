#include "lualib_stream.h"
#include "lualib_filebase.h"
#include "lualib_mem.h"
#include "mem_tool.h"
#include "syslog.h"
static bool stream_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CHECK_IS_UD_READABLE(CStream,ud);
    CStream *p = (CStream*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}    

bool is_stream(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_STREAM,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return stream_is_userdata_valid(ud);  
}

CStream *get_stream(lua_State *L, int idx)
{
    lua_userdata *ud = NULL;
    if(is_stream(L,idx))
    {
        ud = (lua_userdata*)lua_touserdata(L,idx);		
    }
    ASSERT(ud);
    return (CStream *)ud->p;
} 

lua_userdata *stream_new_userdata(lua_State *L,CStream *pobj,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pobj);
    ud->p = pobj;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pobj->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_STREAM);
    lua_setmetatable(L,-2);
    return ud;
}

static int stream_gc_(lua_State *L)
{
    if(!is_stream(L,1)) return 0;
	
    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);		
    ASSERT(ud);
	
    if(!(ud->is_attached))
    {
        CStream *pstream = (CStream*)ud->p;
        DEL(pstream);
    }
    return 0;
}

static int stream_issame_(lua_State *L)
{
    CStream *pstream1 = get_stream(L,1);
    ASSERT(pstream1);
    CStream *pstream2 = get_stream(L,2);
    ASSERT(pstream2);
    int is_same = (pstream1==pstream2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int stream_tostring_(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    char buf[1024];
    sprintf(buf,"userdata:stream:%p",pstream);
    lua_pushstring(L,buf);
    return 1;
}
/****************************************/
static status_t stream_new(lua_State *L)
{
    CStream *pstream;
    NEW(pstream,CStream);
    pstream->Init();
    stream_new_userdata(L,pstream,0);
    return 1;
}
static int stream_putstring(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    const char* str = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pstream->PutString(str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getstring(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    CMem ret;
    ret.Init();
    pstream->GetString(&ret);
    lua_pushstring(L,ret.CStr());
    return 1;
}
static int stream_getbinary(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);

    CStream *ret;
    NEW(ret,CStream);
    ret->Init();
    pstream->GetBinary(ret);
    stream_new_userdata(L,ret,0);

    return 1;
}
static int stream_putbinary(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    CStream *bin = get_stream(L,2);
    ASSERT(bin);
    int _ret_0 = (int)pstream->PutBinary(bin);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getzeroendstring(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    CMem ret;
    ret.Init();
    pstream->GetZeroEndString(&ret);
    lua_pushstring(L,ret.CStr());
    return 1;
}
static int stream_putzeroendstring(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    const char* str = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pstream->PutZeroEndString(str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_putdouble(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    float d = (float)lua_tonumber(L,2);
    int _ret_0 = (int)pstream->PutDouble(d);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getdouble(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    float _ret_0 = (float)pstream->GetDouble();
    lua_pushnumber(L,_ret_0);
    return 1;
}
static int stream_putbool(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int b = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pstream->PutBool(b!=0);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getbool(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int _ret_0 = (int)pstream->GetBool();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_putint8(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int i = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pstream->PutInt8(i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getint8(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int _ret_0 = (int)pstream->GetInt8();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int stream_putint16(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int i = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pstream->PutInt16(i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getint16(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int _ret_0 = (int)pstream->GetInt16();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int stream_putint32(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int i = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pstream->PutInt32(i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getint32(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int _ret_0 = (int)pstream->GetInt32();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int stream_putint64(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int64_t i = (int64_t)lua_tointeger(L,2);
    int _ret_0 = (int)pstream->PutInt64(i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getint64(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int64_t _ret_0 = pstream->GetInt64();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int stream_copy(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    CStream *_p = get_stream(L,2);
    ASSERT(_p);
    int _ret_0 = (int)pstream->Copy(_p);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int stream_getasstring(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    int len = (int)lua_tointeger(L,2);

    LOCAL_MEM(tmp);
    
    if(len > LBUF_SIZE)
    {
        tmp.Destroy();
        tmp.Init();
        tmp.Malloc(len + 4);
    }
    pstream->GetAsString(&tmp,len);
    lua_pushstring(L,tmp.CStr());
    return 1;
}
static int stream_filebase(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    filebase_new_userdata(L,pstream,1);
    return 1;
}
static int stream_mem(lua_State *L)
{
    CStream *pstream = get_stream(L,1);
    ASSERT(pstream);
    mem_new_userdata(L,pstream,1);
    return 1;
}
static const luaL_Reg stream_lib[] = {
    {"new",stream_new},
    {"__gc",stream_gc_},
    {"__tostring",stream_tostring_},
    {"IsSame",stream_issame_},
    {"PutString",stream_putstring},
    {"GetString",stream_getstring},
    {"GetBinary",stream_getbinary},
    {"PutBinary",stream_putbinary},
    {"GetZeroEndString",stream_getzeroendstring},
    {"PutZeroEndString",stream_putzeroendstring},
    {"PutDouble",stream_putdouble},
    {"GetDouble",stream_getdouble},
    {"PutBool",stream_putbool},
    {"GetBool",stream_getbool},
    {"PutInt8",stream_putint8},
    {"GetInt8",stream_getint8},
    {"PutInt16",stream_putint16},
    {"GetInt16",stream_getint16},
    {"PutInt32",stream_putint32},
    {"GetInt32",stream_getint32},
    {"PutInt64",stream_putint64},
    {"GetInt64",stream_getint64},
    {"Copy",stream_copy},
    {"GetAsString",stream_getasstring},
    {"FileBase",stream_filebase},
    {"Mem",stream_mem},
    {NULL, NULL}
};

const luaL_Reg* get_stream_funcs()
{
    return stream_lib;
}

static int luaL_register_stream(lua_State *L)
{	
    static luaL_Reg _stream_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_stream_funcs_,&_index,get_filebase_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_stream_funcs_,&_index,get_mem_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_stream_funcs_,&_index,get_stream_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_STREAM);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_stream_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_stream_funcs_);
    return 1;
}        

int luaopen_stream(lua_State *L)
{
    luaL_requiref(L, "Stream",luaL_register_stream,1);
    lua_pop(L, 1);
    return 0;
}        

