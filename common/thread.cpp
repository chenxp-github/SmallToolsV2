#include "thread.h"
#include "sys_log.h"

CThread::CThread()
{
    this->InitBasic();
}
CThread::~CThread()
{
    this->Destroy();
}
status_t CThread::InitBasic()
{
    this->m_Handle = 0;
    this->m_IsRunning = false;
    return OK;
}
status_t CThread::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t CThread::Destroy()
{
    this->Stop();
    this->WaitComplete();
    this->InitBasic();
    return OK;
}

static void _thread_proc(void *lpvoid)
{
    CThread *self = (CThread*)lpvoid;
    VASSERT(self);
    self->Run();
    self->m_IsRunning = false;
    self->m_Handle = 0;
}

status_t CThread::Start()
{
    ASSERT(!this->m_IsRunning);
    ASSERT(this->m_Handle == 0);
    this->m_IsRunning = true;
    this->m_Handle = crt_create_thread(_thread_proc, 1024*1024,this);
    return m_Handle != 0;
}

bool CThread::IsComplete()
{
    return m_Handle == 0;
}

status_t CThread::WaitCompleteTimeout(int timeout)
{
    int t = 0;
    int sleep_time = 10;
    while(!IsComplete())
    {
        crt_msleep(sleep_time);
        t += sleep_time;
        if(t > timeout)
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_WARNING,
                "thread WaitComplete timeout: %d",t 
            );
            break;
        }
    }
    return OK;
}

status_t CThread::WaitComplete(int timeout)
{
    int t = 0;
    int sleep_time = 10;
    while(!IsComplete())
    {
        crt_msleep(sleep_time);
        t += sleep_time;
        if(t > timeout)
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_WARNING,
                "thread WaitComplete timeout: %d",t 
            );
            sleep_time = 1000;
        }
    }
    return OK;
}

bool CThread::IsRunning()
{
    return m_IsRunning != 0;
}

status_t CThread::Stop()
{
    m_IsRunning = 0;
    return OK;
}
status_t CThread::Cancel()
{
    THREAD_HANDLE old_handle = m_Handle;
    this->m_Handle = 0;
    this->m_IsRunning = 0;
    return crt_cancel_thread(old_handle);
}

status_t CThread::Sleep(int ms)
{
    if(ms <= 50)
    {
        crt_msleep(ms);
        return OK;
    } 
    
    int t = 0;
    while(m_IsRunning && t < ms)
    {
        crt_msleep(50);
        t += 50;
    }

    return OK;
}