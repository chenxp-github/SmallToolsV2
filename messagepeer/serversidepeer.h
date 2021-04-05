#ifndef __S2_SERVERSIDEPEER_H
#define __S2_SERVERSIDEPEER_H

#include "cruntime.h"
#include "mem.h"
#include "peermessage.h"
#include "memstk.h"
#include "taskmgr.h"
#include "weak_pointer.h"

class CPeerProxy;
class CServerSidePeer{
public:
    PEER_GLOBAL_CONTEXT_DEFINE();
    TASK_CONTAINER_DEFINE();
    WEAK_REF_DEFINE();
public:
    bool quit,can_fetch_message;
    CWeakPointer<CPeerProxy> iHostProxy;
    CMem *mName;
    CMemStk *mAllConnectedPeers;
    CClosure *mCallback;
    int mTaskFetchMessage;
public:
	status_t OnInitNameMessage(CPeerMessage *msg);
    int GetAliveClientNumber();
    status_t ClearSendingQueue();
    int GetSendingQueueLength(const char *proxy_name);    
    status_t ResumeFetchMessageTask();
    status_t SuspendFetchMessageTask();
    status_t StartFetchMessageTask();
    status_t SetCanFetchMessage(bool can);
    CClosure * Callback();
    status_t Connect(const char *peer_name);
    status_t Start();
    CMem* GetName();
    status_t SetName(CMem* _name);
    const char* GetNameStr();
    status_t SetName(const char *_name);
    bool IsMarkQuit();
    status_t OnGotMessage(CPeerMessage *msg);
    status_t SendMessage(CPeerMessage *msg);
    CServerSidePeer();
    virtual ~CServerSidePeer();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr,const void *peer_globals);
    status_t Destroy();
};

#endif
