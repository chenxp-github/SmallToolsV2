#include "taskmgr.h"
#include "syslog.h"
#include "mem_tool.h"

#define TASK_IS_RUNNING     0x00000001
#define TASK_IS_DELETED     0x00000002
#define TASK_IS_SLEEPING    0x00000004

CTask::CTask()
{
    this->InitBasic();
}
CTask::~CTask()
{
    this->Destroy();
}
status_t CTask::InitBasic()
{
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();
    this->mId = 0;
    this->mName = NULL;
    this->mFlags = 0;
    this->mLastRunTime = 0;
    this->mSleepTime = 0;
    this->mLastSleepTime = 0;
    this->mTotalSleepTime = 0;
    this->mParent = NULL;
    this->mParentId = 0;
    this->mTurboOnMaxTime = 0;
    this->mTurboOnTime = 0;
    return OK;
}
status_t CTask::Init(CTaskMgr *mgr)
{
    this->Destroy();
    
    TASK_CONTAINER_INIT(mgr);
    this->mLastRunTime = crt_get_sys_timer();
    this->mLastSleepTime = this->mLastRunTime;
    if(mgr)
    {
        this->mId = mgr->AllocUniqueId();
        mgr->AddTask(this);
    }
    return OK;
}
status_t CTask::Destroy()
{
    WEAK_REF_DESTROY();
    FREE(mName);
    this->InitBasic();
    return OK;
}
status_t CTask::Copy(CTask *pt)
{
    return OK;
}
status_t CTask::Print()
{
    //add your code
    return TRUE;
}
status_t CTask::Quit()
{
    this->mFlags |= TASK_IS_DELETED;
    return OK;
}
status_t CTask::Suspend()
{
    this->mFlags &= (~TASK_IS_RUNNING);
    return OK;
}
status_t CTask::Resume()
{
    this->mFlags |= TASK_IS_RUNNING;
    return OK;
}
bool CTask::IsRunning()
{
    if(this->IsDead())return false;
    return (this->mFlags&TASK_IS_RUNNING) != 0;
}
const char* CTask::GetName()
{
    return this->mName;
}

status_t CTask::SetName(const char *name)
{
    FREE(mName);
    if(name == NULL)
        return OK;
    int len = strlen(name);
    MALLOC(mName,char,len+1);
    memcpy(mName,name,len+1);
    return OK;
}

int CTask::GetId()
{
    return this->mId;
}
bool CTask::IsDead()
{
    if((this->mFlags & TASK_IS_DELETED) != 0)
        return true;
    if(this->IsParentDead())
        return true;
    return false;
}

bool CTask::IsSleeping(uint32_t interval)
{
    if(this->mFlags & TASK_IS_SLEEPING)
    {
        this->mSleepTime += interval;
        if(this->mSleepTime >= this->mTotalSleepTime)
        {
            this->mFlags &= (~TASK_IS_SLEEPING);
            this->mSleepTime = 0;
            this->mTotalSleepTime = 0;
        }
        return true;
    }
    return false;
}

status_t CTask::Sleep(uint32_t ms)
{
    if(ms > 0)
    {
        this->mFlags |= TASK_IS_SLEEPING;
        this->mSleepTime = 0;
        this->mTotalSleepTime = ms;
    }
    return OK;
}

status_t CTask::SetParentTask(CTask *ptask)
{
    ASSERT(ptask);
    ASSERT(ptask->GetId() > 0);
    this->mParentId = ptask->GetId();
    this->mParent = ptask;
    return OK;
}
bool CTask::IsParentDead()
{
    if(mParent == NULL)
        return false;
    if(mParent->IsDead())
        return true;
    if(mParent->GetId() != this->mParentId)
        return true;
    return false;
}
status_t CTask::OnMessage(int event, CClosure *param)
{
    //override, do nothing
    return OK;
}
status_t CTask::SendMessage(int event, CClosure *param)
{
    return this->OnMessage(event,param);
}

status_t CTask::TurboOn(uint32_t max_time)
{
    mTurboOnTime = 0;
    mTurboOnMaxTime = max_time;
    return OK;
}
status_t CTask::TurboOff()
{
    mTurboOnTime = 0;
    mTurboOnMaxTime = 0;
    return OK;
}
bool CTask::IsTurboOn(uint32_t interval)
{
    if(mTurboOnMaxTime == 0)
        return false;

    mTurboOnTime += interval;

    if(mTurboOnTime >= mTurboOnMaxTime)
    {
        this->TurboOff();
        return false;
    }
    return true;
}

bool CTask::IsInvalidInterval(uint32_t interval)
{
    //system time maybe adjusted.
    return mTotalSleepTime > 10 && interval > mTotalSleepTime*100;
}
status_t CTask::SetFlags(uint32_t flags)
{
    mFlags = flags;
    return OK;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTaskMgr::CTaskMgr()
{
    this->InitBasic();
}
CTaskMgr::~CTaskMgr()
{
    this->Destroy();
}
status_t CTaskMgr::InitBasic()
{
    this->index = NULL;
    this->top = 0;;
    this->size = 0;
    this->_unique_id = 1;
    this->nested = 0;
    this->turbo_on = false;
    this->callback = NULL;
    return OK;
}
status_t CTaskMgr::Init(int init_size)
{
    int i;
    this->InitBasic();
    this->size = init_size;
    MALLOC(this->index,CTask * ,this->size);
    for(i = 0; i < this->size; i++)
        this->index[i] = NULL;
    NEW(callback,CClosure);
    callback->Init();
    return OK;
}
status_t CTaskMgr::Destroy()
{
    int i;

    if(this->index == NULL)
        return ERROR;    
    DEL(callback);
    CTask **p = this->index;
    this->index = NULL;

    for(i = 0; i < this->size; i++)
    {
        if(p[i])
        {
            DEL(p[i]);
        }
    }

    FREE(p);
    this->InitBasic();
    return OK;
}
status_t CTaskMgr::IsEmpty()
{
    if(this->top <= 0)
        return TRUE;
    else
        return FALSE;
}
status_t CTaskMgr::IsFull()
{
    if(this->top >= this->size)
        return TRUE;
    return FALSE;
}
status_t CTaskMgr::Push(CTask *node)
{
    ASSERT(!this->IsFull());

    this->index[top] = node;
    this->top ++;
    return OK;
}
CTask * CTaskMgr::Pop()
{
    if(this->IsEmpty())
        return NULL;
    this->top--;
    return this->index[top];
}

status_t CTaskMgr::Copy(CTaskMgr *p)
{
    ASSERT(0);
    return OK;
}

status_t CTaskMgr::Print(CFileBase *_buf)
{
    ASSERT(0);
    return OK;
}

int CTaskMgr::GetLen()
{
    return this->top;
}
CTask * CTaskMgr::GetTop()
{
    if(this->IsEmpty())
        return NULL;
    return this->index[this->top - 1];
}
status_t CTaskMgr::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->top--;
    DEL(this->index[this->top]);
    return OK;
}
status_t CTaskMgr::Clear()
{
    while(this->DelTop());
    return OK;
}
CTask * CTaskMgr::GetElem(int i)
{
    if(i < 0 || i >= this->top)
        return NULL;
    return this->index[i];
}
status_t CTaskMgr::DelElem(int i)
{
    CTask *p;
    ASSERT(i >= 0 && i < this->top);

    p = this->GetElem(i);
    DEL(p);

    for(int k = i; k < this->top-1; k++)
    {
        this->index[k] = this->index[k + 1];
    }

    this->top --;
    this->index[top] = NULL;

    return OK;
}
CTask * CTaskMgr::RemoveElem(int i)
{
    ASSERT(i >= 0 && i < this->top);

    CTask *p = this->GetElem(i);
    this->index[i] = NULL; //avoid freed
    this->DelElem(i);
    return p;
}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
CTask * CTaskMgr::GetTask(int task_id)
{
    int index = this->IdToIndex(task_id);
    if(index < 0 || index >= this->GetLen())
        return NULL;
    return this->GetElem(index);
}
status_t CTaskMgr::AddTask(CTask *task)
{
    ASSERT(task);
    if(this->IdToIndex(task->GetId()) >= 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "task(%d) already exist,add fail",task->GetId()
        );
        return ERROR;
    }

    CTask *top = this->GetTop();
    if(top)
    {
        ASSERT(task->GetId() > top->GetId());
    }

    task->SetTaskMgr(this);
    return this->Push(task);
}

int CTaskMgr::IdToIndex(int task_id)
{
    if(task_id == 0)return -1;
    if(this->index == NULL)
        return -1;

    int low,high,mid,id;
    low = 0;
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;
        id = this->index[mid]->mId;
        if(id == task_id)
            return mid;
        else if(id > task_id)
            high = mid-1;
        else
            low = mid+1;
    }
    return -1;
}
bool CTaskMgr::Schedule()
{
    uint32_t now,interval;
    int i,len;

    if(!index)
        return FALSE;

    if(this->nested == 0)
        this->CheckDelete();

    bool is_turbo_on = this->turbo_on;

    this->nested ++;
    len = this->GetLen();
    for(i = 0; i < len; i++)
    {
        CTask *pt = this->GetElem(i);
        if(pt == NULL)continue;
        now = crt_get_sys_timer();

        if(!pt->IsRunning())
        {
            pt->mLastSleepTime = now;
            continue;
        }

        interval = now - pt->mLastSleepTime;
        pt->mLastSleepTime = now;

        if(pt->IsInvalidInterval(interval))
            continue;

        if(pt->IsSleeping(interval))
            continue;

        if(pt->IsTurboOn(interval))
        {
            is_turbo_on = true;
        }

        interval = now - pt->mLastRunTime;
        pt->mLastRunTime = now;
        pt->Run(interval);

        if(this->turbo_on)
        {
            is_turbo_on = true;
        }    
    }
    this->nested --;
    this->turbo_on = false;
    return is_turbo_on;
}
status_t CTaskMgr::SuspendTask(int task_id)
{
    CTask *pt = this->GetTask(task_id);
    if(pt == NULL)
        return ERROR;
    pt->Suspend();
    return OK;
}
status_t CTaskMgr::ResumeTask(int task_id)
{
    CTask *pt = this->GetTask(task_id);
    if(pt == NULL)
        return ERROR;
    pt->Resume();
    return OK;
}
bool CTaskMgr::IsTaskRunning(int task_id)
{
    CTask *pt = this->GetTask(task_id);
    if(pt == NULL)
        return ERROR;
    return pt->IsRunning();
}
status_t CTaskMgr::DelTask(int task_id)
{
    int i = this->IdToIndex(task_id);
    if(i >= 0  && i < this->GetLen())
        return this->DelElem(i);
    return ERROR;
}

bool CTaskMgr::IsTask(int tid)
{
    CTask *pt = this->GetTask(tid);
    if(pt == NULL)return false;
    return !pt->IsDead();
}

status_t CTaskMgr::CheckDelete()
{
    for(int i = 0; i < this->GetLen(); i++)
    {
        CTask *pt = this->GetElem(i);
        ASSERT(pt);

        if(pt->IsDead())
        {
            callback->SetParamPointer(1,pt);
            callback->SetParamPointer(2,this);
            callback->Run(EVENT_BEFORE_DEL_TASK);
            //if param 1 is changed to NULL, then 
            //the task is not allowed to delete.
            void *p = callback->GetParamPointer(1);            
            if(p != NULL)
            {
                this->DelElem(i);
                i--;
            }
        }
    }
    return true;
}

status_t CTaskMgr::QuitTask(int *task_id)
{
    ASSERT(task_id);
    CTask *pt = this->GetTask(*task_id);
    if(pt)
    {
        pt->Quit();
        *task_id = 0;
        return OK;
    }

    return ERROR;
}

int CTaskMgr::AllocUniqueId()
{
    this->_unique_id++;
    if(this->_unique_id == 0)
        this->_unique_id ++;
    return this->_unique_id;
}

status_t CTaskMgr::TurboOn()
{
    this->turbo_on = true;
    return OK;
}

CClosure* CTaskMgr::Callback()
{
    return this->callback;
}

status_t CTaskMgr::OnSocketConnected(int sock)
{
	callback->SetParamInt(1,sock);
	callback->SetParamPointer(2,this);
	callback->Run(EVENT_SOCKET_CONNECTED);
	return OK;
}
