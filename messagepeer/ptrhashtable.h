#ifndef __PTRHASHTABLE_H
#define __PTRHASHTABLE_H

#include "cruntime.h"
#include "filebase.h"
#include "closure.h"

class CPtrHashNode{
public:
    void *mPtr;
    CPtrHashNode *next;
public:
    CPtrHashNode()
    {
        this->mPtr = NULL;
        this->next = NULL;
    }
    ~CPtrHashNode()
    {
        this->mPtr = NULL;
        this->next = NULL;
    }
    status_t Init(){return OK;}
    status_t Copy(CPtrHashNode *p)
    {
        this->mPtr = p->mPtr;
        return OK;
    }
    int Comp(CPtrHashNode *p)
    {
        return ((int_ptr_t)(this->mPtr)) - ((int_ptr_t)(p->mPtr));
    }
    int_ptr_t GetIntValue()
    {
        return (int_ptr_t)this->mPtr;
    }
    void* Get()
    {
        return this->mPtr;
    }
    status_t Set(void *p)
    {
        this->mPtr = p;
        return OK;
    }
};

class CPtrHashTable{
public:
    CPtrHashNode **mData;
    int mCapacity;
    int mSize;

private:
    status_t del(CPtrHashNode *key);
    status_t put(CPtrHashNode *ptrhashnode);
    CPtrHashNode* get(CPtrHashNode *key);
public: 
    bool IsExist(void *ptr);
    status_t Del(void *ptr);
    status_t Put(void *ptr);
    bool IsEmpty();
    int GetSize();
    CPtrHashNode* Remove(CPtrHashNode *key);
    status_t EnumAll(CClosure *closure);        
    status_t PutPtr(CPtrHashNode *ptr);
    int HashCode(CPtrHashNode *ptrhashnode);
    static bool Equals(CPtrHashNode *ptrhashnode1, CPtrHashNode *ptrhashnode2); 
    static CPtrHashNode * CloneNode(CPtrHashNode *ptrhashnode);
    status_t DelNode(CPtrHashNode *ptrhashnode);
    CPtrHashTable();
    virtual ~CPtrHashTable();
    status_t InitBasic();
    status_t Init(int capacity);
    status_t Destroy();
    status_t Print(CFileBase *_buf);
};

#endif

