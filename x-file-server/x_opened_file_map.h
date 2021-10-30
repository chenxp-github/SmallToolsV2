#ifndef __X_OPENED_FILE_MAP_H
#define __X_OPENED_FILE_MAP_H

#include "filebase.h"
#include "closure.h"
#include "file.h"

class xOpenedFileMap_HashEntry{

public:
    CFile *m_RawPtr;
    xOpenedFileMap_HashEntry *next;
public:
    xOpenedFileMap_HashEntry();
    ~xOpenedFileMap_HashEntry();
    CFile *get();
    status_t set(CFile *node);
};

class CxOpenedFileMap{

private:
    xOpenedFileMap_HashEntry **m_Data;
    int m_Capacity;
    int m_Size;
public:    
    status_t Clear();
    status_t DiscardAll();
    bool IsEmpty();
    int GetSize();
    int GetCapacity();
    status_t Del(CFile *key);
    xOpenedFileMap_HashEntry* Remove(CFile *key);
    status_t EnumAll(CClosure *closure);        
    CFile* Get(CFile *key);    
    status_t Put(CFile *hashentry);
    status_t PutPtr(CFile *ptr);
    int HashCode(CFile *hashentry);
    status_t ToArray(CFile *arr[], int *len);

    static int HashCode(CFile *hashentry,int capacity);
    static bool Equals(CFile *hashentry1, CFile *hashentry2);    
    static CFile * CloneNode(CFile *hashentry);

    CxOpenedFileMap();
    virtual ~CxOpenedFileMap();
    status_t InitBasic();
    status_t Init(int capacity=1024);
    status_t Destroy();
    status_t Copy(CxOpenedFileMap *p);
    status_t Comp(CxOpenedFileMap *p);
    status_t Print(CFileBase *_buf);    

    CFile *GetByHandle(int handle);
    FILE_HANDLE OpenFile(const char *filename, const char *mode);
    status_t CloseFile(int handle);
};



#endif


