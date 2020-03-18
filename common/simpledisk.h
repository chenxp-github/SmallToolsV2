#ifndef __SIMPLEDISK_H
#define __SIMPLEDISK_H

#include "indexfile.h"
#include "minibson.h"
#include "closure.h"
#include "simplediskfsindex.h"
#include "hashfile.h"

typedef CHashFile<CSimpleDiskFsIndex,CSimpleDiskFsEntry> SimpleDiskHashFile;

class CSimpleDisk{
public:
    WEAK_REF_DEFINE();
public:
    CIndexFile *mIndexFile;
    CFileBase *iDstFile;
    CMiniBson *mFileSystem;
    SimpleDiskHashFile *mFastFsIndex; 
public:
    status_t BuildFastFsIndex(CFileBase *out);
    status_t RebuildFastIndex();
    status_t AddFastFsIndex();
    status_t GetFsEntry(const char *_filename,CSimpleDiskFsEntry *entry);
    status_t LoadFastFsIndex();
    status_t LoadFastFsIndex(int64_t index);
    status_t EndFolder(fsize_t offset);
    status_t BeginFolder(const char *name,fsize_t *offset);
    status_t GetFileOffsetAndSize(int64_t index, fsize_t *offset, fsize_t *size);   
    CSimpleDisk();
    virtual ~CSimpleDisk();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t SetDstFile(CFileBase* iFile);
    status_t AddFile(const char *name,CFileBase *file);
    status_t Save(void);
    status_t AddFolder(CMem * dir);
    status_t AddFolder(const char *dir);
    status_t LoadDiskImage(CFileBase * file);
    int64_t GetFileSize(int64_t index);
    status_t GetFile(int64_t index , CFileBase *file);
    status_t GhostFile(int64_t index, CPartFile *file);
    status_t ExtractAllFiles(CMem * destDir);
    status_t ExtractAllFiles(const char * destDir);
    status_t SearchDirectory(CClosure *closure);
    status_t Reset(void);
};

#endif
