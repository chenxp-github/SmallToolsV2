#include "lualib_nativeprocessmanager.h"
#include "lualib_nativeprocess.h"
#include "mem_tool.h"
#include "syslog.h"
#include "globals.h"

CNativeProcessManager *get_fivprocessmanager(lua_State *L, int idx)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, idx, LUA_USERDATA_FIVPROCESSMANAGER);
    ASSERT(ud && ud->p);
    ASSERT(ud->__weak_ref_id != 0);
    CNativeProcessManager *p = (CNativeProcessManager*)ud->p;
    ASSERT(p->__weak_ref_id == ud->__weak_ref_id);
    return p;
}
lua_userdata *fivprocessmanager_new_userdata(lua_State *L,CNativeProcessManager *pt,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pt);
    ud->p = pt;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pt->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_FIVPROCESSMANAGER);
    lua_setmetatable(L,-2);
    return ud;
}

static int fivprocessmanager_new(lua_State *L)
{
    CNativeProcessManager *pt;
    NEW(pt,CNativeProcessManager);
    GLOBAL_TASKMGR(taskmgr);
    pt->Init(taskmgr,1024);
    fivprocessmanager_new_userdata(L,pt,0);
    return 1;
}

static int fivprocessmanager_destroy(lua_State *L)
{
    lua_userdata *ud = (lua_userdata*)luaL_checkudata(L, 1, LUA_USERDATA_FIVPROCESSMANAGER);
    ASSERT(ud);
    CNativeProcessManager *pfivprocessmanager = (CNativeProcessManager*)ud->p;
    if(!(ud->is_attached))
    {
        DEL(pfivprocessmanager);
    }
    return 0;
}
static int fivprocessmanager_issame(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager1 = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager1);
    CNativeProcessManager *pfivprocessmanager2 = get_fivprocessmanager(L,2);
    ASSERT(pfivprocessmanager2);
    int is_same = (pfivprocessmanager1==pfivprocessmanager2);
    lua_pushboolean(L,is_same);
    return 1;
}
static int fivprocessmanager_tostring(lua_State *L)
{
    lua_pushstring(L,"userdata:fivprocessmanager");
    return 1;
}
/****************************************/
static int fivprocessmanager_stopcpulimittask(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int _ret_0 = (int)pfivprocessmanager->StopCpuLimitTask();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int fivprocessmanager_startcpulimittask(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int auto_clear = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pfivprocessmanager->StartCpuLimitTask(auto_clear);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int fivprocessmanager_unlimitallcpu(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int _ret_0 = (int)pfivprocessmanager->UnlimitAllCpu();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int fivprocessmanager_limitallcpu(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int cpu_limit = (int)lua_tointeger(L,2);
    int max_cpu_usage = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pfivprocessmanager->LimitAllCpu(cpu_limit,max_cpu_usage);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int fivprocessmanager_limitcpu(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int pid = (int)lua_tointeger(L,2);
    int cpu_limit = (int)lua_tointeger(L,3);
    int max_cpu_usage = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pfivprocessmanager->LimitCpu(pid,cpu_limit,max_cpu_usage);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int fivprocessmanager_getprocessbypid(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int pid = (int)lua_tointeger(L,2);
    CNativeProcess *_ret_0 = pfivprocessmanager->GetProcessByPid(pid);
    if(_ret_0==NULL)return 0;
    nativeprocess_new_userdata(L,_ret_0,1);
    return 1;
}
static int fivprocessmanager_addprocess(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int pid = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pfivprocessmanager->AddProcess(pid);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int fivprocessmanager_findprocessbyname(lua_State *L)
{
    const char* name_pattern = (const char*)lua_tostring(L,1);
    int_ptr_t pids[1024];
    int size = sizeof(pids)/sizeof(pids[0]);

    CNativeProcessManager::FindProcessByName(name_pattern,pids,&size);

    if(size > 0)
    {
        lua_newtable(L);
        int top = lua_gettop(L);
        for(int i = 0; i < size; i++)
        {
            lua_pushinteger(L, i+1);
            lua_pushinteger(L, pids[i]);
            lua_settable(L, top);
        }
        return 1;
    }
    return 0;
}

static status_t fivprocessmanager_callback_enum_func(lua_State *L, int _cb_id, bool _once,CNativeProcess *process)
{
    if(_cb_id == LUA_REFNIL)
        return ERROR;
    ASSERT(L);
    ASSERT(process);

    CLuaVm vm;
    vm.Init(L);

    lua_rawgeti(L,LUA_REGISTRYINDEX,_cb_id);
    if(_once)luaL_unref(L,LUA_REGISTRYINDEX,_cb_id);
    nativeprocess_new_userdata(L,process,true);
    vm.Run(1,0);
    vm.ClearStack();
    return OK;
}
static int fivprocessmanager_enumallprocess(lua_State *L)
{
    int enum_func = CLuaVm::ToFunction(L,1);
    
    BEGIN_CLOSURE(_enum)
    {
        CLOSURE_PARAM_PTR(CNativeProcess*,process,0);
        CLOSURE_PARAM_PTR(lua_State*,L,10);
        CLOSURE_PARAM_INT(enum_func,11);
        fivprocessmanager_callback_enum_func(L,enum_func,false,process);
        return OK;
    }
    END_CLOSURE(_enum);
    
    _enum.SetParamPointer(10,L);
    _enum.SetParamInt(11,enum_func);

    CNativeProcessManager::EnumAllProcess(&_enum);
    CLuaVm::ReleaseFunction(L,enum_func);
    return 0;
}
static int fivprocessmanager_delelem(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int i = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pfivprocessmanager->DelElem(i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int fivprocessmanager_getelem(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int index = (int)lua_tointeger(L,2);
    CNativeProcess *_ret_0 = pfivprocessmanager->GetElem(index);
    if(_ret_0==NULL)return 0;
    nativeprocess_new_userdata(L,_ret_0,1);
    return 1;
}
static int fivprocessmanager_getlen(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int _ret_0 = (int)pfivprocessmanager->GetLen();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int fivprocessmanager_stopallcpu(lua_State *L)
{
    CNativeProcessManager *pfivprocessmanager = get_fivprocessmanager(L,1);
    ASSERT(pfivprocessmanager);
    int stop = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pfivprocessmanager->StopAllCpu(stop);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static const luaL_Reg fivprocessmanager_lib[] = {
    {"new",fivprocessmanager_new},
    {"__gc",fivprocessmanager_destroy},
    {"__tostring",fivprocessmanager_tostring},
    {"IsSame",fivprocessmanager_issame},
    {"StopCpuLimitTask",fivprocessmanager_stopcpulimittask},
    {"StartCpuLimitTask",fivprocessmanager_startcpulimittask},
    {"UnlimitAllCpu",fivprocessmanager_unlimitallcpu},
    {"LimitAllCpu",fivprocessmanager_limitallcpu},
    {"LimitCpu",fivprocessmanager_limitcpu},
    {"GetProcessByPid",fivprocessmanager_getprocessbypid},
    {"AddProcess",fivprocessmanager_addprocess},
    {"FindProcessByName",fivprocessmanager_findprocessbyname},
    {"EnumAllProcess",fivprocessmanager_enumallprocess},
    {"DelElem",fivprocessmanager_delelem},
    {"GetElem",fivprocessmanager_getelem},
    {"GetLen",fivprocessmanager_getlen},
    {"StopAllCpu",fivprocessmanager_stopallcpu},
    {NULL, NULL}
};
static int luaL_register_fivprocessmanager(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_FIVPROCESSMANAGER);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,fivprocessmanager_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,fivprocessmanager_lib);
    return 1;
}

int luaopen_fivprocessmanager(lua_State *L)
{
    luaL_requiref(L, "NativeProcessManager",luaL_register_fivprocessmanager,1);
    lua_pop(L, 1);
    return 0;
}
