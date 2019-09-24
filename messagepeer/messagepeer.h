#ifndef __MESSAGEPEER_H
#define __MESSAGEPEER_H

#include "cruntime.h"
#include "mem.h"
#include "taskmgr.h"
#include "peermessagestk.h"
#include "memstk.h"
#include "peercommon.h"

#define MESSAGE_PEER_FLAG_FUNC(func,bit) FLAG_FUNC(mFlags,func, bit)

class CMessagePeer{
public:
    WEAK_REF_ID_DEFINE();
    TASK_CONTAINER_DEFINE();
public:
    CMem *mName;
    CMem *mServer;
    int mPort;
    CPeerMessageStk *mSendingQueue;
    int mTaskPeerClient;
    CMemStk *mAllConnectedPeers;
    CClosure *mCallback;
    int mMaxRetries;
    uint32_t mFlags;
public:
    status_t OnSocketDisconnected();
    status_t OnSocketConnected();
    status_t OnTaskPeerClientStop(int err);
    status_t SetMaxRetries(int max_retries);
    int GetRetries();
    int SetRetries(int retry);
    status_t ClearSendingQueue();
    int GetSendingQueueLength();
    bool IsConnected();
    status_t SetCanFetchMessage(bool can_fetch);
    CClosure* Callback();
    CMemStk * GetAllConnectedPeers();
    status_t Connect(const char *peer_name);
    status_t SendMessage(CPeerMessage *msg);
    status_t Start();
    CMessagePeer();
    virtual ~CMessagePeer();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Copy(CMessagePeer *p);
    int Comp(CMessagePeer *p);
    status_t Print();

    status_t SetName(CMem *name);
    status_t SetName(const char *name);
    status_t SetServer(CMem *server);
    status_t SetServer(const char *server);
    status_t SetPort(int port);
    CMem* GetName();
    const char* GetNameStr();
    CMem* GetServer();
    const char* GetServerStr();
    int GetPort();

    MESSAGE_PEER_FLAG_FUNC(IsClosedPermanently,0x00000001);
};

#endif
