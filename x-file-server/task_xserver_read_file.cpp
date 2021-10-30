#include "task_xserver_read_file.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "weak_pointer.h"
#include "linkrpc_xfs_types.h"

enum{
    STEP_NONE = 0,
    STEP_CHECK_SOURCE_FILE,
    STEP_OPEN_SOURCE_FILE,
    STEP_READ_FILE,
};

#define FIRST_STEP STEP_CHECK_SOURCE_FILE

#define MAX_READ_SIZE (8*1024*1024)

CTaskXServerReadFile::CTaskXServerReadFile()
{
    this->InitBasic();
}
CTaskXServerReadFile::~CTaskXServerReadFile()
{
    this->Destroy();
}
status_t CTaskXServerReadFile::InitBasic()
{
    CTask::InitBasic();
/*##Begin InitBasic##*/
    this->m_src_file_name.InitBasic();
    this->m_src_handle = ERROR_FILE_HANDLE;
    this->m_start_offset = 0;
    this->m_read_size = -1;
    this->m_already_read_size = 0;
    this->m_out_file = NULL;
    this->m_out_file_name.InitBasic();
    this->m_file_client.InitBasic();
/*##End InitBasic##*/
    m_tmp_out_file.InitBasic();
    this->m_Step = 0;
    this->m_Callback.InitBasic();
    return OK;
}
status_t CTaskXServerReadFile::Init(CTaskMgr *mgr)
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_src_file_name.Init();
    this->m_out_file_name.Init();
    this->m_file_client.Init();
/*##End Init##*/
    m_tmp_out_file.Init();
    CTask::Init(mgr);
    this->m_Callback.Init();
    return OK;
}
status_t CTaskXServerReadFile::Destroy()
{
    this->m_Callback.Destroy();
/*##Begin Destroy##*/
    this->m_src_file_name.Destroy();
    this->m_out_file_name.Destroy();
    this->m_file_client.Destroy();
/*##End Destroy##*/
    m_tmp_out_file.Destroy();
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

const char * CTaskXServerReadFile::ErrStr(int err)
{
    ERR_STR(ERROR_NONE,"no error");
    ERR_STR(ERROR_NO_SOURCE_FILE,"no source file set");
    ERR_STR(ERROR_NO_OUT_FILE,"no out file set");
    ERR_STR(ERROR_OPEN_SOURCE_FILE_FAIL,"open source file fail.");
    ERR_STR(ERROR_OPEN_OUT_FILE_FAIL,"open out file fail.");
    ERR_STR(ERROR_SOCKET_ERROR,"socket error.");
    ERR_STR(ERROR_READ_FILE_FAIL,"read file fail.");
    return "unknown error";
}

status_t CTaskXServerReadFile::ReportError(int err)
{
    XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
        "CTaskXServerReadFile(%d): exit with error:%s",
        this->GetId(),this->ErrStr(err)
    );
    return OK;
}
status_t CTaskXServerReadFile::Start()
{
    this->Resume();
    GOTO_STATE(0,FIRST_STEP);
    return OK;
}
status_t CTaskXServerReadFile::Stop(int err)
{
    if(this->IsDead())return OK;
    this->Quit();
    m_Callback.SetParamInt(1,err);
    m_Callback.SetParamPointer(2,(void*)ErrStr(err));
    m_Callback.Run(EVENT_STOPPED);
    return OK;
}

CClosure* CTaskXServerReadFile::Callback()
{
    return &this->m_Callback;
}

status_t CTaskXServerReadFile::Run(uint32_t interval)
{
    ASSERT(m_file_client.get());
///////////////
    BEGIN_FSM();
///////////////
    BEGIN_STATE(STEP_CHECK_SOURCE_FILE)
    {
        if(m_src_handle != ERROR_FILE_HANDLE)
        {
            GOTO_STATE(0,STEP_READ_FILE);
        }
        else if(m_src_file_name.StrLen() > 0)
        {
            GOTO_STATE(0,STEP_OPEN_SOURCE_FILE);
        }
        else
        {
            this->Stop(ERROR_NO_SOURCE_FILE);
            return OK;
        }        
    }
    END_STATE(STEP_CHECK_SOURCE_FILE);
///////////////
    BEGIN_STATE(STEP_OPEN_SOURCE_FILE)
    {
        BEGIN_NEW_LINKRPC_CALLBACK(opendone,CRetVal_XfsOpenFile)
        {            
            CLOSURE_PARAM_WEAKPTR(CTaskXServerReadFile*,self,10);    
            self->Resume();

            if(_e != OK)
            {
                self->Stop( CTaskXServerReadFile::ERROR_OPEN_SOURCE_FILE_FAIL);
            }
            else
            {
                if(_ret.GetResult() != OK)
                {
                    self->Stop( CTaskXServerReadFile::ERROR_OPEN_SOURCE_FILE_FAIL);
                }
                else
                {
                    self->m_already_read_size = 0;
                    self->SetSrcHandle(_ret.GetHandle());
                    self->m_Step = STEP_READ_FILE;
                }                
            }
            return OK;
        }
        END_NEW_LINKRPC_CALLBACK(opendone,CRetVal_XfsOpenFile);

        CMem mode("rb");
        CWeakPointer<CTaskXServerReadFile> wp(this);
        opendone->SetParamWeakPointer(10,&wp);        
        m_file_client->OpenFile(&m_src_file_name,&mode,opendone);
        this->m_Step = STEP_NONE;
        this->Suspend();
    }
    END_STATE(STEP_OPEN_SOURCE_FILE);
///////////////
    BEGIN_STATE(STEP_READ_FILE)
    {
        ASSERT(m_src_handle != ERROR_FILE_HANDLE);

        fsize_t need_read = m_read_size - m_already_read_size;
        if(m_read_size < 0)
        {
            need_read = MAX_READ_SIZE;
        }

        if(need_read <= 0)
        {
            return Stop(ERROR_NONE);
        }
        
        if(need_read > MAX_READ_SIZE)
        {
            need_read = MAX_READ_SIZE;   
        }

        BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsReadFile)
        {
            CLOSURE_PARAM_WEAKPTR(CTaskXServerReadFile*,self,10);   
            self->Resume();
            self->m_Step = STEP_READ_FILE; //continue reading
        
            if(_e != OK)
            {
                self->Stop( CTaskXServerReadFile::ERROR_READ_FILE_FAIL);
            }
            else
            {
                if(_ret.GetResult() != OK)
                {
                    self->Stop( CTaskXServerReadFile::ERROR_READ_FILE_FAIL);
                }
                else
                {
                    if(_ret.GetDataSize() > 0)
                    {
                        if(!self->MakeSureOutFile())
                        {
                            return ERROR;
                        }
                        ASSERT(self->m_out_file);                                                        
                        ASSERT(_ret.GetData());
                        int_ptr_t ws = self->m_out_file->Write(_ret.GetData(),_ret.GetDataSize());
                        
                        if(ws > 0)
                        {
                            self->m_already_read_size += ws;
                            if(self->m_already_read_size == self->m_read_size)
                            {
                                self->Stop(ERROR_NONE);
                            }
                        }
                    }
                    else if(self->m_read_size < 0) //unknown size mode
                    {
                        self->Stop(ERROR_NONE);
                    }
                }
            }
            return OK;
        }
        END_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsReadFile);

        CWeakPointer<CTaskXServerReadFile> wp(this);
        done->SetParamWeakPointer(10,&wp);        
         
        m_file_client->ReadFile(m_src_handle,
            m_start_offset+m_already_read_size, 
            need_read,done
        );
        this->Suspend();
        this->m_Step = STEP_NONE;            
        
    }
    END_STATE(STEP_READ_FILE);
///////////////
    END_FSM();
///////////////    
    return OK;
}

/*@@Begin Function GetSrcFileName@@*/
CMem* CTaskXServerReadFile::GetSrcFileName()
{
    return &m_src_file_name;
}
/*@@End  Function GetSrcFileName@@*/

/*@@Begin Function GetSrcFileNameStr@@*/
const char* CTaskXServerReadFile::GetSrcFileNameStr()
{
    return m_src_file_name.CStr();
}
/*@@End  Function GetSrcFileNameStr@@*/

/*@@Begin Function GetSrcHandle@@*/
int CTaskXServerReadFile::GetSrcHandle()
{
    return m_src_handle;
}
/*@@End  Function GetSrcHandle@@*/

/*@@Begin Function GetStartOffset@@*/
fsize_t CTaskXServerReadFile::GetStartOffset()
{
    return m_start_offset;
}
/*@@End  Function GetStartOffset@@*/

/*@@Begin Function GetReadSize@@*/
fsize_t CTaskXServerReadFile::GetReadSize()
{
    return m_read_size;
}
/*@@End  Function GetReadSize@@*/

/*@@Begin Function GetOutFile@@*/
CFileBase* CTaskXServerReadFile::GetOutFile()
{
    return m_out_file;
}
/*@@End  Function GetOutFile@@*/

/*@@Begin Function GetOutFileName@@*/
CMem* CTaskXServerReadFile::GetOutFileName()
{
    return &m_out_file_name;
}
/*@@End  Function GetOutFileName@@*/

/*@@Begin Function GetOutFileNameStr@@*/
const char* CTaskXServerReadFile::GetOutFileNameStr()
{
    return m_out_file_name.CStr();
}
/*@@End  Function GetOutFileNameStr@@*/

/*@@Begin Function SetSrcFileName@@*/
status_t CTaskXServerReadFile::SetSrcFileName(CMem *_src_file_name)
{
    ASSERT(_src_file_name);
    return this->m_src_file_name.Copy(_src_file_name);
}
/*@@End  Function SetSrcFileName@@*/

/*@@Begin Function SetSrcFileName-CStr@@*/
status_t CTaskXServerReadFile::SetSrcFileName(const char *_src_file_name)
{
    CMem tmp(_src_file_name);
    return this->SetSrcFileName(&tmp);
}
/*@@End  Function SetSrcFileName-CStr@@*/

/*@@Begin Function SetSrcHandle@@*/
status_t CTaskXServerReadFile::SetSrcHandle(int _src_handle)
{
    this->m_src_handle = _src_handle;
    return OK;
}
/*@@End  Function SetSrcHandle@@*/

/*@@Begin Function SetStartOffset@@*/
status_t CTaskXServerReadFile::SetStartOffset(fsize_t _start_offset)
{
    this->m_start_offset = _start_offset;
    return OK;
}
/*@@End  Function SetStartOffset@@*/

/*@@Begin Function SetReadSize@@*/
status_t CTaskXServerReadFile::SetReadSize(fsize_t _read_size)
{
    this->m_read_size = _read_size;
    return OK;
}
/*@@End  Function SetReadSize@@*/

/*@@Begin Function SetOutFile@@*/
status_t CTaskXServerReadFile::SetOutFile(CFileBase *_out_file)
{
    this->m_out_file = _out_file;
    return OK;
}
/*@@End  Function SetOutFile@@*/

/*@@Begin Function SetOutFileName@@*/
status_t CTaskXServerReadFile::SetOutFileName(CMem *_out_file_name)
{
    ASSERT(_out_file_name);
    return this->m_out_file_name.Copy(_out_file_name);
}
/*@@End  Function SetOutFileName@@*/

/*@@Begin Function SetOutFileName-CStr@@*/
status_t CTaskXServerReadFile::SetOutFileName(const char *_out_file_name)
{
    CMem tmp(_out_file_name);
    return this->SetOutFileName(&tmp);
}
/*@@End  Function SetOutFileName-CStr@@*/

/*@@Begin Function GetFileClient@@*/
CxFileClient* CTaskXServerReadFile::GetFileClient()
{
    return m_file_client.get();
}
/*@@End  Function GetFileClient@@*/

/*@@Begin Function SetFileClient@@*/
status_t CTaskXServerReadFile::SetFileClient(CxFileClient *_file_client)
{
    ASSERT(_file_client);
    return this->m_file_client.WeakRef(_file_client);
}
/*@@End  Function SetFileClient@@*/

/*@@Begin Function GetAlreadyReadSize@@*/
fsize_t CTaskXServerReadFile::GetAlreadyReadSize()
{
    return m_already_read_size;
}
/*@@End  Function GetAlreadyReadSize@@*/
/*@@ Insert Function Here @@*/

status_t CTaskXServerReadFile::MakeSureOutFile()
{
    if(m_out_file)return OK;
    if(m_out_file_name.StrLen() <= 0)
    {
        this->Stop(ERROR_NO_OUT_FILE);
        return ERROR;
    }

    m_tmp_out_file.CloseFile();
    if(!m_tmp_out_file.OpenFile(m_out_file_name.CStr(),"wb+"))
    {
        this->Stop(ERROR_OPEN_OUT_FILE_FAIL);
        return ERROR;
    }
    m_out_file = &m_tmp_out_file;
    return OK;    
}