#include "rpc_call_context.h"
#include "syslog.h"
#include "mem_tool.h"

CRpcCallContext::CRpcCallContext()
{
    this->InitBasic();
}

CRpcCallContext::~CRpcCallContext()
{
    this->Destroy();
}

status_t CRpcCallContext::InitBasic()
{
    this->m_from.InitBasic();
    this->m_callback_id = 0;
    this->m_method = 0;
    return OK;
}

status_t CRpcCallContext::Init()
{
    this->InitBasic();

    this->m_from.Init();
    return OK;
}

status_t CRpcCallContext::Destroy()
{
    this->m_from.Destroy();
    this->InitBasic();
    return OK;
}

status_t CRpcCallContext::Copy(CRpcCallContext *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

    this->m_from.Copy(&_p->m_from);
    this->m_callback_id = _p->m_callback_id;
    this->m_method = _p->m_method;
    return OK;
}

status_t CRpcCallContext::Comp(CRpcCallContext *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRpcCallContext::Print(CFileBase *_buf)
{

    _buf->Log("from = %s",
        m_from.StrLen()>0?m_from.CStr():"<null>"
    );
    _buf->Log("callback_id = %d",m_callback_id);
    _buf->Log("method = %d",m_method);
    return OK;
}

CMem* CRpcCallContext::GetFrom()
{
    return &m_from;
}

const char* CRpcCallContext::GetFromStr()
{
    return m_from.CStr();
}

int CRpcCallContext::GetCallbackId()
{
    return m_callback_id;
}

int CRpcCallContext::GetMethod()
{
    return m_method;
}

status_t CRpcCallContext::SetFrom(CMem *_from)
{
    ASSERT(_from);
    return this->m_from.Copy(_from);
}

status_t CRpcCallContext::SetFrom(const char *_from)
{
    CMem tmp(_from);
    return this->SetFrom(&tmp);
}

status_t CRpcCallContext::SetCallbackId(int _callback_id)
{
    this->m_callback_id = _callback_id;
    return OK;
}

status_t CRpcCallContext::SetMethod(int _method)
{
    this->m_method = _method;
    return OK;
}


status_t CRpcCallContext::FromPeerMessage(CPeerMessage *msg)
{
    ASSERT(msg);
    this->SetFrom(msg->GetFrom());
    this->SetCallbackId(msg->GetCallbackId());
    this->SetMethod(msg->GetFunc());
    return OK;
}

status_t CRpcCallContext::Searialize(CStream *stream)
{
    ASSERT(stream);
    stream->PutString(&m_from);
    stream->PutInt32(m_callback_id);
    stream->PutInt32(m_method);
    return OK;
}

status_t CRpcCallContext::Desearialize(CStream *stream)
{
    ASSERT(stream);
    stream->GetString(&m_from);
    m_callback_id = stream->GetInt32();
    m_method = stream->GetInt32();
    return OK;
}

status_t CRpcCallContext::Desearialize(const void *buf, size_t size)
{
    ASSERT(buf);
    CStream stream;
    stream.Init();
    stream.SetRawBuf((void*)buf,size,true);
    return this->Desearialize(&stream);
}

status_t CRpcCallContext::Searialize(CClosure *closure, int index)
{
    ASSERT(closure);
    LOCAL_STREAM(stream);
    this->Searialize(&stream);
    closure->Malloc(index,stream.GetRawBuf(),stream.GetSize());
    return OK;
}

status_t CRpcCallContext::Desearialize(CClosure *closure, int index)
{
    ASSERT(closure);
    ASSERT(closure->GetParamType(index) == PARAM_TYPE_MALLOC);
    void *data = closure->GetParamPointer(index);
    int size = closure->GetMallocSize(index);
    return this->Desearialize(data,size);
}


status_t CRpcCallContext::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    _bson->PutInt32("callback_id",m_callback_id);
    _bson->PutString("from",&m_from);
    _bson->PutInt32("method",m_method);
    return OK;
}

status_t CRpcCallContext::SaveBson(CMem *_mem)
{
    ASSERT(_mem);
    CMiniBson _bson;
    _bson.Init();
    _bson.SetRawBuf(_mem);
    _bson.StartDocument();
    this->SaveBson(&_bson);
    _bson.EndDocument();
    _mem->SetSize(_bson.GetDocumentSize());
    return OK;
}

status_t CRpcCallContext::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    BSON_CHECK(_bson->GetInt32("callback_id",&m_callback_id));
    /******from begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("from",&_tmp_str));
    this->SetFrom(&_tmp_str);
    /******from end*******/}
    BSON_CHECK(_bson->GetInt32("method",&m_method));
    return OK;
}

status_t CRpcCallContext::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

status_t CRpcCallContext::Clear()
{
    this->Destroy();
    this->Init();
    return OK;
}