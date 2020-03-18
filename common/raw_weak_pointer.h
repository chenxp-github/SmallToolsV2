#ifndef __RAW_WEAK_POINTER_H
#define __RAW_WEAK_POINTER_H

#include "cruntime.h"
#include "weak_ref.h"

class CRawWeakPointer{
public:
    WeakRefContext_t *m_context;
    int m_WeakRefId;
    void* m_RawPtr;
public:
    CRawWeakPointer();
    ~CRawWeakPointer();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Clear();
    bool IsValid();
    status_t WeakRef(const void *ptr,WeakRefContext_t *context);
    void *GetRawPtr();
    status_t Release();
    status_t AddRef();
    status_t Copy(CRawWeakPointer *wp);
    WeakRefContext_t *GetWeakRefContext();
};
    
#endif

