#ifndef __WEB_SOCKET_SERVICE_BASE_H
#define __WEB_SOCKET_SERVICE_BASE_H

#include "mem.h"
#include "memfile.h"
#include "websocketserver.h"
#include "callbackmap.h"
#include "web_socket_param_base.h"
#include "web_socket_call_context.h"

#define WS_PART_OK 100

#define BEGIN_NEW_WS_CALLBACK(name,ret_type)\
BEGIN_NEW_CLOSURE(name){\
CLOSURE_PARAM_INT(_e,0);\
UNUSED(_e);\
CLOSURE_PARAM_PTR(CMiniBson*,_bson,2);\
ret_type _ret;\
_ret.Init();\
ASSERT(_ret.LoadBson(_bson));\

#define END_NEW_WS_CALLBACK(name,ret_type)\
return OK;}END_NEW_CLOSURE(name)\

class CWebSocketServiceBase:public CWebSocketServer{
public:
    CCallbackMap m_CallbackMap;

public:
    CWebSocketServiceBase();
    ~CWebSocketServiceBase();

    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
	CCallbackMap* GetCallbackMap();

	status_t SendRequest(CWebSocketParamBase *params,int method, int callback);
	status_t SendReturnValue(CWebSocketCallContext *context, CWebSocketParamBase *ret);
	status_t SendPartReturnValue(CWebSocketCallContext *context, CWebSocketParamBase *ret);

	status_t SetupCallback();
	int AddCallback(CClosure *closure);
	virtual status_t OnRequest(CWebSocketMessage *_message);
	status_t OnResponse(CWebSocketMessage *msg);
	status_t OnPartResponse(CWebSocketMessage *msg);
};

#endif
