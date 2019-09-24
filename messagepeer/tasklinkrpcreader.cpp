#include "tasklinkrpcreader.h"
#include "mem.h"
#include "syslog.h"
#include "mem_tool.h"

enum {
    STEP_PREPARE_READ_HEADER = 1,
    STEP_READ_HEADER,
    STEP_PREPARE_READ_HEADER_DATA,
    STEP_READ_HEADER_DATA,
    STEP_PREPARE_READ_DATA,
    STEP_READ_DATA,
};

CTaskLinkRpcReader::CTaskLinkRpcReader()
{
    this->InitBasic();
}
CTaskLinkRpcReader::~CTaskLinkRpcReader()
{
    this->Destroy();
}
status_t CTaskLinkRpcReader::InitBasic()
{
    CTask::InitBasic();

    this->iSocketRwer = NULL;
    this->iHeaderBuf = NULL;
    this->iDataBuf = NULL;
    this->mHeader = NULL;
    this->mStep = 0;
    this->mCallback = NULL;
    return OK;
}
status_t CTaskLinkRpcReader::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);

    NEW(this->mHeader,CMem);
    this->mHeader->Init();
    this->mHeader->Malloc(sizeof(LINKRPC_HEADER));

    NEW(mCallback,CClosure);
    mCallback->Init();

    return OK;
}
status_t CTaskLinkRpcReader::Destroy()
{
    DEL(mCallback);
    DEL(this->mHeader);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

status_t CTaskLinkRpcReader::Run(uint32_t interval)
{
    ASSERT(this->iSocketRwer);

    if(this->mStep == STEP_PREPARE_READ_HEADER)
    {
        if(this->mCallback->Run(EVENT_CAN_READ_NEXT_PACKAGE))
        {
            this->iSocketRwer->PrepareFileForRead(this->mHeader,sizeof(LINKRPC_HEADER),true);
            this->mStep = STEP_READ_HEADER;
        }
        else
        {
            this->Sleep(10);
        }
    }

    if(this->mStep == STEP_READ_HEADER)
    {
        if(this->iSocketRwer->DoRead(interval))
        {
            LINKRPC_HEADER *header = this->Header();
            if(header->tag != LINKRPC_TAG)
            {
                this->Stop(ERROR_WRONG_PACKAGE_TAG);
                return ERROR;
            }
            if((header->cmd != LINKRPC_CMD_REQUEST)
                &&(header->cmd != LINKRPC_CMD_RESPONSE)
                &&(header->cmd != LINKRPC_CMD_HEARTBEAT))
            {
                this->Stop(ERROR_WRONG_CMD);
                return ERROR;
            }
            this->mStep = STEP_PREPARE_READ_HEADER_DATA;
        }       
    }

    if(this->mStep == STEP_PREPARE_READ_HEADER_DATA)
    {
        this->iSocketRwer->PrepareFileForRead(this->iHeaderBuf,Header()->header_size,true);
        this->mStep = STEP_READ_HEADER_DATA;
    }

    if(this->mStep == STEP_READ_HEADER_DATA)
    {
        if(this->iSocketRwer->DoRead(interval))
        {
            this->mStep = STEP_PREPARE_READ_DATA;
            mCallback->SetParamPointer(1,Header());
            mCallback->SetParamPointer(2,iHeaderBuf);
            mCallback->SetParamPointer(3,this);
            mCallback->Run(EVENT_GOT_HEADER);
        }
    }

    if(this->mStep == STEP_PREPARE_READ_DATA)
    {
        this->iSocketRwer->PrepareFileForRead(this->iDataBuf,Header()->data_size,true);
        this->mStep = STEP_READ_DATA;
    }

    if(this->mStep == STEP_READ_DATA)
    {
        if(this->iSocketRwer->DoRead(interval))
        {
            this->mStep = STEP_PREPARE_READ_HEADER;
            mCallback->SetParamPointer(1,Header());
            mCallback->SetParamPointer(2,iHeaderBuf);
            mCallback->SetParamPointer(3,iDataBuf);
            mCallback->Run(EVENT_GOT_DATA);
        }
    }
    return OK;
}

const char * CTaskLinkRpcReader::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_READ_TIME_OUT)
        return "read time out";
    if(err == ERROR_READ_ERROR)
        return "read error";
    if(err == ERROR_CONNECTION_CLOSED)
        return "connection closed";
    if(err == ERROR_WRONG_PACKAGE_TAG)
        return "wrong package tag";
    if(err == ERROR_WRONG_CMD)
        return "wrong cmd";
    return "unknown error";
}
status_t CTaskLinkRpcReader::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskLinkRpcReader ");
    mem.Printf("(%d) ",this->GetId());
    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s\n",mem.CStr()
    );
    return OK;
}
status_t CTaskLinkRpcReader::Start()
{
    this->mStep = STEP_PREPARE_READ_HEADER;
    this->Resume();
    return OK;
}
status_t CTaskLinkRpcReader::Stop(status_t err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);
    mCallback->SetParamInt(1,err);
    mCallback->SetParamPointer(2,this);
    mCallback->Run(EVENT_STOP);
    return OK;
}

LINKRPC_HEADER * CTaskLinkRpcReader::Header()
{
    return (LINKRPC_HEADER*)this->mHeader->GetRawBuf();
}

status_t CTaskLinkRpcReader::SetHeaderBuf(CMem *iBuf)
{
    this->iHeaderBuf = iBuf;
    return OK;
}

status_t CTaskLinkRpcReader::SetDataBuf(CFileBase *iBuf)
{
    this->iDataBuf = iBuf;
    return OK;
}

CClosure* CTaskLinkRpcReader::Callback()
{
    return mCallback;
}
