#ifndef __SIMPLEDISKFSINDEX_H
#define __SIMPLEDISKFSINDEX_H

#include "cruntime.h"
#include "filebase.h"
#include "simplediskfsentry.h"
#include "closure.h"

class CSimpleDiskFsIndex{
private:
    CSimpleDiskFsEntry **m_Data;
    int m_Capacity;
    int m_Size;
public: 
    status_t DiscardAll();
    bool IsEmpty();
    int GetSize();
    int GetCapacity();
    status_t Del(CSimpleDiskFsEntry *key);
    CSimpleDiskFsEntry* Remove(CSimpleDiskFsEntry *key);
    status_t EnumAll(CClosure *closure);        
    CSimpleDiskFsEntry* Get(CSimpleDiskFsEntry *key);   
    status_t Put(CSimpleDiskFsEntry *simplediskfsentry);
    status_t PutPtr(CSimpleDiskFsEntry *ptr);
    int HashCode(CSimpleDiskFsEntry *simplediskfsentry);
    static int HashCode(CSimpleDiskFsEntry *simplediskfsentry,int capacity);
    static bool Equals(CSimpleDiskFsEntry *simplediskfsentry1, CSimpleDiskFsEntry *simplediskfsentry2); 
    static CSimpleDiskFsEntry * CloneNode(CSimpleDiskFsEntry *simplediskfsentry);
    status_t DelNode(CSimpleDiskFsEntry *simplediskfsentry);
    CSimpleDiskFsIndex();
    virtual ~CSimpleDiskFsIndex();
    status_t InitBasic();
    status_t Init(int capacity);
    status_t Destroy();
    status_t Print(CFileBase *_buf);
};

#endif
