#include "minitask.h"
#include "syslog.h"

CMiniTask::CMiniTask()
{
    this->InitBasic();
}
CMiniTask::~CMiniTask()
{
    this->Destroy();
}
status_t CMiniTask::InitBasic()
{
    CTask::InitBasic();
    this->m_Step = 0;
    return OK;
}
status_t CMiniTask::Init(CTaskMgr *mgr)
{
    this->Destroy();
    CTask::Init(mgr);
    return OK;
}
status_t CMiniTask::Destroy()
{
    CTask::Destroy();
    this->InitBasic();
    return OK;
}

status_t CMiniTask::Stop(const char *szFormat, ...)
{
    char szBuffer [4096];
    crt_va_list pArgList;
    crt_va_start(pArgList, szFormat);
    crt_vsprintf(szBuffer, szFormat, pArgList);
    crt_va_end (pArgList);
    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,szBuffer);
    this->Quit();
    return OK;
}
status_t CMiniTask::Goto(int state)
{
    this->m_Step = state;
    return OK;
}
status_t CMiniTask::Start(int state)
{
    this->Resume();
    this->Goto(state);
    return OK;
}

int CMiniTask::Step()
{
    return m_Step;
}
