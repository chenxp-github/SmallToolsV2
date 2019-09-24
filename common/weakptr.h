#ifndef __WEAKPTR_H
#define __WEAKPTR_H

#include "cruntime.h"
#include "syslog.h"

template <class T>
class CWeakPtr{
public:
    int mWeakRefId;
    T* mRawPtr;
public: 

    CWeakPtr()
    {
        this->InitBasic();
    }
    
    CWeakPtr(T *t)
    {
        this->InitBasic();
        this->WeakRef(t);
    }

    ~CWeakPtr()
    {
        this->Destroy();
    }

    status_t InitBasic()
    {
        this->mWeakRefId = 0;
        this->mRawPtr = 0;
        return OK;
    }
    
    status_t Init()
    {
        this->InitBasic();
        return OK;
    }
    
    status_t Destroy()
    {
        this->InitBasic();
        return OK;
    }

    status_t Clear()
    {
        return this->Destroy();
    }

    status_t Copy(CWeakPtr<T> *that)
    {
        ASSERT(that);
        this->mWeakRefId = that->mWeakRefId;
        this->mRawPtr = that->mRawPtr;
        return OK;
    }

    status_t WeakRef(T *t)
    {
        ASSERT(t);
        this->mWeakRefId = t->__weak_ref_id;
        this->mRawPtr = t;
        return OK;
    }

    T *get()
    {
        if(!this->IsValid())
        {
            return NULL;
        }
        return this->mRawPtr;
    }

    bool IsValid()
    {
        if(this->mRawPtr == NULL)
            return false;
        if(mRawPtr->__weak_ref_id == 0)
            return false;
        if(this->mWeakRefId == 0)
            return false;
        if(this->mWeakRefId != mRawPtr->__weak_ref_id)
            return false;
        return true;
    }
    
    T* operator->()
    {
        ASSERT(this->get());
        return this->get();
    }

};

#endif
