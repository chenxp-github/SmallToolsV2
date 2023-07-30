#include "lualib_remotedesktopsnapshottor.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_remotedesktoppixelbuffer.h"

#if _IS_WINDOWS_
#include "lualib_remotedesktopsnapshottor_gdi.h"
#endif

#if _SUPPORT_X11_
#include "lualib_remotedesktopsnapshottor_x11.h"
#endif

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopSnapshottor,remotedesktopsnapshottor)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopSnapshottor,remotedesktopsnapshottor)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopSnapshottor,remotedesktopsnapshottor,REMOTEDESKTOPSNAPSHOTTOR)
LUA_GC_FUNC(CRemoteDesktopSnapshottor,remotedesktopsnapshottor)
LUA_IS_SAME_FUNC(CRemoteDesktopSnapshottor,remotedesktopsnapshottor)
LUA_TO_STRING_FUNC(CRemoteDesktopSnapshottor,remotedesktopsnapshottor)

bool is_remotedesktopsnapshottor(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR,

#if _IS_WINDOWS_
		LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_GDI,
#endif

#if _SUPPORT_X11_
        LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR_X11,
#endif

    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktopsnapshottor_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktopsnapshottor_destroy(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    status_t ret0 = premotedesktopsnapshottor->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_getname(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    const char* ret0 = premotedesktopsnapshottor->GetNameStr();
    ASSERT(ret0);
    lua_pushstring(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_getlastframedata(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    CRemoteDesktopPixelBuffer *ret0 = premotedesktopsnapshottor->GetLastFrameData();
    ASSERT(ret0);
    remotedesktoppixelbuffer_new_userdata(L,ret0,1);
    return 1;
}

static status_t remotedesktopsnapshottor_getlastframetimestamp(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    int64_t ret0 = premotedesktopsnapshottor->GetLastFrameTimestamp();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_getsnapshotmininterval(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    int ret0 = premotedesktopsnapshottor->GetSnapshotMinInterval();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_setname(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = premotedesktopsnapshottor->SetName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_setlastframedata(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    CRemoteDesktopPixelBuffer *_last_frame_data = get_remotedesktoppixelbuffer(L,2);
    ASSERT(_last_frame_data);
    status_t ret0 = premotedesktopsnapshottor->SetLastFrameData(_last_frame_data);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_setlastframetimestamp(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    int64_t _last_frame_timestamp = (int64_t)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor->SetLastFrameTimestamp(_last_frame_timestamp);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_setsnapshotmininterval(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    int _snapshot_min_interval = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor->SetSnapshotMinInterval(_snapshot_min_interval);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_takesnapshot_v1(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    CRemoteDesktopPixelBuffer *pixelbuf = get_remotedesktoppixelbuffer(L,2);
    ASSERT(pixelbuf);
    status_t ret0 = premotedesktopsnapshottor->TakeSnapshot(pixelbuf);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_takesnapshot_v2(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    status_t ret0 = premotedesktopsnapshottor->TakeSnapshot();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_takesnapshot(lua_State *L)
{
    if(is_remotedesktoppixelbuffer(L,2))
    {
        return remotedesktopsnapshottor_takesnapshot_v1(L);
    }

    return remotedesktopsnapshottor_takesnapshot_v2(L);
}

static status_t remotedesktopsnapshottor_gettype(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    int ret0 = premotedesktopsnapshottor->GetType();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottor_settype(lua_State *L)
{
    CRemoteDesktopSnapshottor *premotedesktopsnapshottor = get_remotedesktopsnapshottor(L,1);
    ASSERT(premotedesktopsnapshottor);
    int type = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopsnapshottor->SetType(type);
    lua_pushboolean(L,ret0);
    return 1;
}
/****************************************************/
static const luaL_Reg remotedesktopsnapshottor_funcs_[] = {
    {"__gc",remotedesktopsnapshottor_gc_},
    {"__tostring",remotedesktopsnapshottor_tostring_},
    {"__is_same",remotedesktopsnapshottor_issame_},
    {"SetName",remotedesktopsnapshottor_setname},
    {"SetLastFrameData",remotedesktopsnapshottor_setlastframedata},
    {"SetLastFrameTimestamp",remotedesktopsnapshottor_setlastframetimestamp},
    {"SetSnapshotMinInterval",remotedesktopsnapshottor_setsnapshotmininterval},
    {"TakeSnapshot",remotedesktopsnapshottor_takesnapshot},
    {"Destroy",remotedesktopsnapshottor_destroy},
    {"GetLastFrameTimestamp",remotedesktopsnapshottor_getlastframetimestamp},
    {"GetSnapshotMinInterval",remotedesktopsnapshottor_getsnapshotmininterval},
    {"GetName",remotedesktopsnapshottor_getname},
    {"GetLastFrameData",remotedesktopsnapshottor_getlastframedata},
	{"SetType",remotedesktopsnapshottor_settype},
	{"GetType",remotedesktopsnapshottor_gettype},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktopsnapshottor_funcs()
{
    return remotedesktopsnapshottor_funcs_;
}

static int luaL_register_remotedesktopsnapshottor(lua_State *L)
{	
    static luaL_Reg _remotedesktopsnapshottor_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktopsnapshottor_funcs_,&_index,get_remotedesktopsnapshottor_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTOR);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktopsnapshottor_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktopsnapshottor_funcs_);
    return 1;
}        

int luaopen_remotedesktopsnapshottor(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopSnapshottor",luaL_register_remotedesktopsnapshottor,1);
    lua_pop(L, 1);
    return 0;
}        

