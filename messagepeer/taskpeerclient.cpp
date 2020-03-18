#include "taskpeerclient.h"
#include "syslog.h"
#include "tasktcpconnector.h"
#include "peercommon.h"

CTaskPeerClient::CTaskPeerClient()
{
    this->InitBasic();
}
CTaskPeerClient::~CTaskPeerClient()
{
    this->Destroy();
}
status_t CTaskPeerClient::InitBasic()
{
    CTaskLinkRpc::InitBasic();
    this->iHostPeer.InitBasic();
    this->mTaskTcpConnectror = 0;
    this->mDataRecvBuf = NULL;
    this->mMessageHeader = NULL;
    this->mHeaderRecvBuf = NULL;
    mCallback = NULL;
    mCanReadNextPackage = true;
    return OK;
}
status_t CTaskPeerClient::Init(CTaskMgr *mgr)
{
    this->Destroy();

    CTaskLinkRpc::Init(mgr);

    NEW(this->mDataRecvBuf,CMem);
    this->mDataRecvBuf->Init();
    this->SetDataBuf(this->mDataRecvBuf);

    NEW(this->mMessageHeader,CMem);
    this->mMessageHeader->Init();
    this->mMessageHeader->Malloc(4096);

    NEW(this->mHeaderRecvBuf,CMem);
    this->mHeaderRecvBuf->Init();
    this->mHeaderRecvBuf->Malloc(4096);
    this->SetHeaderBuf(this->mHeaderRecvBuf);

    NEW(mCallback,CClosure);
    mCallback->Init();

    return OK;
}
status_t CTaskPeerClient::Destroy()
{
    DEL(mCallback);
    QuitTask(&this->mTaskTcpConnectror);
    DEL(this->mMessageHeader);
    DEL(this->mHeaderRecvBuf);
    DEL(this->mDataRecvBuf);
	iHostPeer.Destroy();
    CTaskLinkRpc::Destroy();
    this->InitBasic();
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
static status_t on_tcp_connector_event(CClosure *closure)
{
    CLOSURE_PARAM_INT(event,0);
    CLOSURE_PARAM_PTR(CTaskPeerClient*,self,10);
    ASSERT(self);

    if(event == CTaskTcpConnector::EVENT_STOP)
    {
        CLOSURE_PARAM_INT(err,1);
        if(err != CTaskTcpConnector::ERROR_NONE)
        {
            self->Retry(CTaskTcpConnector::ERROR_CONNECT_ERROR);
        }
    }

    else if(event == CTaskTcpConnector::EVENT_CONNECTED)
    {
        ASSERT(self->iHostPeer.get());

        CLOSURE_PARAM_PTR(CTcpClient*,client,2);
        ASSERT(client);
        self->TransferSocket(client);
        CPeerMessage *msg;
        NEW(msg,CPeerMessage);
        msg->Init();
        msg->SetFunc(PEER_FUNC_INIT_NAME);
        msg->SetFrom(self->iHostPeer->GetName());

		MessagePeerInitParam_t init_param;
		init_param.size = sizeof(init_param);
		init_param.version = MESSAGE_PEER_VERSION;
		init_param.socket_rw_timeout = SOCKETRW_TIMEOUT;
		
		CMemStk *all = self->iHostPeer->GetAllConnectedPeers();
        ASSERT(all);

        LOCAL_MEM_WITH_SIZE(buf,32*1024);
		buf.Write(&init_param,sizeof(init_param));
        all->SaveLines(&buf);
        msg->SetBody(&buf);
        //first thing , tell the server my name
        msg->AddRef();
        self->AddMessageToHead(msg);

        self->iHostPeer->OnSocketConnected();

    }
    return OK;
}

status_t CTaskPeerClient::OnGetSocket()
{
    ASSERT(this->iHostPeer.get());
    ASSERT(!IsTask(this->mTaskTcpConnectror));

    CTaskTcpConnector *pc;

    NEW(pc,CTaskTcpConnector);
    pc->Init(this->GetTaskMgr());
    pc->SetServerName(this->iHostPeer->GetServer());
    pc->SetPort(this->iHostPeer->GetPort());
    pc->Callback()->SetFunc(on_tcp_connector_event);
    pc->Callback()->SetParamPointer(10,this);
    pc->Start();

    this->mTaskTcpConnectror = pc->GetId();
    return OK;
}

bool CTaskPeerClient::CanReadNextPackage()
{
    return mCanReadNextPackage;
}

status_t CTaskPeerClient::OnGotPackageHeader(LINKRPC_HEADER *header,CMem *header_data)
{
    this->mDataRecvBuf->Destroy();
    this->mDataRecvBuf->Init();
    this->mDataRecvBuf->Malloc(header->data_size);
    return OK;
}

status_t CTaskPeerClient::OnGotPackageData(LINKRPC_HEADER *header,CMem *header_data,CFileBase *data)
{
    CPeerMessage tmp;
    tmp.Init();
    header_data->Seek(0);
    tmp.UnSerializeHeader(header_data);
    ASSERT(tmp.CheckHeader());
    tmp.TransferBody(this->mDataRecvBuf);

    if(tmp.GetFunc() == PEER_FUNC_INIT_CHECK_FAIL)
    {
        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,
            "%s",tmp.GetFrom()->CStr()
        );
        this->Stop(ERROR_INIT_CHECK_FAIL);
        return ERROR;
    }
    ASSERT(this->iHostPeer.get());
    iHostPeer->Callback()->SetParamPointer(1,&tmp);
    iHostPeer->Callback()->Run(PEER_EVENT_GOT_MESSAGE);
    return OK;
}

status_t CTaskPeerClient::OnPreparePackageToSend()
{
    CPeerMessageStk *sending_queue = this->GetSendingQueue();
    ASSERT(sending_queue);
    CPeerMessage *msg = sending_queue->GetElem(0);
    if(msg)
    {
        this->mMessageHeader->SetSize(0);
        msg->SerializeHeader(this->mMessageHeader);
        this->SendRequest(this->mMessageHeader,msg->GetBody());
    }
    return FALSE;
}

status_t CTaskPeerClient::OnPackageSendOk()
{
    CPeerMessageStk *sending_queue = this->GetSendingQueue();
    ASSERT(sending_queue);
    sending_queue->DelElem(0);
    return OK;
}

status_t CTaskPeerClient::SendMessage(CPeerMessage *msg)
{
    ASSERT(msg && this->GetSendingQueue());
    return this->GetSendingQueue()->PushPtr(msg);
}

status_t CTaskPeerClient::AddMessageToHead(CPeerMessage *msg)
{
    ASSERT(msg && this->GetSendingQueue());
    return this->GetSendingQueue()->InsertElemPtr(0,msg);
}

CPeerMessageStk * CTaskPeerClient::GetSendingQueue()
{
    ASSERT(this->iHostPeer.get());
    return this->iHostPeer->mSendingQueue;
}

const char * CTaskPeerClient::ErrorToString(int err)
{
    const char *ori_str = NULL;
    if(err == ERROR_INIT_CHECK_FAIL)
        ori_str = "peer init check fail.";
    else
        ori_str = CTaskLinkRpc::ErrorToString(err);

    static char err_buf[LBUF_SIZE];

    if(iHostPeer.get())
    {
        sprintf(err_buf,"(%s) %s",iHostPeer->GetNameStr(),ori_str);
        return err_buf;
    }
    else
    {
        return ori_str;
    }
}

CClosure* CTaskPeerClient::Callback()
{
    return mCallback;
}

status_t CTaskPeerClient::SetCanReadNextPackage(bool can_read)
{
    mCanReadNextPackage = can_read;
    return OK;
}

status_t CTaskPeerClient::Stop(int err)
{
    if(iHostPeer.get())
    {
        iHostPeer->OnTaskPeerClientStop(err);
    }
    return CTaskLinkRpc::Stop(err);
}

status_t CTaskPeerClient::OnSocketError()
{
    if(iHostPeer.get())
    {
        iHostPeer->OnSocketDisconnected();
    }
    return OK;
}