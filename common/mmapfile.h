#ifndef __S2_MMAPFILE_H
#define __S2_MMAPFILE_H

#include "cruntime.h"
#include "mem.h"

class CMMapFile{
public: 
    WEAK_REF_DEFINE();
public:
#if HAVE_WINDOWS_H
    HANDLE m_MapHandle;
    HANDLE m_FileHandle;
#else
    FILE_HANDLE m_FileHandle;
#endif
    void *m_Data;
    int_ptr_t m_DataSize;
    CMem m_FileName;
public:
#if HAVE_WINDOWS_H
    status_t MakeFileMappingName(const char *filename,CMem *obj_name);
#endif
    status_t Unlink();
    status_t SetFileName(CMem* _filename);
    status_t SetFileName(const char *_filename);
    status_t OpenReadWrite(const char *filename);
    status_t Sync();
    int_ptr_t GetSize();
    void* GetData();
    status_t OpenCreate(const char *filename,int_ptr_t size);
    status_t OpenReadOnly(const char *filename);
    status_t Close();
    CMMapFile();
    virtual ~CMMapFile();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CMMapFile *_p);
    int Comp(CMMapFile *_p);
    status_t Print(CFileBase *_buf);
};

#endif
