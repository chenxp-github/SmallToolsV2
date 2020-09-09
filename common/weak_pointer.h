#ifndef __WEAK_POINTER_H
#define __WEAK_POINTER_H

#include "raw_weak_pointer.h"
#include "mem_tool.h"

template <class T>
class CWeakPointer:public CRawWeakPointer{
public:
    CWeakPointer()
    {
        this->InitBasic();
    }

    CWeakPointer(T *t)
    {
        this->InitBasic();
        this->WeakRef(t);
    }

    ~CWeakPointer()
    {   
        this->Destroy();
    }

    status_t Copy(CWeakPointer<T> *that)
    {       
        return this->WeakRef(that->get());
    }

    status_t WeakRef(T *t)
    {
        if(!t)
        {
            this->Clear();
            return OK;
        }
        WEAK_REF_CONTEXT_CREATE(t->__weak_ref_context);
        return CRawWeakPointer::WeakRef(t,t->__weak_ref_context);
    }

    T *get()
    {
        return (T*)this->GetRawPtr();
    }
    
    T* operator->()
    {
        return this->get();
    }
};

#endif

