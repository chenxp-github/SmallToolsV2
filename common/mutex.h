#ifndef __CMUTEX_H
#define __CMUTEX_H

#include "cruntime.h"

class CMutex  
{
public:
    MUTEX_TYPE m_Mutex;
    uint32_t m_Flags;
public:
    CMutex();
    virtual ~CMutex();
    status_t Unlock();
    status_t Lock();
    status_t Destroy();
    status_t Init();
    status_t InitBasic();

    FLAG_FUNC(m_Flags,IsInitialized,0x00000001);
    FLAG_FUNC(m_Flags,IsLocked,0x00000002);
};

#endif 

