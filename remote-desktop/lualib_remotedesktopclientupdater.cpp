#include "lualib_remotedesktopclientupdater.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "lualib_remotedesktopclient.h"
#include "lualib_minibson.h"
#include "lualib_remotedesktoppixelbuffer.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CRemoteDesktopClientUpdater,remotedesktopclientupdater)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CRemoteDesktopClientUpdater,remotedesktopclientupdater)
LUA_NEW_USER_DATA_FUNC(CRemoteDesktopClientUpdater,remotedesktopclientupdater,REMOTEDESKTOPCLIENTUPDATER)
LUA_GC_FUNC(CRemoteDesktopClientUpdater,remotedesktopclientupdater)
LUA_IS_SAME_FUNC(CRemoteDesktopClientUpdater,remotedesktopclientupdater)
LUA_TO_STRING_FUNC(CRemoteDesktopClientUpdater,remotedesktopclientupdater)

bool is_remotedesktopclientupdater(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_REMOTEDESKTOPCLIENTUPDATER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return remotedesktopclientupdater_is_userdata_valid(ud);  
}

/****************************************************/
static status_t remotedesktopclientupdater_new(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater;
    NEW(premotedesktopclientupdater,CRemoteDesktopClientUpdater);
	ASSERT(how_to_get_global_taskmgr);
    premotedesktopclientupdater->Init(how_to_get_global_taskmgr(L));
    remotedesktopclientupdater_new_userdata(L,premotedesktopclientupdater,0);
    return 1;
}

static status_t remotedesktopclientupdater_destroy(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    status_t ret0 = premotedesktopclientupdater->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_getframeno(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int ret0 = premotedesktopclientupdater->GetFrameNo();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_getcurscreen(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    CRemoteDesktopPixelBuffer *ret0 = premotedesktopclientupdater->GetCurScreen();
    ASSERT(ret0);
    remotedesktoppixelbuffer_new_userdata(L,ret0,1);
    return 1;
}

static status_t remotedesktopclientupdater_getclient(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    CRemoteDesktopClient *ret0 = premotedesktopclientupdater->GetClient();
    ASSERT(ret0);
    remotedesktopclient_new_userdata(L,ret0,1);
    return 1;
}

static status_t remotedesktopclientupdater_getremotemonitorindex(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int ret0 = premotedesktopclientupdater->GetRemoteMonitorIndex();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_getremotemonitornames(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    CMemStk *names = premotedesktopclientupdater->GetRemoteMonitorNames();
	ASSERT(names);
	CLuaVm::PushStringArray(L,names);
    return 1;
}

static status_t remotedesktopclientupdater_setframeno(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int _frame_no = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopclientupdater->SetFrameNo(_frame_no);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_setclient(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    CRemoteDesktopClient *client = get_remotedesktopclient(L,2);
    ASSERT(client);
    status_t ret0 = premotedesktopclientupdater->SetClient(client);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_setremotemonitorindex(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int remote_monitor_index = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopclientupdater->SetRemoteMonitorIndex(remote_monitor_index);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_startautoupdatetask(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    status_t ret0 = premotedesktopclientupdater->StartAutoUpdateTask();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_stopautoupdatetask(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    status_t ret0 = premotedesktopclientupdater->StopAutoUpdateTask();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_sendinput(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    CMiniBson *bson = get_minibson(L,2);
    ASSERT(bson);
	bson->ResetPointer();
	CRemoteDesktopInput input;
	input.Init();
	ASSERT(input.LoadBson(bson));
    status_t ret0 = premotedesktopclientupdater->SendInput(&input);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_callback_onevent(lua_State *L, int _funcid, bool _once, int event)
{
	
    if(_funcid == LUA_REFNIL)
    {
        return ERROR;
    }
	
    CLuaVm vm;
    vm.Init(L);
    lua_rawgeti(L,LUA_REGISTRYINDEX,_funcid);
    if(_once)
    {
        luaL_unref(L,LUA_REGISTRYINDEX,_funcid);
    }
    lua_pushinteger(L,event);
    vm.Run(1,0);
    vm.ClearStack();
    return OK;
}

#define CALLBACK_INDEX_ON_EVENT (MAX_PARAMS-1)

static status_t remotedesktopclientupdater_setcallback(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int onevent = CLuaVm::ToFunction(L,2);
	if(premotedesktopclientupdater->Callback()->GetParamType(CALLBACK_INDEX_ON_EVENT) == PARAM_TYPE_INT)
	{
		int cb = premotedesktopclientupdater->Callback()->GetParamInt(CALLBACK_INDEX_ON_EVENT);
		if(cb != LUA_REFNIL)
		{
			CLuaVm::ReleaseFunction(L,cb);
		}
	}

	BEGIN_CLOSURE_FUNC(on_event)
	{
		CLOSURE_PARAM_INT(e,0);
		CLOSURE_PARAM_INT(onevent,CALLBACK_INDEX_ON_EVENT);
		CLOSURE_PARAM_PTR(lua_State*,L,10);
		ASSERT(how_to_get_global_luavm);
		CLuaVm *vm = how_to_get_global_luavm(L);
		ASSERT(vm);
		remotedesktopclientupdater_callback_onevent(vm->GetLuaState(),onevent,false,e);
		return OK;
	}
	END_CLOSURE_FUNC(on_event);

	premotedesktopclientupdater->Callback()->SetFunc(on_event);
	premotedesktopclientupdater->Callback()->SetParamInt(CALLBACK_INDEX_ON_EVENT,onevent);
	premotedesktopclientupdater->Callback()->SetParamPointer(10,L);
	return 0;
}
static status_t remotedesktopclientupdater_setdelayperframe(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    uint32_t ms = (uint32_t)lua_tointeger(L,2);
    status_t ret0 = premotedesktopclientupdater->SetDelayPerFrame(ms);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_setbyteperpixel(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int bpp = (int)lua_tointeger(L,2);
    status_t ret0 = premotedesktopclientupdater->SetBytePerPixel(bpp);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_settotaltransfersize(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int64_t _total_transfer_size = (int64_t)lua_tointeger(L,2);
    status_t ret0 = premotedesktopclientupdater->SetTotalTransferSize(_total_transfer_size);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t remotedesktopclientupdater_gettotaltransfersize(lua_State *L)
{
    CRemoteDesktopClientUpdater *premotedesktopclientupdater = get_remotedesktopclientupdater(L,1);
    ASSERT(premotedesktopclientupdater);
    int64_t ret0 = premotedesktopclientupdater->GetTotalTransferSize();
    lua_pushinteger(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg remotedesktopclientupdater_funcs_[] = {
    {"__gc",remotedesktopclientupdater_gc_},
    {"__tostring",remotedesktopclientupdater_tostring_},
    {"__is_same",remotedesktopclientupdater_issame_},
    {"new",remotedesktopclientupdater_new},
    {"Destroy",remotedesktopclientupdater_destroy},
    {"GetFrameNo",remotedesktopclientupdater_getframeno},
    {"GetCurScreen",remotedesktopclientupdater_getcurscreen},
    {"GetClient",remotedesktopclientupdater_getclient},
    {"GetRemoteMonitorIndex",remotedesktopclientupdater_getremotemonitorindex},
    {"GetRemoteMonitorNames",remotedesktopclientupdater_getremotemonitornames},
    {"SetFrameNo",remotedesktopclientupdater_setframeno},
    {"SetClient",remotedesktopclientupdater_setclient},
    {"SetRemoteMonitorIndex",remotedesktopclientupdater_setremotemonitorindex},
    {"StartAutoUpdateTask",remotedesktopclientupdater_startautoupdatetask},
    {"StopAutoUpdateTask",remotedesktopclientupdater_stopautoupdatetask},
    {"SendInput",remotedesktopclientupdater_sendinput},
    {"SetCallback",remotedesktopclientupdater_setcallback},
    {"SetDelayPerFrame",remotedesktopclientupdater_setdelayperframe},
    {"SetBytePerPixel",remotedesktopclientupdater_setbyteperpixel},
    {"SetTotalTransferSize",remotedesktopclientupdater_settotaltransfersize},
    {"GetTotalTransferSize",remotedesktopclientupdater_gettotaltransfersize},
    {NULL,NULL},
};

const luaL_Reg* get_remotedesktopclientupdater_funcs()
{
    return remotedesktopclientupdater_funcs_;
}

static int luaL_register_remotedesktopclientupdater(lua_State *L)
{	
    static luaL_Reg _remotedesktopclientupdater_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_remotedesktopclientupdater_funcs_,&_index,get_remotedesktopclientupdater_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_REMOTEDESKTOPCLIENTUPDATER);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_remotedesktopclientupdater_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_remotedesktopclientupdater_funcs_);
    return 1;
}        

int luaopen_remotedesktopclientupdater(lua_State *L)
{
    luaL_requiref(L, "RemoteDesktopClientUpdater",luaL_register_remotedesktopclientupdater,1);
    lua_pop(L, 1);
    return 0;
}        

