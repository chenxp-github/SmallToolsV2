#ifndef __S2_SHAREDMEMORY_H
#define __S2_SHAREDMEMORY_H

#include "cruntime.h"
#include "filebase.h"

#define SHARED_MEMORY_FLAG_FUNC(func,bit) FLAG_FUNC(m_Flags,func,bit)

class CSharedMemory{
public:
    WEAK_REF_DEFINE();
public:
    char *m_Data;
    int m_ShmId;
    int m_Name;
    int m_Size;
    uint32_t m_Flags;   
public:
    status_t Zero();
    char *GetData();
    int GetSize();
    status_t OpenReadWrite();
    status_t Open(int size, uint32_t flags);
    status_t OpenReadOnly();
    status_t OpenCreate(int size);
    status_t SetName(int name); 
    status_t Unlink();
    status_t Close();
    CSharedMemory();
    virtual ~CSharedMemory();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    SHARED_MEMORY_FLAG_FUNC(IsOwner,0x00000001);
};

#endif
