#ifndef __COMBFILE_H
#define __COMBFILE_H

#include "cruntime.h"
#include "filebase.h"

class CCombFile:public CFileBase{
public:
    fsize_t mOffset;
    CFileBase **mAllFiles;
    int mFileNum;
    int mMaxFileNum;
public:
    status_t Clear();
    status_t AddFile(CFileBase *file);
    int InWhichFile(fsize_t off,fsize_t *subOff);
    fsize_t GetOffset();
    fsize_t GetSize();
    int_ptr_t Read(void *buf,int_ptr_t n);
    int_ptr_t Write(const void *buf,int_ptr_t n);
    status_t SetSize(fsize_t ssize);
    fsize_t Seek(fsize_t off);
    status_t AddBlock(fsize_t bsize);
    fsize_t GetMaxSize();
    CCombFile();
    virtual ~CCombFile();
    status_t InitBasic();
    status_t Init(int max);
    status_t Destroy();
};

#endif
