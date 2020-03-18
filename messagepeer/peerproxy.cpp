#include "peerproxy.h"
#include "peerglobals.h"
#include "syslog.h"
#include "peercommon.h"
#include "taskpeerserver.h"

CPeerProxy::CPeerProxy()
{
    this->InitBasic();
}
CPeerProxy::~CPeerProxy()
{
    this->Destroy();
}
status_t CPeerProxy::InitBasic()
{
    TASK_CONTAINER_CLEAR();
    WEAK_REF_CLEAR();

    this->mPeerName = NULL;
    this->mConnectedNames = NULL;
    this->mPendingMessages = NULL;
    this->mTaskPeerServer = 0;
    this->mLifeTime = 0;
    this->mServerSidePeer.InitBasic();
	this->mCallback = NULL;
    return OK;
}
status_t CPeerProxy::Init(CTaskMgr *mgr)
{
    this->Destroy();
    
    this->SetTaskMgr(mgr);

    NEW(this->mPeerName,CMem);
    this->mPeerName->Init();
    this->mPeerName->Malloc(512);

    NEW(this->mPendingMessages,CPeerMessageStk);
    this->mPendingMessages->Init(MESSAGE_PEER_SENDING_QUEUE_LEN);

	NEW(this->mCallback,CClosure);
	this->mCallback->Init();
    return OK;
}
status_t CPeerProxy::Destroy()
{
    WEAK_REF_DESTROY();
    QuitTask(&this->mTaskPeerServer);
    DEL(this->mPendingMessages);
    DEL(this->mConnectedNames);
    DEL(this->mPeerName);
	DEL(this->mCallback);
	mServerSidePeer.Destroy();
    this->InitBasic();
    return OK;
}
status_t CPeerProxy::Copy(CPeerProxy *p)
{
    ASSERT(p && this != p);
    //add your code
    return OK;
}
int CPeerProxy::Comp(CPeerProxy *p)
{
    ASSERT(p);
    return this->mPeerName->StrCmp(p->GetName());
}
status_t CPeerProxy::Print()
{
    //add your code
    return OK;
}
status_t CPeerProxy::SetName(CMem *peername)
{
    ASSERT(peername);
    this->mPeerName->StrCpy(peername);
    return OK;
}
status_t CPeerProxy::SetName(const char *peername)
{
    ASSERT(peername);
    CMem mem_peername(peername);
    return this->SetName(&mem_peername);
}
CMem* CPeerProxy::GetName()
{
    return this->mPeerName;
}

const char * CPeerProxy::GetNameStr()
{
    return this->mPeerName->CStr();
}

status_t CPeerProxy::SetMaxConnectedPeers(int max)
{
    ASSERT(this->mConnectedNames == NULL);
    NEW(this->mConnectedNames,CMemStk);
    this->mConnectedNames->Init(max);
    this->mMaxConnectedPeer = max;
    return OK;
}

status_t CPeerProxy::AddPeer(CMem *name)
{
    ASSERT(name);
    ASSERT(this->mConnectedNames);
    if(this->mConnectedNames->GetLen() >= this->mMaxConnectedPeer)
        return ERROR;
    return this->mConnectedNames->InsOrdered(name,0,1);
}

status_t CPeerProxy::AddPeer(const char *name)
{
    CMem mem(name);
    return this->AddPeer(&mem);
}

status_t CPeerProxy::AddPendingMessage(CPeerMessage *msg)
{
    ASSERT(msg);	

    ASSERT(!this->mPendingMessages->IsPtrExist(msg));

    if(mServerSidePeer.get())
        mServerSidePeer->ResumeFetchMessageTask();

    msg->AddRef();
    if(!this->mPendingMessages->PushPtr(msg))
    {
        msg->Release();
        return ERROR;
    }

    CTaskPeerServer *pt = (CTaskPeerServer *)GetTask(mTaskPeerServer);
    if(pt)pt->WriteImmediately();
    return OK;
}

CMemStk * CPeerProxy::GetAllConnectedPeerNames()
{
    return this->mConnectedNames;
}

status_t CPeerProxy::AttachTaskPeerServer(int task_id)
{
    ASSERT(!IsTask(this->mTaskPeerServer));
    this->mTaskPeerServer = task_id;
	this->OnSocketConnected();
    return OK;
}

bool CPeerProxy::IsConnectionAlive()
{
    if(mServerSidePeer.get())
        return !mServerSidePeer->IsMarkQuit();
    return IsTask(this->mTaskPeerServer);
}

CPeerMessageStk * CPeerProxy::GetSendingQueue()
{
    return this->mPendingMessages;
}

bool CPeerProxy::IsPeerConnectedMe(CPeerProxy *peer)
{
    CMemStk *stk = peer->GetAllConnectedPeerNames();
    ASSERT(stk);
    return stk->BSearchNode(this->GetName(),0)!= NULL;
}

status_t CPeerProxy::AddAllPeers(CFileBase *lines)
{
    ASSERT(lines);

    this->mConnectedNames->Clear();

    LOCAL_MEM(mem);
    lines->Seek(0);
    while(lines->ReadLine(&mem))
    {
        if(mem.C(0) == 0)continue;
        this->AddPeer(&mem);
    }

    return OK;
}

uint32_t CPeerProxy::GetLifeTime()
{
    return this->mLifeTime;
}

status_t CPeerProxy::SetLifeTime(uint32_t ltime)
{
    this->mLifeTime = ltime;
    return OK;
}

status_t CPeerProxy::AttachServerSidePeer(CServerSidePeer *peer)
{
    ASSERT(peer);
    return mServerSidePeer.WeakRef(peer);
}

int CPeerProxy::GetSendingQueueLength()
{
    return mPendingMessages->GetLen();
}

status_t CPeerProxy::ClearSendingQueue()
{
    return mPendingMessages->Clear();
}

bool CPeerProxy::IsServerSide()
{
    return mServerSidePeer.IsValid();
}

CClosure *CPeerProxy::Callback()
{
	return mCallback;
}

status_t CPeerProxy::OnSocketConnected()
{
    Callback()->SetParamPointer(1,this);
    Callback()->Run(PEER_EVENT_CONNECTED);
    return OK;
}

status_t CPeerProxy::OnSocketDisconnected()
{
    PEER_GLOBALS(peer_globals);
    peer_globals->NotifyPeerDisconnected(this);
    Callback()->SetParamPointer(1,this);
    Callback()->Run(PEER_EVENT_DISCONNECTED);
    return OK;
}

CSocket *CPeerProxy::GetSocket()
{
    if(IsServerSide())
        return NULL;
    CTaskPeerServer *pt = (CTaskPeerServer *)GetTask(mTaskPeerServer);
    if(!pt)return NULL;
    return pt->GetSocket();
}

