#ifndef __FILEINFO_H
#define __FILEINFO_H

#include "mem.h"
#include "minibson.h"

class CFileInfo{
public:
    CMem *mName;
    int64_t mLastWriteTime;
    fsize_t mSize;
    CMem *mMd5Sum;
    bool mIsDir;
public:
    status_t SaveToBson(CMiniBson *bson);
    status_t LoadFromBson(CMiniBson *bson);
    bool IsDir();
    bool GetIsDir();
    status_t SetIsDir(bool isdir);
    const char * GetName();
    int64_t GetLastWriteTime();
    fsize_t GetSize();
    const char * GetMd5Sum();
    status_t SetName(const char *name);
    status_t SetName(CMem * name);
    status_t SetLastWriteTime(int64_t lastwritetime);
    status_t SetSize(fsize_t size);
    status_t SetMd5Sum(const char *md5sum);
    status_t SetMd5Sum(CMem * md5sum);
    CFileInfo();
    virtual ~CFileInfo();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CFileInfo *p);
    int Comp(CFileInfo *p);
    status_t Print(CFileBase *_buf);
};

#endif
