#ifndef __T_REMOTE_DESKTOP_SNAPSHOTTOR_LIST_H
#define __T_REMOTE_DESKTOP_SNAPSHOTTOR_LIST_H

#include "mem.h"
#include "remote_desktop_snapshottor.h"
#include "taskmgr.h"


class CRemoteDesktopSnapshottorList{
public:
    WEAK_REF_DEFINE();
    TASK_CONTAINER_DEFINE();

private:
    CRemoteDesktopSnapshottor **m_Index;
    int m_Top;
    int m_Size;
	uint32_t m_Flags;
public:

	FLAG_FUNC(m_Flags,IsWeakTable,0x00000001);

    CRemoteDesktopSnapshottor* RemoveElem(int index);
    status_t InsElemPtr(int i, CRemoteDesktopSnapshottor *node);
    CRemoteDesktopSnapshottor* PopPtr();
    status_t AutoResize();
    status_t PushPtr(CRemoteDesktopSnapshottor *node);
    status_t InitBasic();  
    int BSearchPos(CRemoteDesktopSnapshottor *node,int order,int *find_flag);
    status_t InsOrderedPtr(CRemoteDesktopSnapshottor *node,int order,int unique);
    status_t InsOrdered(CRemoteDesktopSnapshottor *node,int order,int unique);
    status_t DelElem(int i);
    status_t InsElem(int i,CRemoteDesktopSnapshottor *node);
    int BSearch(CRemoteDesktopSnapshottor *node,int order);
    CRemoteDesktopSnapshottor* BSearchNode(CRemoteDesktopSnapshottor *node,int order);
    status_t Sort(int order);
    CRemoteDesktopSnapshottor* GetElem(int index);
    CRemoteDesktopSnapshottor* GetTopPtr();
    CRemoteDesktopSnapshottor* Search(CRemoteDesktopSnapshottor *node);
    int SearchPos(CRemoteDesktopSnapshottor *node);
    status_t Clear();
    status_t DelTop();
    status_t Pop(CRemoteDesktopSnapshottor *node);
    status_t Push(CRemoteDesktopSnapshottor *node);
    int GetLen();
    bool IsEmpty();
    bool IsFull();

    int CompNode(CRemoteDesktopSnapshottor *node1, CRemoteDesktopSnapshottor *node2);
    status_t CopyNode(CRemoteDesktopSnapshottor *dst, CRemoteDesktopSnapshottor *src);
    status_t DelNode(CRemoteDesktopSnapshottor *node);
    CRemoteDesktopSnapshottor* CloneNode(CRemoteDesktopSnapshottor *node);

    CRemoteDesktopSnapshottorList();
    ~CRemoteDesktopSnapshottorList();
    status_t Destroy();
    status_t Init(CTaskMgr *_taskmgr,int init_size=256);
    status_t Print(CFileBase *_buf);
    status_t Copy(CRemoteDesktopSnapshottorList *stk);
    int Comp(CRemoteDesktopSnapshottorList *stk);

	CRemoteDesktopSnapshottor *GetByName(CMem *name);
	status_t AddSnapshotter(CRemoteDesktopSnapshottor *snapshotter);
};


#endif

