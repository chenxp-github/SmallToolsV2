#include "rpc_service_base.h"
#include "syslog.h"
#include "mem_tool.h"
#include "peerglobals.h"

CRpcServiceBase::CRpcServiceBase()
{
    this->InitBasic();
}

CRpcServiceBase::~CRpcServiceBase()
{
    this->Destroy();
}

status_t CRpcServiceBase::InitBasic()
{
    CPeerServiceBase::InitBasic();
    this->m_dest_peer_name.InitBasic();
    return OK;
}

status_t CRpcServiceBase::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CPeerServiceBase::Init(mgr);
    this->m_dest_peer_name.Init();
    return OK;
}

status_t CRpcServiceBase::Destroy()
{
    CPeerServiceBase::Destroy();
    this->m_dest_peer_name.Destroy();
    this->InitBasic();
    return OK;
}

status_t CRpcServiceBase::Copy(CRpcServiceBase *_p)
{
    ASSERT(0);
    return OK;
}

status_t CRpcServiceBase::Comp(CRpcServiceBase *_p)
{
    ASSERT(0);
    return 0;
}

status_t CRpcServiceBase::Print(CFileBase *_buf)
{
    ASSERT(0);
    return OK;
}

status_t CRpcServiceBase::SendReturnValue(CRpcCallContext *context, CRpcParamBase *ret)
{
    ASSERT(context && ret);

    CMiniBson bson;
    bson.Init();
    bson.StartDocument();
    ret->SaveBson(&bson);
    bson.EndDocument();

    return this->SendResponse(CPeerMessage::NewBsonMessage(
        context->GetFrom()->CStr(),context->GetMethod(),
        context->GetCallbackId(),&bson
    ));
}

status_t CRpcServiceBase::SendPartReturnValue(CRpcCallContext *context, CRpcParamBase *ret)
{
    ASSERT(context && ret);
	
    CMiniBson bson;
    bson.Init();
    bson.StartDocument();
    ret->SaveBson(&bson);
    bson.EndDocument();
	
    return this->SendPartResponse(CPeerMessage::NewBsonMessage(
        context->GetFrom()->CStr(),context->GetMethod(),
        context->GetCallbackId(),&bson
	));
}

int CRpcServiceBase::AddCallback(CClosure *closure)
{
    if(!closure)return 0;    
    GLOBAL_PEER_CALLBACK_MAP(map);    
    int callback_id = map->AllocUniqueId();
    if(!map->AddClosure(closure,callback_id))
    {
        callback_id = 0;
    }
    return callback_id;
}

status_t CRpcServiceBase::SetCallbackTimeout(int cbid,int timeout)
{
    GLOBAL_PEER_CALLBACK_MAP(map);    
    return map->SetCallbackTimeout(cbid,timeout);
}

CMem* CRpcServiceBase::GetDestPeerName()
{
    return &m_dest_peer_name;
}

const char* CRpcServiceBase::GetDestPeerNameStr()
{
    if(m_dest_peer_name.StrLen() == 0)
        return "";
    return m_dest_peer_name.CStr();
}

status_t CRpcServiceBase::SetDestPeerName(CMem *_dest_peer_name)
{
    ASSERT(_dest_peer_name);
    return this->m_dest_peer_name.Copy(_dest_peer_name);
}

status_t CRpcServiceBase::SetDestPeerName(const char *_dest_peer_name)
{
    CMem tmp(_dest_peer_name);
    return this->SetDestPeerName(&tmp);
}

status_t CRpcServiceBase::SendRequest(CRpcParamBase *params,int method, int callback)
{
    CMiniBson *pbson = NULL;
    CMiniBson bson;

    if(params)
    {        
        bson.Init();
        bson.StartDocument();
        params->SaveBson(&bson);
        bson.EndDocument();
        pbson = &bson;
    }
    return CPeerServiceBase::SendRequest(CPeerMessage::NewBsonMessage(
        GetDestPeerNameStr(),method,callback,pbson
    ));
}

status_t CRpcServiceBase::Start()
{
	CPeerServiceBase::Start();
	if(IsInServiceSide())
	{
		this->SetCanFetchMessage(true);
		this->StartFetchMessageTask();    
	}
	return OK;
}