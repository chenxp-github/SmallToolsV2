#include "websocketservicebase.h"
#include "syslog.h"
#include "mem_tool.h"

CWebSocketServiceBase::CWebSocketServiceBase()
{
    this->InitBasic();
}

CWebSocketServiceBase::~CWebSocketServiceBase()
{
    this->Destroy();
}

status_t CWebSocketServiceBase::InitBasic()
{
    CWebSocketServer::InitBasic();
    this->m_CallbackMap.InitBasic();
    return OK;
}

status_t CWebSocketServiceBase::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CWebSocketServer::Init(mgr);
    this->m_CallbackMap.Init(mgr,4096);
    this->SetupCallback();
    m_CallbackMap.StartAutoTimeoutTimer();
    return OK;
}

status_t CWebSocketServiceBase::Destroy()
{
    CWebSocketServer::Destroy();
    this->m_CallbackMap.Destroy();
    this->InitBasic();
    return OK;
}

CCallbackMap* CWebSocketServiceBase::GetCallbackMap()
{
    return &m_CallbackMap;
}


status_t CWebSocketServiceBase::SendRequest(CWebSocketParamBase *params,int method, int callback)
{
    ASSERT(params);
    CMiniBson bson;
    bson.Init();
    bson.StartDocument();
    params->SaveBson(&bson);
    bson.EndDocument();

	CWebSocketMessage *msg;
	NEW(msg,CWebSocketMessage);
	msg->Init();
	msg->SetIsOnHeap(true);
	msg->SetMethod(method);
	msg->SetCallbackId(callback);
	msg->SetMessageType(CWebSocketMessage::REQUEST);
	msg->SetDataType(CWebSocketMessage::TYPE_BSON);
	msg->TransferData(bson.GetRawData());

	return this->SendMessage(msg);
}

status_t CWebSocketServiceBase::SendReturnValue(CWebSocketCallContext *context, CWebSocketParamBase *ret)
{
    ASSERT(context && ret);
	
    CMiniBson bson;
    bson.Init();
    bson.StartDocument();
    ret->SaveBson(&bson);
    bson.EndDocument();

	CWebSocketMessage *msg;
	NEW(msg,CWebSocketMessage);
	msg->Init();
	msg->SetIsOnHeap(true);
	msg->SetMessageType(CWebSocketMessage::RESPONSE);
	msg->SetDataType(CWebSocketMessage::TYPE_BSON);
	msg->SetCallbackId(context->GetCallbackId());
	msg->SetMethod(context->GetMethod());
	msg->TransferData(bson.GetRawData());
	
	return this->SendMessage(msg);
}

status_t CWebSocketServiceBase::SendPartReturnValue(CWebSocketCallContext *context, CWebSocketParamBase *ret)
{
    ASSERT(context && ret);
	
    CMiniBson bson;
    bson.Init();
    bson.StartDocument();
    ret->SaveBson(&bson);
    bson.EndDocument();
	
	CWebSocketMessage *msg;
	NEW(msg,CWebSocketMessage);
	msg->Init();
	msg->SetIsOnHeap(true);
	msg->SetMessageType(CWebSocketMessage::PART_RESPONSE);
	msg->SetDataType(CWebSocketMessage::TYPE_BSON);
	msg->SetCallbackId(context->GetCallbackId());
	msg->SetMethod(context->GetMethod());
	msg->TransferData(bson.GetRawData());
	
	return this->SendMessage(msg);
}

status_t CWebSocketServiceBase::SetupCallback()
{
    BEGIN_CLOSURE_FUNC(func)
    {
        CLOSURE_PARAM_PTR(CWebSocketServiceBase*,self,10);
        CLOSURE_PARAM_INT(e,0);

        if(e == CWebSocketServer::EVENT_GOT_PAYLOAD)
        {
            CLOSURE_PARAM_PTR(CMem*,payload,1);
            CLOSURE_PARAM_PTR(CWebSocketFrameHeader*,header,2);
			
			CWebSocketMessage message;
			message.Init();
			message.AttachPayload(payload);

			if(message.GetMessageType() == CWebSocketMessage::REQUEST)
				return self->OnRequest(&message);

			if(message.GetMessageType() == CWebSocketMessage::RESPONSE)
				return self->OnResponse(&message);

			if(message.GetMessageType() == CWebSocketMessage::PART_RESPONSE)
				return self->OnPartResponse(&message);			
        }
        return OK;
    }
    END_CLOSURE_FUNC(func);
	
    this->Callback()->SetFunc(func);
    this->Callback()->SetParamPointer(10,this);
    return OK;
}

status_t CWebSocketServiceBase::OnRequest(CWebSocketMessage *_message)
{
	ASSERT(0); //not expected
	return OK;
}

int CWebSocketServiceBase::AddCallback(CClosure *closure)
{
    if(!closure)return 0;

    int callback_id = crt_get_unique_id();
    	
    if(!m_CallbackMap.AddClosure(closure,callback_id))
    {
        callback_id = 0;
    }
    return callback_id;
}


status_t CWebSocketServiceBase::OnResponse(CWebSocketMessage *msg)
{
    ASSERT(msg);
	
    CCallback *cb = m_CallbackMap.Get(msg->GetCallbackId());
    if(cb != NULL)
    {
        CClosure *closure = cb->GetClosure();
        ASSERT(closure);
        closure->SetParamPointer(1,msg);
		
        if(msg->GetDataType()==CWebSocketMessage::TYPE_BSON)
        {
            CMiniBson bson;
            bson.Init();
            bson.LoadRawBuf(msg->GetData());
            closure->SetParamPointer(2,&bson);
            closure->Run(OK);
        }
        else
        {
            closure->SetParamPointer(2,msg->GetData());
            closure->Run(OK);
        }
        cb->DelClosure();
        m_CallbackMap.Del(cb);
    }
    return OK;
}

status_t CWebSocketServiceBase::OnPartResponse(CWebSocketMessage *msg)
{
    ASSERT(msg);
	
    CCallback *cb = m_CallbackMap.Get(msg->GetCallbackId());
    if(cb != NULL)
    {
        CClosure *closure = cb->GetClosure();
        ASSERT(closure);
        closure->SetParamPointer(1,msg);
		
        if(msg->GetDataType()==CWebSocketMessage::TYPE_BSON)
        {
            CMiniBson bson;
            bson.Init();
            bson.LoadRawBuf(msg->GetData());
            closure->SetParamPointer(2,&bson);
            closure->Run(WS_PART_OK);
        }
        else
        {
            closure->SetParamPointer(2,msg->GetData());
            closure->Run(WS_PART_OK);
        }
    }
    return OK;
}
