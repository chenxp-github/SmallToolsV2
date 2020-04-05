#include "sharedmemory.h"
#include "syslog.h"
#include "mem_tool.h"
#include <sys/ipc.h>
#include <sys/shm.h>

CSharedMemory::CSharedMemory()
{
    this->InitBasic();
}
CSharedMemory::~CSharedMemory()
{
    this->Destroy();
}
status_t CSharedMemory::InitBasic()
{
    WEAK_REF_CLEAR();
    this->m_ShmId = -1;
    m_Flags = 0;
    m_Data = NULL;
    m_Name = 0;
    m_Size = 0;
    return OK;
}
status_t CSharedMemory::Init()
{
    this->InitBasic();    
    return OK;
}
status_t CSharedMemory::Destroy()
{
    WEAK_REF_DESTROY();
    this->Close();
    this->InitBasic();
    return OK;
}

status_t CSharedMemory::Close()
{
    if(this->m_ShmId < 0)
        return ERROR;
    
    if(m_Data)
    {
        int ret = shmdt(m_Data);
        ASSERT(ret == 0);
        m_Data = NULL;
    }

    return OK;
}

status_t CSharedMemory::Unlink()
{
    this->Close();

    if(m_ShmId >= 0)
    {
        int ret = shmctl(m_ShmId,IPC_RMID,0);
        ASSERT(ret >= 0);
        m_ShmId = -1;
        return OK;
    }
    else
    {
        int tmpid = shmget(m_Name,0,0);
        if(tmpid >= 0)
        {
            int ret = shmctl(tmpid,IPC_RMID,0);
            ASSERT(ret >= 0);
            return OK;
        }
    }
    return ERROR;
}

status_t CSharedMemory::OpenCreate(int size)
{
    status_t ret = this->Open(size,SHM_R|SHM_W|IPC_CREAT|IPC_EXCL);
    this->SetIsOwner(true);
    return ret;
}

status_t CSharedMemory::OpenReadOnly()
{
    return this->Open(0,SHM_R|IPC_EXCL);
}

status_t CSharedMemory::SetName(int name)
{
    m_Name = name;
    return OK;
}

status_t CSharedMemory::Open(int size, uint32_t flags)
{
    ASSERT(m_Name != 0);
    ASSERT(m_ShmId < 0);
    m_ShmId = shmget(m_Name,size,flags);
    if(m_ShmId < 0)
    {
        XLOG(LOG_LEVEL_ERROR,LOG_MODULE_COMMON,
            "open shared memory 0x%x fail,mode=0x%x",m_Name,flags
        );
        return ERROR;
    }
    m_Data = (char*)shmat(m_ShmId,0,0);
    ASSERT(m_Data);
    
    struct shmid_ds ds;
    memset(&ds,0,sizeof(ds));
    ASSERT(shmctl(m_ShmId,IPC_STAT,&ds) == 0);

    m_Size = ds.shm_segsz;

    return OK;
}

status_t CSharedMemory::OpenReadWrite()
{
    return this->Open(0,SHM_R|SHM_W|IPC_EXCL);
}

int CSharedMemory::GetSize()
{
    return m_Size;
}

char *CSharedMemory::GetData()
{
    return m_Data;
}

status_t CSharedMemory::Zero()
{
    ASSERT(m_Data);
    memset(m_Data,0,m_Size);
    return OK;
}
