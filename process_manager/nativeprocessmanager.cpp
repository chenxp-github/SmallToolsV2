#include "nativeprocessmanager.h"
#include "mem_tool.h"
#include "syslog.h"
#include "dirmgr.h"
#include "misc.h"
#include "minitask.h"

CNativeProcessManager::CNativeProcessManager()
{
    this->InitBasic();  
}
CNativeProcessManager::~CNativeProcessManager()
{
    this->Destroy();
}
status_t CNativeProcessManager::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    TASK_CONTAINER_CLEAR();
    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;
    this->m_TaskCpuLimit = 0;
    return OK;
}
status_t CNativeProcessManager::Init(CTaskMgr *mgr,int init_size)
{
    int i;
    this->InitBasic();  
    WEAK_REF_ID_INIT();
    this->SetTaskMgr(mgr);
    this->m_Size = init_size;
    MALLOC(this->m_Index,CNativeProcess* ,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}
status_t CNativeProcessManager::Destroy()
{
    int i;
    if(this->m_Index == NULL)
        return ERROR;
    this->StopAllCpu(false);
    QuitTask(&m_TaskCpuLimit);
    for(i = 0; i < this->m_Top; i++)
    {
        this->DelNode(this->m_Index[i]);
    }
    FREE(this->m_Index);
    this->InitBasic();  
    return OK;
}
bool CNativeProcessManager::IsEmpty()
{
    return this->m_Top <= 0;
}
bool CNativeProcessManager::IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t CNativeProcessManager::AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,CNativeProcess*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t CNativeProcessManager::Push(CNativeProcess *nativeprocess)
{
    CNativeProcess *tmp;
    ASSERT(nativeprocess);
    tmp = this->CloneNode(nativeprocess);
    if(!this->PushPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CNativeProcessManager::PushPtr(CNativeProcess *nativeprocess)
{
    ASSERT(nativeprocess);  
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = nativeprocess;
    this->m_Top++;
    return OK;
}

CNativeProcess * CNativeProcessManager::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

status_t CNativeProcessManager::Pop(CNativeProcess *nativeprocess)
{
    ASSERT(!this->IsEmpty());   
    this->m_Top--;
    this->CopyNode(nativeprocess,this->m_Index[this->m_Top]);
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
int CNativeProcessManager::GetLen()
{
    return this->m_Top;
}

int CNativeProcessManager::SearchPos(CNativeProcess *nativeprocess)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],nativeprocess) == 0)
            return i;
    }
    return -1;
}

CNativeProcess* CNativeProcessManager::Search(CNativeProcess *nativeprocess)
{
    int pos = this->SearchPos(nativeprocess);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}
CNativeProcess* CNativeProcessManager::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}
status_t CNativeProcessManager::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t CNativeProcessManager::Clear()
{
    while(this->DelTop());
    return OK;
}

CNativeProcess* CNativeProcessManager::GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

CNativeProcess* CNativeProcessManager::BSearchNode(CNativeProcess *nativeprocess,int order)
{
    return this->GetElem(this->BSearch(nativeprocess,order));
}
int CNativeProcessManager::BSearch(CNativeProcess *nativeprocess,int order)
{
    int find,pos;
    pos = this->BSearchPos(nativeprocess,order,&find);
    if(find) return pos;
    return -1;
}

status_t CNativeProcessManager::InsElemPtr(int i, CNativeProcess *nativeprocess)
{
    int k;

    ASSERT(nativeprocess);
    ASSERT(i >= 0 && i <= this->m_Top);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(k = this->m_Top; k > i; k--)
    {
        this->m_Index[k] = this->m_Index[k - 1];
    }
    this->m_Index[i] = nativeprocess;
    this->m_Top++;
    return OK;
}

status_t CNativeProcessManager::InsElem(int i, CNativeProcess *nativeprocess)
{
    CNativeProcess *tmp;
    ASSERT(nativeprocess);
    tmp = this->CloneNode(nativeprocess);
    if(!this->InsElemPtr(i,tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CNativeProcess* CNativeProcessManager::RemoveElem(int index)
{
    CNativeProcess *p;
    int k;
    
    ASSERT(index >= 0 && index < this->m_Top);
    p = this->GetElem(index);

    for(k = index; k < this->m_Top-1; k++)
    {
        this->m_Index[k] = this->m_Index[k + 1];
    }
    this->m_Top --;
    this->m_Index[this->m_Top] = NULL;
    return p;
}

status_t CNativeProcessManager::DelElem(int i)
{
    CNativeProcess *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CNativeProcessManager::InsOrderedPtr(CNativeProcess *nativeprocess, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(nativeprocess,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,nativeprocess);
}

status_t CNativeProcessManager::InsOrdered(CNativeProcess *nativeprocess, int order,int unique)
{
    int pos,find;
    ASSERT(nativeprocess);
    pos = this->BSearchPos(nativeprocess,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElem(pos,nativeprocess);
}

int CNativeProcessManager::BSearchPos(CNativeProcess *nativeprocess, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = this->CompNode(this->m_Index[mid],nativeprocess);
        if(comp == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
    
        if(order != 0) comp = -comp;
        if(comp>0)high=mid-1;else low=mid+1;
    }
    *find_flag = FALSE;
    return low;
}

status_t CNativeProcessManager::Sort(int order)
{
    int i,len;
    CNativeProcessManager tmp;

    len = this->GetLen();
    tmp.Init(GetTaskMgr(),len);

    for(i = 0; i < len; i++)
    {
        tmp.PushPtr(this->GetElem(i));
    }

    this->m_Top = 0;
    for(i = 0; i < len; i++)
    {
        this->InsOrderedPtr(tmp.PopPtr(),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.m_Index[i] = NULL;
    }

    return OK;
}
status_t CNativeProcessManager::Copy(CNativeProcessManager *stk)
{
    int i;

    ASSERT(stk);
    this->Destroy();
    this->Init(GetTaskMgr(),stk->GetLen());
    for(i = 0; i < stk->GetLen(); i++)
    {
        CNativeProcess *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
status_t CNativeProcessManager::Print(CFileBase *_buf)
{
    for(int i = 0; i < this->m_Top; i++)
    {
        if(this->m_Index[i])
        {
            _buf->Log("[%d]={",i);
            _buf->IncLogLevel(1);
            this->m_Index[i]->Print(_buf);
            _buf->IncLogLevel(-1);
            _buf->Log("}");
        }
    }
    _buf->Log("top = %d",this->m_Top);
    _buf->Log("size = %d",this->m_Size);
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CNativeProcessManager::CopyNode(CNativeProcess *dst, CNativeProcess *src)
{
    return dst->Copy(src);
}

CNativeProcess* CNativeProcessManager::CloneNode(CNativeProcess *nativeprocess)
{
    CNativeProcess *tmp;
    NEW(tmp,CNativeProcess);
    tmp->Init();
    this->CopyNode(tmp,nativeprocess);
    return tmp;
}

status_t CNativeProcessManager::DelNode(CNativeProcess *nativeprocess)
{
    DEL(nativeprocess);
    return OK;
}

int CNativeProcessManager::CompNode(CNativeProcess *nativeprocess1, CNativeProcess *nativeprocess2)
{
    ASSERT(nativeprocess1 && nativeprocess2);
    return nativeprocess1->Comp(nativeprocess2);
}

status_t CNativeProcessManager::EnumAllProcess(CClosure *enum_func)
{
    ASSERT(enum_func);

    BEGIN_CLOSURE(func)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(char*,filename,1);
        CLOSURE_PARAM_PTR(char*,fullpath,2);
        CLOSURE_PARAM_PTR(CClosure*,enum_func,10);

        if(event == CDirMgr::EVENT_BEGIN_DIR)
        {
            int pid = atoi(filename);
            if(pid > 0)
            {
                CNativeProcess process;
                process.Init();
                process.SetPid(pid);
                enum_func->SetParamPointer(0,&process);
                enum_func->Run();
            }
        }
        return OK;
    }
    END_CLOSURE(func);

    func.SetParamPointer(10,enum_func);

    CDirMgr::SearchDir("/proc/",0,&func);
    return OK;
}

status_t CNativeProcessManager::FindProcessByName(const char *name_pattern,int_ptr_t *buf, int *buf_size)
{
    ASSERT(name_pattern&&buf&&buf_size);

    BEGIN_CLOSURE(func)
    {
        CLOSURE_PARAM_PTR(CNativeProcess*,process,0);
        CLOSURE_PARAM_PTR(char*,name_pattern,5);
        CLOSURE_PARAM_PTR(int_ptr_t*,buf,6);
        CLOSURE_PARAM_PTR(int*,buf_size,7);
        CLOSURE_PARAM_PTR(int*,pindex,8);

        if(process->IsExist())
        {
            process->UpdateExeName();
            if(wild_match(name_pattern,process->GetExeNameStr()))
            {
                if(*pindex < *buf_size)
                {
                    buf[(*pindex)++] = process->GetPid();
                }
            }
        }
        return OK;
    }
    END_CLOSURE(func);

    int index = 0;
    func.SetParamPointer(5,(void*)name_pattern);
    func.SetParamPointer(6,buf);
    func.SetParamPointer(7,buf_size);
    func.SetParamPointer(8,&index);
    EnumAllProcess(&func);
    *buf_size = index;
    return OK;
}

status_t CNativeProcessManager::AddProcess(int_ptr_t pid)
{
    CNativeProcess *p;
    NEW(p,CNativeProcess);
    p->Init();
    p->SetPid(pid);
    if(!this->InsOrderedPtr(p,0,1))
    {
        DEL(p);
    }
    return OK;
}

CNativeProcess* CNativeProcessManager::GetProcessByPid(int_ptr_t pid)
{
    CNativeProcess tmp;
    tmp.Init();
    tmp.SetPid(pid);
    return BSearchNode(&tmp,0);
}

status_t CNativeProcessManager::StartCpuLimitTask(bool auto_clear)
{
    QuitTask(&m_TaskCpuLimit);
    this->StopAllCpu(true);

    BEGIN_MINI_TASK(task)
    {
        CNativeProcessManager *self;
        bool auto_clear;

        status_t Run(uint32_t interval)
        {
            if(self->GetLen() == 0)
            {
                this->Quit();
                return ERROR;
            }

            for(int i = 0; i < self->GetLen(); i++)
            {
                CNativeProcess *process = self->GetElem(i);
                if(!process->IsCpuUsageLimited())
                {
                    process->StopCpu(false);
                    continue;
                }

                bool success = process->StopCpu(process->m_CpuUsageTick >= process->m_CpuUsageLimit);
                if(process->m_CpuUsageTick >= process->m_MaxCpuUsage)
                {
                    process->m_CpuUsageTick = 0;
                    process->StopCpu(false);
                }
                process->m_CpuUsageTick++;

                if(auto_clear && (!success))
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_WARNING,
                        "process %d is not alive anymore.",process->GetPid()
                    );
                    self->DelElem(i);
                    i--;
                }
            }

            return OK;
        }
    }
    END_MINI_TASK(task);

    task->Init(GetTaskMgr());
    task->Start(1);
    task->self = this;
    task->auto_clear = auto_clear;
    m_TaskCpuLimit = task->GetId();
    return OK;
}

status_t CNativeProcessManager::StopCpuLimitTask()
{
    this->StopAllCpu(false);
    QuitTask(&m_TaskCpuLimit);
    return OK;
}

status_t CNativeProcessManager::LimitCpu(int_ptr_t pid, int cpu_limit, int max_cpu_usage)
{
    ASSERT(pid != 0);
    this->AddProcess(pid);
    CNativeProcess *p = this->GetProcessByPid(pid);
    if(p)
    {
        return p->LimitCpu(cpu_limit,max_cpu_usage);
    }
    return ERROR;
}

status_t CNativeProcessManager::LimitAllCpu(int cpu_limit, int max_cpu_usage)
{
    for(int i = 0; i < this->GetLen(); i++)
    {
        CNativeProcess *process = this->GetElem(i);
        process->LimitCpu(cpu_limit,max_cpu_usage);
    }
    return OK;
}

status_t CNativeProcessManager::UnlimitAllCpu()
{
    for(int i = 0; i < this->GetLen(); i++)
    {
        CNativeProcess *process = this->GetElem(i);
        process->SetMaxCpuUsage(0);
    }
    return OK;
}

status_t CNativeProcessManager::StopAllCpu(bool stop)
{
    for(int i = 0; i < this->GetLen(); i++)
    {
        CNativeProcess *process = this->GetElem(i);
        if(process->IsCpuUsageLimited() && stop)
            process->StopCpu(true);
        else
            process->StopCpu(false);            
    }
    return OK;
}
