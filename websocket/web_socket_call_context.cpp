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


status_t CWebSocketCallContext::Searialize(CMem *stream)
{
    ASSERT(stream);  
    stream->Write(&m_Method,sizeof(m_Method));
    stream->Write(&m_CallbackId,sizeof(m_CallbackId));
    return OK;
}

status_t CWebSocketCallContext::Desearialize(CMem *stream)
{
    ASSERT(stream);    
    stream->Read(&m_Method,sizeof(m_Method));
    stream->Read(&m_CallbackId,sizeof(m_CallbackId));
    return OK;
}

status_t CWebSocketCallContext::Desearialize(const void *buf, size_t size)
{
    ASSERT(buf);
    CMem stream;
    stream.Init();
    stream.SetRawBuf((void*)buf,size,true);
    return this->Desearialize(&stream);
}

status_t CWebSocketCallContext::Searialize(CClosure *closure, int index)
{
    ASSERT(closure);
    LOCAL_MEM(stream);
    this->Searialize(&stream);
    closure->Malloc(index,stream.GetRawBuf(),stream.GetSize());
    return OK;
}

status_t CWebSocketCallContext::Desearialize(CClosure *closure, int index)
{
    ASSERT(closure);
    ASSERT(closure->GetParamType(index) == PARAM_TYPE_MALLOC);
    void *data = closure->GetParamPointer(index);
    int size = closure->GetMallocSize(index);
    return this->Desearialize(data,size);
}
