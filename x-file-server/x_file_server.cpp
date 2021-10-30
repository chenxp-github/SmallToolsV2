#include "x_file_server.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "linkrpc_xfs_types.h"
#include "dirmgr.h"
#include "tasktimer.h"

#define MAX_READ_SIZE (8*1024*1024)

CxFileServer::CxFileServer()
{
    this->InitBasic();
}

CxFileServer::~CxFileServer()
{
    this->Destroy();
}

status_t CxFileServer::InitBasic()
{
    m_RootPath.InitBasic();
    m_SessionMap.InitBasic();
    CRpcServiceBase::InitBasic();
    m_TaskAutoClear = 0;
    return OK;
}

status_t CxFileServer::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CRpcServiceBase::Init(mgr);
    m_SessionMap.Init();
    m_RootPath.Init();
    this->StartAutoClearSessionTask();
    return OK;
}

status_t CxFileServer::Destroy()
{
    QuitTask(&m_TaskAutoClear);
    m_SessionMap.Destroy();
    CRpcServiceBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CxFileServer::OnSocketConnected()
{
    return OK;
}

status_t CxFileServer::OnSocketDisconnected()
{
    return OK;
}

status_t CxFileServer::OnRequest(CPeerMessage *_message)
{
    ASSERT(_message);

    int method = _message->GetFunc();
    this->SetDestPeerName(_message->GetFrom());

/*##Begin OnRequest_Cpp ##*/
    switch(method)
    {
        case METHOD_XFS_LISTFOLDER:
            this->OnListFolder(_message);
            break;
        case METHOD_XFS_GETINFO:
            this->OnGetInfo(_message);
            break;
        case METHOD_XFS_CHANGECURRENTDIRECTORY:
            this->OnChangeCurrentDirectory(_message);
            break;
        case METHOD_XFS_GETCURRENTDIRECTORY:
            this->OnGetCurrentDirectory(_message);
            break;
        case METHOD_XFS_OPENFILE:
            this->OnOpenFile(_message);
            break;
        case METHOD_XFS_CLOSEFILE:
            this->OnCloseFile(_message);
            break;
        case METHOD_XFS_READFILE:
            this->OnReadFile(_message);
            break;
        case METHOD_XFS_WRITEFILE:
            this->OnWriteFile(_message);
            break;
        case METHOD_XFS_CREATEFOLDER:
            this->OnCreateFolder(_message);
            break;
        case METHOD_XFS_DELETEFOLDER:
            this->OnDeleteFolder(_message);
            break;
        case METHOD_XFS_DELETEFILE:
            this->OnDeleteFile(_message);
            break;
        default:
            XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
                "unknown method: %d",method
            );
            break;
    }
/*##End OnRequest_Cpp ##*/
    return OK;
}

CxFileSession *CxFileServer::GetSession(CMem *name)
{
    ASSERT(name);
    CxFileSession *session = m_SessionMap.GetByName(name);
    if(session)
    {
        //auto reset timer whenever GetSession
        session->SetCurTimer(0);
    }
    return session;
}

CxFileSession *CxFileServer::GetOrCreateSession(CMem *name)
{
    ASSERT(name);
    CxFileSession *session = this->GetSession(name);
    if(session)return session;

    NEW(session,CxFileSession);
    session->Init();
    session->SetName(name);
    session->SetCurDir("/");
    ASSERT(m_SessionMap.PutPtr(session));

    return session;
}

status_t CxFileServer::SetRootPath(CMem *path)
{
    ASSERT(path);
    m_RootPath.Copy(path);
    return OK;
}

status_t CxFileServer::SetRootPath(const char *path)
{
    ASSERT(path);
    CMem mem(path);
    return this->SetRootPath(&mem);
}

status_t CxFileServer::BuildLocalPath(CxFileSession *session, CMem *rpath,CMem *out)
{
    ASSERT(session && rpath && out);
    ASSERT(m_RootPath.StrLen() > 1);

    LOCAL_MEM(fullpath);

    out->SetSize(0);
    fullpath.Puts(session->GetCurDir());
    fullpath.Putc(crt_get_path_splitor());
    fullpath.Puts(rpath);
    CDirMgr::ToAbsPath(&m_RootPath,&fullpath);

    if(fullpath.StartWith(&m_RootPath,1,0))
    {
        out->Puts(&fullpath);
    }  
    else
    {
        return ERROR;
    }    
    return OK;
}


status_t CxFileServer::OnAutoClearSession(uint32_t interval)
{
    BEGIN_CLOSURE(on_enum)
    {
        CLOSURE_PARAM_PTR(CxFileSession*,session,0);
        CLOSURE_PARAM_INT(interval,10);

        uint32_t t = session->GetCurTimer();
        session->SetCurTimer(t+interval);

        if(session->GetCurTimer() > 60*1000)
        {
            XLOG(LOG_MODULE_USER,LOG_LEVEL_INFO,
                "auto delete session %s",
                session->GetNameStr()
            );
            DEL(session);
            closure->SetParamPointer(0,NULL);
        }        
        return OK;
    }
    END_CLOSURE(on_enum);

    on_enum.SetParamInt(10,interval);
    m_SessionMap.EnumAll(&on_enum);
    return OK;
}

status_t CxFileServer::StartAutoClearSessionTask()
{
    #define TIMEOUT 1000
    ASSERT(!IsTask(m_TaskAutoClear));
    BEGIN_CLOSURE_FUNC(on_timer)
    {
        CLOSURE_PARAM_PTR(CxFileServer*,self,10);
        self->OnAutoClearSession(TIMEOUT);
        return OK;
    }
    END_CLOSURE_FUNC(on_timer);
    CTaskTimer *pt = CTaskTimer::NewTimer(GetTaskMgr(),TIMEOUT,false);
    pt->Callback()->SetFunc(on_timer);
    pt->Callback()->SetParamPointer(10,this);
    m_TaskAutoClear = pt->GetId();
    return OK;
}

/*@@Begin Function OnListFolder @@*/
status_t CxFileServer::OnListFolder(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsListFolder _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsListFolder _ret;
    _ret.Init();
    //////////////////

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    LOCAL_MEM(path);
    if(!this->BuildLocalPath(session,_param.GetPath(),&path))
    {
        _ret.SetResult(ERROR);
        this->SendReturnValue(&_context,&_ret);
        return OK;
    }

    if(!CDirMgr::IsDirExist(&path))
    {
        _ret.SetResult(ERROR);
        this->SendReturnValue(&_context,&_ret);
        return OK;
    }

    BEGIN_CLOSURE(on_search_dir)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(char*,file_name,1);
        CLOSURE_PARAM_PTR(char*,full_name,2);
        CLOSURE_PARAM_INT(is_dir,3);
        CLOSURE_PARAM_INT64(size,4);
        CLOSURE_PARAM_PTR(CxFileInfoList*, file_info_list,10);

        if(event == CDirMgr::EVENT_BEGIN_DIR || event == CDirMgr::EVENT_SINGLE_FILE)
        {
            CxFileInfo info;
            info.Init();
            info.SetIsDir(is_dir!=0);
            info.SetSize(size);
            info.SetFileName(file_name);
            file_info_list->Push(&info);
        }
        return OK;
    }
    END_CLOSURE(on_search_dir);
    on_search_dir.SetParamPointer(10,_ret.GetList());

    CDirMgr::SearchDir(&path,false,&on_search_dir);

    _ret.SetResult(OK);
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnListFolder @@*/

/*@@Begin Function OnGetInfo @@*/
status_t CxFileServer::OnGetInfo(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsGetInfo _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsGetInfo _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    LOCAL_MEM(path);
    if(!this->BuildLocalPath(session,_param.GetPath(),&path))
    {
        _ret.SetResult(ERROR);
        this->SendReturnValue(&_context,&_ret);
        return OK;
    }

    CxFileInfo info;
    info.Init();
    info.SetFileName(_param.GetPath());

    if(CDirMgr::IsDirExist(&path))
    {
        info.SetSize(0);
        info.SetIsDir(true);
        info.SetFullPath(&path);        
        _ret.SetResult(OK);
    }
    else if(CDirMgr::IsFileExist(&path))
    {
        info.SetSize(CDirMgr::GetFileSize(&path));
        info.SetIsDir(false);
        info.SetFullPath(&path);        
        _ret.SetResult(OK);
    }
    else
    {
        _ret.SetResult(ERROR);
    }
    
    _ret.SetInfo(&info);
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnGetInfo @@*/

/*@@Begin Function OnChangeCurrentDirectory @@*/
status_t CxFileServer::OnChangeCurrentDirectory(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsChangeCurrentDirectory _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsChangeCurrentDirectory _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    LOCAL_MEM(path);
    if(!this->BuildLocalPath(session,_param.GetDir(),&path))
    {
        _ret.SetResult(ERROR);
    }
    else if(!CDirMgr::IsDirExist(&path))
    {
        _ret.SetResult(ERROR);
    }
    else
    {
        _ret.SetResult(OK);
        path.SetSize(0);
        path.Puts(_param.GetDir());
        CDirMgr::ToAbsPath("",&path);
        session->SetCurDir(&path);
    }
        
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnChangeCurrentDirectory @@*/

/*@@Begin Function OnGetCurrentDirectory @@*/
status_t CxFileServer::OnGetCurrentDirectory(CPeerMessage *_message)
{
    ASSERT(_message);
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsGetCurrentDirectory _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    _ret.SetResult(OK);
    _ret.SetDir(session->GetCurDir());
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnGetCurrentDirectory @@*/

/*@@Begin Function OnOpenFile @@*/
status_t CxFileServer::OnOpenFile(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsOpenFile _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsOpenFile _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    LOCAL_MEM(path);
    if(!this->BuildLocalPath(session,_param.GetPath(),&path))
    {
        _ret.SetResult(ERROR);
        _ret.SetHandle(ERROR_FILE_HANDLE);        
    }
    else
    {
        CxOpenedFileMap *map = session->GetOpenedFileMap();
        ASSERT(map);
        FILE_HANDLE handle = map->OpenFile(path.CStr(),_param.GetModeStr());
        if(handle == ERROR_FILE_HANDLE)
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "open local file %s fail.",path.CStr()
            );
        
            _ret.SetResult(ERROR);
            _ret.SetHandle(ERROR_FILE_HANDLE);
        }
        else
        {
            _ret.SetResult(OK);
            _ret.SetHandle(handle);
        }        
    }
    
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnOpenFile @@*/

/*@@Begin Function OnCloseFile @@*/
status_t CxFileServer::OnCloseFile(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsCloseFile _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsCloseFile _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);
    CxOpenedFileMap *map = session->GetOpenedFileMap();
    ASSERT(map);

    if(map->CloseFile(_param.GetHandle()))
    {
        _ret.SetResult(OK);
    }
    else
    {
        _ret.SetResult(ERROR);
    }
    
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnCloseFile @@*/

/*@@Begin Function OnReadFile @@*/
status_t CxFileServer::OnReadFile(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsReadFile _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
        
    CRetVal_XfsReadFile _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);
    CxOpenedFileMap *map = session->GetOpenedFileMap();
    ASSERT(map);

    CFile *file = map->GetByHandle(_param.GetHandle());
    if(!file)
    {
        _ret.SetResult(ERROR);
    }
    else
    {
        fsize_t total_size = file->GetSize();
        fsize_t offset = _param.GetOffset();
        fsize_t need_read = _param.GetSize();
        if(need_read > total_size-offset)
        {
            need_read = total_size-offset;
        }

        if(need_read > MAX_READ_SIZE)
            need_read = MAX_READ_SIZE;

        if(need_read > 0)
        {
            file->Seek(_param.GetOffset());
            _ret.AllocData(need_read);
            int_ptr_t rs = file->Read(_ret.GetData(),need_read);
            _ret.m_data_size = rs; //it's ok to set data_size            
        }   

        _ret.SetResult(OK);     
    }
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnReadFile @@*/

/*@@Begin Function OnWriteFile @@*/
status_t CxFileServer::OnWriteFile(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsWriteFile _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsWriteFile _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);
    CxOpenedFileMap *map = session->GetOpenedFileMap();
    ASSERT(map);

    CFile *file = map->GetByHandle(_param.GetHandle());
    if(!file)
    {
        _ret.SetResult(ERROR);
        _ret.SetWs(0);
    }
    else
    {
        fsize_t total_size = file->GetSize();
        if(_param.GetOffset() > total_size)
        {
            _ret.SetResult(ERROR);
            _ret.SetWs(0);
        }
        else
        {
            file->Seek(_param.GetOffset());
            int_ptr_t ws = file->Write(_param.GetData(),_param.GetDataSize());
            _ret.SetResult(OK);
            _ret.SetWs(ws);
        }        
    }

    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnWriteFile @@*/

/*@@Begin Function OnCreateFolder @@*/
status_t CxFileServer::OnCreateFolder(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsCreateFolder _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsCreateFolder _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    LOCAL_MEM(path);
    if(!this->BuildLocalPath(session,_param.GetPath(),&path))
    {
        _ret.SetResult(ERROR);
    }
    else
    {
        if(CDirMgr::CreateDirSuper(&path))
        {
            _ret.SetResult(OK);
        }
        else
        {
            _ret.SetResult(ERROR);
        }
    }

    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnCreateFolder @@*/

/*@@Begin Function OnDeleteFolder @@*/
status_t CxFileServer::OnDeleteFolder(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsDeleteFolder _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsDeleteFolder _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    LOCAL_MEM(path);
    if(!this->BuildLocalPath(session,_param.GetPath(),&path))
    {
        _ret.SetResult(ERROR);
    }
    else
    {
        if(!CDirMgr::IsDirExist(&path))
        {
            _ret.SetResult(OK);
        }
        else if(CDirMgr::DeleteDir(&path))
        {
            _ret.SetResult(OK);
        }
        else
        {
            _ret.SetResult(ERROR);
        }
    }

    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnDeleteFolder @@*/

/*@@Begin Function OnDeleteFile @@*/
status_t CxFileServer::OnDeleteFile(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_XfsDeleteFile _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_XfsDeleteFile _ret;
    _ret.Init();

    CxFileSession *session = this->GetOrCreateSession(_context.GetFrom());
    ASSERT(session);

    LOCAL_MEM(path);
    if(!this->BuildLocalPath(session,_param.GetPath(),&path))
    {
        _ret.SetResult(ERROR);
    }
    else
    {
        if(!CDirMgr::IsFileExist(&path))
        {
            _ret.SetResult(OK);
        }
        else if(CDirMgr::DeleteFile(&path))
        {
            _ret.SetResult(OK);
        }
        else
        {
            _ret.SetResult(ERROR);
        }
    }

    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnDeleteFile @@*/
/*@@ Insert Function Here @@*/
const char *CxFileServer::GetRootPathStr()
{
    return m_RootPath.CStr();
}

CMem *CxFileServer::GetRootPath()
{
    return &m_RootPath;
}

