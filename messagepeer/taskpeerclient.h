#ifndef __TASKPEERCLIENT_H
#define __TASKPEERCLIENT_H

#include "cruntime.h"
#include "tasklinkrpc.h"
#include "peermessagestk.h"
#include "messagepeer.h"
#include "weakptr.h"

class CTaskPeerClient:public CTaskLinkRpc{
public:
    enum{
        ERROR_INIT_CHECK_FAIL = 1,
    };

    CWeakPtr<CMessagePeer> iHostPeer;
    int mTaskTcpConnectror;

    CMem *mDataRecvBuf;
    CMem *mHeaderRecvBuf;
    CMem *mMessageHeader;
    CClosure *mCallback;
    bool mCanReadNextPackage;
public:
    status_t SetCanReadNextPackage(bool can_read);
    CClosure* Callback();
    CPeerMessageStk * GetSendingQueue();
    status_t AddMessageToHead(CPeerMessage *msg);
    status_t SendMessage(CPeerMessage *msg);
    CTaskPeerClient();
    ~CTaskPeerClient();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgs);
    status_t Destroy();
    const char * ErrorToString(int err);

    //connection
    status_t OnGetSocket();
    virtual status_t Stop(int err);
    virtual status_t OnSocketError();

    //reading package
    bool CanReadNextPackage();
    status_t OnGotPackageHeader(LINKRPC_HEADER *header,CMem *header_data);
    status_t OnGotPackageData(LINKRPC_HEADER *header,CMem *header_data,CFileBase *data);

    //writing package
    status_t OnPreparePackageToSend();
    status_t OnPackageSendOk();
};

#endif
