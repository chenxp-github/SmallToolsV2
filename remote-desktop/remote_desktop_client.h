#ifndef __REMOTE_DESKTOP_CLIENT_H
#define __REMOTE_DESKTOP_CLIENT_H

#include "mem.h"
#include "memfile.h"
#include "rpc_service_base.h"
#include "rpc_call_context.h"
#include "remote_desktop_input.h"

class CRemoteDesktopClient:public CRpcServiceBase{

public:
    CRemoteDesktopClient();
    virtual ~CRemoteDesktopClient();

    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();    
    virtual status_t OnSocketConnected();
    virtual status_t OnSocketDisconnected();
    virtual status_t OnRequest(CPeerMessage *msg);

/*##Begin ServiceFunctions_H ##*/
    status_t Login(int _version, CClosure *_callback);
    status_t GetMonitorList(CClosure *_callback);
    status_t GetNextScreen(CMem *_monitorName, int _curFrameNo, int _bytePerPixel, int _full, CClosure *_callback);
    status_t SendInput(CRemoteDesktopInput *_input, CClosure *_callback);
/*##End ServiceFunctions_H ##*/
};

#endif



