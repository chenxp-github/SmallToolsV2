#ifndef __S2_TASKRUNNER_H
#define __S2_TASKRUNNER_H

#include "cruntime.h"
#include "filebase.h"
#include "closurelist.h"
#include "mutex.h"

class CTaskRunner{
public:
    CClosureList m_ClosureList;
    uint32_t m_LastTimerVal;
    CMutex m_Mutex;
    int_ptr_t m_ThreadId;
public:
    status_t Wait(int n);
    int Schedule();
    status_t AddClosure(CClosure *closure,uint32_t delay);
    status_t AddClosureAndWait(CClosure *closure, int timeout, int *running);
    CTaskRunner();
    virtual ~CTaskRunner();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CTaskRunner *p);
    status_t Print(CFileBase *_buf);
    int GetLen();
    bool IsRunning();
};

#endif
