#ifndef __TASKTCPCONNECTOR_H
#define __TASKTCPCONNECTOR_H

#include "taskmgr.h"
#include "tcp.h"
#include "mem.h"
#include "closure.h"

class CTaskTcpConnector:public CTask{
public:
    enum{
        ERROR_NONE=0,
        ERROR_RESOLVE_HOST_ERROR,
        ERROR_CONNECT_TIMEOUT,
        ERROR_EXCEED_MAX_RETRIES,
        ERROR_CONNECT_ERROR,
        ERROR_CONNECTION_CLOSED,
    };

    enum{
        EVENT_CONNECTED = 1,
        EVENT_STOP,
    };

    CTcpClient *mTcpClient;
    CMem *mServerName;
    int mPort;
    int mTimeout;
    int mConnectTime;
    int_ptr_t *hostoip_context;
    int mStep;
    CClosure *mCallback;
public:
    status_t InitTcpClient(const char *ip_address, int port);
    status_t CheckConnectTimeout(uint32_t interval);
    CClosure* Callback();
    status_t SetTimeout(int to);
    status_t SetServerName(CMem *servername);
    status_t SetServerName(const char *servername);
    status_t SetPort(int port);
    CMem* GetServerName();
    int GetPort();
    CTaskTcpConnector();
    virtual ~CTaskTcpConnector();
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
