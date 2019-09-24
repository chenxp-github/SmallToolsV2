#ifndef __CLOSURELIST_H
#define __CLOSURELIST_H

#include "closure.h"
#include "filebase.h"

class CClosureList{
public:
    CClosure **m_Index;
    int m_Top;
    int m_Size;
public:     
    int CompNode(CClosure *closure1, CClosure *closure2);
    status_t CopyNode(CClosure *dst, CClosure *src);
    status_t DelNode(CClosure *closure);
    CClosure* CloneNode(CClosure *closure);
    status_t Copy(CClosureList *stk);
    CClosure* RemoveElem(int index);
    status_t InsElemPtr(int i, CClosure *closure);
    CClosure* PopPtr();
    status_t AutoResize();
    status_t PushPtr(CClosure *closure);
    status_t InitBasic();  
    int BSearchPos(CClosure *closure,int order,int *find_flag);
    status_t InsOrderedPtr(CClosure *closure,int order,int unique);
    status_t InsOrdered(CClosure *closure,int order,int unique);
    status_t DelElem(int i);
    status_t InsElem(int i,CClosure *closure);
    int BSearch(CClosure *closure,int order);
    CClosure* BSearchNode(CClosure *closure,int order);
    status_t Sort(int order);
    CClosure* GetElem(int index);
    CClosure* GetTopPtr();
    CClosure* Search(CClosure *closure);
    int SearchPos(CClosure *closure);
    CClosureList();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CClosure *closure);
    status_t Print(int _level,CFileBase *_buf);
    status_t Push(CClosure *closure);
    int GetLen();
    ~CClosureList();
};

#endif
