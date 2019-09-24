#include "userfunc.h"
#include "syslog.h"
#include "mem_tool.h"

CUserFunc::CUserFunc()
{
    this->InitBasic();
}
CUserFunc::~CUserFunc()
{
    this->Destroy();
}
status_t CUserFunc::InitBasic()
{
    _Flags_ = 0;
    _UserFunc_.InitBasic();
    return OK;
}
status_t CUserFunc::Init()
{
    this->InitBasic();
    _UserFunc_.Init();
    return OK;
}
status_t CUserFunc::Destroy()
{
    _UserFunc_.Destroy();
    this->InitBasic();
    return OK;
}

CClosure* CUserFunc::Callback()
{
    return &_UserFunc_;
}

bool CUserFunc::IsUserFunc(uint32_t flags)
{
    return (_Flags_ & flags) == flags;
}

status_t CUserFunc::SetUseUserFunc(uint32_t flags)
{
    _Flags_ = flags;
    return OK;
}

uint32_t CUserFunc::GetUseUserFunc()
{
    return _Flags_;
}

