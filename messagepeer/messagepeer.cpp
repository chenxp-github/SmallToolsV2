#include "messagepeer.h"
#include "syslog.h"
#include "taskpeerclient.h"
#include "peercommon.h"

CMessagePeer::CMessagePeer()
{
    this->InitBasic();
}
CMessagePeer::~CMessagePeer()
{
    this->Destroy();
}
status_t CMessagePeer::InitBasic()
{
    TASK_CONTAINER_CLEAR();
    WEAK_REF_ID_CLEAR();
    this->mName = NULL;
    this->mServer = NULL;
    this->mPort = 0;
    this->mTaskPeerClient = 0;
    this->mSendingQueue = NULL;
    this->mAllConnectedPeers = NULL;
    this->mCallback = NULL;
    this->mMaxRetries = -1;
    this->mFlags = 0;
    return OK;
}
status_t CMessagePeer::Init(CTaskMgr *mgr)
{
    this->Destroy();
    WEAK_REF_ID_INIT();

    this->SetTaskMgr(mgr);

    NEW(this->mName,CMem);
    this->mName->Init();

    NEW(this->mServer,CMem);
    this->mServer->Init();

    NEW(this->mSendingQueue,CPeerMessageStk);
    this->mSendingQueue->Init(MESSAGE_PEER_SENDING_QUEUE_LEN);

    NEW(this->mAllConnectedPeers,CMemStk);
    this->mAllConnectedPeers->Init(64);

    NEW(mCallback,CClosure);
    mCallback->Init();

    return OK;
}
status_t CMessagePeer::Destroy()
{
    QuitTask(&this->mTaskPeerClient);
    DEL(this->mName);
    DEL(this->mServer);
    DEL(this->mSendingQueue);
    DEL(this->mAllConnectedPeers);
    DEL(mCallback);

    this->InitBasic();
    return OK;
}
status_t CMessagePeer::Copy(CMessagePeer *p)
{
    ASSERT(p && this != p);
    this->mName->Copy(p->mName);
    this->mServer->Copy(p->mServer);
    this->mPort = p->mPort;
    return OK;
}
int CMessagePeer::Comp(CMessagePeer *p)
{
    ASSERT(p);
    return 0;
}
status_t CMessagePeer::Print()
{
    syslog_printf("Name = %s",this->mName->CStr());
    syslog_printf("Server = %s",this->mServer->CStr());
    char _str_mport[64];
    sprintf(_str_mport,"%d",mPort);
    syslog_printf("Port = %s",_str_mport);
    return OK;
}

status_t CMessagePeer::SetName(CMem *name)
{
    ASSERT(name);
    this->mName->Copy(name);
    return OK;
}
status_t CMessagePeer::SetName(const char *name)
{
    ASSERT(name);
    CMem mem_name(name);
    return this->SetName(&mem_name);
}
status_t CMessagePeer::SetServer(CMem *server)
{
    ASSERT(server);
    this->mServer->Copy(server);
    return OK;
}
status_t CMessagePeer::SetServer(const char *server)
{
    ASSERT(server);
    CMem mem_server(server);
    return this->SetServer(&mem_server);
}
status_t CMessagePeer::SetPort(int port)
{
    this->mPort = port;
    return OK;
}
CMem* CMessagePeer::GetName()
{
    return this->mName;
}
const char* CMessagePeer::GetNameStr()
{
    return this->mName->CStr();
}
CMem* CMessagePeer::GetServer()
{
    return this->mServer;
}
const char* CMessagePeer::GetServerStr()
{
    return this->mServer->CStr();
}
int CMessagePeer::GetPort()
{
    return this->mPort;
}

status_t CMessagePeer::Start()
{
    ASSERT(!IsTask(this->mTaskPeerClient));
    CTaskPeerClient *task;
    NEW(task,CTaskPeerClient);
    task->Init(this->GetTaskMgr());
    task->iHostPeer.WeakRef(this);
    task->SetMaxRetries(mMaxRetries);
    task->Start();
    this->mTaskPeerClient = task->GetId();
    this->SetIsClosedPermanently(false);

    return OK;
}

status_t CMessagePeer::SendMessage(CPeerMessage *msg)
{
    ASSERT(msg);

    if(this->IsClosedPermanently())
        return ERROR;

    if(this->mTaskPeerClient != 0)
    {
        if(!IsTask(this->mTaskPeerClient))
        {
            XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_ERROR,
                "peer \"%s\" is not available anymore.",this->GetNameStr()
            );
            return ERROR;
        }
    }

    msg->AddRef();
    status_t ret = this->mSendingQueue->PushPtr(msg);

    CTaskPeerClient *pt = (CTaskPeerClient *)GetTask(mTaskPeerClient);
    if(pt)pt->WriteImmediately();
    return ret;
}

status_t CMessagePeer::Connect(const char *peer_name)
{
    ASSERT(peer_name);
    CMem mem(peer_name);
    return this->mAllConnectedPeers->InsOrdered(&mem,0,1);
}

CMemStk * CMessagePeer::GetAllConnectedPeers()
{
    return this->mAllConnectedPeers;
}

CClosure* CMessagePeer::Callback()
{
    return mCallback;
}

status_t CMessagePeer::SetCanFetchMessage(bool can_fetch)
{
    CTaskPeerClient *pt = (CTaskPeerClient *)GetTask(mTaskPeerClient);
    ASSERT(pt);
    return pt->SetCanReadNextPackage(can_fetch);
}

bool CMessagePeer::IsConnected()
{
    CTaskPeerClient *pt = (CTaskPeerClient *)GetTask(mTaskPeerClient);
    if(pt == NULL)return false;
    CSocket *socket = pt->GetSocket();
    ASSERT(socket);
    return socket->IsConnected();
}

int CMessagePeer::GetSendingQueueLength()
{
    return mSendingQueue->GetLen();
}

status_t CMessagePeer::SetMaxRetries(int max_retries)
{
    mMaxRetries = max_retries;
    CTaskPeerClient *pt = (CTaskPeerClient *)GetTask(mTaskPeerClient);
    if(pt)
    {
        pt->SetMaxRetries(mMaxRetries);
    }
    return OK;
}

status_t CMessagePeer::OnTaskPeerClientStop(int err)
{
    mSendingQueue->Clear();
    SetIsClosedPermanently(true);
    Callback()->SetParamInt(1,err);
    Callback()->Run(PEER_EVENT_STOPPED);
    return OK;
}

int CMessagePeer::GetRetries()
{
    CTaskPeerClient *pt = (CTaskPeerClient *)GetTask(mTaskPeerClient);
    if(pt)
    {
        return pt->GetRetries();
    }
    return -1;
}

status_t CMessagePeer::SetRetries(int retry)
{
    CTaskPeerClient *pt = (CTaskPeerClient *)GetTask(mTaskPeerClient);
    if(pt)
    {
        return pt->SetRetries(retry);
    }
    return ERROR;
}


status_t CMessagePeer::ClearSendingQueue()
{
    return mSendingQueue->Clear();
}

status_t CMessagePeer::OnSocketConnected()
{
    Callback()->SetParamPointer(1,this);
    Callback()->Run(PEER_EVENT_CONNECTED);
    return OK;
}

status_t CMessagePeer::OnSocketDisconnected()
{
    Callback()->SetParamPointer(1,this);
    Callback()->Run(PEER_EVENT_DISCONNECTED);
    return OK;
}