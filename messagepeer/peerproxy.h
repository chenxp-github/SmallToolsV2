#ifndef __PEERPROXY_H
#define __PEERPROXY_H

#include "cruntime.h"
#include "mem.h"
#include "memstk.h"
#include "peermessagestk.h"
#include "taskmgr.h"
#include "serversidepeer.h"
#include "weakptr.h"

class CPeerProxy{
public:
    TASK_CONTAINER_DEFINE();
    WEAK_REF_ID_DEFINE();
    CMem *mPeerName;
    CMemStk *mConnectedNames;
    int mMaxConnectedPeer;
    CPeerMessageStk *mPendingMessages;
    int mTaskPeerServer;
    CWeakPtr<CServerSidePeer> mServerSidePeer;
    uint32_t mLifeTime;
	CClosure *mCallback;
public:
	CClosure *Callback();
    bool IsServerSide();
    status_t ClearSendingQueue();
    int GetSendingQueueLength();
    status_t AttachServerSidePeer(CServerSidePeer *peer);
    status_t SetLifeTime(uint32_t ltime);
    uint32_t GetLifeTime();
    status_t AddAllPeers(CFileBase *lines);
    bool IsPeerConnectedMe(CPeerProxy *peer);
    CPeerMessageStk * GetSendingQueue();
    bool IsConnectionAlive();
    status_t AttachTaskPeerServer(int task_id);
    CMemStk * GetAllConnectedPeerNames();
    status_t AddPendingMessage(CPeerMessage *msg);
    status_t AddPeer(const char *name);
    status_t AddPeer(CMem *name);
    status_t SetMaxConnectedPeers(int max);
    const char * GetNameStr();
    status_t SetName(CMem *peername);
    status_t SetName(const char *peername);
    CMem* GetName();

    CPeerProxy();
    virtual ~CPeerProxy();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CPeerProxy *p);
    int Comp(CPeerProxy *p);
    status_t Print();

	status_t OnSocketConnected();
	status_t OnSocketDisconnected();
};

#endif
