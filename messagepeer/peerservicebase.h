#ifndef __S2_PEERSERVICEBASE_H
#define __S2_PEERSERVICEBASE_H

#include "cruntime.h"
#include "filebase.h"
#include "serversidepeer.h"
#include "messagepeer.h"
#include "peercommon.h"
#include "taskmgr.h"

#define PART_OK	 100

#define SERVICE_BASE_FLAG_FUNC(func,bit) FLAG_FUNC(m_Flags,func, bit)

class CPeerServiceBase{
public:
    WEAK_REF_ID_DEFINE();
    TASK_CONTAINER_DEFINE();
public:
    CServerSidePeer m_ServerSidePeer;
    CMessagePeer m_ClientSidePeer;
    uint32_t m_Flags;
    int_ptr_t m_CreateThreadId;
public:
    bool IsClosedPermanently();
    int GetRetries();
    status_t SetRetries(int retry);
    status_t ClearSendingQueue();
    status_t SetMaxRetries(int max_retries);
    int GetSendingQueueLength();
    bool IsConnected();
    const char * GetName();
    status_t Connect(const char *peer_name);
    status_t SendMessage(CPeerMessage *msg);
    status_t InitClientSidePeer(const char *server, int port);
    status_t InitServiceSidePeer();
    status_t PostCallback(CClosure *closure, int cb_id);
    status_t OnResponse(CPeerMessage *msg);
	status_t OnPartResponse(CPeerMessage *msg);
    status_t StartFetchMessageTask();
    status_t SetCanFetchMessage(bool can);
    status_t SendRequest(CPeerMessage *msg);
    status_t SendResponse(CPeerMessage *msg);
	status_t SendPartResponse(CPeerMessage *msg);
    status_t SendSignal(CPeerMessage *msg);
    status_t SetName(const char *name);
    virtual status_t OnRequest(CPeerMessage *msg) = 0;
    virtual status_t OnSocketConnected();
    virtual status_t OnSocketDisconnected();
    status_t Start();
    CPeerServiceBase();
    virtual ~CPeerServiceBase();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();

    SERVICE_BASE_FLAG_FUNC(IsPeerInitiated,0x00000001);
    SERVICE_BASE_FLAG_FUNC(IsInServiceSide,0x00000002);
};

#endif
