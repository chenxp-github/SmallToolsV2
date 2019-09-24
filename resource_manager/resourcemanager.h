#ifndef __S2_RESOURCEMANAGER_H
#define __S2_RESOURCEMANAGER_H

#include "cruntime.h"
#include "filebase.h"
#include "resourcehashmap.h"
#include "hashfile.h"

typedef CHashFile<CResourceHashMap,CResource> CResourceHashFile;

class CResourceManager{
public:
    WEAK_REF_ID_DEFINE();
    LUA_USER_DATA_DEFINE();
    CResourceHashMap m_HashMap; //for saving
    CResourceHashFile m_HashFile; //for loading
    CMem m_PathPrefix;
public:
    status_t DumpResourceFile(const char *filename);
    status_t DumpResourceFile(CFileBase *out);
    status_t LoadResourceFileTransfer(CMem *from);
    status_t LoadResourceFileGhost(CMem *data);
    status_t LoadResourceFileCopy(CFileBase *data);
    status_t SetPathPrefix(const char *_pathprefix);
    status_t SetPathPrefix(CMem* _pathprefix);
    const char* GetPathPrefixStr();
    CMem* GetPathPrefix();
    status_t LoadResourceFile(const char *filename);
    status_t SearchByString(const char *theme,const char *language,const char *id, CResource *value);
    status_t SearchByString(CMem *theme,CMem *language,CMem *id, CResource *value);
    status_t SearchResource(CResource *key, CResource *value);
    status_t LoadResourceFile(CFileBase *i_file);
    status_t SaveResourceFile(const char *filename);
    status_t AddResource(CResource *res);
    CResourceManager();
    virtual ~CResourceManager();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CResourceManager *_p);
    int Comp(CResourceManager *_p);
    status_t Print(CFileBase *_buf);
    CResourceHashMap* GetHashMap();
};

#endif
