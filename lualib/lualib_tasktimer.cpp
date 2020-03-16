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

CTaskTimer *get_tasktimer(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_TASKTIMER);
    ASSERT(ud && ud->p);
    ASSERT(ud->__weak_ref_id != 0);
    CTaskTimer *p = (CTaskTimer*)ud->p;
    ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
    return p;
}
lua_userdata *tasktimer_new_userdata(lua_State *L,CTaskTimer *pt,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pt);
    ud->p = pt;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pt->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_TASKTIMER);
    lua_setmetatable(L,-2);
    return ud;
}

static int tasktimer_new(lua_State *L)
{
    CTaskTimer *pt;

    NEW(pt,CTaskTimer);
    pt->Init(how_to_get_global_taskmgr());
    tasktimer_new_userdata(L,pt,0);
    return 1;
}

static bool tasktimer_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CHECK_IS_UD_READABLE(CTaskTimer,ud);
    CTaskTimer *p = (CTaskTimer*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}

static int tasktimer_destroy(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_TASKTIMER);
    if(!tasktimer_is_userdata_valid(ud))
        return 0;

    CTaskTimer *ptasktimer = (CTaskTimer*)ud->p;
    if(!(ud->is_attached))
    {
        RELEASE_CALLBACK(ptasktimer);
        ptasktimer->Quit();//do not use DEL()
    }
    return 0;
}
static int tasktimer_issame(lua_State *L)
{
    CTaskTimer *ptasktimer1 = get_tasktimer(L,1);
    ASSERT(ptasktimer1);
    CTaskTimer *ptasktimer2 = get_tasktimer(L,2);
    ASSERT(ptasktimer2);
    int is_same = (ptasktimer1==ptasktimer2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int tasktimer_tostring(lua_State *L)
{
    lua_pushstring(L,"userdata:tasktimer");
    return 1;
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
    {"new",tasktimer_new},
    {"__gc",tasktimer_destroy},
    {"__tostring",tasktimer_tostring},
    {"IsSame",tasktimer_issame},
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
