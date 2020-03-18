#ifndef __TASKMGR_H
#define __TASKMGR_H

#include "cruntime.h"
#include "closure.h"

#define BEGIN_FSM() switch(m_Step){
#define END_FSM() case 0:default:this->Suspend();break;}

#define BEGIN_STATE(state) case state:{
#define END_STATE(state)   }break

#define GOTO_STATE(ms,state) do{Sleep(ms);m_Step=state;return OK;}while(0)
#define CONTINUE(ms) do{Sleep(ms);return OK;}while(0)

#define ERR_STR(eno,str) if(err == eno)return str

#define TASK_CONTAINER_CLEAR() \
this->SetTaskMgr(NULL)\

#define TASK_CONTAINER_INIT(mgr) \
this->SetTaskMgr(mgr)\

#define TASK_CONTAINER_DEFINE() \
CTaskMgr *_task_mgr;\
status_t QuitTask(int *id)\
{\
    if(this->_task_mgr)\
        _task_mgr->QuitTask(id);\
    return OK;\
}\
CTask *GetTask(int id)\
{\
    if(this->_task_mgr == NULL)\
        return NULL;\
    return _task_mgr->GetTask(id);\
}\
status_t SuspendTask(int id)\
{\
    if(this->_task_mgr)\
        return _task_mgr->SuspendTask(id);\
    return ERROR;\
}\
status_t ResumeTask(int id)\
{\
    if(this->_task_mgr)\
        return _task_mgr->ResumeTask(id);\
    return ERROR;\
}\
bool IsTask(int id)\
{\
    if(this->_task_mgr)\
        return _task_mgr->IsTask(id);\
    return false;\
}\
status_t SetTaskMgr(CTaskMgr *mgr)\
{\
    this->_task_mgr = mgr;\
    return OK;\
}\
CTaskMgr *GetTaskMgr()\
{\
    return this->_task_mgr;\
}\

class CTask;
class CTaskMgr{
public:
	enum{
		EVENT_SOCKET_CONNECTED = 1,
        EVENT_BEFORE_DEL_TASK,
	};
public:
    CTask **index;
    int top;
    int size;
    int _unique_id;
    int nested;
    bool turbo_on;
    CClosure *callback;
public:
	status_t OnSocketConnected(int sock);
    CClosure* Callback();
    status_t TurboOn();
    int AllocUniqueId();
    status_t QuitTask(int *task_id);
    status_t CheckDelete();
    bool IsTask(int tid);
    status_t DelTask(int task_id);
    bool IsTaskRunning(int task_id);
    status_t ResumeTask(int task_id);
    status_t SuspendTask(int task_id);
    bool Schedule();
    int IdToIndex(int task_id);
    CTask * GetTask(int task_id);
    status_t AddTask(CTask *task);
    CTask * RemoveElem(int i);
    CTask * GetElem(int index);
    CTask * GetTop();
    CTask * Pop();

    CTaskMgr();
    ~CTaskMgr();
    status_t Copy(CTaskMgr *p);
    status_t Print(CFileBase *_buf);

    status_t Clear();
    status_t DelElem(int i);
    status_t DelTop();
    status_t Destroy();
    status_t Init(int init_size=1024);
    status_t InitBasic();
    status_t IsEmpty();
    status_t IsFull();
    status_t Push(CTask *node);
    int GetLen();
    
};

class CTask{
public:
    WEAK_REF_DEFINE();
    TASK_CONTAINER_DEFINE();
public:
    int mId;
    char *mName;
    CTask *mParent;
    int mParentId;
    uint32_t mLastSleepTime;
    uint32_t mSleepTime;
    uint32_t mTotalSleepTime;
    uint32_t mLastRunTime;
    uint32_t mTurboOnTime;
    uint32_t mTurboOnMaxTime;
    uint32_t mFlags;
public:
    status_t SetFlags(uint32_t flags);
    bool IsInvalidInterval(uint32_t interval);
    status_t TurboOn(uint32_t max_time=0xffffffff);
    status_t TurboOff();
    bool IsTurboOn(uint32_t interval);
    status_t SetParentTask(CTask *ptask);
    bool IsParentDead();
    status_t Sleep(uint32_t ms);
    bool IsSleeping(uint32_t interval);
    bool IsDead();
    int GetId();
    status_t SetName(const char *name);
    const char* GetName();
    bool IsRunning();
    status_t Resume();
    status_t Suspend();
    status_t Copy(CTask *pt);
    status_t Quit();
    virtual status_t Run(uint32_t interval) = 0;
    virtual status_t OnMessage(int event, CClosure *param);
    status_t SendMessage(int event, CClosure *param);

    CTask();
    virtual ~CTask();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Print();
    status_t InitBasic();
};


#endif
