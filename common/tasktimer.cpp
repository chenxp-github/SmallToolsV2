#include "tasktimer.h"
#include "syslog.h"
#include "mem_tool.h"

#define STEP_WAIT       1
#define STEP_TRIGGER    2

CTaskTimer::CTaskTimer()
{
    this->InitBasic();
}
CTaskTimer::~CTaskTimer()
{
    this->Destroy();
}
status_t CTaskTimer::InitBasic()
{
    CTask::InitBasic();
    this->timeout = -1;
    this->one_shot = 0;
    this->step = 0;
    this->callback = NULL;

    return OK;
}
status_t CTaskTimer::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    NEW(callback,CClosure);
    callback->Init();

    return OK;
}
status_t CTaskTimer::Destroy()
{
    DEL(callback);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskTimer::Run(uint32_t interval)
{
    mSleepTime += interval;
    if(mSleepTime >= this->timeout)
    {
        callback->SetParamPointer(1,this);
        callback->Run(mSleepTime);
        mSleepTime = 0;
        if(one_shot)this->Quit();
    }
    return OK;
}

status_t CTaskTimer::SetOneShot(bool os)
{
    this->one_shot = os;
    return OK;
}

CTaskTimer * CTaskTimer::NewTimer(CTaskMgr *mgr,uint32_t timeout,bool one_shot)
{
    ASSERT(mgr);
    CTaskTimer *pt;
    NEW(pt,CTaskTimer);
    pt->Init(mgr);
    pt->SetTimeout(timeout);
    pt->one_shot = one_shot;
    pt->step = STEP_WAIT;
    pt->Resume();
    return pt;
}

status_t CTaskTimer::SetTimeout(uint32_t ms)
{
    this->timeout = ms;
    return OK;
}

uint32_t CTaskTimer::GetTimeout()
{
    return this->timeout;
}

CClosure* CTaskTimer::Callback()
{
    return callback;
}

status_t CTaskTimer::Start()
{
    this->step = STEP_WAIT;
    return this->Resume();
}

status_t CTaskTimer::Stop()
{
    this->step = STEP_WAIT;
    this->mSleepTime = 0;
    return this->Quit();
}
