#ifndef __S2_GLOBALS_H
#define __S2_GLOBALS_H

#include "cruntime.h"
#include "filebase.h"
#include "luavm.h"
#include "taskmgr.h"
#include "memstk.h"
#include "epoll.h"
#include "weak_pointer.h"
#include "peerglobals.h"

#define GLOBALS_FLAG_FUNC(func,bit) FLAG_FUNC(m_Flags,func,bit)

class CGlobals{
public:
    CTaskMgr m_TaskMgr;
    bool m_MainLoopRunning;
    CLuaVm m_LuaVm;
    uint32_t m_Flags;
    int m_TaskAutoLuaGc;
    CMemStk m_MainLuaFileList;
    CMem m_LuaFilesPath;
	int m_Turbo;
	CEpoll m_Epoll;
    CPeerGlobals m_PeerGlobals;
public:
    status_t FastFindAppName(const char *lua_fn, CMem *name);
    status_t LoadEnv();
    CMem * GetLuaFilesPath();
    status_t SetLuaFilesPath(CMem *path);
    static const char * OS();
    status_t RunAppMain(CMem *lua_file, int argc, char **argv);
    CMem* FindMainLuaFileByAppName(const char *app_name);
    status_t AddLinePrefix(const char *prefix, CFileBase *file);
    status_t RunStringLuaFunction(const char *func_name,CMem *ret);
    status_t RunLua(CMem *fn);
    status_t FindAllMainLua(CMem *lua_path, CMemStk *all);
    status_t BareMain();
    status_t Main(int argc, char **argv);
    static status_t MakeFullPathWithExe(const char *rpath, CMem *out);
    static status_t GetSelfExePath(CMem *fullpath);
    status_t StartLuaAutoGcTask();
    status_t StopLua();
    status_t InitLuaVm();
    status_t AddLuaSearchPath(const char *path, const char *type, const char *ext, bool recursive);
    status_t AddLuaSearchPath(lua_State *L,const char *path, const char *type, const char *ext, bool recursive);
    status_t QuitMainLoop();
    status_t MainLoop();
    CGlobals();
    virtual ~CGlobals();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    GLOBALS_FLAG_FUNC(IsLuaRunning,0x00000001);
    GLOBALS_FLAG_FUNC(IsInitiated,0x00000002);
};
//////////////////////////////////////////
extern CGlobals g_globals;
extern CGlobals *g_globals_ptr;

#define GLOBALS(g) CGlobals *g = g_globals_ptr
#define GLOBAL_TASKMGR(mgr) CTaskMgr *mgr=&g_globals_ptr->m_TaskMgr
#define GLOBAL_LUAVM(vm) CLuaVm *vm = &g_globals_ptr->m_LuaVm
#define GLOBAL_PEER_GLOBALS(p) CPeerGlobals *p=&g_globals_ptr->m_PeerGlobals
//////////////////////////////////////////
#endif

