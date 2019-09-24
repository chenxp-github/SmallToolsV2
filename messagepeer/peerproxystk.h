#ifndef __PEERPROXYSTK_H
#define __PEERPROXYSTK_H

#include "peerproxy.h"

class CPeerProxyStk{
public:
    CPeerProxy **mIndex;
    int mTop;
    int mSize;
public:     
    int CompNode(CPeerProxy *peerproxy1, CPeerProxy *peerproxy2);
    status_t CopyNode(CPeerProxy *dst, CPeerProxy *src);
    status_t DelNode(CPeerProxy *peerproxy);
    CPeerProxy * CloneNode(CPeerProxy *peerproxy);
    status_t Copy(CPeerProxyStk *stk);
    CPeerProxy * RemoveElem(int index);
    status_t InsertElemPtr(int i, CPeerProxy *peerproxy);
    CPeerProxy * PopPtr();
    status_t AutoResize();
    status_t PushPtr(CPeerProxy *peerproxy);
    status_t InitBasic();  
    int BSearchPos(CPeerProxy *peerproxy,int order,int *find_flag);
    status_t InsOrderedPtr(CPeerProxy *peerproxy,int order,int unique);
    status_t InsOrdered(CPeerProxy *peerproxy,int order,int unique);
    status_t DelElem(int i);
    status_t InsertElem(int i,CPeerProxy *peerproxy);
    int BSearch(CPeerProxy *peerproxy,int order);
    CPeerProxy * BSearchNode(CPeerProxy *peerproxy,int order);
    status_t Sort(int order);
    CPeerProxy * GetElem(int index);
    CPeerProxy * GetTopPtr();
    CPeerProxy * Search(CPeerProxy *peerproxy);
    int SearchPos(CPeerProxy *peerproxy);
    CPeerProxyStk();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CPeerProxy *peerproxy);
    status_t Print();
    status_t Push(CPeerProxy *peerproxy);
    int GetLen();
    ~CPeerProxyStk();
};

#endif
