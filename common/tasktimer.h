#ifndef __TASKTIMER_H
#define __TASKTIMER_H

#include "taskmgr.h"
#include "closure.h"

class CTaskTimer:public CTask{
private:
    uint32_t timeout;
    bool one_shot;
    int step;
    CClosure *callback;
public:
    status_t Stop();
    status_t Start();
    CClosure* Callback();
    uint32_t GetTimeout();
    static CTaskTimer * NewTimer(CTaskMgr *mgr,uint32_t timeout,bool one_shot);
    status_t SetOneShot(bool os);
    CTaskTimer();
    virtual ~CTaskTimer();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t Run(uint32_t interval);
    status_t SetTimeout(uint32_t ms);

};

#endif
