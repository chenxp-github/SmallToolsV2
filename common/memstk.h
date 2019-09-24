#ifndef __MEMSTK_H
#define __MEMSTK_H

#include "mem.h"
#include "filebase.h"
#include "closure.h"
#include "userfunc.h"

class CMemStk:public CUserFunc{
public:
    CMem **mIndex;
    int mTop;
    int mSize;
public:
    status_t PushFile(CFileBase *file);
    status_t Push(const char *str);
    status_t LoadLines(CFileBase *file);
    status_t SaveLines(CFileBase *file);
    bool Equal(CMemStk *stk);
    status_t LoadPath(CFileBase* path);
    status_t SavePath(CFileBase *file);
    int CompNode(CMem *mem1, CMem *mem2);
    status_t CopyNode(CMem *dst, CMem *src);
    status_t DelNode(CMem *mem);
    CMem* CloneNode(CMem *mem);
    status_t Copy(CMemStk *stk);
    CMem* RemoveElem(int index);
    status_t InsElemPtr(int i, CMem *mem);
    CMem* PopPtr();
    status_t AutoResize();
    status_t PushPtr(CMem *mem);
    status_t InitBasic();  
    int BSearchPos(CMem *mem,int order,int *find_flag);
    status_t InsOrderedPtr(CMem *mem,int order,int unique);
    status_t InsOrdered(CMem *mem,int order,int unique);
    status_t DelElem(int i);
    status_t InsElem(int i,CMem *mem);
    int BSearch(CMem *mem,int order);
    CMem* BSearchNode(CMem *mem,int order);
    status_t Sort(int order);
    CMem* GetElem(int index);
    CMem* GetTopPtr();
    CMem* Search(CMem *mem);
    int SearchPos(CMem *mem);
    CMemStk();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size=256);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CMem *mem);
    status_t Print(CFileBase *_buf);
    status_t Push(CMem *mem);
    int GetLen();
    ~CMemStk();
#if _UNICODE_
    status_t PushW(const wchar_t *str);
#endif
};

#endif
