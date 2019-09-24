#ifndef __PEERMESSAGESTK_H
#define __PEERMESSAGESTK_H

#include "peermessage.h"
#include "ptrhashtable.h"

class CPeerMessageStk{
public:
    CPeerMessage **mIndex;
    CPtrHashTable *mExistTable;
    int mTop,mSize;
public:     
    bool IsPtrExist(CPeerMessage *msg);
    int CompNode(CPeerMessage *peermessage1, CPeerMessage *peermessage2);
    status_t CopyNode(CPeerMessage *dst, CPeerMessage *src);
    status_t DelNode(CPeerMessage *peermessage);
    CPeerMessage * CloneNode(CPeerMessage *peermessage);
    status_t Copy(CPeerMessageStk *stk);
    CPeerMessage * RemoveElem(int index);
    status_t InsertElemPtr(int i, CPeerMessage *peermessage);
    CPeerMessage * PopPtr();
    status_t AutoResize();
    status_t PushPtr(CPeerMessage *peermessage);
    status_t InitBasic();  
    int BSearchPos(CPeerMessage *peermessage,int order,int *find_flag);
    status_t InsOrderedPtr(CPeerMessage *peermessage,int order,int unique);
    status_t InsOrdered(CPeerMessage *peermessage,int order,int unique);
    status_t DelElem(int i);
    status_t InsertElem(int i,CPeerMessage *peermessage);
    int BSearch(CPeerMessage *peermessage,int order);
    CPeerMessage * BSearchNode(CPeerMessage *peermessage,int order);
    status_t Sort(int order);
    CPeerMessage * GetElem(int index);
    CPeerMessage * GetTopPtr();
    CPeerMessage * Search(CPeerMessage *peermessage);
    int SearchPos(CPeerMessage *peermessage);
    CPeerMessageStk();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CPeerMessage *peermessage);
    status_t Print();
    status_t Push(CPeerMessage *peermessage);
    int GetLen();
    ~CPeerMessageStk();
};

#endif
