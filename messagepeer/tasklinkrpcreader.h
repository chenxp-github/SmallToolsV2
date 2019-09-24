#ifndef __TASKLINKRPCREADER_H
#define __TASKLINKRPCREADER_H

#include "taskmgr.h"
#include "linkrpccommon.h"
#include "mem.h"
#include "socketreaderwriter.h"

class CTaskLinkRpcReader:public CTask{
public:

    enum{
        ERROR_NONE = 0,
        ERROR_READ_TIME_OUT,
        ERROR_READ_ERROR,
        ERROR_CONNECTION_CLOSED,
        ERROR_WRONG_PACKAGE_TAG,
        ERROR_WRONG_CMD,
    };

    enum{
        EVENT_GOT_HEADER = 1,
        EVENT_GOT_DATA,
        EVENT_STOP,
        EVENT_CAN_READ_NEXT_PACKAGE,
    };

    CSocketReaderWriter *iSocketRwer;
    CMem *iHeaderBuf;
    CFileBase *iDataBuf;
    CMem *mHeader;
    int mStep;
    CClosure *mCallback;
public:
    status_t SetDataBuf(CFileBase *iBuf);
    status_t SetHeaderBuf(CMem *iBuf);
    LINKRPC_HEADER * Header();
    CTaskLinkRpcReader();
    virtual ~CTaskLinkRpcReader();
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
