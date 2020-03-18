#include "lualib_nativeprocess.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CNativeProcess,nativeprocess)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CNativeProcess,nativeprocess)
LUA_NEW_USER_DATA_FUNC(CNativeProcess,nativeprocess,NATIVEPROCESS)
LUA_GC_FUNC(CNativeProcess,nativeprocess)
LUA_IS_SAME_FUNC(CNativeProcess,nativeprocess)
LUA_TO_STRING_FUNC(CNativeProcess,nativeprocess)

bool is_nativeprocess(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_NATIVEPROCESS,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return nativeprocess_is_userdata_valid(ud);  
}

/****************************************/
static int nativeprocess_new(lua_State *L)
{
    CNativeProcess *pt;
    NEW(pt,CNativeProcess);
    pt->Init();
    nativeprocess_new_userdata(L,pt,0);
    return 1;
}
static int nativeprocess_limitcpu(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int cpu_limit = (int)lua_tointeger(L,2);
    int max_cpu_usage = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pnativeprocess->LimitCpu(cpu_limit,max_cpu_usage);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_iscpuusagelimited(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->IsCpuUsageLimited();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_stopcpu(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int stop = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pnativeprocess->StopCpu(stop);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_isexist(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->IsExist();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_kill(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int sig = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->Kill(sig);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_updatememoryusage(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->UpdateMemoryUsage();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_getdirtymemorykb(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->GetDirtyMemoryKb();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_updateexename(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->UpdateExeName();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_updatecmdline(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->UpdateCmdLine();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_getpid(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->GetPid();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_getexename(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    const char* _ret_0 = (const char*)pnativeprocess->GetExeNameStr();
    lua_pushstring(L,_ret_0);
    return 1;
}
static int nativeprocess_getcmdline(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    CMemStk *cmdline = pnativeprocess->GetCmdLine();
    ASSERT(cmdline);

    lua_newtable(L);
    int top = lua_gettop(L);
    
    for(int i = 0; i < cmdline->GetLen(); i++)
    {
        CMem *pmem = cmdline->GetElem(i);
        ASSERT(pmem);
        lua_pushinteger(L, i+1);
        lua_pushstring(L, pmem->CStr());
        lua_settable(L, top);
    }

    if(cmdline->GetLen() > 0)
        return 1;
    return 0;
}
static int nativeprocess_getmemoryusage(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->GetMemoryUsage();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_getmaxcpuusage(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->GetMaxCpuUsage();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_getcpuusagelimit(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->GetCpuUsageLimit();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_getcpuusagetick(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->GetCpuUsageTick();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_setpid(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _pid = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->SetPid(_pid);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_setmemoryusage(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _memoryusage = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->SetMemoryUsage(_memoryusage);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_setmaxcpuusage(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _maxcpuusage = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->SetMaxCpuUsage(_maxcpuusage);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_setcpuusagelimit(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _cpuusagelimit = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->SetCpuUsageLimit(_cpuusagelimit);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_setcpuusagetick(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _cpuusagetick = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->SetCpuUsageTick(_cpuusagetick);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_setexename(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    const char* _exename = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pnativeprocess->SetExeName(_exename);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_schedsetaffinity(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    uint32_t mask = (uint32_t)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->SchedSetAffinity(mask);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_schedgetaffinity(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    uint32_t _ret_0 = (uint32_t)pnativeprocess->SchedGetAffinity();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_schedsetnicevalue(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int nice = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pnativeprocess->SchedSetNiceValue(nice);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_schedgetnicevalue(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->SchedGetNiceValue();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_schedgetschedpriority(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->SchedGetSchedPriority();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_schedgetscheduler(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int _ret_0 = (int)pnativeprocess->SchedGetScheduler();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_schedsetscheduler(lua_State *L)
{
    CNativeProcess *pnativeprocess = get_nativeprocess(L,1);
    ASSERT(pnativeprocess);
    int policy = (int)lua_tointeger(L,2);
    int priority = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pnativeprocess->SchedSetScheduler(policy,priority);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int nativeprocess_schedgetprioritymax(lua_State *L)
{
    int sched_type = (int)lua_tointeger(L,1);
    int _ret_0 = (int)CNativeProcess::SchedGetPriorityMax(sched_type);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int nativeprocess_schedgetprioritymin(lua_State *L)
{
    int sched_type = (int)lua_tointeger(L,1);
    int _ret_0 = (int)CNativeProcess::SchedGetPriorityMin(sched_type);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static const luaL_Reg nativeprocess_lib[] = {
    {"__gc",nativeprocess_gc_},
    {"__tostring",nativeprocess_tostring_},
    {"__is_same",nativeprocess_issame_},
    {"new",nativeprocess_new},
    {"LimitCpu",nativeprocess_limitcpu},
    {"IsCpuUsageLimited",nativeprocess_iscpuusagelimited},
    {"StopCpu",nativeprocess_stopcpu},
    {"IsExist",nativeprocess_isexist},
    {"Kill",nativeprocess_kill},
    {"UpdateMemoryUsage",nativeprocess_updatememoryusage},
    {"GetDirtyMemoryKb",nativeprocess_getdirtymemorykb},
    {"UpdateExeName",nativeprocess_updateexename},
    {"UpdateCmdLine",nativeprocess_updatecmdline},
    {"GetPid",nativeprocess_getpid},
    {"GetExeName",nativeprocess_getexename},
    {"GetCmdLine",nativeprocess_getcmdline},
    {"GetMemoryUsage",nativeprocess_getmemoryusage},
    {"GetMaxCpuUsage",nativeprocess_getmaxcpuusage},
    {"GetCpuUsageLimit",nativeprocess_getcpuusagelimit},
    {"GetCpuUsageTick",nativeprocess_getcpuusagetick},
    {"SetPid",nativeprocess_setpid},
    {"SetMemoryUsage",nativeprocess_setmemoryusage},
    {"SetMaxCpuUsage",nativeprocess_setmaxcpuusage},
    {"SetCpuUsageLimit",nativeprocess_setcpuusagelimit},
    {"SetCpuUsageTick",nativeprocess_setcpuusagetick},
    {"SetExeName",nativeprocess_setexename},
    {"SchedSetAffinity",nativeprocess_schedsetaffinity},
    {"SchedGetAffinity",nativeprocess_schedgetaffinity},
    {"SchedSetNiceValue",nativeprocess_schedsetnicevalue},
    {"SchedGetNiceValue",nativeprocess_schedgetnicevalue},
    {"SchedGetSchedPriority",nativeprocess_schedgetschedpriority},
    {"SchedGetScheduler",nativeprocess_schedgetscheduler},
    {"SchedSetScheduler",nativeprocess_schedsetscheduler},
    {"SchedGetPriorityMax",nativeprocess_schedgetprioritymax},
    {"SchedGetPriorityMin",nativeprocess_schedgetprioritymin},    
    {NULL, NULL}
};
static int luaL_register_nativeprocess(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_NATIVEPROCESS);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,nativeprocess_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,nativeprocess_lib);
    return 1;
}

int luaopen_nativeprocess(lua_State *L)
{
    luaL_requiref(L, "NativeProcess",luaL_register_nativeprocess,1);
    lua_pop(L, 1);
    return 0;
}
