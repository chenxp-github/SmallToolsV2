#include "callback.h"
#include "syslog.h"
#include "mem_tool.h"

CCallback::CCallback()
{
    this->InitBasic();
}
CCallback::~CCallback()
{
    this->Destroy();
}
status_t CCallback::InitBasic()
{
    next = NULL;
    m_CallbackId = 0;
    i_Closure = NULL;
    m_Time = 0;
    m_Timeout = 30*1000;
    return OK;
}
status_t CCallback::Init()
{
    this->InitBasic();
    return OK;
}
status_t CCallback::Destroy()
{
    this->InitBasic();
    return OK;
}
int CCallback::Comp(CCallback *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return m_CallbackId - _p->m_CallbackId;
}
status_t CCallback::Copy(CCallback *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_CallbackId = _p->m_CallbackId;
    this->i_Closure = _p->i_Closure;
    return OK;
}
status_t CCallback::Print(CFileBase *_buf)
{
    _buf->Log("CallbackId = %d",this->m_CallbackId);
    return OK;
}
int CCallback::GetCallbackId()
{
    return this->m_CallbackId;
}
status_t CCallback::SetCallbackId(int _callbackid)
{
    this->m_CallbackId = _callbackid;
    return OK;
}

status_t CCallback::SetClosure(CClosure *closure)
{
    i_Closure = closure;
    return OK;
}

CClosure* CCallback::GetClosure()
{
    return i_Closure;
}
status_t CCallback::SetTimeout(int to)
{
    m_Timeout = to;
    return OK;
}
status_t CCallback::IncTime(int ms)
{
    m_Time += ms;
    return OK;
}
bool CCallback::IsTimeout()
{
    if(m_Timeout < 0)return false;
    return m_Time > m_Timeout;
}
status_t CCallback::DelClosure()
{
    if(i_Closure)
    {
        if(i_Closure->IsOnHeap())
            DEL(i_Closure);
    }
    return OK;
}

status_t CCallback::OnTimeout()
{
    ASSERT(i_Closure);
    i_Closure->SetIsTimeout(true);
    i_Closure->Run(E_TIMEOUT);
    return OK;
}

status_t CCallback::ResetTimeout()
{
	m_Time = 0;
	return OK;
}
