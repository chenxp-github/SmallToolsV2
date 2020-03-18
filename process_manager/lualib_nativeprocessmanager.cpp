#include "lualib_nativeprocessmanager.h"
#include "lualib_nativeprocess.h"
#include "mem_tool.h"
#include "syslog.h"
#include "globals.h"

LUA_IS_VALID_USER_DATA_FUNC(CNativeProcessManager,nativeprocessmanager)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CNativeProcessManager,nativeprocessmanager)
LUA_NEW_USER_DATA_FUNC(CNativeProcessManager,nativeprocessmanager,NATIVEPROCESSMANAGER)
LUA_GC_FUNC(CNativeProcessManager,nativeprocessmanager)
LUA_IS_SAME_FUNC(CNativeProcessManager,nativeprocessmanager)
LUA_TO_STRING_FUNC(CNativeProcessManager,nativeprocessmanager)

bool is_nativeprocessmanager(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_NATIVEPROCESSMANAGER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return nativeprocessmanager_is_userdata_valid(ud);  
}
/****************************************/
static int nativeprocessmanager_new(lua_State *L)
{
    CNativeProcessManager *pt;
    NEW(pt,CNativeProcessManager);
    GLOBAL_TASKMGR(taskmgr);
    pt->Init(taskmgr,1024);
    nativeprocessmanager_new_userdata(L,pt,0);
    return 1;
}

static int nativeprocessmanager_stopcpulimittask(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int _ret_0 = (int)pnativeprocessmanager->StopCpuLimitTask();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_startcpulimittask(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int auto_clear = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pnativeprocessmanager->StartCpuLimitTask(auto_clear);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_unlimitallcpu(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int _ret_0 = (int)pnativeprocessmanager->UnlimitAllCpu();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_limitallcpu(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int cpu_limit = (int)lua_tointeger(L,2);
    int max_cpu_usage = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pnativeprocessmanager->LimitAllCpu(cpu_limit,max_cpu_usage);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_limitcpu(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int pid = (int)lua_tointeger(L,2);
    int cpu_limit = (int)lua_tointeger(L,3);
    int max_cpu_usage = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pnativeprocessmanager->LimitCpu(pid,cpu_limit,max_cpu_usage);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_getprocessbypid(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int pid = (int)lua_tointeger(L,2);
    CNativeProcess *_ret_0 = pnativeprocessmanager->GetProcessByPid(pid);
    if(_ret_0==NULL)return 0;
    nativeprocess_new_userdata(L,_ret_0,1);
    return 1;
}
static int nativeprocessmanager_addprocess(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int pid = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocessmanager->AddProcess(pid);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_findprocessbyname(lua_State *L)
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

static status_t nativeprocessmanager_callback_enum_func(lua_State *L, int _cb_id, bool _once,CNativeProcess *process)
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
static int nativeprocessmanager_enumallprocess(lua_State *L)
{
    int enum_func = CLuaVm::ToFunction(L,1);
    
    BEGIN_CLOSURE(_enum)
    {
        CLOSURE_PARAM_PTR(CNativeProcess*,process,0);
        CLOSURE_PARAM_PTR(lua_State*,L,10);
        CLOSURE_PARAM_INT(enum_func,11);
        nativeprocessmanager_callback_enum_func(L,enum_func,false,process);
        return OK;
    }
    END_CLOSURE(_enum);
    
    _enum.SetParamPointer(10,L);
    _enum.SetParamInt(11,enum_func);

    CNativeProcessManager::EnumAllProcess(&_enum);
    CLuaVm::ReleaseFunction(L,enum_func);
    return 0;
}
static int nativeprocessmanager_delelem(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int i = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocessmanager->DelElem(i);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_getelem(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int index = (int)lua_tointeger(L,2);
    CNativeProcess *_ret_0 = pnativeprocessmanager->GetElem(index);
    if(_ret_0==NULL)return 0;
    nativeprocess_new_userdata(L,_ret_0,1);
    return 1;
}
static int nativeprocessmanager_getlen(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int _ret_0 = (int)pnativeprocessmanager->GetLen();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocessmanager_stopallcpu(lua_State *L)
{
    CNativeProcessManager *pnativeprocessmanager = get_nativeprocessmanager(L,1);
    ASSERT(pnativeprocessmanager);
    int stop = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pnativeprocessmanager->StopAllCpu(stop);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static const luaL_Reg nativeprocessmanager_lib[] = {
    {"__gc",nativeprocessmanager_gc_},
    {"__tostring",nativeprocessmanager_tostring_},
    {"__is_same",nativeprocessmanager_issame_},
    {"new",nativeprocessmanager_new},
    {"StopCpuLimitTask",nativeprocessmanager_stopcpulimittask},
    {"StartCpuLimitTask",nativeprocessmanager_startcpulimittask},
    {"UnlimitAllCpu",nativeprocessmanager_unlimitallcpu},
    {"LimitAllCpu",nativeprocessmanager_limitallcpu},
    {"LimitCpu",nativeprocessmanager_limitcpu},
    {"GetProcessByPid",nativeprocessmanager_getprocessbypid},
    {"AddProcess",nativeprocessmanager_addprocess},
    {"FindProcessByName",nativeprocessmanager_findprocessbyname},
    {"EnumAllProcess",nativeprocessmanager_enumallprocess},
    {"DelElem",nativeprocessmanager_delelem},
    {"GetElem",nativeprocessmanager_getelem},
    {"GetLen",nativeprocessmanager_getlen},
    {"StopAllCpu",nativeprocessmanager_stopallcpu},
    {NULL, NULL}
};
static int luaL_register_nativeprocessmanager(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_NATIVEPROCESSMANAGER);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,nativeprocessmanager_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,nativeprocessmanager_lib);
    return 1;
}

int luaopen_nativeprocessmanager(lua_State *L)
{
    luaL_requiref(L, "NativeProcessManager",luaL_register_nativeprocessmanager,1);
    lua_pop(L, 1);
    return 0;
}
