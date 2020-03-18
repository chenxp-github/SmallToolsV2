#include "lualib_tasktimer.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"

#define CBID_PARAM_INDEX 10

#define RELEASE_CALLBACK(ptasktimer) do{\
if(ptasktimer->Callback()->GetParamType(CBID_PARAM_INDEX) == PARAM_TYPE_INT)\
{\
    int old_cbid = ptasktimer->Callback()->GetParamInt(CBID_PARAM_INDEX);\
    CLuaVm::ReleaseFunction(L,old_cbid);\
    ptasktimer->Callback()->SetParamInt(CBID_PARAM_INDEX,LUA_REFNIL);\
}\
}while(0)\

LUA_IS_VALID_USER_DATA_FUNC(CTaskTimer,tasktimer)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CTaskTimer,tasktimer)
LUA_NEW_USER_DATA_FUNC(CTaskTimer,tasktimer,TASKTIMER)
LUA_IS_SAME_FUNC(CTaskTimer,tasktimer)
LUA_TO_STRING_FUNC(CTaskTimer,tasktimer)

bool is_tasktimer(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_TASKTIMER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return tasktimer_is_userdata_valid(ud);  
}
/*************************************************/
static int tasktimer_new(lua_State *L)
{
    CTaskTimer *pt;

    NEW(pt,CTaskTimer);
    pt->Init(how_to_get_global_taskmgr());
    tasktimer_new_userdata(L,pt,0);
    return 1;
}

static int tasktimer_gc_(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_TASKTIMER);
	ASSERT(ud);

    if(!tasktimer_is_userdata_valid(ud))
    {
		ud->weak_ptr.Destroy();
		return 0;
	}

    CTaskTimer *ptasktimer = (CTaskTimer*)ud->weak_ptr.GetRawPtr();
    if(!(ud->is_attached))
    {
        RELEASE_CALLBACK(ptasktimer);
        ptasktimer->Quit();//do not use DEL()
    }
	ud->weak_ptr.Destroy();
    return 0;
}

/****************************************/
static int tasktimer_gettimeout(lua_State *L)
{
    CTaskTimer *ptasktimer = get_tasktimer(L,1);
    ASSERT(ptasktimer);
    uint32_t _ret_0 = (uint32_t)ptasktimer->GetTimeout();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int tasktimer_setoneshot(lua_State *L)
{
    CTaskTimer *ptasktimer = get_tasktimer(L,1);
    ASSERT(ptasktimer);
    bool os = lua_toboolean(L,2) != 0;
    bool _ret_0 = ptasktimer->SetOneShot(os)!=0;
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int tasktimer_settimeout(lua_State *L)
{
    CTaskTimer *ptasktimer = get_tasktimer(L,1);
    ASSERT(ptasktimer);
    uint32_t ms = (uint32_t)lua_tointeger(L,2);
    int _ret_0 = (int)ptasktimer->SetTimeout(ms);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int tasktimer_start(lua_State *L)
{
    CTaskTimer *ptasktimer = get_tasktimer(L,1);
    ASSERT(ptasktimer);
    int _ret_0 = (int)ptasktimer->Start();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int tasktimer_stop(lua_State *L)
{
    CTaskTimer *ptasktimer = get_tasktimer(L,1);
    ASSERT(ptasktimer);

    RELEASE_CALLBACK(ptasktimer);

    int _ret_0 = (int)ptasktimer->Stop();
    lua_pushboolean(L,_ret_0);
    return 1;
}

static status_t tasktimer_callback_ontimercallback(lua_State *L, int _cb_id, bool _once,uint32_t interval)
{
    if(_cb_id == LUA_REFNIL)
        return ERROR;
    ASSERT(L);

    CLuaVm vm;
    vm.Init(L);

    lua_rawgeti(L,LUA_REGISTRYINDEX,_cb_id);
    if(_once)luaL_unref(L,LUA_REGISTRYINDEX,_cb_id);
    lua_pushinteger(L,interval);
    vm.Run(1,0);
    vm.ClearStack();
    return OK;
}

static int tasktimer_setcallback(lua_State *L)
{
    CTaskTimer *ptasktimer = get_tasktimer(L,1);
    ASSERT(ptasktimer);
    int onTimerCallback = CLuaVm::ToFunction(L,2);
    ASSERT(onTimerCallback);
    
    RELEASE_CALLBACK(ptasktimer);    
    
    BEGIN_CLOSURE_FUNC(trigger)
    {
        CLOSURE_PARAM_INT(interval,0);
        CLOSURE_PARAM_INT(cbid,CBID_PARAM_INDEX);
        CLuaVm *vm = how_to_get_global_luavm();
		ASSERT(vm);
        tasktimer_callback_ontimercallback(vm->GetLuaState(),cbid,false,interval);
        return OK;
    }
    END_CLOSURE_FUNC(trigger);

    ptasktimer->Callback()->SetParamInt(CBID_PARAM_INDEX,onTimerCallback);
    ptasktimer->Callback()->SetFunc(trigger);
    return 0;
}

static const luaL_Reg tasktimer_lib[] = {
    {"__gc",tasktimer_gc_},
    {"__tostring",tasktimer_tostring_},
    {"__is_same",tasktimer_issame_},
    {"new",tasktimer_new},
    {"GetTimeout",tasktimer_gettimeout},
    {"SetOneShot",tasktimer_setoneshot},
    {"SetTimeout",tasktimer_settimeout},
    {"Start",tasktimer_start},
    {"Stop",tasktimer_stop},
    {"SetCallback",tasktimer_setcallback},
    {NULL, NULL}
};
static int luaL_register_tasktimer(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_TASKTIMER);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,tasktimer_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,tasktimer_lib);
    return 1;
}

int luaopen_tasktimer(lua_State *L)
{
    luaL_requiref(L, "Timer",luaL_register_tasktimer,1);
    lua_pop(L, 1);
    return 0;
}
