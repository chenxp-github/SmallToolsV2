#ifndef __NATIVEPROCESSMANAGER_H
#define __NATIVEPROCESSMANAGER_H

#include "nativeprocess.h"
#include "filebase.h"
#include "taskmgr.h"

class CNativeProcessManager{
public:
    WEAK_REF_ID_DEFINE();
    TASK_CONTAINER_DEFINE();
    int m_TaskCpuLimit;
private:
    CNativeProcess **m_Index;
    int m_Top;
    int m_Size; 
public:     
    status_t StopAllCpu(bool stop);
    status_t UnlimitAllCpu();
    status_t LimitAllCpu(int cpu_limit, int max_cpu_usage);
    status_t LimitCpu(int_ptr_t pid,int cpu_limit, int max_cpu_usage);
    status_t StopCpuLimitTask();
    status_t StartCpuLimitTask(bool auto_clear);
    CNativeProcess* GetProcessByPid(int_ptr_t pid);
    status_t AddProcess(int_ptr_t pid);
    static status_t FindProcessByName(const char *name_pattern,int_ptr_t *buf, int *buf_size);
    static status_t EnumAllProcess(CClosure *enum_func);
    int CompNode(CNativeProcess *nativeprocess1, CNativeProcess *nativeprocess2);
    status_t CopyNode(CNativeProcess *dst, CNativeProcess *src);
    status_t DelNode(CNativeProcess *nativeprocess);
    CNativeProcess* CloneNode(CNativeProcess *nativeprocess);
    status_t Copy(CNativeProcessManager *stk);
    CNativeProcess* RemoveElem(int index);
    status_t InsElemPtr(int i, CNativeProcess *nativeprocess);
    CNativeProcess* PopPtr();
    status_t AutoResize();
    status_t PushPtr(CNativeProcess *nativeprocess);
    status_t InitBasic();  
    int BSearchPos(CNativeProcess *nativeprocess,int order,int *find_flag);
    status_t InsOrderedPtr(CNativeProcess *nativeprocess,int order,int unique);
    status_t InsOrdered(CNativeProcess *nativeprocess,int order,int unique);
    status_t DelElem(int i);
    status_t InsElem(int i,CNativeProcess *nativeprocess);
    int BSearch(CNativeProcess *nativeprocess,int order);
    CNativeProcess* BSearchNode(CNativeProcess *nativeprocess,int order);
    status_t Sort(int order);
    CNativeProcess* GetElem(int index);
    CNativeProcess* GetTopPtr();
    CNativeProcess* Search(CNativeProcess *nativeprocess);
    int SearchPos(CNativeProcess *nativeprocess);
    CNativeProcessManager();
    status_t Clear();
    status_t DelTop();
    status_t Destroy();
    status_t Init(CTaskMgr *mgr,int init_size);
    bool IsEmpty();
    bool IsFull();
    status_t Pop(CNativeProcess *nativeprocess);
    status_t Print(CFileBase *_buf);
    status_t Push(CNativeProcess *nativeprocess);
    int GetLen();
    ~CNativeProcessManager();
};

#endif

