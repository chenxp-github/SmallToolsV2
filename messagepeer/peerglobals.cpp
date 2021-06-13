#include "peerglobals.h"
#include "syslog.h"
#include "tasktcpacceptor.h"
#include "taskpeerserver.h"
#include "tasktimer.h"

#define _PEER_PROXY_MAX_LIFE_TIME (30*1000)

CPeerGlobals::CPeerGlobals()
{
    this->InitBasic();
}
CPeerGlobals::~CPeerGlobals()
{
    this->Destroy();
}
status_t CPeerGlobals::InitBasic()
{
    TASK_CONTAINER_CLEAR();
    this->mListenPort = 2008;
    this->mPeerManager = NULL;
    this->mTaskPeerKiller = 0;
    this->mTaskAcceptor = 0;
    this->mCallbackMap = NULL;
    this->mCallback = NULL;
	this->mTrustMode = true;
    return OK;
}
status_t CPeerGlobals::Init(CTaskMgr *mgr)
{
    this->Destroy();
    this->SetTaskMgr(mgr);

    NEW(this->mCallback,CClosure);
    this->mCallback->Init();

    NEW(this->mPeerManager,CPeerProxyManager);
    this->mPeerManager->Init();

    NEW(this->mCallbackMap,CCallbackMap);
    this->mCallbackMap->Init(mgr,1024);
    this->mCallbackMap->StartAutoTimeoutTimer();

    this->StartTaskPeerKiller();
    return OK;
}
status_t CPeerGlobals::Destroy()
{
    DEL(this->mCallback);
    DEL(this->mCallbackMap);
    DEL(this->mPeerManager);
    QuitTask(&this->mTaskPeerKiller);
    QuitTask(&mTaskAcceptor);
    this->InitBasic();
    return OK;
}
status_t CPeerGlobals::SetListenPort(int port)
{
    this->mListenPort = port;
    return OK;
}

status_t CPeerGlobals::DispatchMessage(CPeerMessage *msg)
{
    ASSERT(msg);

    CMem *to = msg->GetTo();
    ASSERT(to);

    //broadcast
    if(to->StrLen() <= 0)
    {
        CPeerProxy *peer = this->mPeerManager->GetPeerByName(msg->GetFrom());
        if(peer != NULL)
        {
            int i;
            //broadcast to all connected peers
            CMemStk *all_connected = peer->GetAllConnectedPeerNames();
            for(i = 0; i < all_connected->GetLen(); i++)
            {
                CMem *name = all_connected->GetElem(i);
                CPeerProxy *pp = this->mPeerManager->GetPeerByName(name);
                if(pp != NULL)
                {
                    pp->AddPendingMessage(msg);
                }
            }

            //broadcast all peers connected to me
            for(i = 0; i < this->mPeerManager->GetTotalPeers(); i++)
            {
                CPeerProxy *p = this->mPeerManager->GetPeer(i);
                if(p == peer) continue;

                if(peer->IsPeerConnectedMe(p))
                {
                    p->AddPendingMessage(msg);
                }
            }
        }
    }
    else
    {
        CPeerProxy *peer = this->mPeerManager->GetPeerByName(msg->GetTo());
        if(peer == NULL)
        {
            const char *name = to->CStr();
            peer = this->NewPendingPeerProxy(name);
            ASSERT(peer);
        }
        else if(msg->GetFunc() == PEER_FUNC_RESET_PEER_PROXY)
        {
            peer->Reset();
        }

        return peer->AddPendingMessage(msg);
    }
    return OK;
}

status_t CPeerGlobals::AutoKillPeers(uint32_t interval)
{
    for(int i = 0; i < this->mPeerManager->GetTotalPeers(); i++)
    {
        CPeerProxy *p = this->mPeerManager->GetPeer(i);
        if(p->IsConnectionAlive())
        {
            p->SetLifeTime(0);
        }
        else
        {
            uint32_t ltime = p->GetLifeTime();
            if(interval < _PEER_PROXY_MAX_LIFE_TIME)
                p->SetLifeTime(ltime+interval);
            if(p->GetLifeTime() >= _PEER_PROXY_MAX_LIFE_TIME)
            {
                this->NotifyPeerDeleted(p);
                XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
                    "delete peer \"%s\" for inactive too long.",p->GetNameStr()
                );
                this->mPeerManager->DeletePeer(i);
                i--;
            }
        }
    }
    return OK;
}
status_t CPeerGlobals::StartTaskPeerKiller()
{
    ASSERT(!IsTask(this->mTaskPeerKiller));

    BEGIN_CLOSURE_FUNC(auto_kill)
    {
        CLOSURE_PARAM_PTR(CPeerGlobals*,self,10);
        ASSERT(self);
        CLOSURE_PARAM_INT(interval,11);
        self->AutoKillPeers(interval);
        return OK;
    }
    END_CLOSURE_FUNC(auto_kill);

    uint32_t interval = 10*1000;
    CTaskTimer *pt = CTaskTimer::NewTimer(this->GetTaskMgr(),interval,false);
    pt->Callback()->SetFunc(auto_kill);
    pt->Callback()->SetParamPointer(10,this);
    pt->Callback()->SetParamInt(11,interval);
    this->mTaskPeerKiller = pt->GetId();
    return OK;
}

status_t CPeerGlobals::StartAccept(int port)
{
    ASSERT(!IsTask(mTaskAcceptor));

    BEGIN_CLOSURE_FUNC(on_accept)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(CTaskMgr*,mgr,10);
        CLOSURE_PARAM_PTR(CPeerGlobals*,self,11);

        if(event == CTaskTcpAcceptor::EVENT_NEW_CLIENT)
        {
            CLOSURE_PARAM_INT(fd,1);
            CTaskPeerServer *server;
            NEW(server,CTaskPeerServer);
            server->Init(mgr,self);
            server->SetMaxRetries(1);
            server->SetSocket(fd);
            server->Start();
        }
        else if(event == CTaskTcpAcceptor::EVENT_STOP)
        {
            CClosure *callback = self->Callback();
            callback->Run(CPeerGlobals::EVENT_ACCEPTOR_ERROR);
        }
        return OK;
    }
    END_CLOSURE_FUNC(on_accept);

    CTaskTcpAcceptor *acceptor;
    NEW(acceptor,CTaskTcpAcceptor);
    acceptor->Init(GetTaskMgr());
    acceptor->SetListenPort(port);
    acceptor->Start();
    acceptor->Callback()->SetFunc(on_accept);
    acceptor->Callback()->SetParamPointer(10,GetTaskMgr());
    acceptor->Callback()->SetParamPointer(11,this);
    mTaskAcceptor = acceptor->GetId();
    return OK;
}

CClosure *CPeerGlobals::Callback()
{
    return this->mCallback;
}

status_t CPeerGlobals::NotifyNewPeer(CPeerProxy *peer)
{
    ASSERT(peer);
    mCallback->SetParamPointer(1,peer);
    mCallback->Run(EVENT_NEW_PEER);
    return OK;
}

status_t CPeerGlobals::NotifyPeerDeleted(CPeerProxy *peer)
{
    ASSERT(peer);
    mCallback->SetParamPointer(1,peer);
    mCallback->Run(EVENT_PEER_DELETED);
    return OK;
}

status_t CPeerGlobals::NotifyPeerDisconnected(CPeerProxy *peer)
{
    ASSERT(peer);
    mCallback->SetParamPointer(1,peer);
    mCallback->Run(EVENT_PEER_DISCONNECTED);
    return OK;
}

CPeerProxy* CPeerGlobals::NewPendingPeerProxy(const char *name)
{
    ASSERT(name);
    CPeerProxy *peer;
    NEW(peer,CPeerProxy);
    peer->Init(this->GetTaskMgr());
    peer->SetMaxConnectedPeers(64);
    peer->SetName(name);

    if(!this->mPeerManager->AddPeer(peer))
    {
        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
            "add pending peer \"%s\" fail.",name
        );
        DEL(peer);
        return NULL;
    }

    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
        "new pending peer \"%s\"",name
    );

    return peer;
}
bool CPeerGlobals::InTrustMode()
{
    return mTrustMode;
}

status_t CPeerGlobals::SetTrustMode(bool enable)
{
    mTrustMode = enable;
    return OK;
}

int CPeerGlobals::GetTotalSendingQueueLength()
{
	int t = 0;
	for(int i = 0; i < mPeerManager->GetTotalPeers(); i++)
	{
		CPeerProxy *peer = mPeerManager->GetPeer(i);
		ASSERT(peer);
		t += peer->GetSendingQueueLength();
	}
	return t;
}

CCallbackMap *CPeerGlobals::GetCallbackMap()
{
    return mCallbackMap;
}

CPeerProxyManager *CPeerGlobals::GetPeerProxyManager()
{
    return mPeerManager;
}

CPeerProxy *CPeerGlobals::GetPeerProxyByName(const char *name)
{
    ASSERT(name);
    return mPeerManager->GetPeerByName(name);
}

bool CPeerGlobals::IsPeerConnected(const char *name)
{
    ASSERT(name);
    CPeerProxy *proxy = this->GetPeerProxyByName(name);
    if(!proxy)return false;
    return proxy->IsConnected();
}
