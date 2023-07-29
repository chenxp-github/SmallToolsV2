#ifndef __REMOTE_DESKTOP_SERVER_H
#define __REMOTE_DESKTOP_SERVER_H

#include "mem.h"
#include "memfile.h"
#include "rpc_service_base.h"
#include "rpc_call_context.h"
#include "remote_desktop_session_manager.h"

class CRemoteDesktopSnapshottorManager;

class CRemoteDesktopServer:public CRpcServiceBase{
public:
	CRemoteDesktopSessionManager m_SessionManager;
	CRemoteDesktopSnapshottorManager *m_SnapshottorManager;
public:
    CRemoteDesktopServer();
    virtual ~CRemoteDesktopServer();

    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();    
    virtual status_t OnSocketConnected();
    virtual status_t OnSocketDisconnected();
    virtual status_t OnRequest(CPeerMessage *msg);

/*##Begin ServiceFunctions_H ##*/
    status_t OnLogin(CPeerMessage *_message);
    status_t OnGetMonitorList(CPeerMessage *_message);
    status_t OnGetNextScreen(CPeerMessage *_message);
    status_t OnSendInput(CPeerMessage *_message);
/*##End ServiceFunctions_H ##*/

	status_t SetRemoteDesktopSnapshottorManager(CRemoteDesktopSnapshottorManager *mgr);
};

#endif



