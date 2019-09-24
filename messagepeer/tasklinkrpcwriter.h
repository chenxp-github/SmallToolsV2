#ifndef __TASKLINKRPCWRITER_H
#define __TASKLINKRPCWRITER_H

#include "taskmgr.h"
#include "mem.h"
#include "combfile.h"
#include "socketreaderwriter.h"
#include "linkrpccommon.h"

class CTaskLinkRpcWriter:public CTask{
public:
    enum{
        ERROR_NONE = 0,
        ERROR_WRONG_PACKAGE_TAG,
        ERROR_WRONG_CMD,
        ERROR_CONNECTION_CLOSED,
        ERROR_WRITE_TIME_OUT,
        ERROR_WRITE_ERROR,
    };

    enum{
        EVENT_GET_NEXT_PACKAGE = 1,
        EVENT_PACKAGE_SEND_OK,
        EVENT_STOP,
    };

    CSocketReaderWriter *iSocketRwer;
    CFileBase *iHeaderData;
    CFileBase *iData;
    CMem *mHeader;
    CCombFile *mPackage;
    int mStep;
    uint32_t mHearBeatTime;
    CClosure *mCallback;
public:
    status_t WriteImmediately();
    status_t CombPackage();
    status_t SendResponse(CFileBase *header, CFileBase *data);
    status_t SendPackage(int cmd, CFileBase *header, CFileBase *data);
    status_t SendRequest(CFileBase *header, CFileBase *data);
    LINKRPC_HEADER * Header();
    CTaskLinkRpcWriter();
    virtual ~CTaskLinkRpcWriter();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t Run(uint32_t interval);
    const char * ErrorToString(int err);
    status_t ReportError(int err);  
    status_t Start();
    status_t Stop(status_t err);
    CClosure* Callback();
};

#endif
