#ifndef __THREAD_H
#define __THREAD_H

#include "cruntime.h"

class CThread{
public:
    THREAD_HANDLE m_Handle;
    int m_IsRunning;
public:
    status_t Sleep(int ms);
    status_t Stop();
    bool IsRunning();
    status_t WaitComplete(int timeout=10000);
    status_t WaitCompleteTimeout(int timeout=10000);
    bool IsComplete();
    virtual status_t Run() = 0;
    virtual status_t Cancel();
    status_t Start();
    CThread();
    virtual ~CThread();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
};

#endif
