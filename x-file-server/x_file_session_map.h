#ifndef __X_FILE_SESSION_MAP_H
#define __X_FILE_SESSION_MAP_H

#include "filebase.h"
#include "closure.h"
#include "x_file_session.h"



class xFileSessionMap_HashEntry{

public:
    CxFileSession *m_RawPtr;
    xFileSessionMap_HashEntry *next;
public:
    xFileSessionMap_HashEntry();
    ~xFileSessionMap_HashEntry();
    CxFileSession *get();
    status_t set(CxFileSession *node);
};

class CxFileSessionMap{

private:
    xFileSessionMap_HashEntry **m_Data;
    int m_Capacity;
    int m_Size;
public:    
    status_t Clear();
    status_t DiscardAll();
    bool IsEmpty();
    int GetSize();
    int GetCapacity();
    status_t Del(CxFileSession *key);
    xFileSessionMap_HashEntry* Remove(CxFileSession *key);
    status_t EnumAll(CClosure *closure);        
    CxFileSession* Get(CxFileSession *key);    
    status_t Put(CxFileSession *hashentry);
    status_t PutPtr(CxFileSession *ptr);
    int HashCode(CxFileSession *hashentry);
    status_t ToArray(CxFileSession *arr[], int *len);

    static int HashCode(CxFileSession *hashentry,int capacity);
    static bool Equals(CxFileSession *hashentry1, CxFileSession *hashentry2);    
    static CxFileSession * CloneNode(CxFileSession *hashentry);

    CxFileSessionMap();
    virtual ~CxFileSessionMap();
    status_t InitBasic();
    status_t Init(int capacity=1024);
    status_t Destroy();
    status_t Copy(CxFileSessionMap *p);
    status_t Comp(CxFileSessionMap *p);
    status_t Print(CFileBase *_buf);    


    CxFileSession *GetByName(CMem *name);
};



#endif


