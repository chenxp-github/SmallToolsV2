#ifndef __MINI_TASK_H
#define __MINI_TASK_H

#include "taskmgr.h"

#define BEGIN_MINI_TASK(task) \
struct C##task:public CMiniTask \

#define END_MINI_TASK(task) ;\
C##task *task; \
NEW(task,C##task)

class CMiniTask:public CTask{
public:
    int m_Step;
public:
    CMiniTask();
    virtual ~CMiniTask();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t Stop(const char *szFormat, ...);
    status_t Goto(int state);
    status_t Start(int state);
    int Step();
};


#endif
