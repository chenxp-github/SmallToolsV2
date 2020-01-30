#ifndef __RPC_SERVICE_BASE_H
#define __RPC_SERVICE_BASE_H

#include "mem.h"
#include "memfile.h"
#include "minibson.h"
#include "peerservicebase.h"
#include "closure.h"
#include "rpc_call_context.h"
#include "rpc_param_base.h"

#define BEGIN_NEW_LINKRPC_CALLBACK(name,ret_type)\
BEGIN_NEW_CLOSURE(name){\
    CLOSURE_PARAM_INT(_e,0);\
    X_UNUSED(_e);\
    ret_type _ret;\
    _ret.Init();\
    if(closure->GetParamType(2) == PARAM_TYPE_POINTER)\
    {\
        CLOSURE_PARAM_PTR(CMiniBson*,_bson,2);\
        ASSERT(_ret.LoadBson(_bson));\
    }\

#define END_NEW_LINKRPC_CALLBACK(name,ret_type)\
return OK;}END_NEW_CLOSURE(name)\

class CRpcServiceBase:public CPeerServiceBase{
public:
    CMem m_dest_peer_name;

public:
	int AddCallback(CClosure *closure);
	status_t SendReturnValue(CRpcCallContext *context, CRpcParamBase *ret);
	status_t SendPartReturnValue(CRpcCallContext *context, CRpcParamBase *ret);
    status_t SendRequest(CRpcParamBase *params,int method, int callback);

    CRpcServiceBase();
    ~CRpcServiceBase();

    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();

    status_t Copy(CRpcServiceBase *_p);
    int Comp(CRpcServiceBase *_p);
    status_t Print(CFileBase *_buf);
    CMem* GetDestPeerName();
    const char* GetDestPeerNameStr();
    status_t SetDestPeerName(CMem *_dest_peer_name);
    status_t SetDestPeerName(const char *_dest_peer_name);	

	status_t Start();
};

#endif
