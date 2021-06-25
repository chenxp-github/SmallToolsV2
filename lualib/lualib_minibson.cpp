#include "lualib_minibson.h"
#include "lualib_mem.h"
#include "lualib_filebase.h"
#include "mem_tool.h"
#include "syslog.h"
#include "memstk.h"
#include "memfile.h"

LUA_IS_VALID_USER_DATA_FUNC(CMiniBson,minibson)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CMiniBson,minibson)
LUA_NEW_USER_DATA_FUNC(CMiniBson,minibson,MINIBSON)
LUA_GC_FUNC(CMiniBson,minibson)
LUA_IS_SAME_FUNC(CMiniBson,minibson)
LUA_TO_STRING_FUNC(CMiniBson,minibson)

bool is_minibson(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_MINIBSON,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return minibson_is_userdata_valid(ud);  
}

/*************************************************/
static status_t minibson_new(lua_State *L)
{
    CMiniBson *pminibson;
    NEW(pminibson,CMiniBson);
    pminibson->Init();
    minibson_new_userdata(L,pminibson,0);
    return 1;
}

static int minibson_isend(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    int _ret_0 = (int)pminibson->IsEnd();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_getdocument(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    
    CMiniBson *_ret;
    NEW(_ret,CMiniBson);
    _ret->Init();
    if(pminibson->GetDocument(name,_ret))
    {
        minibson_new_userdata(L,_ret,false);
        return 1;
    }
    DEL(_ret);
    return 0;
}
static int minibson_startdocument(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    
    if(!lua_isstring(L,2))
    {
        int _ret = pminibson->StartDocument();
        lua_pushinteger(L,_ret);
        return 1;
    }
    
    const char* name = (const char*)lua_tostring(L,2);
    fsize_t off = 0;
    pminibson->StartDocument(name,&off);
    lua_pushinteger(L,off);
    return 1;
}
static int minibson_enddocument(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);

    if(!lua_isinteger(L,2))
    {
        int _ret = pminibson->EndDocument();
        lua_pushboolean(L,_ret);
        return 1;
    }

    int offset = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pminibson->EndDocument(offset);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_resetpointer(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    int _ret_0 = (int)pminibson->ResetPointer();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_getstring(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    CMem str;
    if(pminibson->GetString(name,&str))
    {
        lua_pushlstring(L,str.CStr(),str.StrLen());
        return 1;
    }
    return 0;
}
static int minibson_getboolean(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    bool b;
    if(pminibson->GetBoolean(name,&b))
    {
        lua_pushboolean(L,b);
        return 1;
    }

    return 0;
}
static int minibson_getdouble(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    double d;
    if(pminibson->GetDouble(name,&d))
    {
        lua_pushnumber(L,d);
        return 1;
    }
    return 0;
}
static int minibson_getint32(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int i;
    if(pminibson->GetInt32(name,&i))
    {
        lua_pushinteger(L,i);
        return 1;
    }
    return 0;
}
static int minibson_getint64(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int64_t i;
    if(pminibson->GetInt64(name,&i))
    {
        lua_pushinteger(L,i);
        return 1;
    }
    return 0;
}
static int minibson_putdouble(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    float d = (float)lua_tonumber(L,3);
    int _ret_0 = (int)pminibson->PutDouble(name,d);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_putboolean(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    bool b = lua_toboolean(L,3) != 0;
    int _ret_0 = (int)pminibson->PutBoolean(name,b);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_putdocument(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    CMiniBson *bson = get_minibson(L,3);
    ASSERT(bson);
    int _ret_0 = (int)pminibson->PutDocument(name,bson);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_getdocumentsize(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    int _ret_0 = (int)pminibson->GetDocumentSize();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int minibson_putstring(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    const char* str = (const char*)lua_tostring(L,3);
    int _ret_0 = (int)pminibson->PutString(name,str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_putint32(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int i = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pminibson->PutInt32(name,i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_putint64(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int64_t i = (int64_t)lua_tointeger(L,3);
    int _ret_0 = (int)pminibson->PutInt64(name,i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_putint16(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int i = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pminibson->PutInt16(name,i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_putint8(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int i = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pminibson->PutInt8(name,i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_getpointerpos(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    int _ret_0 = (int)pminibson->GetPointerPos();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int minibson_setpointerpos(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    int pos = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pminibson->SetPointerPos(pos);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int minibson_peeknext(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    int type;
    LOCAL_MEM(name);
    if(pminibson->PeekNext(&type,&name))
    {
        lua_pushinteger(L,type);
        lua_pushstring(L,name.CStr());
        return 2;
    }
    return 0;
}
static int minibson_tojson(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);

    CMemFile mf;
    mf.Init();
    pminibson->ToJson(&mf,true);
	
	CMem mem;
	mem.Init();
	mem.Copy(&mf);

    lua_pushlstring(L,mem.CStr(),mem.StrLen());
    return 1;
}

static int minibson_getint16(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int16_t v;
    if(pminibson->GetInt16(name,&v))
    {
        lua_pushinteger(L,v);
        return 1;
    }
    return 0;
}
static int minibson_getint8(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    int8_t v;
    if(pminibson->GetInt8(name,&v))
    {
        lua_pushinteger(L,v);
        return 1;
    }
    return 0;
}
static int minibson_startarray(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	const char* name = (const char*)lua_tostring(L,2);
	ASSERT(name);
	fsize_t off = 0;
	if(pminibson->StartArray(name,&off))
    {
        lua_pushinteger(L,off);
	    return 1;
    }
	return 0;
}
static int minibson_endarray(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	int offset = (int)lua_tointeger(L,2);
	int array_len = (int)lua_tointeger(L,3);
	int _ret_0 = (int)pminibson->EndArray(offset,array_len);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int minibson_getarray(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	const char* name = (const char*)lua_tostring(L,2);
	ASSERT(name);
	int32_t array_length = 0;
	CMiniBson *bson;
	NEW(bson,CMiniBson);
	bson->Init();
	if(pminibson->GetArray(name,bson,&array_length))
    {
        minibson_new_userdata(L,bson,0);
	    lua_pushinteger(L,array_length);
	    return 2;
    }

    DEL(bson);
	return 0;
}

static int minibson_traverse(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	int recursive = (int)lua_toboolean(L,2);
	int callback_id = CLuaVm::ToFunction(L,3);

	if(callback_id == LUA_REFNIL)
		return 0;
	
	BEGIN_CLOSURE(on_traverse)
	{
		CLOSURE_PARAM_INT(event,CMiniBson::INDEX_EVENT);
		CLOSURE_PARAM_INT(type,CMiniBson::INDEX_TYPE);
		CLOSURE_PARAM_PTR(const char*,name,CMiniBson::INDEX_NAME);
		CLOSURE_PARAM_INT(is_in_array,CMiniBson::INDEX_IS_IN_ARRAY);
		CLOSURE_PARAM_INT(level,CMiniBson::INDEX_LEVEL);
		CLOSURE_PARAM_PTR(CMemStk*,path,CMiniBson::INDEX_PATH);

		CLOSURE_PARAM_INT(callback_id,10);
		CLOSURE_PARAM_PTR(lua_State*,L,11);

		lua_rawgeti(L,LUA_REGISTRYINDEX,callback_id);
		
        lua_newtable(L);
        int top = lua_gettop(L);
		
        lua_pushstring(L,"event");
        lua_pushinteger(L,event);
        lua_settable(L, top);
		
        lua_pushstring(L,"type");
        lua_pushinteger(L,type);
        lua_settable(L, top);
		
        lua_pushstring(L,"name");
        lua_pushstring(L,name);
        lua_settable(L, top);
		
        lua_pushstring(L,"level");
        lua_pushinteger(L,level);
        lua_settable(L, top);
		
		lua_pushstring(L,"is_in_array");
        lua_pushboolean(L,is_in_array);
        lua_settable(L, top);

		LOCAL_MEM(path_str);
		path->SavePath(&path_str);
		lua_pushstring(L,"path");
        lua_pushlstring(L,path_str.CStr(),path_str.StrLen());
        lua_settable(L, top);
		
		if(event == CMiniBson::EVENT_BEGIN_DOCUMENT)
		{
			CLOSURE_PARAM_INT(is_array,CMiniBson::INDEX_IS_ARRAY);
			lua_pushstring(L,"is_array");
			lua_pushboolean(L,is_array);
			lua_settable(L, top);
		}
		
		else if(event == CMiniBson::EVENT_SINGLE_VALUE)
		{
			if(type == CMiniBson::BSON_TYPE_INT32)
			{
				CLOSURE_PARAM_INT(t,CMiniBson::INDEX_VALUE);
				lua_pushstring(L,"value");
				lua_pushinteger(L,t);
				lua_settable(L, top);
			}
			else if(type == CMiniBson::BSON_TYPE_INT64)
			{
				CLOSURE_PARAM_INT64(t,CMiniBson::INDEX_VALUE);
				lua_pushstring(L,"value");
				lua_pushinteger(L,t);
				lua_settable(L, top);
			}
			else if(type == CMiniBson::BSON_TYPE_DOUBLE)
			{
				CLOSURE_PARAM_DOUBLE(t,CMiniBson::INDEX_VALUE);
				lua_pushstring(L,"value");
				lua_pushnumber(L,t);
				lua_settable(L, top);
			}
			else if(type == CMiniBson::BSON_TYPE_STRING)
			{
				CLOSURE_PARAM_PTR(CMem*,mem,CMiniBson::INDEX_VALUE);
				lua_pushstring(L,"value");
				lua_pushlstring(L,mem->CStr(),mem->StrLen());
				lua_settable(L, top);
			}
			else if(type == CMiniBson::BSON_TYPE_BOOLEAN)
			{
				CLOSURE_PARAM_INT(b,CMiniBson::INDEX_VALUE);
				lua_pushstring(L,"value");
				lua_pushboolean(L,b);
				lua_settable(L, top);
			}
			else if(type == CMiniBson::BSON_TYPE_BINARY)
			{
				CLOSURE_PARAM_PTR(CMem*,bin,CMiniBson::INDEX_VALUE);
				lua_pushstring(L,"value");
				mem_new_userdata(L,bin,1);
				lua_settable(L, top);
			}
		}
		
		else if(event == CMiniBson::EVENT_END_DOCUMENT)
		{
			CLOSURE_PARAM_INT(is_array,CMiniBson::INDEX_IS_ARRAY);
			CLOSURE_PARAM_PTR(CMiniBson*,sub_bson,CMiniBson::INDEX_VALUE);
			lua_pushstring(L,"is_array");
			lua_pushboolean(L,is_array);
			lua_settable(L, top);

			lua_pushstring(L,"value");
			minibson_new_userdata(L,sub_bson,1);
			lua_settable(L, top);
		}
        CLuaVm vm;
        vm.Init(L);
        vm.Run(1,0);
        vm.ClearStack();
		return OK;
	}
	END_CLOSURE(on_traverse);

	ASSERT(CMiniBson::INDEX_MAX < 10);

    CMemStk path;
    path.Init(1024);
	
    on_traverse.SetParamInt(CMiniBson::INDEX_IS_IN_ARRAY,0);
    on_traverse.SetParamPointer(CMiniBson::INDEX_PATH,&path);
	on_traverse.SetParamInt(10,callback_id);
	on_traverse.SetParamPointer(11,L);
	pminibson->ResetPointer();
	pminibson->Traverse(recursive!=0,0,&on_traverse);	
	luaL_unref(L,LUA_REGISTRYINDEX,callback_id);
	return 0;
}
static int minibson_getrawdata(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	CMem *_ret_0 = pminibson->GetRawData();
	if(_ret_0==NULL)return 0;
	mem_new_userdata(L,_ret_0,1);
	return 1;
}
static int minibson_loadbson_v1(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	const char* fn = (const char*)lua_tostring(L,2);
	ASSERT(fn);
	int _ret_0 = (int)pminibson->LoadBson(fn);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int minibson_loadbson_v2(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	CFileBase *file = get_filebase(L,2);
	ASSERT(file);
	int _ret_0 = (int)pminibson->LoadBson(file);
	lua_pushboolean(L,_ret_0);
	return 1;
}

static int minibson_loadbson(lua_State *L)
{
	if(lua_isstring(L,2))
		return minibson_loadbson_v1(L);
	else
		return minibson_loadbson_v2(L);
}

static int minibson_putbinary(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	const char* name = (const char*)lua_tostring(L,2);
	ASSERT(name);
	CFileBase *binary = get_filebase(L,3);
	ASSERT(binary);
	int _ret_0 = (int)pminibson->PutBinary(name,binary);
	lua_pushboolean(L,_ret_0);
	return 1;
}

static int minibson_getbinary(lua_State *L)
{
	CMiniBson *pminibson = get_minibson(L,1);
	ASSERT(pminibson);
	const char* name = (const char*)lua_tostring(L,2);
	ASSERT(name);
	CMem *binary = get_mem(L,3);
	ASSERT(binary);
	int _ret_0 = (int)pminibson->GetBinary(name,binary);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static status_t minibson_putuint32(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    uint32_t i = (uint32_t)lua_tointeger(L,3);
    status_t ret0 = pminibson->PutUInt32(name,i);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t minibson_putuint16(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    uint16_t i = (uint16_t)lua_tointeger(L,3);
    status_t ret0 = pminibson->PutUInt16(name,i);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t minibson_putuint8(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    uint8_t i = (uint8_t)lua_tointeger(L,3);
    status_t ret0 = pminibson->PutUInt8(name,i);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t minibson_getuint8(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    uint8_t ret0 = 0;
    if(pminibson->GetUInt8(name,&ret0))
    {
        lua_pushinteger(L,ret0);
        return 1;
    }
    return 0;
}

static status_t minibson_getuint16(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    uint16_t ret0 = 0;
    if(pminibson->GetUInt16(name,&ret0))
    {
        lua_pushinteger(L,ret0);
        return 1;
    }
    return 0;
}

static status_t minibson_getuint32(lua_State *L)
{
    CMiniBson *pminibson = get_minibson(L,1);
    ASSERT(pminibson);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    uint32_t ret0 = 0;
    if(pminibson->GetUInt32(name,&ret0))
    {
        lua_pushinteger(L,ret0);
        return 1;
    }
    return 0;
}

static const luaL_Reg minibson_lib[] = {

    {"__gc",minibson_gc_},
    {"__tostring",minibson_tostring_},
    {"__is_same",minibson_issame_},
    {"new",minibson_new},
    {"IsEnd",minibson_isend},
    {"GetDocument",minibson_getdocument},
    {"StartDocument",minibson_startdocument},
    {"EndDocument",minibson_enddocument},
    {"ResetPointer",minibson_resetpointer},
    {"GetString",minibson_getstring},
    {"GetBoolean",minibson_getboolean},
    {"GetDouble",minibson_getdouble},
    {"GetInt32",minibson_getint32},
	{"GetInt64",minibson_getint64},
    {"PutDouble",minibson_putdouble},
    {"PutBoolean",minibson_putboolean},
    {"PutDocument",minibson_putdocument},
    {"GetDocumentSize",minibson_getdocumentsize},
	{"GetBinary",minibson_getbinary},
    {"PutString",minibson_putstring},
    {"PutInt32",minibson_putint32},
	{"PutInt64",minibson_putint64},
    {"PutInt16",minibson_putint16},
    {"PutInt8",minibson_putint8},	
	{"PutBinary",minibson_putbinary},
    {"GetPointerPos",minibson_getpointerpos},
    {"SetPointerPos",minibson_setpointerpos},
    {"PeekNext",minibson_peeknext},
    {"ToJson",minibson_tojson},
    {"GetInt16",minibson_getint16},
    {"GetInt8",minibson_getint8},
	{"StartArray",minibson_startarray},
	{"EndArray",minibson_endarray},
	{"GetArray",minibson_getarray},	
	{"Traverse",minibson_traverse},
	{"GetRawData",minibson_getrawdata},
	{"LoadBson",minibson_loadbson},
    {"PutUInt32",minibson_putuint32},
    {"PutUInt16",minibson_putuint16},
    {"PutUInt8",minibson_putuint8},
    {"GetUInt8",minibson_getuint8},
    {"GetUInt16",minibson_getuint16},
    {"GetUInt32",minibson_getuint32},    
    {NULL, NULL}
};
static int luaL_register_minibson(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_MINIBSON);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,minibson_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,minibson_lib);
    return 1;
}

void luaopen_minibson(lua_State *L)
{
    luaL_requiref(L, "Bson",luaL_register_minibson,1);
    lua_pop(L, 1);
}


