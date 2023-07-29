#ifndef __T_REMOTE_DESKTOP_SESSION_MANAGER_H
#define __T_REMOTE_DESKTOP_SESSION_MANAGER_H

#include "mem.h"
#include "remote_desktop_session.h"
#include "taskmgr.h"


class CRemoteDesktopSessionManager{
public:
    WEAK_REF_DEFINE();
    TASK_CONTAINER_DEFINE();

private:
    CRemoteDesktopSession **m_Index;
    int m_Top;
    int m_Size;

	int m_task_auto_clear;
public:
    CRemoteDesktopSession* RemoveElem(int index);
    status_t InsElemPtr(int i, CRemoteDesktopSession *node);
    CRemoteDesktopSession* PopPtr();
    status_t AutoResize();
    status_t PushPtr(CRemoteDesktopSession *node);
    status_t InitBasic();  
    int BSearchPos(CRemoteDesktopSession *node,int order,int *find_flag);
    status_t InsOrderedPtr(CRemoteDesktopSession *node,int order,int unique);
    status_t InsOrdered(CRemoteDesktopSession *node,int order,int unique);
    status_t DelElem(int i);
    status_t DelElems(int index, int size);
    status_t InsElem(int i,CRemoteDesktopSession *node);
    int BSearch(CRemoteDesktopSession *node,int order);
    CRemoteDesktopSession* BSearchNode(CRemoteDesktopSession *node,int order);
    status_t Sort(int order);
    CRemoteDesktopSession* GetElem(int index);
    CRemoteDesktopSession* GetTopPtr();
    CRemoteDesktopSession* Search(CRemoteDesktopSession *node);
    int SearchPos(CRemoteDesktopSession *node);
    status_t Clear();
    status_t DelTop();
    status_t Pop(CRemoteDesktopSession *node);
    status_t Push(CRemoteDesktopSession *node);
    int GetLen();
    bool IsEmpty();
    bool IsFull();

    int CompNode(CRemoteDesktopSession *node1, CRemoteDesktopSession *node2);
    status_t CopyNode(CRemoteDesktopSession *dst, CRemoteDesktopSession *src);
    status_t DelNode(CRemoteDesktopSession *node);
    CRemoteDesktopSession* CloneNode(CRemoteDesktopSession *node);

    CRemoteDesktopSessionManager();
    ~CRemoteDesktopSessionManager();
    status_t Destroy();
    status_t Init(CTaskMgr *_taskmgr,int init_size=256);
    status_t Print(CFileBase *_buf);
    status_t Copy(CRemoteDesktopSessionManager *stk);
    int Comp(CRemoteDesktopSessionManager *stk);


	status_t StartAutoClearTask();
	status_t OnAutoClear(uint32_t interval);

	CRemoteDesktopSession *CreateSession(CMem *name);
	CRemoteDesktopSession *GetSession(CMem *name);
	status_t DelSession(CMem *name);
	status_t SessionReactive(CMem *name);
};


#endif

