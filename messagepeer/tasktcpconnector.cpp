#include "tasktcpconnector.h"
#include "mem.h"
#include "syslog.h"
#include "mem_tool.h"

#ifndef TCP_CONNECTOR_TIMEOUT 
#define TCP_CONNECTOR_TIMEOUT (30*1000)
#endif

#define STEP_RESOLVE_HOST                   2
#define STEP_WAIT_RESOLVE_HOST              3
#define STEP_CONNECT                        4
#define STEP_CHECK_CONNECT_COMPLETE         5

CTaskTcpConnector::CTaskTcpConnector()
{
    this->InitBasic();
}
CTaskTcpConnector::~CTaskTcpConnector()
{
    this->Destroy();
}
status_t CTaskTcpConnector::InitBasic()
{
    CTask::InitBasic();
    this->mServerName = NULL;
    this->mPort = 2008;
    this->mStep = 0;
    this->hostoip_context = 0;
    this->mTimeout = 30*1000;
    this->mConnectTime = 0;
    this->mTcpClient = NULL;
    mCallback = NULL;
    return OK;
}
status_t CTaskTcpConnector::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
            
    NEW(this->mServerName,CMem);
    this->mServerName->Init();

    NEW(this->mTcpClient,CTcpClient);
    this->mTcpClient->Init();
    
    NEW(mCallback,CClosure);
    mCallback->Init();

    return OK;
}
status_t CTaskTcpConnector::Destroy()
{
    DEL(mCallback);
    DEL(this->mServerName);
    DEL(this->mTcpClient);
    crt_free_host_to_ip_context(&this->hostoip_context);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

status_t CTaskTcpConnector::CheckConnectTimeout(uint32_t interval)
{
    if(interval < (uint32_t)mTimeout)
        this->mConnectTime+=interval;
    if(this->mConnectTime >= this->mTimeout)
    {
        this->mConnectTime = 0;
        this->Stop(ERROR_CONNECT_TIMEOUT);
        return ERROR;
    }
    this->Sleep(100);
    return OK;
}

status_t CTaskTcpConnector::InitTcpClient(const char *ip_address,int port)
{
    ASSERT(ip_address);
    this->mTcpClient->CloseConnect();
    this->mTcpClient->SetServerIP(ip_address);
    this->mTcpClient->SetPort(port);
    this->mTcpClient->SetBlocking(0);
    return OK;
}
status_t CTaskTcpConnector::Run(uint32_t interval)
{
    if(this->mStep == STEP_RESOLVE_HOST)
    {
        if(CSocket::IsIpAddress(mServerName))
        {
            this->InitTcpClient(mServerName->CStr(),mPort);     
            this->mStep = STEP_CONNECT;
        }
        else
        {
            crt_host_to_ip_async(this->mServerName->CStr(),&this->hostoip_context);
            this->mStep = STEP_WAIT_RESOLVE_HOST;
        }
    }
    
    if(this->mStep == STEP_WAIT_RESOLVE_HOST)
    {
        if(this->hostoip_context[0] == 0)
        {
            this->Sleep(10);
            return ERROR;
        }
        if(this->hostoip_context[1] == 0)
        {
            this->Stop(ERROR_RESOLVE_HOST_ERROR);           
            return ERROR;
        }
        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
            "resolve host %s ok",this->mServerName->CStr()
        );
        
        CMem mem((const char*)&hostoip_context[5]);
        LOCAL_MEM(ip);
        mem.Seek(0);
        mem.ReadLine(&ip);

        this->InitTcpClient(ip.CStr(),mPort);       
        this->mStep = STEP_CONNECT;
    }
    
    if(this->mStep == STEP_CONNECT)
    {
        if(this->mTcpClient->Connect())
        {
            this->mStep = STEP_CHECK_CONNECT_COMPLETE; 
        }
        else
        {
            this->CheckConnectTimeout(interval);
        }
    }
    
    if(mStep == STEP_CHECK_CONNECT_COMPLETE)
    {
        if(this->mTcpClient->IsConnectComplete())
        {
            XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
                "connect to %s:%d ok",
                this->mServerName->CStr(),this->mPort
            );
            this->mConnectTime = 0;
			          
			GetTaskMgr()->OnSocketConnected(mTcpClient->GetSocketFd());

            mCallback->SetParamPointer(1,this);
            mCallback->SetParamPointer(2,mTcpClient);
            mCallback->Run(EVENT_CONNECTED);
            this->Stop(ERROR_NONE);
        }
        else
        {
            this->CheckConnectTimeout(interval);
        }
    }
    return OK;
}

const char * CTaskTcpConnector::ErrorToString(int err)
{
    if(err == ERROR_NONE)
        return "none";
    if(err == ERROR_RESOLVE_HOST_ERROR)
        return "resolve host error";
    if(err == ERROR_CONNECT_TIMEOUT)
        return "connect timeout";
    if(err == ERROR_EXCEED_MAX_RETRIES)
        return "exceed max retries";
    if(err == ERROR_CONNECTION_CLOSED)
        return "connection closed";
    return "unknown error";
}
status_t CTaskTcpConnector::ReportError(int err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskTcpConnector ");
    mem.Printf("(%d) ",this->GetId());

    mem.Puts("exit with error:");
    mem.Puts(this->ErrorToString(err));     
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
        "%s",mem.CStr()
    );
    return OK;
}
status_t CTaskTcpConnector::Start()
{
    this->mStep = STEP_RESOLVE_HOST;
    this->Resume();
    return OK;
}
status_t CTaskTcpConnector::Stop(status_t err)
{
    if(this->IsDead())return OK;
    this->Quit();
    this->ReportError(err);
    mCallback->SetParamInt(1,err);
    mCallback->SetParamPointer(2,this);
    mCallback->Run(EVENT_STOP);
    return OK;
}

status_t CTaskTcpConnector::SetServerName(CMem *servername)
{
    ASSERT(servername);
    this->mServerName->Copy(servername);
    return OK;
}
status_t CTaskTcpConnector::SetServerName(const char *servername)
{
    ASSERT(servername);
    CMem mem_servername(servername);
    return this->SetServerName(&mem_servername);
}
status_t CTaskTcpConnector::SetPort(int port)
{
    this->mPort = port;
    return OK;
}
CMem* CTaskTcpConnector::GetServerName()
{
    return this->mServerName;
}
int CTaskTcpConnector::GetPort()
{
    return this->mPort;
}

status_t CTaskTcpConnector::SetTimeout(int to)
{
    this->mTimeout = to;
    return OK;
}

CClosure* CTaskTcpConnector::Callback()
{
    return mCallback;
}
