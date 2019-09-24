#ifndef __FILEINFOLIST_H
#define __FILEINFOLIST_H

#include "fileinfo.h"
#include "userfunc.h"

class CFileInfoList:public CUserFunc{
public:
    CFileInfo **mIndex;
    int mTop;
    int mSize;
public:     
    status_t SearchDir(const char *dir,bool fullpath, bool recursive);
    int SearchOnlyByName(const char *name);
    status_t RemoveDirPrefix(const char *dir);
    status_t LoadFromBson(const char *filename);
    status_t LoadFromBson(CFileBase *file);
    status_t SaveToBson(const char *filename);
    status_t SaveToBson(CFileBase *file);
    int CompNode(CFileInfo *fileinfo1, CFileInfo *fileinfo2);
    status_t CopyNode(CFileInfo *dst, CFileInfo *src);
    status_t DelNode(CFileInfo *fileinfo);
    CFileInfo * CloneNode(CFileInfo *fileinfo);
    status_t Copy(CFileInfoList *stk);
    CFileInfo * RemoveElem(int index);
    status_t InsertElemPtr(int i, CFileInfo *fileinfo);
    CFileInfo * PopPtr();
    status_t AutoResize();
    status_t PushPtr(CFileInfo *fileinfo);
    status_t InitBasic();  
    int BSearchPos(CFileInfo *fileinfo,int order,int *find_flag);
    status_t InsOrderedPtr(CFileInfo *fileinfo,int order,int unique);
    status_t InsOrdered(CFileInfo *fileinfo,int order,int unique);
    status_t DelElem(int i);
    status_t InsertElem(int i,CFileInfo *fileinfo);
    int BSearch(CFileInfo *fileinfo,int order);
    CFileInfo * BSearchNode(CFileInfo *fileinfo,int order);
    status_t Sort(int order);
    CFileInfo * GetElem(int index);
    CFileInfo * GetTopPtr();
    CFileInfo * Search(CFileInfo *fileinfo);
    int SearchPos(CFileInfo *fileinfo);
    CFileInfoList();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CFileInfo *fileinfo);
    status_t Print(CFileBase *_buf);
    status_t Push(CFileInfo *fileinfo);
    int GetLen();
    ~CFileInfoList();
};

#endif
