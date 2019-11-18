#include "globals.h"
#include "syslog.h"
#include "mem_tool.h"
#include "dirmgr.h"
#include "encoder.h"
#include "tasktimer.h"
#include "misc.h"
#include "memfile.h"
#include "peerglobals.h"
#include "lua_helper.h"
#include "lualib_tasktimer.h"
#include "lualib_minibson.h"
#include "lualib_stream.h"
#include "lualib_filemanager.h"
#include "lualib_app.h"
#include "lualib_commandline.h"
#include "lualib_filebase.h"
#include "lualib_mem.h"
#include "lualib_memfile.h"
#include "lualib_file.h"
#include "lualib_cmdentry.h"
#include "lualib_mmapfile.h"
#include "lualib_cfunc.h"
#include "lualib_ximage.h"
#include "lualib_ximagehelper.h"
#include "lualib_resource.h"
#include "lualib_resourcemanager.h"
#include "lualib_simpledisk.h"
#include "lualib_messagepeer.h"
#include "lualib_websocketserver.h"
#include "lualib_websocketmessage.h"
#include "lualib_tcpsocket.h"
#include "lualib_udpsocket.h"
#include "lualib_serversidepeer.h"
#include "lualib_xml.h"
#include "lualib_xmlnode.h"

#if _IS_LINUX_
#include "lualib_nativeprocess.h"
#include "lualib_nativeprocessmanager.h"
#endif

#if _SUPPORT_X11_ 
#include "lualib_xdisplay.h"
#include "lualib_xwindow.h"
#include "lualib_xevent.h"
#endif

CGlobals g_globals;
CGlobals *g_globals_ptr = NULL;

static CTaskMgr* get_global_taskmgr()
{
	GLOBAL_TASKMGR(mgr);
	return mgr;
}

static CLuaVm* get_global_luavm()
{
    GLOBAL_LUAVM(vm);
	return vm;
}

static int* get_lua_running_flag()
{
    static int running = 1;
    return &running;
}
/////////////////////////////////////////////////////

CGlobals::CGlobals()
{
    this->InitBasic();
}
CGlobals::~CGlobals()
{
    this->Destroy();
}
status_t CGlobals::InitBasic()
{
    m_TaskAutoLuaGc = 0;
    m_Flags = 0;
    m_TaskMgr.InitBasic();
    m_MainLoopRunning = false;
    m_LuaVm.InitBasic();
    m_MainLuaFileList.InitBasic();
    m_LuaFilesPath.InitBasic();
	m_Turbo = 0;
	m_Epoll.InitBasic();
    return OK;
}

static status_t on_taskmgr_event(CClosure *closure)
{
	CLOSURE_PARAM_INT(event,0);
	CLOSURE_PARAM_PTR(CGlobals*,self,10);

	if(event == CTaskMgr::EVENT_SOCKET_CONNECTED)
	{
		CLOSURE_PARAM_INT(socket,1);
		CEpoll *epoll = &self->m_Epoll;
		if(epoll->AddFd(socket))
		{
			XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
                "socket %d is added to epoll.",socket);
		}
	}
	
	return OK;
}

status_t CGlobals::Init()
{
    this->InitBasic();
    SetIsInitiated(true);
	m_Epoll.Init(1024);
    m_TaskMgr.Init(1024);
	m_TaskMgr.Callback()->SetFunc(on_taskmgr_event);
	m_TaskMgr.Callback()->SetParamPointer(10,this);
    m_MainLoopRunning = true;
    m_MainLuaFileList.Init(1024);
    m_LuaFilesPath.Init();
	PEER_GLOBALS(g);
	g->Init(&m_TaskMgr);

	how_to_get_global_luavm = get_global_luavm;
	how_to_get_global_taskmgr = get_global_taskmgr;
    how_to_get_lua_running_flag = get_lua_running_flag;
    return OK;
}
status_t CGlobals::Destroy()
{
    if(IsInitiated())
    {
        SetIsInitiated(false);
    }
    m_LuaVm.Destroy();
    m_TaskMgr.Destroy();
    m_MainLuaFileList.Destroy();
    m_LuaFilesPath.Destroy();
	m_Epoll.Destroy();
    this->InitBasic();
    return OK;
}

status_t CGlobals::MainLoop()
{
    while(m_MainLoopRunning)
    {   
		if(!m_TaskMgr.Schedule())
			m_Epoll.Wait(1);
    }
    return OK;
}

status_t CGlobals::QuitMainLoop()
{
    m_MainLoopRunning = false;
    return OK;
}   

static status_t escape_windows_path(const char *path,CMem *out)
{
    ASSERT(path && out);
    int len = strlen(path);
    
    for(int i = 0; i < len; i++)
    {
        if(path[i]=='\\')
        {
            out->Puts("/"); //escape
        }
        else
        {
            out->Putc(path[i]);
        }
    }
    return OK;
}
status_t CGlobals::AddLuaSearchPath(lua_State *L,const char *path, const char *type, const char *ext, bool recursive)
{
    ASSERT(L && path && type && ext);
    ASSERT(path && type && ext);
    
    LOCAL_MEM(abs_path);
    abs_path.StrCpy(path);
    CDirMgr::ToAbsPath(&abs_path);
    
    if(!CDirMgr::IsDirExist(&abs_path))
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "path \"%s\" is not exist",abs_path.CStr());
        return ERROR;
    }
    
    LOCAL_MEM_WITH_SIZE(code,32*1024);
    LOCAL_MEM(tmp);
    
    code.Printf("package.%s=package.%s..\";",type,type);
    
    escape_windows_path(abs_path.CStr(),&tmp);
    code.Puts(&tmp);
    
    code.Printf("/?.%s;",ext);
    BEGIN_CLOSURE(search_dir)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(const char*, fullname,2);
        CLOSURE_PARAM_PTR(CMem*,code,10);
        CLOSURE_PARAM_PTR(const char*,ext,11);
        CLOSURE_PARAM_PTR(CGlobals*,self,12);
        
        if(event == CDirMgr::EVENT_BEGIN_DIR)
        {
            LOCAL_MEM(tmp);
            escape_windows_path(fullname,&tmp);
            code->Puts(&tmp);
            code->Printf("/?.%s;",ext);
        }
        return OK;
    }
    END_CLOSURE(search_dir);
    
    search_dir.SetParamPointer(10,&code);
    search_dir.SetParamPointer(11,(void*)ext);
    search_dir.SetParamPointer(12,this);
    
    CDirMgr::SearchDir(abs_path.CStr(),recursive,&search_dir);
    code.Puts("\"\n");
    
    CLuaVm tmp_vm;
    tmp_vm.Init(L);

    tmp_vm.LoadMem(&code,"lua_package_path");
    tmp_vm.Run(0,0);
    tmp_vm.ClearStack();
    return OK;
}
status_t CGlobals::AddLuaSearchPath(const char *path, const char *type, const char *ext, bool recursive)
{
    return this->AddLuaSearchPath(m_LuaVm.GetLuaState(),path,type,ext,recursive);
}
status_t CGlobals::InitLuaVm()
{
    m_LuaVm.Init();
    lua_State *L = m_LuaVm.GetLuaState();
    luaopen_filemanager(L);
    luaopen_minibson(L);
    luaopen_tasktimer(L);
    luaopen_stream(L);
    luaopen_app(L);
    luaopen_commandline(L);
    luaopen_filebase(L);
    luaopen_memfile(L);
    luaopen_mem(L);
    luaopen_file(L);
    luaopen_cmdentry(L);
    luaopen_mmapfile(L);
    luaopen_cfunc(L);
    luaopen_ximage(L);
    luaopen_ximagehelper(L);
    luaopen_resource(L);
    luaopen_resourcemanager(L);
    luaopen_simpledisk(L);
	luaopen_messagepeer(L);
    luaopen_websocketmessage(L);
    luaopen_websocketserver(L);
    luaopen_tcpsocket(L);
	luaopen_udpsocket(L);
	luaopen_serversidepeer(L);
    luaopen_xmlnode(L);
    luaopen_xml(L);
    
#if _IS_LINUX_
    luaopen_nativeprocess(L);
    luaopen_fivprocessmanager(L);
#endif

#if _SUPPORT_X11_
    luaopen_xwindow(L);
    luaopen_xdisplay(L);
    luaopen_xevent(L);
#endif

    this->StartLuaAutoGcTask();
    this->AddLuaSearchPath(m_LuaFilesPath.CStr(),"cpath","so",true);
    this->AddLuaSearchPath(m_LuaFilesPath.CStr(),"path","lua",true);
    return OK;
}

status_t CGlobals::StopLua()
{
    m_LuaVm.Destroy();
    m_TaskMgr.QuitTask(&m_TaskAutoLuaGc);
    this->SetIsLuaRunning(false);
    return OK;
}

status_t CGlobals::StartLuaAutoGcTask()
{
    ASSERT(!m_TaskMgr.IsTask(m_TaskAutoLuaGc));
    
    CTaskTimer *timer = CTaskTimer::NewTimer(&m_TaskMgr,10000,false);
    ASSERT(timer);
    
    BEGIN_CLOSURE_FUNC(auto_gc)
    {
        CLOSURE_PARAM_PTR(CGlobals*,self,10);
        self->m_LuaVm.FullGC();
        return OK;
    }
    END_CLOSURE_FUNC(auto_gc);
    
    timer->Callback()->SetFunc(auto_gc);
    timer->Callback()->SetParamPointer(10,this);
    timer->Start();
    m_TaskAutoLuaGc = timer->GetId();
    return OK;
}

status_t CGlobals::GetSelfExePath(CMem *fullpath)
{
    ASSERT(fullpath);
    fullpath->SetSize(0);
#if HAVE_WINDOWS_H
    int ret = GetModuleFileNameW(NULL,(wchar_t*)fullpath->GetRawBuf(),(int)(fullpath->GetMaxSize()/sizeof(wchar_t)));
    if(ret > 0)
    {
        fullpath->SetSize(ret*sizeof(wchar_t));
        CEncoder::UnicodeToUtf8(fullpath);
        return OK;
    }
#else
    int n=readlink( "/proc/self/exe",fullpath->GetRawBuf(),fullpath->GetMaxSize()-1);
    if(n > 0)
    {
        fullpath->SetSize(n);
        return OK;
    }
#endif
    return ERROR;
}

status_t CGlobals::MakeFullPathWithExe(const char *rpath, CMem *out)
{
    ASSERT(rpath && out);
    out->SetSize(0);
    
    if(CDirMgr::IsAbsPath(rpath))
    {
        out->StrCpy(rpath);
        return OK;
    }
    
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);
    GetSelfExePath(&mem);
    CDirMgr::GetFileName(&mem,&buf,FN_PATH);
    
    out->Puts(&buf);
    out->Putc(crt_get_path_splitor());
    out->Puts(rpath);
    CDirMgr::ToAbsPath("",out);
    return OK;
}

status_t CGlobals::Main(int argc, char **argv)
{
    if(argc < 2)
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "%s <lua-path>",argv[0]);
        return ERROR;
    }

    const char *rpath = argv[1];

    LOCAL_MEM(lua_path);
    MakeFullPathWithExe(rpath,&lua_path);
    this->SetLuaFilesPath(&lua_path);

    this->FindAllMainLua(&lua_path,&m_MainLuaFileList);

    if(argc == 2)
    {
        return this->BareMain();
    }

    const char *app_name = argv[2];
    CMem *lua_file_name = FindMainLuaFileByAppName(app_name);

    if(!lua_file_name)
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "can not find app: '%s'",app_name);
        return ERROR;
    }
    this->RunAppMain(lua_file_name,argc-2,argv+2);
    return OK;
}

status_t CGlobals::BareMain()
{
    LOCAL_MEM_WITH_SIZE(tmp,32*1024);

    for(int i = 0; i < m_MainLuaFileList.GetLen(); i++)
    {       
        CMem *lua_file = m_MainLuaFileList.GetElem(i);
        if(!this->RunLua(lua_file))
            return ERROR;       
        if(!this->RunStringLuaFunction("app_short_help",&tmp))
            return ERROR;
        tmp.Trim();
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "%s: %s",lua_file->GetFileName(),tmp.CStr());
    }
    return OK;
}

status_t CGlobals::FindAllMainLua(CMem *lua_path, CMemStk *all)
{
    ASSERT(lua_path && all);

    BEGIN_CLOSURE(search_dir)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(const char*, fullname,2);
        CLOSURE_PARAM_PTR(CMemStk*,all,10);
        CLOSURE_PARAM_PTR(CGlobals*,self,11);
        CLOSURE_PARAM_INT(has_error,12);
        
        if(has_error)
        {
            return ERROR;
        }

        if(event == CDirMgr::EVENT_SINGLE_FILE)
        {
            LOCAL_MEM(tmp);
            LOCAL_MEM(name);

            escape_windows_path(fullname,&tmp);
            if(wild_match("*.main.lua",tmp.CStr()))
            {
                if(!self->RunLua(&tmp))
                {
                    closure->SetParamInt(12,1); //has error
                    return ERROR;
                }
                if(!self->RunStringLuaFunction("app_name",&name))
                    return ERROR;
                tmp.SetFileName(name.CStr());
                all->InsOrdered(&tmp,0,1);
            }
        }
        return OK;
    }
    END_CLOSURE(search_dir);
    search_dir.SetParamPointer(10,all);
    search_dir.SetParamPointer(11,this);
    search_dir.SetParamInt(12,0); //have errors
    CDirMgr::SearchDir(lua_path->CStr(),true,&search_dir);
    return OK;
}

status_t CGlobals::RunLua(CMem *fn)
{
    ASSERT(fn);
    this->StopLua();
    this->InitLuaVm();  
    if(!m_LuaVm.LoadFile(fn->CStr()))
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "run lua file '%s' fail",fn->CStr());
        return ERROR;
    }
    
    this->SetIsLuaRunning(true);
    return m_LuaVm.Run(0,0);
}

status_t CGlobals::RunStringLuaFunction(const char *func_name,CMem *ret)
{
    ASSERT(func_name && ret);
    ret->SetSize(0);
    if(m_LuaVm.GetGlobalFunction(func_name))
    {
        if(!m_LuaVm.Run(0,1))
            return ERROR;
        const char * lua_ret = lua_tostring(m_LuaVm.GetLuaState(),-1);
        if(lua_ret)ret->StrCpy(lua_ret);
    }
    return OK;
}

status_t CGlobals::AddLinePrefix(const char *prefix, CFileBase *file)
{
    ASSERT(prefix && file);

    CMem tmp;
    tmp.Init();
    tmp.Malloc((int_ptr_t)file->GetSize());
    tmp.WriteFile(file);

    file->SetSize(0);
    LOCAL_MEM(mem);

    tmp.Seek(0);
    while(tmp.ReadLine(&mem))
    {
        file->Puts(prefix);
        file->Puts(&mem);
        file->Puts("\r\n");
    }
    return OK;
}

CMem* CGlobals::FindMainLuaFileByAppName(const char *app_name)
{
    ASSERT(app_name);

    for(int i = 0; i < m_MainLuaFileList.GetLen(); i++)
    {
        CMem *mem = m_MainLuaFileList.GetElem(i);
        if(strcmp(mem->GetFileName(),app_name) == 0)
            return mem;
    }
    return NULL;
}

static void create_arg_table (lua_State *L,int argc, char **argv) 
{
    lua_newtable(L);
    int top = lua_gettop(L);
    for(int i = 0; i < argc; i++)
    {
        lua_pushinteger(L,i+1);
        lua_pushstring(L,argv[i]);
        lua_settable(L, top);
    }
}

status_t CGlobals::RunAppMain(CMem *lua_file, int argc, char **argv)
{
    ASSERT(lua_file);

    if(!this->RunLua(lua_file))
        return ERROR;

    if(m_LuaVm.GetGlobalFunction("app_main"))
    {
        create_arg_table(m_LuaVm.GetLuaState(),argc,argv);
        return m_LuaVm.Run(1,0);
    }
    return OK;
}

const char * CGlobals::OS()
{
#if HAVE_WINDOWS_H 
    return "win32";
#endif
    return "linux";
}

status_t CGlobals::SetLuaFilesPath(CMem *path)
{
    ASSERT(path);
    m_LuaFilesPath.Copy(path);
    return OK;
}

CMem * CGlobals::GetLuaFilesPath()
{
    return &m_LuaFilesPath;
}

