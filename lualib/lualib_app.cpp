#include "lualib_app.h"
#include "mem_tool.h"
#include "syslog.h"
#include "globals.h"
#include "peerglobals.h"
#include "tasktcpacceptor.h"
#include "taskpeerserver.h"
#include "mkdisk.h"
#include "lua_helper.h"
#include "lualib_simpledisk.h"
#include "lualib_peerproxy.h"
#include "dirmgr.h"
#include "thread.h"

#if _IS_LINUX_
#include "nativeprocessmanager.h"
#endif

/****************************************/
static int app_getsystemtimer(lua_State *L)
{
    int _ret_0 = (int)crt_get_sys_timer();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int app_getselfexepath(lua_State *L)
{
    LOCAL_MEM(mem);
    CGlobals::GetSelfExePath(&mem);
    lua_pushstring(L,mem.CStr());
    return 1;
}
static int app_makefullpathwithexe(lua_State *L)
{
    const char* rpath = (const char*)lua_tostring(L,1);
    LOCAL_MEM(tmp);
    CGlobals::MakeFullPathWithExe(rpath,&tmp);
    lua_pushstring(L,tmp.CStr());
    return 1;
}
static int app_addluasearchpath(lua_State *L)
{
    const char* path = (const char*)lua_tostring(L,1);
    const char* type = (const char*)lua_tostring(L,2);
    const char* ext = (const char*)lua_tostring(L,3);
    int recursive = (int)lua_toboolean(L,4);
    GLOBALS(g);
    g->AddLuaSearchPath(L,path,type,ext,recursive!=0);
    return 0;
}
static int app_quitmainloop(lua_State *L)
{
    GLOBALS(g);
    int _ret_0 = (int)g->QuitMainLoop();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int app_os(lua_State *L)
{
    const char* _ret_0 = (const char*)CGlobals::OS();
    lua_pushstring(L,_ret_0);
    return 1;
}
static int app_mainloop(lua_State *L)
{
    GLOBALS(g);
    g->MainLoop(L);
    return 0;
}
static int app_luamain(lua_State *L)
{       
    GLOBALS(old_world);

    CGlobals new_world;
    g_globals_ptr = &new_world; //point to a new globals

    new_world.Init();
    new_world.SetLuaFilesPath(old_world->GetLuaFilesPath());
    new_world.InitLuaVm();
    
    lua_State *new_L=new_world.m_LuaVm.GetLuaState();

    if(!lua_istable(L,1))
    {
        int ret = lua_main(new_L,0,NULL);
        new_world.m_LuaVm.mL = NULL; //already call lua_close()
        exit(ret);
        return OK;
    }

    int top = lua_gettop(L);
    lua_pushnil(L);

    const int _MAX = 1024;
    char *argv[_MAX];
    
    int argc = 0;
    while (lua_next(L, 1) != 0) 
    {   
        ASSERT(argc <= _MAX);
        argv[argc++] = strdup(lua_tostring(L,-1));
        lua_pop(L, 1);
    }
    argv[argc] = NULL;
    lua_settop(L,top);

    if(argc >= 2)
    {        
        LOCAL_MEM(mem);
        CDirMgr::GetFileName(argv[1],&mem,FN_PATH);
        if(CDirMgr::IsDirExist(&mem))
        {
            GLOBALS(g);
            g->AddLuaSearchPath(mem.CStr(),"path","lua",false);
        }
    }
    
    int ret = lua_main(new_L,argc,argv);
    new_world.m_LuaVm.mL = NULL; //already call lua_close()

    for(int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    exit(ret);
    return 0;
}
static int app_putenv(lua_State *L)
{
    const char *env = lua_tostring(L,1);
    ASSERT(env);
    putenv((char*)env);
    return 0;
}
//////////////////////////////////////
static status_t on_accept(CClosure *closure)
{
	CLOSURE_PARAM_INT(event,0);
	CLOSURE_PARAM_PTR(CTaskMgr*,mgr,10);
	ASSERT(mgr);
	
	if(event == CTaskTcpAcceptor::EVENT_NEW_CLIENT)
	{
        GLOBAL_PEER_GLOBALS(peer_globals);
		CLOSURE_PARAM_INT(fd,1);
		CTaskPeerServer *server;
		NEW(server,CTaskPeerServer);
		server->Init(mgr,peer_globals);
		server->SetMaxRetries(1);
		server->SetSocket(fd);
		server->Start();
	}
	return OK;
}
static status_t start_message_center(int port, int trust_mode)
{
	GLOBAL_TASKMGR(taskmgr);
	CTaskTcpAcceptor *acceptor;
	
	NEW(acceptor,CTaskTcpAcceptor);
	acceptor->Init(taskmgr);
	acceptor->SetListenPort(port);	
	acceptor->Callback()->SetFunc(on_accept);
	acceptor->Callback()->SetParamPointer(10,taskmgr);
	acceptor->Start();

    GLOBAL_PEER_GLOBALS(peer_globals);
    peer_globals->SetTrustMode(trust_mode!=0);
	return OK;
}
//////////////////////////////////////
static int app_startmessagecenter(lua_State *L)
{
	int port = (int)lua_tointeger(L,1);
    int trust_mode = lua_toboolean(L,2);

	int _ret_0 = (int)start_message_center(port,trust_mode);
	lua_pushinteger(L,_ret_0);
	return 1;
}

static status_t app_getallpeers(lua_State *L)
{
    GLOBAL_PEER_GLOBALS(g);
    CPeerProxyManager *mgr = g->GetPeerProxyManager();
	ASSERT(mgr);

    if(mgr->GetTotalPeers() <= 0)
        return 0;

    lua_newtable(L);
    int top = lua_gettop(L);
    
	for(int i = 0; i < mgr->GetTotalPeers(); i++)
	{
        lua_pushinteger(L,i+1);
        peerproxy_new_userdata(L,mgr->GetPeer(i),1);
        lua_settable(L, top);    
	}

    return 1;
}

static status_t app_sleep(lua_State *L)
{
    int ms = (int)lua_tointeger(L,1);
    crt_msleep(ms);
    return 0;
}

extern int g_argc;
extern char **g_argv;

static status_t app_getargs(lua_State *L)
{
    if(g_argv == NULL)
        return 0;

    CMemStk args;
    args.Init();

    for(int i = 0; i < g_argc; i++)
    {
        args.Push(g_argv[i]);
    }

    if(CLuaVm::PushStringArray(L,&args))
    {
        return 1;
    }    
    return 0;
}

static int app_makeordersimplediskfromdir(lua_State *L)
{
    LUA_TO_LOCAL_STRING(dir,L,1);
    CSimpleDisk *disk = get_simpledisk(L,2);
    ASSERT(disk);
    int _ret_0 = (int)MakeOrderSimpleDiskFromDir(dir,disk);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static status_t app_quittask(lua_State *L)
{
    int tid = (int)lua_tointeger(L,1);
    CTaskMgr *taskmgr = how_to_get_global_taskmgr(L);
    ASSERT(taskmgr);
    taskmgr->QuitTask(&tid);
    return 0;
}

#if _IS_LINUX_
static status_t kill_all_child_processes(int ppid)
{
    #define MAX_PIDS 1024

    BEGIN_CLOSURE(on_enum)
    {
        CLOSURE_PARAM_PTR(CNativeProcess*,process,0);
        CLOSURE_PARAM_INT(ppid,10);
        CLOSURE_PARAM_PTR(int_ptr_t*,pids,11);
        CLOSURE_PARAM_PTR(int*,k,12);

        if(CNativeProcess::IsChildProcessOf(ppid,process->GetPid()))
        {
            ASSERT((*k) < MAX_PIDS);
            pids[(*k)++] = process->GetPid();
        }
        return OK;
    }
    END_CLOSURE(on_enum);

    int_ptr_t pids[MAX_PIDS];
    int k = 0;

    on_enum.SetParamInt(10,ppid);
    on_enum.SetParamPointer(11,pids);
    on_enum.SetParamPointer(12,&k);

    CNativeProcessManager::EnumAllProcess(&on_enum);

    XLOG(LOG_MODULE_USER,LOG_LEVEL_INFO,"");
    for(int i = 0; i < k; i++)
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_INFO,
            "terminate child process %d",pids[i]
        );
        kill(pids[i],9);
    }

    return OK;
}
#endif

class CAutoExitThread:public CThread{
public:
    uint32_t timeout;
    int exit_code;
public:

   CAutoExitThread(uint32_t to,int exit_code)
   {
       this->timeout = to;
       this->exit_code = exit_code;
   }

   status_t Run()
   {
       uint32_t start_tm = crt_get_sys_timer();
       while(IsRunning())
       {
           uint32_t now_tm = crt_get_sys_timer();
           if(now_tm - start_tm > this->timeout)
           {
                #if _IS_LINUX_
                kill_all_child_processes(getpid());
                kill(getpid(),9);
                #else
				exit(0);
                #endif

                break;
           }
           crt_msleep(10);
       }
       return OK;
   }
};

static status_t app_startautoexitthread(lua_State *L)
{
    int ms = (int)lua_tointeger(L,1);
    int exit_code = (int)lua_tointeger(L,2);
    //this process will exit when timeout
    CAutoExitThread *thread;
    NEW(thread,CAutoExitThread(ms,exit_code));
    thread->Init();
    thread->Start();
    return 0;
}

static const luaL_Reg app_lib[] = {
    {"GetSystemTimer",app_getsystemtimer},
    {"GetSelfExePath",app_getselfexepath},
    {"MakeFullPathWithExe",app_makefullpathwithexe},
    {"AddLuaSearchPath",app_addluasearchpath},
    {"QuitMainLoop",app_quitmainloop},
    {"MainLoop",app_mainloop},
    {"OS",app_os},
    {"LuaMain",app_luamain},
    {"PutEnv",app_putenv},
	{"StartMessageCenter",app_startmessagecenter},
    {"GetAllPeers",app_getallpeers},
    {"Sleep",app_sleep},	
    {"GetArgs",app_getargs},
	{"MakeOrderSimpleDiskFromDir",app_makeordersimplediskfromdir},
    {"QuitTask",app_quittask},    
    {"StartAutoExitThread",app_startautoexitthread},    
    {NULL, NULL}
};

static int luaL_register_app(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_APP);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,app_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,app_lib);
    return 1;
}

int luaopen_app(lua_State *L)
{
    luaL_requiref(L, "App",luaL_register_app,1);
    lua_pop(L, 1);
    return 0;
}
