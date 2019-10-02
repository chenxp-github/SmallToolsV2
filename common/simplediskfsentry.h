#ifndef __S2_SIMPLEDISKFSENTRY_H
#define __S2_SIMPLEDISKFSENTRY_H

#include "cruntime.h"
#include "filebase.h"
#include "mem.h"

#define HASH_ENTRY_DEFINE(type)\
type *__next;\
int32_t __next_index,__cur_index\

#define HASH_ENTRY_CLEAR()\
__next = NULL;\
__next_index=-1;__cur_index=-1\


class CSimpleDiskFsEntry{
public:
    HASH_ENTRY_DEFINE(CSimpleDiskFsEntry);

    CMem m_FileName;
    int64_t m_Index;
    char m_Md5[36]; 
public:
    CSimpleDiskFsEntry();
    virtual ~CSimpleDiskFsEntry();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CSimpleDiskFsEntry *_p);
    int Comp(CSimpleDiskFsEntry *_p);
    status_t Print(CFileBase *_buf);
    CMem* GetFileName();
    int64_t GetIndex();
    status_t SetFileName(CMem* _filename);
    status_t SetIndex(int64_t _index);
    const char* GetFileNameStr();
    const char* GetMd5Str();
    status_t SetFileName(const char *_filename);
    status_t SetMd5(const char *_md5);
    status_t Serialize(CFileBase *file);
    status_t UnSerialize(CFileBase *file);
};

#endif
