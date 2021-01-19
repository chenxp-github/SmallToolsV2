#include "serversidepeer.h"
#include "syslog.h"
#include "mem_tool.h"
#include "peercommon.h"
#include "peerglobals.h"
#include "tasktimer.h"
#include "socketreaderwriter.h"

CServerSidePeer::CServerSidePeer()
{
    this->InitBasic();
}
CServerSidePeer::~CServerSidePeer()
{
    this->Destroy();
}
status_t CServerSidePeer::InitBasic()
{
    WEAK_REF_CLEAR();
    PEER_GLOBAL_CONTEXT_CLEAR();
    this->quit = false;
    this->iHostProxy.InitBasic();
    this->mName = NULL;
    this->mAllConnectedPeers = NULL;
    this->mCallback = NULL;
    this->mTaskFetchMessage = 0;
    this->can_fetch_message = false;
    return OK;
}
status_t CServerSidePeer::Init(CTaskMgr *mgr,const void *peer_globals)
{
    this->InitBasic();
    this->SetPeerGlobalContext(peer_globals);
    this->SetTaskMgr(mgr);
    NEW(this->mName,CMem);
    this->mName->Init();

    NEW(this->mAllConnectedPeers,CMemStk);
    this->mAllConnectedPeers->Init(64);

    NEW(this->mCallback,CClosure);
    this->mCallback->Init();
    return OK;
}
status_t CServerSidePeer::Destroy()
{
    WEAK_REF_DESTROY();
    DEL(this->mCallback);
    DEL(this->mAllConnectedPeers);
    DEL(this->mName);
    iHostProxy.Destroy();
    this->InitBasic();
    return OK;
}

status_t CServerSidePeer::OnInitNameMessage(CPeerMessage *msg)
{
	ASSERT(msg);
	PEER_GLOBALS(g);

	CMem *name = msg->GetFrom();
	GLOBAL_PEER_MANAGER(mgr);
	CPeerProxy *peer = mgr->GetPeerByName(name);
	
	if(peer && g->InTrustMode())
	{
		int index = mgr->NameToIndex(name);
		if(index  >= 0)
		{
			XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
				"delete existing peer \"%s\" in trust mode",name->CStr()
            );
			mgr->DeletePeer(index);
			peer = NULL;
		}
	}
	
	CMem *body = msg->GetBody();
	ASSERT(body);
	
	MessagePeerInitParam_t init_param;
	body->Seek(0);
	body->Read(&init_param,sizeof(init_param));

	if(	   init_param.version != MESSAGE_PEER_VERSION 
		|| init_param.size != sizeof(init_param) 
		|| init_param.socket_rw_timeout != SOCKETRW_TIMEOUT)
	{
		CPeerMessage tmp;
		tmp.Init();
		tmp.SetFunc(PEER_FUNC_INIT_CHECK_FAIL);
		LOCAL_MEM(mem);
		if(init_param.version != MESSAGE_PEER_VERSION)
			mem.Printf("message peer version check fail %d vs %d",MESSAGE_PEER_VERSION,init_param.version);
		else if(init_param.size != sizeof(init_param))
			mem.Printf("message peer size check fail %d vs %d",sizeof(init_param),init_param.size);
		else if(init_param.socket_rw_timeout != SOCKETRW_TIMEOUT)
			mem.Printf("message peer SOCKETRW_TIMEOUT check fail %d vs %d",SOCKETRW_TIMEOUT,init_param.socket_rw_timeout);
        tmp.SetBodyType(CPeerMessage::RAW);
		tmp.SetFrom(&mem);
		this->OnGotMessage(&tmp);
		this->quit = true;
		return ERROR;
	}

	CMem all_peers;
	all_peers.Init();
	all_peers.SetRawBuf(body->GetRawBuf(),sizeof(init_param),true);
	
	if(peer != NULL)
	{
		if(peer->IsConnectionAlive())
		{
			CPeerMessage tmp;
			tmp.Init();
			tmp.SetFunc(PEER_FUNC_INIT_CHECK_FAIL);
			LOCAL_MEM(mem);
			mem.Printf("peer name \"%s\" already exist",name->CStr());
			tmp.SetFrom(&mem);
			tmp.SetBodyType(CPeerMessage::RAW);
			this->OnGotMessage(&tmp);
			this->quit = true;
		}
		else
		{
			XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
				"active existing peer \"%s\"",name->CStr()
            );
			peer->AttachServerSidePeer(this);
			this->iHostProxy.WeakRef(peer);
			peer->SetLifeTime(0);
			peer->AddAllPeers(&all_peers);
		}
	}
	else
	{
		XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
			"new peer \"%s\"",name->CStr()
        );
		NEW(peer,CPeerProxy);
		peer->Init(GetTaskMgr());
        PASS_GLOBAL_PEER_CONTEXT(this,peer);
		peer->SetMaxConnectedPeers(64);
		peer->SetName(name);
		peer->AttachServerSidePeer(this);
		this->iHostProxy.WeakRef(peer);
		peer->AddAllPeers(&all_peers);
		mgr->AddPeer(peer);
		g->NotifyNewPeer(peer);
    }

	return OK;
}

status_t CServerSidePeer::SendMessage(CPeerMessage *msg)
{
    ASSERT(msg);

    if(IsMarkQuit())
        return ERROR;

    PEER_GLOBALS(g);

    msg->AddRef();
    if(msg->GetFunc() == PEER_FUNC_INIT_NAME)
    {
		this->OnInitNameMessage(msg);
    }
    else
    {
        ASSERT(iHostProxy.get());
        msg->SetFrom(this->iHostProxy->GetName());
        g->DispatchMessage(msg);
    }
    msg->Release();
    return OK;
}

status_t CServerSidePeer::OnGotMessage(CPeerMessage *msg)
{
    if(IsMarkQuit())
        return ERROR;
    mCallback->SetParamPointer(1,msg);
    mCallback->SetParamPointer(2,this);
    mCallback->Run(PEER_EVENT_GOT_MESSAGE);
    return OK;
}

bool CServerSidePeer::IsMarkQuit()
{
    return this->quit;
}
CMem* CServerSidePeer::GetName()
{
    return this->mName;
}
const char* CServerSidePeer::GetNameStr()
{
    return this->mName->CStr();
}
status_t CServerSidePeer::SetName(CMem* _name)
{
    this->mName->Copy(_name);
    return OK;
}
status_t CServerSidePeer::SetName(const char *_name)
{
    CMem _mem(_name);
    return this->SetName(&_mem);
}

status_t CServerSidePeer::Start()
{
    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->SetFunc(PEER_FUNC_INIT_NAME);
    msg->SetFrom(this->GetName());
	
	MessagePeerInitParam_t init_param;
	init_param.size = sizeof(init_param);
	init_param.version = MESSAGE_PEER_VERSION;
	init_param.socket_rw_timeout = SOCKETRW_TIMEOUT;

    LOCAL_MEM_WITH_SIZE(buf,32*1024);
	buf.Write(&init_param,sizeof(init_param));
    mAllConnectedPeers->SaveLines(&buf);
    msg->SetBody(&buf);

    return this->SendMessage(msg);
}

status_t CServerSidePeer::Connect(const char *peer_name)
{
    ASSERT(peer_name);
    CMem mem(peer_name);
    return this->mAllConnectedPeers->InsOrdered(&mem,0,1);
}

CClosure * CServerSidePeer::Callback()
{
    return mCallback;
}

status_t CServerSidePeer::StartFetchMessageTask()
{
    ASSERT(!IsTask(mTaskFetchMessage));
    ASSERT(iHostProxy.get());

    CTaskTimer *pt = CTaskTimer::NewTimer(GetTaskMgr(),0,0);
    CClosure *callback = pt->Callback();

    BEGIN_CLOSURE_FUNC(on_timer)
    {
        CLOSURE_PARAM_PTR(CTaskTimer*,timer,1);
        CLOSURE_PARAM_PTR(CServerSidePeer*,self,10);
        CLOSURE_PARAM_PTR(CPeerProxy*,proxy,11);

        CPeerMessageStk *queue = proxy->GetSendingQueue();
        ASSERT(queue);

        if(!self->can_fetch_message)
        {
            self->SuspendFetchMessageTask();
            return OK;
        }

        if(queue->GetLen() <= 0)
        {
            self->SuspendFetchMessageTask();
            return OK;
        }
        else
        {
            timer->TurboOn();
        }        

        CPeerMessage *msg = queue->GetElem(0);
        ASSERT(msg);
        self->OnGotMessage(msg);
        queue->DelElem(0);
        return OK;
    }
    END_CLOSURE_FUNC(on_timer);

    callback->SetFunc(on_timer);
    callback->SetParamPointer(10,this);
    callback->SetParamPointer(11,iHostProxy.get());

    mTaskFetchMessage = pt->GetId();

    return OK;
}

status_t CServerSidePeer::SuspendFetchMessageTask()
{
    return SuspendTask(mTaskFetchMessage);
}

status_t CServerSidePeer::ResumeFetchMessageTask()
{
    return ResumeTask(mTaskFetchMessage);
}

status_t CServerSidePeer::SetCanFetchMessage(bool can)
{
    this->can_fetch_message = can;
    if(can_fetch_message)
        ResumeFetchMessageTask();
    return OK;
}

int CServerSidePeer::GetSendingQueueLength()
{
  	PEER_GLOBALS(g);
	return g->GetTotalSendingQueueLength(); //return the total pending messages
}

status_t CServerSidePeer::ClearSendingQueue()
{
    if(iHostProxy.get())
    {
        return iHostProxy->ClearSendingQueue();
    }
    return ERROR;
}

int CServerSidePeer::GetAliveClientNumber()
{
    GLOBAL_PEER_MANAGER(mgr);
    return mgr->GetAliveClientNumber();
}

