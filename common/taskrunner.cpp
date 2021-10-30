#include "taskrunner.h"
#include "sys_log.h"
#include "mem_tool.h"

struct closure_extra_info{
    uint32_t total_delay;
    uint32_t cur_delay;
    bool run_end;
    bool do_not_auto_delete;
};

static status_t free_extra_info(CClosure *closure);

CTaskRunner::CTaskRunner()
{
    this->InitBasic();
}
CTaskRunner::~CTaskRunner()
{
    this->Destroy();
}
status_t CTaskRunner::InitBasic()
{
    m_ClosureList.InitBasic();
    m_LastTimerVal = 0;
    m_ThreadId = 0;
    m_Mutex.InitBasic();
    return OK;
}
status_t CTaskRunner::Init()
{
    this->InitBasic();
    m_ClosureList.Init(1024);
    m_LastTimerVal = crt_get_sys_timer();
    m_ThreadId = crt_get_current_thread_id();
    m_Mutex.Init();
    return OK;
}
status_t CTaskRunner::Destroy()
{
    for(int i = 0; i < m_ClosureList.GetLen(); i++)
    {
        free_extra_info(m_ClosureList.GetElem(i));
    }
    m_ClosureList.Destroy();
    m_Mutex.Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskRunner::Copy(CTaskRunner *p)
{
    ASSERT(0);
    return OK;
}

status_t CTaskRunner::Print(CFileBase *_buf)
{
    ASSERT(0);
    return OK;
}

status_t CTaskRunner::AddClosure(CClosure *closure,uint32_t delay)
{
    ASSERT(closure);
    ASSERT(closure->IsOnHeap());
    ASSERT(closure->user_data == NULL);

    if(m_ClosureList.m_Index == NULL)
    {
        m_ClosureList.DelNode(closure);
        return ERROR;
    }

    struct closure_extra_info *info;
    MALLOC(info,struct closure_extra_info,1);
    closure->user_data = (void*)info;

    info->cur_delay = 0;
    info->total_delay = delay;
    info->run_end = false;
    info->do_not_auto_delete = false;

    m_Mutex.Lock();
    status_t ret = m_ClosureList.PushPtr(closure);
    m_Mutex.Unlock();

    return ret;
}

//infinite wait main thread
status_t CTaskRunner::AddClosureAndWait(CClosure *closure, int timeout, int *running)
{
    ASSERT(closure);
    ASSERT(closure->IsOnHeap());
    ASSERT(closure->user_data == NULL);
    ASSERT(timeout < 0);
    ASSERT(m_ThreadId != crt_get_current_thread_id());

    if(m_ClosureList.m_Index == NULL)
    {
        m_ClosureList.DelNode(closure);
        return ERROR;
    }
    
    struct closure_extra_info *info;
    MALLOC(info,struct closure_extra_info,1);
    closure->user_data = (void*)info;

    info->cur_delay = 0;
    info->total_delay = 0;
    info->run_end = false;
    info->do_not_auto_delete = true;

    m_Mutex.Lock();
    status_t ret = m_ClosureList.PushPtr(closure);
    m_Mutex.Unlock();
    ASSERT(ret);

    ret = OK;

    int t = 0;
    int is_breaked = 0;

    while(!info->run_end)
    {
        if(!IsRunning())
        {
            is_breaked = true;
            break;
        }

        if(running)
        {
            if(!(*running))
            {
                is_breaked = true;
                break;
            }
        }

        t += 1;
        if(t > (-timeout))
        {
            t = 0;
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "taskrunner: wait closure too long time.");
        }        
        crt_msleep(1);
    }

    if(!is_breaked)
    {
        FREE(info);
    }
    return ret;
}

int CTaskRunner::Schedule()
{
    uint32_t now = crt_get_sys_timer();
    uint32_t interval = now - m_LastTimerVal;
    m_LastTimerVal = now;

    int has_zero_delay_tasks = 0;

    for(int i = 0; i < m_ClosureList.GetLen(); i++)
    {
        m_Mutex.Lock();   
        CClosure *closure = m_ClosureList.GetElem(i);
        m_Mutex.Unlock();
        
        ASSERT(closure);
        struct closure_extra_info *ex_info = (struct closure_extra_info *)closure->user_data;
        ASSERT(ex_info);

        if(ex_info->total_delay == 0)
            has_zero_delay_tasks ++;

        ex_info->cur_delay += interval;
        if(ex_info->cur_delay >= ex_info->total_delay)
        {
            closure->Run();
            ex_info->run_end = true;
            if(!ex_info->do_not_auto_delete)
            {
                free_extra_info(closure);
            }
            else
            {
                closure->user_data = NULL;
            }        
            m_Mutex.Lock();            
            m_ClosureList.DelElem(i);
            m_Mutex.Unlock();

            i--;
        }
    }

    return has_zero_delay_tasks;
}

int CTaskRunner::GetLen()
{
    return m_ClosureList.GetLen();
}

static status_t free_extra_info(CClosure *closure)
{
    ASSERT(closure);
    struct closure_extra_info *ex_info = (struct closure_extra_info *)closure->user_data;
    if(ex_info)
    {
        FREE(ex_info);
        closure->user_data = NULL;
    }
    return OK;
}

bool CTaskRunner::IsRunning()
{
    return m_ThreadId != 0;
}

status_t CTaskRunner::Wait(int n)
{
    while(GetLen() > n)
    {
        crt_msleep(1);
    }
    return OK;
}