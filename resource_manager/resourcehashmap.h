#ifndef __RESOURCEHASHMAP_H
#define __RESOURCEHASHMAP_H

#include "cruntime.h"
#include "filebase.h"
#include "resource.h"
#include "closure.h"

class CResourceHashMap{
private:
    CResource **m_Data;
    int m_Capacity;
    int m_Size;
public: 
    status_t DiscardAll();
    bool IsEmpty();
    int GetSize();
    int GetCapacity();
    status_t Del(CResource *key);
    CResource* Remove(CResource *key);
    status_t EnumAll(CClosure *closure);        
    CResource* Get(CResource *key); 
    status_t Put(CResource *resource);
    status_t PutPtr(CResource *ptr);
    int HashCode(CResource *resource);
    static int HashCode(CResource *resource,int capacity);
    static bool Equals(CResource *resource1, CResource *resource2); 
    static CResource * CloneNode(CResource *resource);
    status_t DelNode(CResource *resource);
    CResourceHashMap();
    virtual ~CResourceHashMap();
    status_t InitBasic();
    status_t Init(int capacity);
    status_t Destroy();
    status_t Print(CFileBase *_buf);
};

#endif
