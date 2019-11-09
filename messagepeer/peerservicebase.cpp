#include "peerservicebase.h"
#include "peerglobals.h"
#include "syslog.h"
#include "mem_tool.h"

#define CALLBACK_TIMEOUT    10*1000
#define CHECK_SAME_THREAD() ASSERT(m_CreateThreadId == crt_get_current_thread_id())

CPeerServiceBase::CPeerServiceBase()
{
    this->InitBasic();
}
CPeerServiceBase::~CPeerServiceBase()
{
    this->Destroy();
}
status_t CPeerServiceBase::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    TASK_CONTAINER_CLEAR();
    m_Flags = 0;
    m_ServerSidePeer.InitBasic();
    m_ClientSidePeer.InitBasic();
    m_CreateThreadId = 0;
    return OK;
}

status_t CPeerServiceBase::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    WEAK_REF_ID_INIT();
    this->SetTaskMgr(mgr);
    m_CreateThreadId = crt_get_current_thread_id();
    return OK;
}

status_t CPeerServiceBase::InitServiceSidePeer()
{
    m_ServerSidePeer.Init(GetTaskMgr());
    SetIsPeerInitiated(true);
    SetIsInServiceSide(true);
    return OK;
}

status_t CPeerServiceBase::InitClientSidePeer(const char *server, int port)
{
    ASSERT(server);
    m_ClientSidePeer.Init(GetTaskMgr());
    m_ClientSidePeer.SetServer(server);
    m_ClientSidePeer.SetPort(port);
    SetIsPeerInitiated(true);
    SetIsInServiceSide(false);
    return OK;
}

status_t CPeerServiceBase::Destroy()
{
    m_ClientSidePeer.Destroy();
    m_ServerSidePeer.Destroy();
    this->InitBasic();
    return OK;
}

static status_t on_peer_got_message(CClosure *closure)
{
    CLOSURE_PARAM_INT(event,0);
    CLOSURE_PARAM_PTR(CPeerServiceBase*,self,10);

    if(event == PEER_EVENT_CONNECTED)
        self->OnSocketConnected();

    if(event == PEER_EVENT_STOPPED || event == PEER_EVENT_DISCONNECTED)    
        self->OnSocketDisconnected();

    if(event == PEER_EVENT_GOT_MESSAGE)
    {
        CLOSURE_PARAM_PTR(CPeerMessage*,msg,1);
        if(msg->GetMessageType() == CPeerMessage::REQUEST || msg->GetMessageType() == CPeerMessage::SINGAL)
            self->OnRequest(msg);
        else if(msg->GetMessageType()==CPeerMessage::RESPONSE)
            self->OnResponse(msg);
        else if(msg->GetMessageType()==CPeerMessage::PART_RESPONSE)
            self->OnPartResponse(msg);
    }
    return OK;
}

status_t CPeerServiceBase::Start()
{
    ASSERT(IsPeerInitiated());

    if(IsInServiceSide())
    {
        CClosure* cb = m_ServerSidePeer.Callback();
        cb->SetParamPointer(10,this);
        cb->SetFunc(on_peer_got_message);
        return m_ServerSidePeer.Start();
    }
    else
    {
        CClosure* cb = m_ClientSidePeer.Callback();
        cb->SetParamPointer(10,this);
        cb->SetFunc(on_peer_got_message);
        return m_ClientSidePeer.Start();
    }
}

status_t CPeerServiceBase::SetName(const char *name)
{
    ASSERT(name);
    ASSERT(IsPeerInitiated());
    if(IsInServiceSide())
        return m_ServerSidePeer.SetName(name);
    else
        return m_ClientSidePeer.SetName(name);
}

status_t CPeerServiceBase::SendRequest(CPeerMessage *msg)
{
    ASSERT(msg);
    msg->SetMessageType(CPeerMessage::REQUEST);
    return this->SendMessage(msg);
}

status_t CPeerServiceBase::SendResponse(CPeerMessage *msg)
{
    ASSERT(msg);
    msg->SetMessageType(CPeerMessage::RESPONSE);
    return this->SendMessage(msg);
}

status_t CPeerServiceBase::SendPartResponse(CPeerMessage *msg)
{
    ASSERT(msg);
    msg->SetMessageType(CPeerMessage::PART_RESPONSE);
    return this->SendMessage(msg);
}

status_t CPeerServiceBase::SendSignal(CPeerMessage *msg)
{
    ASSERT(msg);
    msg->SetMessageType(CPeerMessage::SINGAL);
    return this->SendMessage(msg);
}

status_t CPeerServiceBase::SetCanFetchMessage(bool can)
{
    if(IsInServiceSide())
        return m_ServerSidePeer.SetCanFetchMessage(can);
    else
        return m_ClientSidePeer.SetCanFetchMessage(can);
}

status_t CPeerServiceBase::StartFetchMessageTask()
{
    if(IsInServiceSide())
        return m_ServerSidePeer.StartFetchMessageTask();
    return OK;
}

status_t CPeerServiceBase::OnResponse(CPeerMessage *msg)
{
    ASSERT(msg);
    ASSERT(IsPeerInitiated());
	
    GLOBAL_PEER_CALLBACK_MAP(map);
    CCallback *cb = map->Get(msg->GetCallbackId());
    if(cb != NULL)
    {
        CClosure *closure = cb->GetClosure();
        ASSERT(closure);
        closure->SetParamPointer(1,msg);
		
        if(msg->GetBodyType()==CPeerMessage::BSON)
        {
            CMiniBson bson;
            bson.Init();
            bson.LoadRawBuf(msg->GetBody());
            closure->SetParamPointer(2,&bson);
            closure->Run(OK);
        }
        else
        {
            closure->SetParamPointer(2,msg->GetBody());
            closure->Run(OK);
        }
        cb->DelClosure();
        map->Del(cb);
    }
    return OK;
}

status_t CPeerServiceBase::OnPartResponse(CPeerMessage *msg)
{
    ASSERT(msg);
    ASSERT(IsPeerInitiated());
	
    GLOBAL_PEER_CALLBACK_MAP(map);
    CCallback *cb = map->Get(msg->GetCallbackId());
    if(cb != NULL)
    {
        CClosure *closure = cb->GetClosure();
        ASSERT(closure);
        closure->SetParamPointer(1,msg);
		
        if(msg->GetBodyType()==CPeerMessage::BSON)
        {
            CMiniBson bson;
            bson.Init();
            bson.LoadRawBuf(msg->GetBody());
            closure->SetParamPointer(2,&bson);
            closure->Run(PART_OK);
        }
        else
        {
            closure->SetParamPointer(2,msg->GetBody());
            closure->Run(PART_OK);
        }
		cb->ResetTimeout();
    }
    return OK;
}

status_t CPeerServiceBase::PostCallback(CClosure *closure, int cb_id)
{
    CHECK_SAME_THREAD();
    ASSERT(closure);
    ASSERT(closure->IsOnHeap());

    GLOBAL_PEER_CALLBACK_MAP(map);
    ASSERT(map->Get(cb_id) == NULL);

    CCallback *tmp;
    NEW(tmp,CCallback);
    tmp->Init();
    tmp->SetTimeout(CALLBACK_TIMEOUT);
    tmp->SetCallbackId(cb_id);
    tmp->SetClosure(closure);
    return map->PutPtr(tmp);
}

status_t CPeerServiceBase::SendMessage(CPeerMessage *msg)
{
    ASSERT(msg);
    ASSERT(IsPeerInitiated());
    CHECK_SAME_THREAD();
    if(IsInServiceSide())
        return m_ServerSidePeer.SendMessage(msg);
    else
        return m_ClientSidePeer.SendMessage(msg);
}

status_t CPeerServiceBase::Connect(const char *peer_name)
{
    ASSERT(peer_name);
    ASSERT(IsPeerInitiated());
    CHECK_SAME_THREAD();
    if(IsInServiceSide())
        return m_ServerSidePeer.Connect(peer_name);
    else
        return m_ClientSidePeer.Connect(peer_name);
}

const char * CPeerServiceBase::GetName()
{
    ASSERT(IsPeerInitiated());
    CHECK_SAME_THREAD();
    if(IsInServiceSide())
        return m_ServerSidePeer.GetNameStr();
    else
        return m_ClientSidePeer.GetNameStr();
}

bool CPeerServiceBase::IsConnected()
{
    ASSERT(IsPeerInitiated());
    if(IsInServiceSide())
        return true;
    else
        return m_ClientSidePeer.IsConnected();
}

int CPeerServiceBase::GetSendingQueueLength()
{
    ASSERT(IsPeerInitiated());
    if(IsInServiceSide())
        return m_ServerSidePeer.GetSendingQueueLength();
    else
        return m_ClientSidePeer.GetSendingQueueLength();
}

status_t CPeerServiceBase::SetMaxRetries(int max_retries)
{
    ASSERT(IsPeerInitiated());
    ASSERT(!IsInServiceSide());
    return m_ClientSidePeer.SetMaxRetries(max_retries);
}

bool CPeerServiceBase::IsClosedPermanently()
{
    ASSERT(IsPeerInitiated());
    ASSERT(!IsInServiceSide());
    return m_ClientSidePeer.IsClosedPermanently();
}

int CPeerServiceBase::GetRetries()
{
    ASSERT(IsPeerInitiated());
    ASSERT(!IsInServiceSide());
    return m_ClientSidePeer.GetRetries();
}

status_t CPeerServiceBase::SetRetries(int retry)
{
    ASSERT(IsPeerInitiated());
    ASSERT(!IsInServiceSide());
    return m_ClientSidePeer.SetRetries(retry);
}

status_t CPeerServiceBase::ClearSendingQueue()
{
    ASSERT(IsPeerInitiated());
    if(IsInServiceSide())
    {
        return m_ServerSidePeer.ClearSendingQueue();
    }
    else
    {
        return m_ClientSidePeer.ClearSendingQueue();
    }
}

status_t CPeerServiceBase::OnSocketConnected()
{
    return OK;
}

status_t CPeerServiceBase::OnSocketDisconnected()
{
    return OK;
}
