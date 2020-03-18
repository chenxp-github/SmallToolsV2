#include "raw_weak_pointer.h"
#include "mem_tool.h"
#include "syslog.h"

CRawWeakPointer::CRawWeakPointer()
{
    this->InitBasic();
}

CRawWeakPointer::~CRawWeakPointer()
{
    this->Destroy();
}

status_t CRawWeakPointer::InitBasic()
{
    this->m_context = NULL;
    this->m_WeakRefId = 0;
    this->m_RawPtr = 0;
    return OK;
}

status_t CRawWeakPointer::Init()
{
    this->InitBasic();
    return OK;
}

status_t CRawWeakPointer::Destroy()
{
    this->Release();
    this->InitBasic();
    return OK;
}

status_t CRawWeakPointer::Clear()
{
    return this->Destroy();
}

status_t CRawWeakPointer::Release()
{
    if(m_context)
    {
        WEAK_REF_CONTEXT_RELEASE(m_context);
        m_context = NULL;
    }
    return OK;
}

status_t CRawWeakPointer::AddRef()
{
    if(m_context)    
    {
        WEAK_REF_CONTEXT_ADD_REF(m_context);
    }
    return OK;
}

status_t CRawWeakPointer::WeakRef(const void *ptr,WeakRefContext_t *context)
{
    if(!ptr)return ERROR;
    if(!context)return ERROR;
    if(m_RawPtr == ptr) return OK;
    
    this->Release();
    m_context = context;    
    m_WeakRefId = context->weak_ref_id;        
    m_RawPtr = (void*)ptr;
    this->AddRef();
    return OK;
}

void *CRawWeakPointer::GetRawPtr()
{
    if(!this->IsValid())
    {
        return NULL;
    }
    return this->m_RawPtr;
}

bool CRawWeakPointer::IsValid()
{
    if(this->m_RawPtr == NULL)
        return false;
    if(this->m_WeakRefId == 0)
        return false;
    if(!m_context)
        return false;
    if(m_context->weak_ref_id == 0)
        return false;
    if(m_context->weak_ref_id != this->m_WeakRefId)
        return false;
    return true;
}

WeakRefContext_t *CRawWeakPointer::GetWeakRefContext()
{
    return m_context;
}

status_t CRawWeakPointer::Copy(CRawWeakPointer *wp)
{
    ASSERT(wp);
    return this->WeakRef(wp->GetRawPtr(),wp->GetWeakRefContext());
}