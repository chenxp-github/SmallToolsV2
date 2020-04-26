#include "lua_helper.h"
#include "mem.h"
#include "encoder.h"
#include "syslog.h"

HOW_TO_GET_GLOBAL_TASKMGR how_to_get_global_taskmgr = NULL;
HOW_TO_GET_GLOBAL_LUAVM how_to_get_global_luavm = NULL;
HOW_TO_GET_LUA_RUNNING_FLAG how_to_get_lua_running_flag = NULL;
HOW_TO_GET_PEER_GLOBALS how_to_get_peer_globals = NULL;

const char* lua_push_local_string(lua_State *L, const char *local_string)
{
#if _IS_WINDOWS_
    ASSERT(L && local_string);
    CMem mem_local_string(local_string);
    AUTO_LOCAL_MEM(utf8_string,mem_local_string.StrLen()*3);
    CEncoder::EncodingConvert(
        LOCAL_ENCODING,ENCODING_UTF8,
        &mem_local_string,
        &utf8_string
    );
    return lua_pushlstring(L,utf8_string.CStr(),utf8_string.StrLen());
#else
	return lua_pushstring(L,local_string);
#endif
}

const char* lua_to_local_string(lua_State *L, int index,CMem *out)
{
	ASSERT(out);
	out->SetSize(0);

#if _IS_WINDOWS_ 
    if(!lua_isstring(L,index))return NULL;
	
    size_t len = 0;
    const char *utf8_str = lua_tolstring(L,index,&len);
    ASSERT(utf8_str); ASSERT(len > 0);

    CMem tmp;
    tmp.Init();
    tmp.SetRawBuf((void*)utf8_str,len,true);
    
    CEncoder::EncodingConvert(ENCODING_UTF8,LOCAL_ENCODING,&tmp,out);

    return out->CStr();
#else
	return lua_tostring(L,index);;
#endif
}

const wchar_t* lua_to_unicode_string(lua_State *L, int index,CMem *out)
{
	ASSERT(out);
	out->SetSize(0);
	
    if(!lua_isstring(L,index))return NULL;
	
    size_t len = 0;
    const char *utf8_str = lua_tolstring(L,index,&len);
    ASSERT(utf8_str); ASSERT(len > 0);
	
    CMem tmp;
    tmp.Init();
    tmp.SetRawBuf((void*)utf8_str,len,true);
    
    CEncoder::EncodingConvert(ENCODING_UTF8,ENCODING_UNICODE,&tmp,out);
	
    return out->CStrW();
}
