#ifndef __PTRSTK_H
#define __PTRSTK_H

#include "cruntime.h"
#include "userfunc.h"

typedef void* any_ptr;

class CPtrStk:public CUserFunc{
public:
public:
    any_ptr *mData;
    int mTop,mSize;
    bool mUseFree;
public:
    status_t PushInt(int_ptr_t i);
    int_ptr_t GetInt(int index);
    status_t DestroyAllNodes();
    status_t UseFreeWhenDelete(bool use_free);
    status_t free_mem(int index);

    int CompNode(any_ptr node1, any_ptr node2);
    status_t Copy(CPtrStk *stk);
    status_t AutoResize();
    status_t Sort(int order);
    status_t Push(any_ptr node);
    any_ptr Pop();
    any_ptr GetElem(int index);
    int BSearchPos(any_ptr node, int order, int *find_flag);
    status_t InsertElem(int index, any_ptr node);
    status_t DelElem(int index);
    status_t InsOrdered(any_ptr node, int order, int unique);
    int SearchPos(any_ptr node);
    any_ptr GetTop();
    status_t SetElem(int index, any_ptr node);
    int GetLen();
    bool IsEmpty();
    bool IsFull();
    status_t Clear();
    CPtrStk();
    ~CPtrStk();
    status_t Init(int init_size);
    status_t Destroy();
    status_t Print(CFileBase *_buf);
    status_t InitBasic();
};

#endif
