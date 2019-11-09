#include "taskpeerserver.h"
#include "syslog.h"
#include "peerglobals.h"
#include "peercommon.h"

CTaskPeerServer::CTaskPeerServer()
{
    this->InitBasic();
}
CTaskPeerServer::~CTaskPeerServer()
{
    this->Destroy();
}
status_t CTaskPeerServer::InitBasic()
{
    CTaskLinkRpc::InitBasic();
    this->mRecvDataBuf = NULL;
    this->mRecvHeadBuf = NULL;
    this->mSendHeadBuf = NULL;
    iHostProxy.InitBasic();
    this->quit_after_send = false;
    return OK;
}
status_t CTaskPeerServer::Init(CTaskMgr *mgr)
{
    this->Destroy();
    CTaskLinkRpc::Init(mgr);

    NEW(this->mRecvDataBuf,CMem);
    this->mRecvDataBuf->Init();
    this->SetDataBuf(this->mRecvDataBuf);

    NEW(this->mRecvHeadBuf,CMem);
    this->mRecvHeadBuf->Init();
    this->mRecvHeadBuf->Malloc(4096);
    this->SetHeaderBuf(this->mRecvHeadBuf);

    NEW(this->mSendHeadBuf,CMem);
    this->mSendHeadBuf->Init();
    this->mSendHeadBuf->Malloc(4096);
    return OK;
}
status_t CTaskPeerServer::Destroy()
{
    if(iHostProxy.get())
    {
        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
            "peer server \"%s\" exit",this->iHostProxy->GetNameStr()
        );
    }
    DEL(this->mSendHeadBuf);
    DEL(this->mRecvHeadBuf);
    DEL(this->mRecvDataBuf);
    CTaskLinkRpc::Destroy();
    this->InitBasic();
    return OK;
}

///////////////////////////////////////////////////////////////////////////
status_t CTaskPeerServer::OnGetSocket()
{
    return OK;
}

bool CTaskPeerServer::CanReadNextPackage()
{
    PEER_GLOBALS(g);
	//if too many pending message to send, then
	//just stop reading new messages
	if(g->GetTotalSendingQueueLength() > MESSAGE_PEER_SENDING_QUEUE_LEN/2)
	{
		return false;
	}
    return !quit_after_send;
}

status_t CTaskPeerServer::OnGotPackageHeader(LINKRPC_HEADER *header,CMem *header_data)
{
    this->mRecvDataBuf->Destroy();
    this->mRecvDataBuf->Init();
    this->mRecvDataBuf->Malloc(header->data_size);
    return OK;
}

status_t CTaskPeerServer::OnInitNameMessage(CPeerMessage *msg)
{
	ASSERT(msg);
	PEER_GLOBALS(g);
	
	CMem *name = msg->GetFrom();
	GLOBAL_PEER_MANAGER(mgr);
	CPeerProxy *peer = mgr->GetPeerByName(name);
	
	if(peer && g->InTrustMode() && peer->IsConnectionAlive())
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
		tmp.SetFrom(&mem);
		tmp.SetBodyType(CPeerMessage::RAW);
		tmp.SerializeHeader(this->mSendHeadBuf);
		this->quit_after_send = true;
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
			this->mSendHeadBuf->SetSize(0);
			tmp.SerializeHeader(this->mSendHeadBuf);
			this->quit_after_send = true;
		}
		else
		{
			XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
				"active existing peer \"%s\"",name->CStr()
            );
			iHostProxy.WeakRef(peer);
			peer->AttachTaskPeerServer(this->GetId());
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
		peer->Init(this->GetTaskMgr());
		peer->SetMaxConnectedPeers(64);
		peer->SetName(name);
		iHostProxy.WeakRef(peer);
		peer->AttachTaskPeerServer(this->GetId());
		peer->AddAllPeers(&all_peers);
		mgr->AddPeer(peer);
        g->NotifyNewPeer(peer);
    }

	return OK;
}

status_t CTaskPeerServer::OnGotPackageData(LINKRPC_HEADER *header,CMem *header_data,CFileBase *data)
{
    PEER_GLOBALS(g);

    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->AddRef();
    header_data->Seek(0);
    msg->UnSerializeHeader(header_data);

    if(!msg->CheckHeader())
    {
        msg->Release();
        this->Stop(ERROR_WRONG_HEADER_FORMAT);
        return ERROR;
    }

    msg->TransferBody(this->mRecvDataBuf);

    if(msg->GetFunc() == PEER_FUNC_INIT_NAME)
    {
		this->OnInitNameMessage(msg);
    }
    else
    {
        msg->SetFrom(this->iHostProxy->GetName());
        g->DispatchMessage(msg);
    }

    msg->Release();
    return OK;
}

status_t CTaskPeerServer::OnPreparePackageToSend()
{
    if(quit_after_send)
    {
        this->SendResponse(this->mSendHeadBuf,NULL);
        return OK;
    }

    if(this->iHostProxy.get()==NULL)
        return ERROR;

    CPeerMessageStk *queue = this->GetSendingQueue();
    ASSERT(queue);

    CPeerMessage *msg = queue->GetElem(0);
    if(msg)
    {
        this->mSendHeadBuf->SetSize(0);
        msg->SerializeHeader(this->mSendHeadBuf);
        this->SendRequest(this->mSendHeadBuf,msg->GetBody());
    }
    return OK;
}

status_t CTaskPeerServer::OnPackageSendOk()
{
    if(quit_after_send)
    {
        this->Stop(ERROR_NONE);
        return OK;
    }
    CPeerMessageStk *queue = this->GetSendingQueue();
    ASSERT(queue);
    queue->DelElem(0);
    return OK;
}

///////////////////////////////////////////////////////////////////////////
CPeerMessageStk * CTaskPeerServer::GetSendingQueue()
{
    ASSERT(iHostProxy.get());
    return this->iHostProxy->mPendingMessages;
}

status_t CTaskPeerServer::QuitAfterSend()
{
    this->quit_after_send = true;
    return OK;
}

status_t CTaskPeerServer::OnSocketError()
{
	ASSERT(iHostProxy.get());
	return iHostProxy->OnSocketDisconnected();	
}
