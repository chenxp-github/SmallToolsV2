#include "rpc_param_base.h"
#include "syslog.h"
#include "mem_tool.h"

CRpcParamBase::CRpcParamBase()
{
    this->InitBasic();
}

CRpcParamBase::~CRpcParamBase()
{
    this->Destroy();
}

status_t CRpcParamBase::InitBasic()
{
    return OK;
}

status_t CRpcParamBase::Init()
{
    this->InitBasic();

    return OK;
}

status_t CRpcParamBase::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CRpcParamBase::Copy(CRpcParamBase *_p)
{
    return OK;
}

status_t CRpcParamBase::Comp(CRpcParamBase *_p)
{
    return 0;
}

status_t CRpcParamBase::Print(CFileBase *_buf)
{
    return OK;
}

status_t CRpcParamBase::SaveBson(CMiniBson *_bson)
{
    ASSERT(0);
    return OK;
}

status_t CRpcParamBase::SaveBson(CMem *_mem)
{
    ASSERT(0);
    return OK;
}

status_t CRpcParamBase::LoadBson(CMiniBson *_bson)
{
    ASSERT(0);
    return OK;
}

status_t CRpcParamBase::LoadBson(CFileBase *_file)
{
    ASSERT(0);
    return OK;
}

