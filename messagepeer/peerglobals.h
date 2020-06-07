#ifndef __PEER_GLOBALS_H
#define __PEER_GLOBALS_H

#include "cruntime.h"
#include "peermessagestk.h"
#include "peerproxymanager.h"
#include "taskmgr.h"
#include "callbackmap.h"

class CPeerGlobals{
public:
    enum{
        EVENT_ACCEPTOR_ERROR=1,
        EVENT_NEW_PEER,
        EVENT_PEER_DELETED,
        EVENT_PEER_DISCONNECTED,
    };
public:
    TASK_CONTAINER_DEFINE();
    CPeerProxyManager *mPeerManager;
    CCallbackMap *mCallbackMap;
    int mTaskPeerKiller;
    int mTaskAcceptor;
    int mListenPort;
    bool mTrustMode;
    CClosure *mCallback;
public:
    bool IsPeerConnected(const char *name);
    CPeerProxy *GetPeerProxyByName(const char *name);
    CPeerProxyManager *GetPeerProxyManager();
    CCallbackMap *GetCallbackMap();
	int GetTotalSendingQueueLength();
    bool InTrustMode();
    status_t SetTrustMode(bool enable);
    CPeerProxy *NewPendingPeerProxy(const char *name);
    status_t NotifyPeerDeleted(CPeerProxy *peer);
    status_t NotifyNewPeer(CPeerProxy *peer);
    status_t NotifyPeerDisconnected(CPeerProxy *peer);
    CClosure *Callback();
    status_t StartAccept(int port);
    status_t StartTaskPeerKiller();
    status_t AutoKillPeers(uint32_t interval);
    status_t DispatchMessage(CPeerMessage *msg);
    status_t SetListenPort(int port);
    CPeerGlobals();
    virtual ~CPeerGlobals();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
};

#endif
