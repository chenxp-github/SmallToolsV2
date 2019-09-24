#include "web_socket_param_base.h"
#include "syslog.h"
#include "mem_tool.h"

CWebSocketParamBase::CWebSocketParamBase()
{
    this->InitBasic();
}

CWebSocketParamBase::~CWebSocketParamBase()
{
    this->Destroy();
}

status_t CWebSocketParamBase::InitBasic()
{
    return OK;
}

status_t CWebSocketParamBase::Init()
{
    this->InitBasic();

    return OK;
}

status_t CWebSocketParamBase::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CWebSocketParamBase::Copy(CWebSocketParamBase *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    return OK;
}
status_t CWebSocketParamBase::Comp(CWebSocketParamBase *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}
status_t CWebSocketParamBase::Print(CFileBase *_buf)
{
    return OK;
}
status_t CWebSocketParamBase::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    ASSERT(0);
    return OK;
}

status_t CWebSocketParamBase::SaveBson(CMem *_mem)
{
    ASSERT(0);
    return OK;
}

status_t CWebSocketParamBase::LoadBson(CMiniBson *_bson)
{
    ASSERT(0);
    return OK;
}

status_t CWebSocketParamBase::LoadBson(CFileBase *_file)
{
    ASSERT(0);
    return OK;
}

