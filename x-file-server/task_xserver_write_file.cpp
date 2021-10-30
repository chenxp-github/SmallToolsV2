#include "task_xserver_write_file.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "linkrpc_xfs_types.h"

enum{
    STEP_NONE = 0,
    STEP_CHECK_SOURCE_FILE,
    STEP_OPEN_OUT_FILE,
    STEP_WRITE_FILE,
};

#define MAX_WRITE_SIZE (8*1024*1024)
#define FIRST_STEP STEP_CHECK_SOURCE_FILE

CTaskXServerWriteFile::CTaskXServerWriteFile()
{
    this->InitBasic();
}
CTaskXServerWriteFile::~CTaskXServerWriteFile()
{
    this->Destroy();
}
status_t CTaskXServerWriteFile::InitBasic()
{
    CTask::InitBasic();
/*##Begin InitBasic##*/
    this->m_src_file_name.InitBasic();
    this->m_src_file = NULL;
    this->m_start_offset = 0;
    this->m_write_size = -1;
    this->m_already_write_size = 0;
    this->m_out_handle = 0;
    this->m_out_file_name.InitBasic();
    this->m_file_client.InitBasic();
/*##End InitBasic##*/
    m_tmp_src_file.InitBasic();
    m_tmp_buffer.InitBasic();
    this->m_Step = 0;
    this->m_Callback.InitBasic();
    return OK;
}
status_t CTaskXServerWriteFile::Init(CTaskMgr *mgr)
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_src_file_name.Init();
    this->m_out_file_name.Init();
    this->m_file_client.Init();
/*##End Init##*/
    m_tmp_src_file.Init();
    m_tmp_buffer.Init();
    m_tmp_buffer.Malloc(MAX_WRITE_SIZE);
    CTask::Init(mgr);
    this->m_Callback.Init();
    return OK;
}
status_t CTaskXServerWriteFile::Destroy()
{    
    this->CloseRemoteFile();
    this->m_Callback.Destroy();
/*##Begin Destroy##*/
    this->m_src_file_name.Destroy();
    this->m_out_file_name.Destroy();
    this->m_file_client.Destroy();
/*##End Destroy##*/
    m_tmp_src_file.Destroy();
    m_tmp_buffer.Destroy();
    CTask::Destroy();    
    this->InitBasic();
    return OK;
}
const char * CTaskXServerWriteFile::ErrStr(int err)
{
    ERR_STR(ERROR_NO_SOURCE_FILE,"no source file set");
    ERR_STR(ERROR_OPEN_SOURCE_FILE_FAIL,"open source file fail.");
    ERR_STR(ERROR_OPEN_OUT_FILE_FAIL,"open out file fail.");
    ERR_STR(ERROR_SOCKET_ERROR,"socket error.");
    ERR_STR(ERROR_READ_FILE_FAIL,"read file fail.");    
    ERR_STR(ERROR_WRITE_FILE_FAIL,"write file fail.");    
    ERR_STR(ERROR_NO_OUT_FILE,"no out file name");
    ERR_STR(ERROR_NONE,"no error");
    return "unknown error";
}
status_t CTaskXServerWriteFile::ReportError(int err)
{
    XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
        "CTaskXServerWriteFile(%d): exit with error:%s",
        this->GetId(),this->ErrStr(err)
    );
    return OK;
}
status_t CTaskXServerWriteFile::Start()
{
    this->Resume();
    GOTO_STATE(0,FIRST_STEP);
    return OK;
}
status_t CTaskXServerWriteFile::Stop(int err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->CloseRemoteFile();
    m_tmp_src_file.CloseFile();   
    m_Callback.SetParamInt(1,err);
    m_Callback.SetParamPointer(2,(void*)ErrStr(err));
    m_Callback.Run(EVENT_STOPPED);
    return OK;
}

CClosure* CTaskXServerWriteFile::Callback()
{
    return &this->m_Callback;
}

status_t CTaskXServerWriteFile::Run(uint32_t interval)
{
    ASSERT(m_file_client.get());
///////////////
    BEGIN_FSM();
///////////////
    BEGIN_STATE(STEP_CHECK_SOURCE_FILE)
    {
        if(m_src_file)
        {
            GOTO_STATE(0,STEP_OPEN_OUT_FILE);
        }
        else if(m_src_file_name.StrLen() > 0)
        {
            m_tmp_src_file.CloseFile();
            if(!m_tmp_src_file.OpenFile(m_src_file_name.CStr(),"rb"))
            {
                return this->Stop(ERROR_OPEN_SOURCE_FILE_FAIL);
            }
            m_src_file = &m_tmp_src_file;
            GOTO_STATE(0,STEP_OPEN_OUT_FILE);
        }
        else
        {
            this->Stop(ERROR_NO_SOURCE_FILE);
        }        
    }
    END_STATE(STEP_CHECK_SOURCE_FILE);
///////////////
    BEGIN_STATE(STEP_OPEN_OUT_FILE)
    {
        if(m_out_file_name.StrLen() < 0)
        {
            return Stop(ERROR_NO_OUT_FILE);
        }
        else
        {
            BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsOpenFile)
            {
                CLOSURE_PARAM_WEAKPTR(CTaskXServerWriteFile*,self,10);
                self->Resume();

                if(_e != OK)
                {
                    self->Stop(CTaskXServerWriteFile::ERROR_OPEN_OUT_FILE_FAIL);
                }
                else
                {
                    if(_ret.GetResult() != OK)
                    {
                        self->Stop(CTaskXServerWriteFile::ERROR_OPEN_OUT_FILE_FAIL);
                    }
                    else
                    {
                        self->m_out_handle = _ret.GetHandle();
                        self->m_Step = STEP_WRITE_FILE;
                    }                    
                }                
                return OK;
            }
            END_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsOpenFile);

            CMem mode("wb+");
            CWeakPointer<CTaskXServerWriteFile> wp(this);
            done->SetParamWeakPointer(10,&wp);    
            m_file_client->OpenFile(&m_out_file_name,&mode,done);           
            this->Suspend();
            m_Step = STEP_NONE;
        }
    }
    END_STATE(STEP_OPEN_OUT_FILE);
///////////////
    BEGIN_STATE(STEP_WRITE_FILE)
    {
        ASSERT(m_src_file);        
        fsize_t max_need_write = m_src_file->GetSize() - m_start_offset - m_already_write_size;
        fsize_t need_write = max_need_write;

        if(m_write_size > 0)
        {
            if(need_write > m_write_size - m_already_write_size)
            {
                need_write = m_write_size - m_already_write_size;
            }                    
            
            if(need_write <= 0)
            {
                return Stop(ERROR_NONE);
            }

            if(need_write > MAX_WRITE_SIZE)
            {
                need_write = MAX_WRITE_SIZE;
            }
        }
        else
        {
            need_write = MAX_WRITE_SIZE;
        }
        
        m_src_file->Seek(m_start_offset + m_already_write_size);
        if(m_src_file->IsEnd())
        {
            return Stop(ERROR_NONE);
        }

        int_ptr_t rs = m_src_file->Read(m_tmp_buffer.GetRawBuf(),need_write);
        if(rs <= 0)
        {
            return Stop(ERROR_READ_FILE_FAIL);
        }
        else
        {
            BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsWriteFile)
            {
                CLOSURE_PARAM_WEAKPTR(CTaskXServerWriteFile*,self,10);
                self->Resume();
                self->m_Step = STEP_WRITE_FILE;
            
                if(_e != OK)
                {
                    return self->Stop(CTaskXServerWriteFile::ERROR_WRITE_FILE_FAIL);
                }
                else
                {
                    if(_ret.GetResult() != OK)
                    {
                        return self->Stop(CTaskXServerWriteFile::ERROR_WRITE_FILE_FAIL);
                    }
                    else
                    {
                        if(_ret.GetWs() <= 0)
                        {
                            return self->Stop(CTaskXServerWriteFile::ERROR_WRITE_FILE_FAIL);
                        }
                        else
                        {
                            self->m_already_write_size += _ret.GetWs();
                        }                        
                    }                    
                }
                return OK;
            }
            END_NEW_LINKRPC_CALLBACK(done,CRetVal_XfsWriteFile);

            CWeakPointer<CTaskXServerWriteFile> wp(this);
            done->SetParamWeakPointer(10,&wp);    

            m_file_client->WriteFile(
                m_out_handle,
                m_already_write_size,
                (uint8_t*)m_tmp_buffer.GetRawBuf(),
                rs,done
            );

            m_Step = STEP_NONE;
            this->Suspend();
        }        
    }
    END_STATE(STEP_WRITE_FILE);
///////////////
    END_FSM();
///////////////    
    return OK;
}


/*@@Begin Function GetSrcFileName@@*/
CMem* CTaskXServerWriteFile::GetSrcFileName()
{
    return &m_src_file_name;
}
/*@@End  Function GetSrcFileName@@*/

/*@@Begin Function GetSrcFileNameStr@@*/
const char* CTaskXServerWriteFile::GetSrcFileNameStr()
{
    return m_src_file_name.CStr();
}
/*@@End  Function GetSrcFileNameStr@@*/

/*@@Begin Function GetSrcFile@@*/
CFileBase* CTaskXServerWriteFile::GetSrcFile()
{
    return m_src_file;
}
/*@@End  Function GetSrcFile@@*/

/*@@Begin Function GetStartOffset@@*/
fsize_t CTaskXServerWriteFile::GetStartOffset()
{
    return m_start_offset;
}
/*@@End  Function GetStartOffset@@*/

/*@@Begin Function GetWriteSize@@*/
fsize_t CTaskXServerWriteFile::GetWriteSize()
{
    return m_write_size;
}
/*@@End  Function GetWriteSize@@*/

/*@@Begin Function GetAlreadyWriteSize@@*/
fsize_t CTaskXServerWriteFile::GetAlreadyWriteSize()
{
    return m_already_write_size;
}
/*@@End  Function GetAlreadyWriteSize@@*/

/*@@Begin Function GetOutHandle@@*/
int CTaskXServerWriteFile::GetOutHandle()
{
    return m_out_handle;
}
/*@@End  Function GetOutHandle@@*/

/*@@Begin Function GetOutFileName@@*/
CMem* CTaskXServerWriteFile::GetOutFileName()
{
    return &m_out_file_name;
}
/*@@End  Function GetOutFileName@@*/

/*@@Begin Function GetOutFileNameStr@@*/
const char* CTaskXServerWriteFile::GetOutFileNameStr()
{
    return m_out_file_name.CStr();
}
/*@@End  Function GetOutFileNameStr@@*/

/*@@Begin Function GetFileClient@@*/
CxFileClient* CTaskXServerWriteFile::GetFileClient()
{
    return m_file_client.get();
}
/*@@End  Function GetFileClient@@*/

/*@@Begin Function SetSrcFileName@@*/
status_t CTaskXServerWriteFile::SetSrcFileName(CMem *_src_file_name)
{
    ASSERT(_src_file_name);
    return this->m_src_file_name.Copy(_src_file_name);
}
/*@@End  Function SetSrcFileName@@*/

/*@@Begin Function SetSrcFileName-CStr@@*/
status_t CTaskXServerWriteFile::SetSrcFileName(const char *_src_file_name)
{
    CMem tmp(_src_file_name);
    return this->SetSrcFileName(&tmp);
}
/*@@End  Function SetSrcFileName-CStr@@*/

/*@@Begin Function SetSrcFile@@*/
status_t CTaskXServerWriteFile::SetSrcFile(CFileBase *_src_file)
{
    this->m_src_file = _src_file;
    return OK;
}
/*@@End  Function SetSrcFile@@*/

/*@@Begin Function SetStartOffset@@*/
status_t CTaskXServerWriteFile::SetStartOffset(fsize_t _start_offset)
{
    this->m_start_offset = _start_offset;
    return OK;
}
/*@@End  Function SetStartOffset@@*/

/*@@Begin Function SetWriteSize@@*/
status_t CTaskXServerWriteFile::SetWriteSize(fsize_t _write_size)
{
    this->m_write_size = _write_size;
    return OK;
}
/*@@End  Function SetWriteSize@@*/

/*@@Begin Function SetOutHandle@@*/
status_t CTaskXServerWriteFile::SetOutHandle(int _out_handle)
{
    this->m_out_handle = _out_handle;
    return OK;
}
/*@@End  Function SetOutHandle@@*/

/*@@Begin Function SetOutFileName@@*/
status_t CTaskXServerWriteFile::SetOutFileName(CMem *_out_file_name)
{
    ASSERT(_out_file_name);
    return this->m_out_file_name.Copy(_out_file_name);
}
/*@@End  Function SetOutFileName@@*/

/*@@Begin Function SetOutFileName-CStr@@*/
status_t CTaskXServerWriteFile::SetOutFileName(const char *_out_file_name)
{
    CMem tmp(_out_file_name);
    return this->SetOutFileName(&tmp);
}
/*@@End  Function SetOutFileName-CStr@@*/

/*@@Begin Function SetFileClient@@*/
status_t CTaskXServerWriteFile::SetFileClient(CxFileClient *_file_client)
{
    ASSERT(_file_client);
    return this->m_file_client.WeakRef(_file_client);
}
/*@@End  Function SetFileClient@@*/
/*@@ Insert Function Here @@*/


status_t CTaskXServerWriteFile::CloseRemoteFile()
{
    if(m_out_handle != ERROR_FILE_HANDLE)
    {
        if(m_file_client.get())
        {
            m_file_client->CloseFile(m_out_handle,NULL);
            m_out_handle = ERROR_FILE_HANDLE;
        }
    }
    return OK;
}