#include "tasklinkrpc.h"
#include "syslog.h"
#include "mem_tool.h"

#define STEP_RETRY          1
#define STEP_GET_SOCKET     2
#define STEP_GET_SOCKET_OK  3

CTaskLinkRpc::CTaskLinkRpc()
{
    this->InitBasic();
}
CTaskLinkRpc::~CTaskLinkRpc()
{
    this->Destroy();
}
status_t CTaskLinkRpc::InitBasic()
{
    CTask::InitBasic();
    this->mTaskReader = 0;
    this->mTaskWriter = 0;
    this->mSocket = NULL;
    this->mReader = NULL;
    this->mWriter = NULL;
    this->iDataBuf = NULL;
    this->iHeaderBuf = NULL;
    this->mStep = 0;
    this->mRetries = 0;
    this->mMaxRetries = 1;
    this->mSocketRwer = NULL;
    return OK;
}

static status_t on_socketrwer_event(CClosure *closure)
{
    CLOSURE_PARAM_INT(event,0);
    if(event == CSocketReaderWriter::EVENT_ERROR)
    {
        CLOSURE_PARAM_PTR(const char*,err_str,2);
        CLOSURE_PARAM_PTR(CTaskLinkRpc*,self,10);
        ASSERT(self);
        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
            "CTaskLinkRpc(%d) socket error: %s",self->GetId(),err_str
        );
        self->Retry(CTaskLinkRpc::ERROR_SOCKET_ERROR);
        self->OnSocketError();
    }

    return OK;
}
status_t CTaskLinkRpc::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);

    NEW(this->mSocket,CSocket);
    this->mSocket->Init();

    NEW(this->mSocketRwer,CSocketReaderWriter);
    this->mSocketRwer->Init(mgr);
    this->mSocketRwer->SetSocket(this->mSocket);

    CClosure *callback = this->mSocketRwer->Callback();
    callback->SetFunc(on_socketrwer_event);
    callback->SetParamPointer(10,this);

    return OK;
}
status_t CTaskLinkRpc::Destroy()
{
    QuitTask(&this->mTaskReader);
    QuitTask(&this->mTaskWriter);
    DEL(this->mSocketRwer);
    DEL(this->mSocket);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

static int on_reader_event(CClosure *closure)
{
    CLOSURE_PARAM_INT(event,0);
    CLOSURE_PARAM_PTR(CTaskLinkRpc*,self,10);

    ASSERT(self && self->IsAlive());

    if(event == CTaskLinkRpcReader::EVENT_GOT_HEADER)
    {
        CLOSURE_PARAM_PTR(LINKRPC_HEADER*,header,1);
        CLOSURE_PARAM_PTR(CMem*,data,2);

        ASSERT(data);
        if(header->cmd != LINKRPC_CMD_HEARTBEAT)
        {
            self->OnGotPackageHeader(header,data);
        }
    }
    else if(event == CTaskLinkRpcReader::EVENT_GOT_DATA)
    {
        CLOSURE_PARAM_PTR(LINKRPC_HEADER*,header,1);
        CLOSURE_PARAM_PTR(CMem*,header_data,2);
        CLOSURE_PARAM_PTR(CFileBase*,data,3);

        ASSERT(data && header);
        if(header->cmd != LINKRPC_CMD_HEARTBEAT)
        {
            self->OnGotPackageData(header,header_data,data);
        }
    }

    else if(event == CTaskLinkRpcReader::EVENT_CAN_READ_NEXT_PACKAGE)
    {
        return self->CanReadNextPackage();
    }
    else if(event == CTaskLinkRpcReader::EVENT_STOP)
    {
        self->Retry(CTaskLinkRpc::ERROR_LINKRPC_READER_STOPPED);
    }

    return OK;
}

status_t CTaskLinkRpc::CreateReader()
{
    ASSERT(!IsTask(this->mTaskReader));

    CTaskLinkRpcReader *reader;
    NEW(reader,CTaskLinkRpcReader);
    reader->Init(this->GetTaskMgr());
    reader->iSocketRwer = this->mSocketRwer;
    reader->Callback()->SetFunc(on_reader_event);
    reader->Callback()->SetParamPointer(10,this);
    reader->SetHeaderBuf(this->iHeaderBuf);
    reader->SetDataBuf(this->iDataBuf);
    reader->SetParentTask(this);
    reader->Start();

    this->mTaskReader = reader->GetId();
    this->mReader = reader;

    return OK;
}

static int on_writer_event(CClosure *closure)
{
    CLOSURE_PARAM_INT(event,0);
    CLOSURE_PARAM_PTR(CTaskLinkRpc*,self,10);

    ASSERT(self && self->IsAlive());

    if(event == CTaskLinkRpcWriter::EVENT_GET_NEXT_PACKAGE)
    {
        return self->OnPreparePackageToSend();
    }
    else if(event == CTaskLinkRpcWriter::EVENT_PACKAGE_SEND_OK)
    {
        self->OnPackageSendOk();
    }
    else if(event == CTaskLinkRpcWriter::EVENT_STOP)
    {
        self->Retry(CTaskLinkRpc::ERROR_LINKRPC_WRITER_STOPPED);
    }
    return OK;
}

status_t CTaskLinkRpc::CreateWriter()
{
    ASSERT(!IsTask(this->mTaskWriter));

    CTaskLinkRpcWriter *writer;
    NEW(writer,CTaskLinkRpcWriter);
    writer->Init(this->GetTaskMgr());
    writer->iSocketRwer = this->mSocketRwer;
    writer->Callback()->SetFunc(on_writer_event);
    writer->Callback()->SetParamPointer(10,this);
    writer->SetParentTask(this);
    writer->Start();
    this->mTaskWriter = writer->GetId();
    this->mWriter = writer;
    return OK;
}

status_t CTaskLinkRpc::StartSending()
{
    return this->ResumeTask(this->mTaskWriter);
}

status_t CTaskLinkRpc::SetHeaderBuf(CMem *iBuf)
{
    this->iHeaderBuf = iBuf;
    if(this->mReader)
        mReader->SetHeaderBuf(iBuf);
    return OK;
}

status_t CTaskLinkRpc::SetDataBuf(CFileBase *iBuf)
{
    this->iDataBuf = iBuf;
    if(this->mReader)
        mReader->SetDataBuf(iBuf);
    return OK;
}
status_t CTaskLinkRpc::SetSocket(int32_t fd)
{
    this->mSocket->AttachSocketFd(fd);
    this->mSocket->SetBlocking(0);
    return OK;
}

status_t CTaskLinkRpc::TransferSocket(CSocket *from)
{
    return this->mSocket->TransferSocketFd(from);
}

status_t CTaskLinkRpc::StartReaderAndWriter()
{
    ASSERT(!this->IsAlive());
    ASSERT(this->mSocket->IsConnected());
    this->CreateReader();
    this->CreateWriter();
    return OK;
}
status_t CTaskLinkRpc::SetTimeout(int to)
{
    this->mSocketRwer->SetTimeout(to);
    return OK;
}

status_t CTaskLinkRpc::Stop(int err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);
    return OK;
}

bool CTaskLinkRpc::IsAlive()
{
    return (this->mReader!=NULL && this->mWriter!=NULL);
}

status_t CTaskLinkRpc::SendRequest(CFileBase *header, CFileBase *data)
{
    ASSERT(this->mWriter);
    return this->mWriter->SendRequest(header,data);
}

status_t CTaskLinkRpc::SendResponse(CFileBase *header, CFileBase *data)
{
    ASSERT(this->mWriter);
    return this->mWriter->SendResponse(header,data);
}

status_t CTaskLinkRpc::Run(uint32_t interval)
{
    if(mStep == STEP_RETRY)
    {
        this->mRetries++;
        if(this->mMaxRetries > 0 && this->mRetries > this->mMaxRetries )
        {
            this->Stop(ERROR_EXCEED_MAX_RETRIES);
            return ERROR;
        }
        this->mStep = STEP_GET_SOCKET;
        this->OnGetSocket();
    }

    if(mStep == STEP_GET_SOCKET)
    {
        if(this->mSocket->IsConnected())
        {
            mStep = STEP_GET_SOCKET_OK;
        }
        else
        {
            this->Sleep(10);
        }
    }

    if(mStep == STEP_GET_SOCKET_OK)
    {
        this->mStep = 0;
        this->Suspend();
        this->StartReaderAndWriter();
    }

    return OK;
}
const char * CTaskLinkRpc::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_LINKRPC_READER_STOPPED)
        return "linkrpc reader stopped";
    if(err == ERROR_LINKRPC_WRITER_STOPPED)
        return "linkrpc writer stopped";
    if(err == ERROR_EXCEED_MAX_RETRIES)
        return "exceed max retries";
    if(err == ERROR_SOCKET_ERROR)
        return "socket error";
    if(err == ERROR_CONNECT_ERROR)
        return "connect error";
    if(err == ERROR_WRONG_HEADER_FORMAT)
        return "wrong header format";
    return "unknown error";
}
status_t CTaskLinkRpc::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskLinkRpc ");
    mem.Printf("(%d) ",this->GetId());
    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s",mem.CStr()
    );
    return OK;
}

status_t CTaskLinkRpc::Start()
{
    this->mStep = STEP_RETRY;
    QuitTask(&this->mTaskReader);
    QuitTask(&this->mTaskWriter);
    this->mReader = NULL;
    this->mWriter = NULL;
    this->Resume();
    return OK;
}

status_t CTaskLinkRpc::SetMaxRetries(int max)
{
    this->mMaxRetries = max;
    return OK;
}

status_t CTaskLinkRpc::Retry(int err)
{
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "CTaskLinkRpc: retry with error \"%s\"",this->ErrorToString(err)
    );
    this->mSocket->CloseConnect();
    this->Start();
    this->Sleep(1000);
    return OK;
}


status_t CTaskLinkRpc::WriteImmediately()
{
    CTaskLinkRpcWriter *writer = (CTaskLinkRpcWriter*)GetTask(mTaskWriter);
    if(writer)
    {
        return writer->WriteImmediately();
    }
    return ERROR;
}

CSocket* CTaskLinkRpc::GetSocket()
{
    return mSocket;
}

int CTaskLinkRpc::GetRetries()
{
    return mRetries;
}

status_t CTaskLinkRpc::SetRetries(int retry)
{
    mRetries = retry;
    return OK;
}

status_t CTaskLinkRpc::OnSocketError()
{
    return OK;
}
///////////////////////////////////////////////////////////////////////////////
//override
///////////////////////////////////////////////////////////////////////////////
status_t CTaskLinkRpc::OnGetSocket()
{
    return OK;
}
bool CTaskLinkRpc::CanReadNextPackage()
{
    return true;
}
status_t CTaskLinkRpc::OnGotPackageHeader(LINKRPC_HEADER *header,CMem *header_data)
{
    return OK;
}
status_t CTaskLinkRpc::OnGotPackageData(LINKRPC_HEADER *header,CMem *header_data,CFileBase *data)
{
    return OK;
}
status_t CTaskLinkRpc::OnPreparePackageToSend()
{
    return FALSE;
}
status_t CTaskLinkRpc::OnPackageSendOk()
{
    return OK;
}
