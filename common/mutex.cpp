#include "mutex.h"
#include "syslog.h"

CMutex::CMutex()
{
    this->InitBasic();
}

CMutex::~CMutex()
{
    this->Destroy();
}

status_t CMutex::InitBasic()
{
    this->m_Flags = 0;
    return OK;
}
status_t CMutex::Init()
{
    if(!this->IsInitialized())
    {
        crt_init_mutex(&m_Mutex);
        this->SetIsInitialized(true);
    }
    
    return OK;
}

status_t CMutex::Destroy()
{
    if(this->IsInitialized())
    {
        crt_destroy_mutex(&m_Mutex);
        this->SetIsInitialized(false);
    }
    this->InitBasic();
    return OK;
}

status_t CMutex::Lock()
{
    ASSERT(IsInitialized());
    crt_lock_mutex(&m_Mutex);
    this->SetIsLocked(true);
    return OK;
}

status_t CMutex::Unlock()
{
    ASSERT(IsInitialized());
    crt_unlock_mutex(&m_Mutex);
    this->SetIsLocked(false);
    return OK;
}
