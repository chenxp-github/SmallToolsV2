#ifndef __T_XFILEINFOLIST_H
#define __T_XFILEINFOLIST_H

#include "mem.h"
#include "x_file_info.h"
#include "minibson.h"


class CxFileInfoList{

private:
    CxFileInfo **m_Index;
    int m_Top;
    int m_Size;
public:
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
    CxFileInfo* RemoveElem(int index);
    status_t InsElemPtr(int i, CxFileInfo *node);
    CxFileInfo* PopPtr();
    status_t AutoResize();
    status_t PushPtr(CxFileInfo *node);
    status_t InitBasic();  
    int BSearchPos(CxFileInfo *node,int order,int *find_flag);
    status_t InsOrderedPtr(CxFileInfo *node,int order,int unique);
    status_t InsOrdered(CxFileInfo *node,int order,int unique);
    status_t DelElem(int i);
    status_t InsElem(int i,CxFileInfo *node);
    int BSearch(CxFileInfo *node,int order);
    CxFileInfo* BSearchNode(CxFileInfo *node,int order);
    status_t Sort(int order);
    CxFileInfo* GetElem(int index);
    CxFileInfo* GetTopPtr();
    CxFileInfo* Search(CxFileInfo *node);
    int SearchPos(CxFileInfo *node);
    status_t Clear();
    status_t DelTop();
    status_t Pop(CxFileInfo *node);
    status_t Push(CxFileInfo *node);
    int GetLen();
    bool IsEmpty();
    bool IsFull();

    int CompNode(CxFileInfo *node1, CxFileInfo *node2);
    status_t CopyNode(CxFileInfo *dst, CxFileInfo *src);
    status_t DelNode(CxFileInfo *node);
    CxFileInfo* CloneNode(CxFileInfo *node);

    CxFileInfoList();
    ~CxFileInfoList();
    status_t Destroy();
    status_t Init(int init_size=256);
    status_t Print(CFileBase *_buf);
    status_t Copy(CxFileInfoList *stk);
    int Comp(CxFileInfoList *stk);
};


#endif

