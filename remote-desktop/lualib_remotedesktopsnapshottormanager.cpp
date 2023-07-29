#include "lualib_remotedesktopsnapshottormanager.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_remotedesktopsnapshottor.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopSnapshottorManager,remotedesktopsnapshottormanager)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopSnapshottorManager,remotedesktopsnapshottormanager)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopSnapshottorManager,remotedesktopsnapshottormanager,REMOTEDESKTOPSNAPSHOTTORMANAGER)
LUA_GC_FUNC(CRemoteDesktopSnapshottorManager,remotedesktopsnapshottormanager)
LUA_IS_SAME_FUNC(CRemoteDesktopSnapshottorManager,remotedesktopsnapshottormanager)
LUA_TO_STRING_FUNC(CRemoteDesktopSnapshottorManager,remotedesktopsnapshottormanager)

bool is_remotedesktopsnapshottormanager(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTORMANAGER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktopsnapshottormanager_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktopsnapshottormanager_new(lua_State *L)
{
	ASSERT(how_to_get_global_taskmgr);
    CRemoteDesktopSnapshottorManager *premotedesktopsnapshottormanager;
    NEW(premotedesktopsnapshottormanager,CRemoteDesktopSnapshottorManager);
    premotedesktopsnapshottormanager->Init(how_to_get_global_taskmgr(L));
	premotedesktopsnapshottormanager->SetIsWeakTable(true); //lua is weak table
    remotedesktopsnapshottormanager_new_userdata(L,premotedesktopsnapshottormanager,0);
    return 1;
}

static status_t remotedesktopsnapshottormanager_getlen(lua_State *L)
{
    CRemoteDesktopSnapshottorManager *premotedesktopsnapshottormanager = get_remotedesktopsnapshottormanager(L,1);
    ASSERT(premotedesktopsnapshottormanager);
    int ret0 = premotedesktopsnapshottormanager->GetLen();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottormanager_getelem(lua_State *L)
{
    CRemoteDesktopSnapshottorManager *premotedesktopsnapshottormanager = get_remotedesktopsnapshottormanager(L,1);
    ASSERT(premotedesktopsnapshottormanager);
    int index = (int)lua_tointeger(L,2);
    CRemoteDesktopSnapshottor *ret0 = premotedesktopsnapshottormanager->GetElem(index);
    ASSERT(ret0);
    remotedesktopsnapshottor_new_userdata(L,ret0,1);
    return 1;
}

static status_t remotedesktopsnapshottormanager_getbyname(lua_State *L)
{
    CRemoteDesktopSnapshottorManager *premotedesktopsnapshottormanager = get_remotedesktopsnapshottormanager(L,1);
    ASSERT(premotedesktopsnapshottormanager);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);

	CMem mem_name(name);
    CRemoteDesktopSnapshottor *ret0 = premotedesktopsnapshottormanager->GetByName(&mem_name);
    ASSERT(ret0);
    remotedesktopsnapshottor_new_userdata(L,ret0,1);
    return 1;
}

static status_t remotedesktopsnapshottormanager_addsnapshotter(lua_State *L)
{
    CRemoteDesktopSnapshottorManager *premotedesktopsnapshottormanager = get_remotedesktopsnapshottormanager(L,1);
    ASSERT(premotedesktopsnapshottormanager);
    CRemoteDesktopSnapshottor *snapshotter = get_remotedesktopsnapshottor(L,2);
    ASSERT(snapshotter);
    status_t ret0 = premotedesktopsnapshottormanager->AddSnapshotter(snapshotter);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopsnapshottormanager_setisweaktable(lua_State *L)
{
    CRemoteDesktopSnapshottorManager *premotedesktopsnapshottormanager = get_remotedesktopsnapshottormanager(L,1);
    ASSERT(premotedesktopsnapshottormanager);
    int isweak = lua_toboolean(L,2);
    status_t ret0 = premotedesktopsnapshottormanager->SetIsWeakTable(isweak!=0);
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg remotedesktopsnapshottormanager_funcs_[] = {
    {"__gc",remotedesktopsnapshottormanager_gc_},
    {"__tostring",remotedesktopsnapshottormanager_tostring_},
    {"__is_same",remotedesktopsnapshottormanager_issame_},
	{"new",remotedesktopsnapshottormanager_new},
    {"GetLen",remotedesktopsnapshottormanager_getlen},
    {"GetElem",remotedesktopsnapshottormanager_getelem},
    {"GetByName",remotedesktopsnapshottormanager_getbyname},
    {"AddSnapshotter",remotedesktopsnapshottormanager_addsnapshotter},
    {"SetIsWeakTable",remotedesktopsnapshottormanager_setisweaktable},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktopsnapshottormanager_funcs()
{
    return remotedesktopsnapshottormanager_funcs_;
}

static int luaL_register_remotedesktopsnapshottormanager(lua_State *L)
{	
    static luaL_Reg _remotedesktopsnapshottormanager_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktopsnapshottormanager_funcs_,&_index,get_remotedesktopsnapshottormanager_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPSNAPSHOTTORMANAGER);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktopsnapshottormanager_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktopsnapshottormanager_funcs_);
    return 1;
}        

int luaopen_remotedesktopsnapshottormanager(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopSnapshottorManager",luaL_register_remotedesktopsnapshottormanager,1);
    lua_pop(L, 1);
    return 0;
}        

