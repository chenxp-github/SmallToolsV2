#include "task_xserver_sync_folder.h"
#include "task_xserver_write_file.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "x_folder_differ.h"
#include "linkrpc_xfs_types.h"
#include "weak_pointer.h"
#include "dirmgr.h"

enum{
    STEP_NONE = 0,
    STEP_CHECK_CREATE_LIST,
    STEP_CREATE_FOLDER,
    STEP_UPLOAD_FILE,
    STEP_CHECK_DELETE_LIST,    
    STEP_DELETE_FOLDER,
    STEP_DELETE_FILE,
};

#define FIRST_STEP STEP_CHECK_CREATE_LIST

#define SNAPSHOT_NAME "_snapshot_.bin"

CTaskXServerSyncFolder::CTaskXServerSyncFolder()
{
    this->InitBasic();
}
CTaskXServerSyncFolder::~CTaskXServerSyncFolder()
{
    this->Destroy();
}
status_t CTaskXServerSyncFolder::InitBasic()
{
    CTask::InitBasic();
/*##Begin InitBasic##*/
    this->m_local_folder.InitBasic();
    this->m_remote_folder.InitBasic();
    this->m_current_list.InitBasic();
    this->m_create_list.InitBasic();
    this->m_delete_list.InitBasic();
    this->m_file_client.InitBasic();
    this->m_snapshot_name_prefix.InitBasic();
/*##End InitBasic##*/
    this->m_Step = 0;
    this->m_Callback.InitBasic();
    m_task_upload = 0;
    return OK;
}
status_t CTaskXServerSyncFolder::Init(CTaskMgr *mgr)
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_local_folder.Init();
    this->m_remote_folder.Init();
    this->m_current_list.Init();
    this->m_create_list.Init();
    this->m_delete_list.Init();
    this->m_file_client.Init();
    this->m_snapshot_name_prefix.Init();
/*##End Init##*/
    CTask::Init(mgr);
    this->m_Callback.Init();
    return OK;
}
status_t CTaskXServerSyncFolder::Destroy()
{
    QuitTask(&m_task_upload);    
    this->m_Callback.Destroy();
/*##Begin Destroy##*/
    this->m_local_folder.Destroy();
    this->m_remote_folder.Destroy();
    this->m_current_list.Destroy();
    this->m_create_list.Destroy();
    this->m_delete_list.Destroy();
    this->m_file_client.Destroy();
    this->m_snapshot_name_prefix.Destroy();
/*##End Destroy##*/
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
const char * CTaskXServerSyncFolder::ErrStr(int err)
{
    ERR_STR(ERROR_CREATE_DIR_FAIL,"create dir fail");
    ERR_STR(ERROR_OPEN_FILE_FAIL,"open file fail");
    ERR_STR(ERROR_CLOSE_FILE_FAIL,"close file fail");
    ERR_STR(ERROR_DELETE_FILE_FAIL,"delete file fail");
    ERR_STR(ERROR_DELETE_DIR_FAIL,"delete dir fail");
    ERR_STR(ERROR_UPLOAD_FILE_FAIL,"upload file fail");
    ERR_STR(ERROR_USER_CANCEL,"user cancel");
    ERR_STR(ERROR_NONE,"no error");
    return "unknown error";
}
status_t CTaskXServerSyncFolder::ReportError(int err)
{
    XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
        "CTaskXServerSyncFolder(%d): exit with error:%s",
        this->GetId(),this->ErrStr(err)
    );
    return OK;
}
status_t CTaskXServerSyncFolder::Start()
{
    this->DiffLocalFolder();
    this->Resume();
    GOTO_STATE(0,FIRST_STEP);
    return OK;
}
status_t CTaskXServerSyncFolder::Stop(int err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);
    
    if(err == ERROR_NONE)
    {
        LOCAL_MEM(name);
        this->GetSnapshotFullPath(&name);
        CxFolderDiffer diff;
        diff.Init();
        diff.SaveFolderSnapshot(&m_current_list,name.CStr());
    }

    m_Callback.SetParamInt(1,err);
    m_Callback.SetParamPointer(2,(void*)ErrStr(err));
    m_Callback.Run(EVENT_STOPPED);
    return OK;
}

CClosure* CTaskXServerSyncFolder::Callback()
{
    return &this->m_Callback;
}

status_t CTaskXServerSyncFolder::Run(uint32_t interval)
{
    ASSERT(m_file_client.get());
///////////////    
    BEGIN_FSM();
///////////////
    BEGIN_STATE(STEP_CHECK_CREATE_LIST)
    {
        if(m_create_list.GetLen() <= 0)
        {
            GOTO_STATE(0,STEP_CHECK_DELETE_LIST);
        }
        else
        {
            CxFileInfo *info = m_create_list.GetElem(0);
            ASSERT(info);

            if(info->IsDir())
            {
                ASSERT(info->GetOperation() == CxFileInfo::OPERATION_CREATE);
                GOTO_STATE(0,STEP_CREATE_FOLDER);
            }
            else
            {
                GOTO_STATE(0,STEP_UPLOAD_FILE);
            }
        }
    }
    END_STATE(STEP_CHECK_CREATE_LIST);
///////////////
    BEGIN_STATE(STEP_CREATE_FOLDER)
    {
        BEGIN_NEW_LINKRPC_CALLBACK(onmkdir,CRetVal_XfsCreateFolder)
        {
            CLOSURE_PARAM_WEAKPTR(CTaskXServerSyncFolder*,self,10);
            self->Resume();

            if(_e != OK)
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"fail: timeout");
                self->Stop(CTaskXServerSyncFolder::ERROR_CREATE_DIR_FAIL);
            }
            else
            {
                if(_ret.GetResult() != OK)
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"fail");
                    self->Stop(CTaskXServerSyncFolder::ERROR_CREATE_DIR_FAIL);
                }
                else
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"ok");
                    self->m_Step = STEP_CHECK_CREATE_LIST;
                    self->m_create_list.DelElem(0);
                }
            }
            return OK;
        }
        END_NEW_LINKRPC_CALLBACK(onmkdir,CRetVal_XfsCreateFolder);

        CWeakPointer<CTaskXServerSyncFolder> wp(this);
        onmkdir->SetParamWeakPointer(10,&wp);

        CxFileInfo *info = m_create_list.GetElem(0);
        ASSERT(info);        

        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
            "creating folder %s...",info->GetFullPathStr()
        );

        m_file_client->CreateFolder(info->GetFullPath(),onmkdir);
        this->Suspend();
        m_Step = STEP_NONE;
    }
    END_STATE(STEP_CREATE_FOLDER);
///////////////
    BEGIN_STATE(STEP_UPLOAD_FILE)
    {
        ASSERT(!IsTask(m_task_upload));
        
        BEGIN_CLOSURE_FUNC(done)
        {
            CLOSURE_PARAM_INT(err,1);
            CLOSURE_PARAM_PTR(char*,errStr,2);

            CLOSURE_PARAM_WEAKPTR(CTaskXServerSyncFolder*,self,10);
            self->Resume();
            
            if(err != CTaskXServerSyncFolder::ERROR_NONE)
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"fail: %s",errStr);
                self->Stop(CTaskXServerSyncFolder::ERROR_UPLOAD_FILE_FAIL);
            }
            else
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"ok");
                self->m_Step = STEP_CHECK_CREATE_LIST;
                self->m_create_list.DelElem(0);
            }
            return OK;
        }
        END_CLOSURE_FUNC(done);


        CxFileInfo *info = m_create_list.GetElem(0);
        ASSERT(info);  

        CTaskXServerWriteFile *pt;
        NEW(pt,CTaskXServerWriteFile);
        pt->Init(GetTaskMgr());
        
        LOCAL_MEM(mem);
        mem.Puts(&m_local_folder);
        mem.Putc(crt_get_path_splitor());
        mem.Puts(info->GetFullPath());

        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
            "uploading file %s...",info->GetFullPathStr()
        );

        CWeakPointer<CTaskXServerSyncFolder> wp(this);
        pt->Callback()->SetParamWeakPointer(10,&wp);

        pt->SetSrcFileName(&mem);
        pt->SetOutFileName(info->GetFullPath());
        pt->SetFileClient(m_file_client.get());      
        pt->Callback()->SetFunc(done);
        pt->Start();
        m_task_upload = pt->GetId();  
        
        m_Step = STEP_NONE;
        this->Suspend();
    }
    END_STATE(STEP_UPLOAD_FILE);
///////////////
    BEGIN_STATE(STEP_CHECK_DELETE_LIST)
    {
        if(m_delete_list.GetLen() <= 0)
        {
            this->Stop(ERROR_NONE);
        }
        else
        {
            CxFileInfo *info = m_delete_list.GetElem(0);
            ASSERT(info);

            if(info->IsDir())
            {
                GOTO_STATE(0,STEP_DELETE_FOLDER);
            }
            else
            {                
                GOTO_STATE(0,STEP_DELETE_FILE);
            }
        }
    }
    END_STATE(STEP_CHECK_DELETE_LIST);
///////////////

    BEGIN_STATE(STEP_DELETE_FOLDER)
    {
        CxFileInfo *info = m_delete_list.GetElem(0);
        ASSERT(info);
        ASSERT(info->IsDir());

        BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsDeleteFolder)
        {
            CLOSURE_PARAM_WEAKPTR(CTaskXServerSyncFolder*,self,10);
            self->Resume();

            if(_e != OK)
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"fail: timeout");
                self->Stop(CTaskXServerSyncFolder::ERROR_DELETE_DIR_FAIL);
            }
            else
            {
                if(_ret.GetResult() != OK)
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"fail");                  
                }
                else
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"ok");
                    self->m_Step = STEP_CHECK_DELETE_LIST;
                    self->m_delete_list.DelElem(0);
                }
            }
            return OK;
        }
        END_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsDeleteFolder);

        CWeakPointer<CTaskXServerSyncFolder> wp(this);
        done->SetParamWeakPointer(10,&wp);

        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
            "deleting folder %s...",info->GetFullPathStr()
        );

        m_file_client->DeleteFolder(info->GetFullPath(),done);
        this->Suspend();
        m_Step = STEP_NONE;
    }
    END_STATE(STEP_DELETE_FOLDER);
///////////////
    BEGIN_STATE(STEP_DELETE_FILE)
    {
        CxFileInfo *info = m_delete_list.GetElem(0);
        ASSERT(info);
        ASSERT(!info->IsDir());

        BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsDeleteFile)
        {
            CLOSURE_PARAM_WEAKPTR(CTaskXServerSyncFolder*,self,10);
            self->Resume();

            if(_e != OK)
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"fail: timeout");
                self->Stop(CTaskXServerSyncFolder::ERROR_DELETE_DIR_FAIL);
            }
            else
            {
                if(_ret.GetResult() != OK)
                {
                     XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"fail");
                    self->Stop(CTaskXServerSyncFolder::ERROR_DELETE_DIR_FAIL);
                }
                else
                {
                     XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,"ok");
                    self->m_Step = STEP_CHECK_DELETE_LIST;
                    self->m_delete_list.DelElem(0);
                }
            }
            return OK;
        }
        END_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsDeleteFile);

        CWeakPointer<CTaskXServerSyncFolder> wp(this);
        done->SetParamWeakPointer(10,&wp);

        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
            "deleting file %s...",info->GetFullPathStr()
        );

        m_file_client->DeleteFile(info->GetFullPath(),done);
        this->Suspend();
        m_Step = STEP_NONE;
    }
    END_STATE(STEP_DELETE_FILE);
///////////////
    END_FSM();
///////////////
    return OK;
}


/*@@Begin Function GetLocalFolder@@*/
CMem* CTaskXServerSyncFolder::GetLocalFolder()
{
    return &m_local_folder;
}
/*@@End  Function GetLocalFolder@@*/

/*@@Begin Function GetLocalFolderStr@@*/
const char* CTaskXServerSyncFolder::GetLocalFolderStr()
{
    return m_local_folder.CStr();
}
/*@@End  Function GetLocalFolderStr@@*/

/*@@Begin Function GetRemoteFolder@@*/
CMem* CTaskXServerSyncFolder::GetRemoteFolder()
{
    return &m_remote_folder;
}
/*@@End  Function GetRemoteFolder@@*/

/*@@Begin Function GetRemoteFolderStr@@*/
const char* CTaskXServerSyncFolder::GetRemoteFolderStr()
{
    return m_remote_folder.CStr();
}
/*@@End  Function GetRemoteFolderStr@@*/

/*@@Begin Function SetLocalFolder@@*/
status_t CTaskXServerSyncFolder::SetLocalFolder(CMem *_local_folder)
{
    ASSERT(_local_folder);
    return this->m_local_folder.Copy(_local_folder);
}
/*@@End  Function SetLocalFolder@@*/

/*@@Begin Function SetLocalFolder-CStr@@*/
status_t CTaskXServerSyncFolder::SetLocalFolder(const char *_local_folder)
{
    CMem tmp(_local_folder);
    return this->SetLocalFolder(&tmp);
}
/*@@End  Function SetLocalFolder-CStr@@*/

/*@@Begin Function SetRemoteFolder@@*/
status_t CTaskXServerSyncFolder::SetRemoteFolder(CMem *_remote_folder)
{
    ASSERT(_remote_folder);
    return this->m_remote_folder.Copy(_remote_folder);
}
/*@@End  Function SetRemoteFolder@@*/

/*@@Begin Function SetRemoteFolder-CStr@@*/
status_t CTaskXServerSyncFolder::SetRemoteFolder(const char *_remote_folder)
{
    CMem tmp(_remote_folder);
    return this->SetRemoteFolder(&tmp);
}
/*@@End  Function SetRemoteFolder-CStr@@*/

/*@@Begin Function GetFileClient@@*/
CxFileClient* CTaskXServerSyncFolder::GetFileClient()
{
    return m_file_client.get();
}
/*@@End  Function GetFileClient@@*/

/*@@Begin Function SetFileClient@@*/
status_t CTaskXServerSyncFolder::SetFileClient(CxFileClient *_file_client)
{
    ASSERT(_file_client);
    return this->m_file_client.WeakRef(_file_client);
}
/*@@End  Function SetFileClient@@*/


/*@@Begin Function GetSnapshotNamePrefix@@*/
CMem* CTaskXServerSyncFolder::GetSnapshotNamePrefix()
{
    return &m_snapshot_name_prefix;
}
/*@@End  Function GetSnapshotNamePrefix@@*/

/*@@Begin Function GetSnapshotNamePrefixStr@@*/
const char* CTaskXServerSyncFolder::GetSnapshotNamePrefixStr()
{
    return m_snapshot_name_prefix.CStr();
}
/*@@End  Function GetSnapshotNamePrefixStr@@*/

/*@@Begin Function SetSnapshotNamePrefix@@*/

static bool is_valid_char(char ch)
{
    return (  
         (ch >='a' && ch <='z')
      || (ch >='A' && ch <='Z')
      || (ch >='0' && ch <='9')
      || ch == '_'
    );
}

status_t CTaskXServerSyncFolder::SetSnapshotNamePrefix(CMem *_snapshot_name_prefix)
{
    ASSERT(_snapshot_name_prefix);
    this->m_snapshot_name_prefix.Copy(_snapshot_name_prefix);

    char *p = (char*)m_snapshot_name_prefix.GetRawBuf();
    if(p)
    {
        for(int i = 0; i < m_snapshot_name_prefix.StrLen(); i++)
        {   
            if(!is_valid_char(p[i]))
            {
                p[i] = '_';
            }
        }
    }
    return OK;
}
/*@@End  Function SetSnapshotNamePrefix@@*/

/*@@Begin Function SetSnapshotNamePrefix-CStr@@*/
status_t CTaskXServerSyncFolder::SetSnapshotNamePrefix(const char *_snapshot_name_prefix)
{
    CMem tmp(_snapshot_name_prefix);
    return this->SetSnapshotNamePrefix(&tmp);
}
/*@@End  Function SetSnapshotNamePrefix-CStr@@*/
/*@@ Insert Function Here @@*/

status_t CTaskXServerSyncFolder::GetSnapshotFullPath(CMem *out)
{
    ASSERT(out);
    out->Puts(&m_local_folder);
    out->Putc(crt_get_path_splitor());
    out->Puts(&m_snapshot_name_prefix);
    out->Puts(SNAPSHOT_NAME);
    return OK;
}

status_t CTaskXServerSyncFolder::DiffLocalFolder()
{
    ASSERT(m_local_folder.StrLen() > 0);

    CxFolderDiffer diff;
    diff.Init();

    LOCAL_MEM(name);
    this->GetSnapshotFullPath(&name);

    CxFileInfoList old_list;
    old_list.Init();
    diff.LoadFolderSnapshot(name.CStr(),&old_list);

    m_current_list.Clear();
    diff.GetFolderInfos(m_local_folder.CStr(),&m_current_list);

    m_create_list.Clear();
    m_delete_list.Clear();

    diff.CompTwoFolder(&m_current_list,&old_list,&m_create_list,&m_delete_list);

    for(int i = 0; i < m_create_list.GetLen(); i++)
    {
        CxFileInfo *info = m_create_list.GetElem(i);
        ASSERT(info);

        if(info->GetFullPath()->EndWith(SNAPSHOT_NAME,1,1))
        {
            m_create_list.DelElem(i);
            i--;
        }
    }
    return OK;
}

status_t CTaskXServerSyncFolder::ResetLocalFolder()
{
    LOCAL_MEM(mem);
    this->GetSnapshotFullPath(&mem);
    return CDirMgr::DeleteFile(&mem);
}

status_t CTaskXServerSyncFolder::GetCurrentProgress(CxFileInfo *progress)
{
    ASSERT(progress);

    if(m_create_list.GetLen() > 0)
    {
        CxFileInfo *first = m_create_list.GetElem(0);
        ASSERT(first);
        progress->Copy(first);
        CTaskXServerWriteFile *pt = (CTaskXServerWriteFile*)GetTask(m_task_upload);
        if(pt)
        {
            progress->SetCurrentSize(pt->GetAlreadyWriteSize());
        }  
    }
    else if(m_delete_list.GetLen() > 0)
    {
        CxFileInfo *first = m_delete_list.GetElem(0);
        ASSERT(first);
        progress->Copy(first);
    }
    return OK;
}