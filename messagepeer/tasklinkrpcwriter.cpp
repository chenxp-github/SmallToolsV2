#include "tasklinkrpcwriter.h"
#include "mem.h"
#include "syslog.h"
#include "mem_tool.h"

enum{
    STEP_PREPARE_PACKAGE = 1,
    STEP_WRITE_PACKAGE,
    STEP_WRITE_HEARTBEAT,
};

CTaskLinkRpcWriter::CTaskLinkRpcWriter()
{
    this->InitBasic();
}
CTaskLinkRpcWriter::~CTaskLinkRpcWriter()
{
    this->Destroy();
}
status_t CTaskLinkRpcWriter::InitBasic()
{
    CTask::InitBasic();
    this->iSocketRwer = NULL;
    this->iHeaderData = NULL;
    this->iData = NULL;
    this->mHeader = NULL;
    this->mPackage = NULL;
    this->mStep = 0;
    this->mHearBeatTime = 0;
    mCallback = NULL;
    return OK;
}
status_t CTaskLinkRpcWriter::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);

    NEW(this->mHeader,CMem);
    this->mHeader->Init();
    this->mHeader->Malloc(sizeof(LINKRPC_HEADER));

    NEW(this->mPackage,CCombFile);
    this->mPackage->Init(4);

    NEW(mCallback,CClosure);
    mCallback->Init();
    return OK;
}
status_t CTaskLinkRpcWriter::Destroy()
{
    DEL(mCallback);
    DEL(this->mHeader);
    DEL(this->mPackage);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

status_t CTaskLinkRpcWriter::Run(uint32_t interval)
{
    ASSERT(this->iSocketRwer);
    if(this->mStep == STEP_PREPARE_PACKAGE)
    {
        this->iData = NULL;
        this->iHeaderData = NULL;
        this->mPackage->Clear();
        mCallback->SetParamPointer(1,this);
        mCallback->Run(EVENT_GET_NEXT_PACKAGE);

        if(this->iHeaderData || this->iData)
        {
            this->CombPackage();
            this->mStep = STEP_WRITE_PACKAGE;
        }
        else
        {
            this->mHearBeatTime += interval;
            if(this->mHearBeatTime >= (this->iSocketRwer->GetTimeout()/2))
            {
                this->mHearBeatTime = 0;
                this->SendPackage(LINKRPC_CMD_HEARTBEAT,NULL,NULL);
                this->CombPackage();
                this->mStep = STEP_WRITE_HEARTBEAT;
            }
            else
            {
                this->Sleep(10);
            }
        }
    }

    if(this->mStep == STEP_WRITE_PACKAGE)
    {
        if(this->iSocketRwer->DoWrite(interval))
        {
            this->mPackage->Clear();
            this->mStep = STEP_PREPARE_PACKAGE;
            mCallback->SetParamPointer(1,this);
            mCallback->Run(EVENT_PACKAGE_SEND_OK);
            return OK;
        }
    }

    if(this->mStep == STEP_WRITE_HEARTBEAT)
    {
        if(this->iSocketRwer->DoWrite(interval))
        {
            this->mPackage->Clear();
            this->mStep = STEP_PREPARE_PACKAGE;
            return OK;
        }
    }

    return OK;
}

const char * CTaskLinkRpcWriter::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_CONNECTION_CLOSED)
        return "connection closed";
    if(err == ERROR_WRITE_TIME_OUT)
        return "write time out";
    if(err == ERROR_WRITE_ERROR)
        return "write error";
    if(err == ERROR_WRONG_PACKAGE_TAG)
        return "wrong package tag";
    if(err == ERROR_WRONG_CMD)
        return "wrong cmd";
    return "unknown error";
}
status_t CTaskLinkRpcWriter::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskLinkRpcWriter ");
    mem.Printf("(%d) ",this->GetId());

    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s",mem.CStr()
    );
    return OK;
}
status_t CTaskLinkRpcWriter::Start()
{
    this->mStep = STEP_PREPARE_PACKAGE;
    this->Resume();
    return OK;
}

status_t CTaskLinkRpcWriter::Stop(status_t err)
{
    if(this->IsDead())return OK;
    this->ReportError(err);
    this->Quit();

    mCallback->SetParamInt(1,err);
    mCallback->SetParamPointer(2,this);
    mCallback->Run(EVENT_STOP);

    return OK;
}

LINKRPC_HEADER * CTaskLinkRpcWriter::Header()
{
    return (LINKRPC_HEADER*)this->mHeader->GetRawBuf();
}

status_t CTaskLinkRpcWriter::SendPackage(int cmd, CFileBase *header, CFileBase *data)
{
    ASSERT(mStep == STEP_PREPARE_PACKAGE);

    LINKRPC_HEADER *h = this->Header();

    h->data_size = 0;
    h->header_size = 0;
    h->tag = LINKRPC_TAG;

    if(data)
        h->data_size = (int32_t)data->GetSize();
    if(header)
        h->header_size = (int32_t)header->GetSize();
    h->cmd = cmd;

    this->iHeaderData = header;
    this->iData = data;

    return OK;
}

status_t CTaskLinkRpcWriter::SendResponse(CFileBase *header, CFileBase *data)
{
    return this->SendPackage(LINKRPC_CMD_RESPONSE,header,data);
}

status_t CTaskLinkRpcWriter::SendRequest(CFileBase *header, CFileBase *data)
{
    return this->SendPackage(LINKRPC_CMD_REQUEST,header,data);
}

status_t CTaskLinkRpcWriter::CombPackage()
{
    this->mHeader->SetSize(sizeof(LINKRPC_HEADER));
    this->mPackage->AddFile(this->mHeader);
    if(this->iHeaderData)
        this->mPackage->AddFile(this->iHeaderData);
    if(this->iData)
        this->mPackage->AddFile(this->iData);
    this->iSocketRwer->PrepareFileForWrite(this->mPackage,0,this->mPackage->GetSize());
    return OK;
}
CClosure* CTaskLinkRpcWriter::Callback()
{
    return mCallback;
}

//write immediately when you have data
status_t CTaskLinkRpcWriter::WriteImmediately()
{
    ASSERT(iSocketRwer);

    if(!iSocketRwer->IsConnected())
        return ERROR;

    for(int i = 0; i < 4; i++)
    {
        if(!this->IsDead())
        {
            this->Run(0);
        }
    }
    return OK;
}
