#include "task_xserver_list_folder.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "linkrpc_xfs_types.h"
#include "weak_pointer.h"

enum{
    STEP_NONE = 0,
    STEP_ADD_FIRST_DIR,
    STEP_LIST_NEXT_FOLDER,
};

#define FIRST_STEP STEP_ADD_FIRST_DIR

CTaskXServerListFolder::CTaskXServerListFolder()
{
    this->InitBasic();
}
CTaskXServerListFolder::~CTaskXServerListFolder()
{
    this->Destroy();
}
status_t CTaskXServerListFolder::InitBasic()
{
    CTask::InitBasic();
/*##Begin InitBasic##*/
    this->m_folder_name.InitBasic();
    this->m_file_client.InitBasic();
    this->m_pending_queue.InitBasic();
/*##End InitBasic##*/
    this->m_Step = 0;
    this->m_Callback.InitBasic();
    return OK;
}
status_t CTaskXServerListFolder::Init(CTaskMgr *mgr)
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_folder_name.Init();
    this->m_file_client.Init();
    this->m_pending_queue.Init();
/*##End Init##*/
    CTask::Init(mgr);
    this->m_Callback.Init();
    return OK;
}
status_t CTaskXServerListFolder::Destroy()
{
    CTask::Destroy();
    this->m_Callback.Destroy();
/*##Begin Destroy##*/
    this->m_folder_name.Destroy();
    this->m_file_client.Destroy();
    this->m_pending_queue.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}
const char * CTaskXServerListFolder::ErrStr(int err)
{
    ERR_STR(ERROR_LIST_FOLDER_FAIL,"list folder fail.");
    ERR_STR(ERROR_NO_SOURCE_FOLDER,"no source folder");
    ERR_STR(ERROR_NONE,"no error");
    return "unknown error";
}
status_t CTaskXServerListFolder::ReportError(int err)
{
    XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
        "CTaskXServerListFolder(%d): exit with error:%s",
        this->GetId(),this->ErrStr(err)
    );
    return OK;
}
status_t CTaskXServerListFolder::Start()
{
    this->Resume();
    GOTO_STATE(0,FIRST_STEP);
    return OK;
}
status_t CTaskXServerListFolder::Stop(int err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);
    
    m_Callback.SetParamInt(1,err);
    m_Callback.SetParamPointer(2,(void*)ErrStr(err));
    m_Callback.Run(EVENT_STOPPED);
    return OK;
}

CClosure* CTaskXServerListFolder::Callback()
{
    return &this->m_Callback;
}

status_t CTaskXServerListFolder::Run(uint32_t interval)
{
    ASSERT(m_file_client.get());
///////////////    
    BEGIN_FSM();
///////////////
    BEGIN_STATE(STEP_ADD_FIRST_DIR)
    {
        this->AddPendingFolder(&m_folder_name);
        GOTO_STATE(0,STEP_LIST_NEXT_FOLDER);
    }
    END_STATE(STEP_ADD_FIRST_DIR);
///////////////
    BEGIN_STATE(STEP_LIST_NEXT_FOLDER)
    {
        if(m_pending_queue.GetLen() <= 0)
        {
            return Stop(ERROR_NONE);
        }
        else
        {
            CxFileInfo *info = m_pending_queue.GetElem(0);   
            ASSERT(info);

            BEGIN_NEW_LINKRPC_CALLBACK(onlist,CRetVal_XfsListFolder)
            {
                CLOSURE_PARAM_WEAKPTR(CTaskXServerListFolder*,self,10);
                self->m_Step = STEP_LIST_NEXT_FOLDER;
                self->Resume();
            
                if(_e != OK)
                {
                    self->Stop(CTaskXServerListFolder::ERROR_LIST_FOLDER_FAIL);
                }
                else
                {
                    if(_ret.GetResult() != OK)
                    {
                        self->Stop(CTaskXServerListFolder::ERROR_LIST_FOLDER_FAIL);
                    }
                    else
                    {
                        self->OnRecvFolderListResult(_ret.GetList());
                    }
                }                
                return OK;
            }
            END_NEW_LINKRPC_CALLBACK(onlist,CRetVal_XfsListFolder);

            CWeakPointer<CTaskXServerListFolder> wp(this);
            onlist->SetParamWeakPointer(10,&wp);
            
            m_file_client->ListFolder(info->GetFullPath(),onlist);  
            this->m_Step = STEP_NONE;
            this->Suspend();
        }        
    }
    END_STATE(STEP_LIST_NEXT_FOLDER);

///////////////
    END_FSM();
///////////////    
    return OK;
}


/*@@Begin Function GetFolderName@@*/
CMem* CTaskXServerListFolder::GetFolderName()
{
    return &m_folder_name;
}
/*@@End  Function GetFolderName@@*/

/*@@Begin Function GetFolderNameStr@@*/
const char* CTaskXServerListFolder::GetFolderNameStr()
{
    return m_folder_name.CStr();
}
/*@@End  Function GetFolderNameStr@@*/

/*@@Begin Function GetFileClient@@*/
CxFileClient* CTaskXServerListFolder::GetFileClient()
{
    return m_file_client.get();
}
/*@@End  Function GetFileClient@@*/

/*@@Begin Function SetFolderName@@*/
status_t CTaskXServerListFolder::SetFolderName(CMem *_folder_name)
{
    ASSERT(_folder_name);
    return this->m_folder_name.Copy(_folder_name);
}
/*@@End  Function SetFolderName@@*/

/*@@Begin Function SetFolderName-CStr@@*/
status_t CTaskXServerListFolder::SetFolderName(const char *_folder_name)
{
    CMem tmp(_folder_name);
    return this->SetFolderName(&tmp);
}
/*@@End  Function SetFolderName-CStr@@*/

/*@@Begin Function SetFileClient@@*/
status_t CTaskXServerListFolder::SetFileClient(CxFileClient *_file_client)
{
    ASSERT(_file_client);
    return this->m_file_client.WeakRef(_file_client);
}
/*@@End  Function SetFileClient@@*/
/*@@ Insert Function Here @@*/


status_t CTaskXServerListFolder::AddPendingFolder(CMem *path)
{
    CxFileInfo info;
    info.Init();
    info.SetFullPath(path);
    info.SetIsDir(true);
    this->m_pending_queue.Push(&info);
    return OK;
}

status_t CTaskXServerListFolder::OnRecvFolderListResult(CxFileInfoList *list)
{
    ASSERT(list);

    CxFileInfo *root = m_pending_queue.GetElem(0);
    ASSERT(root);
    CMem *root_path = root->GetFullPath();
    ASSERT(root_path);

    LOCAL_MEM(path);
    for(int i = 0; i < list->GetLen(); i++)
    {
        CxFileInfo *info = list->GetElem(i);
        path.SetSize(0);
        path.Puts(root_path);
        if(path.StrLen() > 0 && path.GetLastChar(true) != crt_get_path_splitor())
        {
            path.Putc(crt_get_path_splitor());
        }    
        path.Puts(info->GetFileName());

        if(info->IsDir())
        {
            this->AddPendingFolder(&path);
        }

        m_Callback.SetParamPointer(1,info);
        m_Callback.SetParamPointer(2,&path);
        m_Callback.Run(EVENT_RECV_FILE_INFO);
    }

    m_pending_queue.DelElem(0);
    return OK;
}