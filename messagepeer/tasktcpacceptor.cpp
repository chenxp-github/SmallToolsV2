#include "tasktcpacceptor.h"
#include "mem.h"
#include "syslog.h"
#include "mem_tool.h"

enum{
    STEP_INIT_TCP_SERVER = 1,
    STEP_ACCEPT,
};

CTaskTcpAcceptor::CTaskTcpAcceptor()
{
    this->InitBasic();
}
CTaskTcpAcceptor::~CTaskTcpAcceptor()
{
    this->Destroy();
}
status_t CTaskTcpAcceptor::InitBasic()
{
    CTask::InitBasic();
    this->mServer = NULL;
    this->mStep = 0;
    mCallback = NULL;
    mFastModeTimeMs = 30*1000;
    return OK;
}
status_t CTaskTcpAcceptor::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);

    NEW(this->mServer,CTcpServer);
    this->mServer->Init();
    this->mServer->InitServer();
    this->mServer->SetBlocking(0);

    NEW(mCallback,CClosure);
    mCallback->Init();

    return OK;
}
status_t CTaskTcpAcceptor::Destroy()
{
    DEL(mCallback);
    DEL(this->mServer);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

status_t CTaskTcpAcceptor::Run(uint32_t interval)
{
    if(mStep == STEP_INIT_TCP_SERVER)
    {
        if(!this->mServer->SetPort(this->mPort))
        {
            this->Stop(ERROR_BIND_FAIL);
            return ERROR;
        }
        this->mStep = STEP_ACCEPT;
    }

    if(this->mStep == STEP_ACCEPT)
    {
        int32_t fd = this->mServer->Accept();
        if(fd > 0)
        {
            crt_set_blocking_mode(fd,0);
			GetTaskMgr()->OnSocketConnected(fd);
            mCallback->SetParamInt(1,fd);
            mCallback->SetParamPointer(2,mServer);
            mCallback->Run(EVENT_NEW_CLIENT);
        }

        if(mFastModeTimeMs > 0)
        {
            this->Sleep(1);
            mFastModeTimeMs -= interval;
        }
        else
        {
            this->Sleep(20);
        }
    }
    return OK;
}

const char * CTaskTcpAcceptor::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_BIND_FAIL)
        return "bind fail";
    return "unknown error";
}
status_t CTaskTcpAcceptor::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskTcpAcceptor ");
    mem.Printf("(%d) ",this->GetId());
    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s",mem.CStr()
    );
    return OK;
}
status_t CTaskTcpAcceptor::Start()
{
    this->mStep = STEP_INIT_TCP_SERVER;
    this->Resume();
    return OK;
}
status_t CTaskTcpAcceptor::Stop(status_t err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);
    mCallback->SetParamInt(1,err);
    mCallback->Run(EVENT_STOP);
    return OK;
}

status_t CTaskTcpAcceptor::SetListenPort(int port)
{
    this->mPort = port;
    return OK;
}

CClosure* CTaskTcpAcceptor::Callback()
{
    return mCallback;
}

status_t CTaskTcpAcceptor::SetFastModeTimeMs(int ms)
{
    mFastModeTimeMs = ms;
    return OK;
}
