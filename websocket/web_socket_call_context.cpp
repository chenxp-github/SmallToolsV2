#include "web_socket_call_context.h"
#include "syslog.h"
#include "mem_tool.h"

CWebSocketCallContext::CWebSocketCallContext()
{
    this->InitBasic();
}

CWebSocketCallContext::~CWebSocketCallContext()
{
    this->Destroy();
}

status_t CWebSocketCallContext::InitBasic()
{
    this->m_Method = 0;
    this->m_CallbackId = 0;
    return OK;
}

status_t CWebSocketCallContext::Init()
{
    this->InitBasic();

    return OK;
}

status_t CWebSocketCallContext::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CWebSocketCallContext::Copy(CWebSocketCallContext *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_Method = _p->m_Method;
    this->m_CallbackId = _p->m_CallbackId;
    return OK;
}
status_t CWebSocketCallContext::Comp(CWebSocketCallContext *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}
status_t CWebSocketCallContext::Print(CFileBase *_buf)
{
    _buf->Log("Method = %d",m_Method);
    _buf->Log("CallbackId = %d",m_CallbackId);
    return OK;
}
int CWebSocketCallContext::GetMethod()
{
    return m_Method;
}

int CWebSocketCallContext::GetCallbackId()
{
    return m_CallbackId;
}

status_t CWebSocketCallContext::SetMethod(int _method)
{
    this->m_Method = _method;
    return OK;
}

status_t CWebSocketCallContext::SetCallbackId(int _callbackid)
{
    this->m_CallbackId = _callbackid;
    return OK;
}

status_t CWebSocketCallContext::FromMessage(CWebSocketMessage *msg)
{
	ASSERT(msg);
	this->m_CallbackId = msg->GetCallbackId();
	this->m_Method = msg->GetMethod();
	return OK;
}
