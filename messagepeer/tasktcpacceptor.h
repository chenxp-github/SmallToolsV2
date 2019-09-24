#ifndef __TASKTCPACCEPTOR_H
#define __TASKTCPACCEPTOR_H

#include "taskmgr.h"
#include "tcp.h"
#include "closure.h"

class CTaskTcpAcceptor:public CTask{
public:
    enum{
        ERROR_NONE = 0,
        ERROR_BIND_FAIL,
    };

    enum{
        EVENT_NEW_CLIENT = 1,
        EVENT_STOP,
    };

    int mPort;
    int mStep;
    CTcpServer *mServer;
    CClosure *mCallback;
    int mFastModeTimeMs;
public:
    status_t SetFastModeTimeMs(int ms);
    CClosure* Callback();
    status_t SetListenPort(int port);
    CTaskTcpAcceptor();
    virtual ~CTaskTcpAcceptor();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t Run(uint32_t interval);
    const char * ErrorToString(int err);
    status_t ReportError(int err);  
    status_t Start();
    status_t Stop(status_t err);
};

#endif
