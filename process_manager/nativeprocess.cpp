#include <sched.h>
#include <sys/resource.h>
#include "nativeprocess.h"
#include "syslog.h"
#include "mem_tool.h"
#include "file.h"
#include "dirmgr.h"

#define IS_SHARED_DIRTY_LINE(line) ( \
line[0]=='S' && line[1]=='h' && line[2]=='a' && line[3]=='r' \
&& line[4]=='e' && line[5]=='d' && line[6]=='_' && line[7]=='D' \
&& line[8]=='i' && line[9]=='r' && line[10]=='t' && line[11]=='y' \
&& line[12]==':' ) \

#define IS_PRIVATE_DIRTY_LINE(line) ( \
line[0]=='P' && line[1]=='r' && line[2]=='i' && line[3]=='v' \
&& line[4]=='a' && line[5]=='t' && line[6]=='e' && line[7]=='_' \
&& line[8]=='D' && line[9]=='i' && line[10]=='r' && line[11]=='t' \
&& line[12]=='y' && line[13]==':') \

#define CHECK_PID() ASSERT(m_Pid>0)

CNativeProcess::CNativeProcess()
{
    this->InitBasic();
}
CNativeProcess::~CNativeProcess()
{
    this->Destroy();
}
status_t CNativeProcess::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    this->m_Pid = 0;
    this->m_ExeName.InitBasic();
    this->m_CmdLine.InitBasic();
    this->m_MemoryUsage = 0;
    this->m_MaxCpuUsage = 0;
    this->m_CpuUsageTick = 0;
    this->m_Flags = 0;
    return OK;
}
status_t CNativeProcess::Init()
{   
    this->InitBasic();
    WEAK_REF_ID_INIT();
    this->m_ExeName.Init();
    this->m_CmdLine.Init(0);
    return OK;
}
status_t CNativeProcess::Destroy()
{
    this->m_ExeName.Destroy();
    this->m_CmdLine.Destroy();
    this->InitBasic();
    return OK;
}
int CNativeProcess::Comp(CNativeProcess *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return m_Pid - _p->m_Pid;
}
status_t CNativeProcess::Copy(CNativeProcess *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_Pid = _p->m_Pid;
    this->m_ExeName.Copy(&_p->m_ExeName);
    this->m_CmdLine.Copy(&_p->m_CmdLine);
    this->m_MemoryUsage = _p->m_MemoryUsage;
    this->m_MaxCpuUsage = _p->m_MaxCpuUsage;
    this->m_CpuUsageLimit = _p->m_CpuUsageLimit;
    this->m_CpuUsageTick = _p->m_CpuUsageTick;
    this->m_Flags = _p->m_Flags;
    return OK;
}
status_t CNativeProcess::Print(CFileBase *_buf)
{
    _buf->Log("Pid = %d",this->m_Pid);
    const char* _str_exename = "<null>";
    if(this->m_ExeName.StrLen() > 0)
        _str_exename = m_ExeName.CStr();
    _buf->Log("ExeName = %s",_str_exename);
    _buf->Log("CmdLine = {");
    _buf->IncLogLevel(1);
    m_CmdLine.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("}");
    _buf->Log("MemoryUsage = %d",this->m_MemoryUsage);
    _buf->Log("MaxCpuUsage = %d",this->m_MaxCpuUsage);
    _buf->Log("CpuUsageLimit = %d",this->m_CpuUsageLimit);
    _buf->Log("CpuUsageTick = %d",this->m_CpuUsageTick);
    return OK;
}
int_ptr_t CNativeProcess::GetPid()
{
    return this->m_Pid;
}
CMem* CNativeProcess::GetExeName()
{
    return &this->m_ExeName;
}
CMemStk* CNativeProcess::GetCmdLine()
{
    return &this->m_CmdLine;
}
int_ptr_t CNativeProcess::GetMemoryUsage()
{
    return this->m_MemoryUsage;
}
int CNativeProcess::GetMaxCpuUsage()
{
    return this->m_MaxCpuUsage;
}
int CNativeProcess::GetCpuUsageLimit()
{
    return this->m_CpuUsageLimit;
}
int CNativeProcess::GetCpuUsageTick()
{
    return this->m_CpuUsageTick;
}
const char* CNativeProcess::GetExeNameStr()
{
    if(m_ExeName.StrLen() <= 0)
        return "";
    return this->m_ExeName.CStr();
}
status_t CNativeProcess::SetPid(int_ptr_t _pid)
{
    this->m_Pid = _pid;
    return OK;
}
status_t CNativeProcess::SetExeName(CMem* _exename)
{
    this->m_ExeName.Copy(_exename);
    return OK;
}

status_t CNativeProcess::SetMemoryUsage(int_ptr_t _memoryusage)
{
    this->m_MemoryUsage = _memoryusage;
    return OK;
}
status_t CNativeProcess::SetMaxCpuUsage(int _maxcpuusage)
{
    this->m_MaxCpuUsage = _maxcpuusage;
    return OK;
}
status_t CNativeProcess::SetCpuUsageLimit(int _cpuusagelimit)
{
    this->m_CpuUsageLimit = _cpuusagelimit;
    return OK;
}
status_t CNativeProcess::SetCpuUsageTick(int _cpuusagetick)
{
    this->m_CpuUsageTick = _cpuusagetick;
    return OK;
}
status_t CNativeProcess::SetExeName(const char *_exename)
{
    CMem _mem(_exename);
    return this->SetExeName(&_mem);
}
status_t CNativeProcess::MakeProcPath(const char *name, CMem *out)
{
    ASSERT(name && out);
    CHECK_PID();
    out->SetSize(0);
    out->Printf("/proc/%d/%s",m_Pid,name);
    return OK;
}

status_t CNativeProcess::LoadProcFile(const char *name, CFileBase *out)
{
    ASSERT(name && out);

    LOCAL_MEM(fn);
    MakeProcPath(name,&fn);

    CFile file;
    file.Init();
    
    out->SetSize(0);

    if(!file.OpenFile(fn.CStr(),"rb"))
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "open %s fail.",fn.CStr()
        );
        return ERROR;
    }

    file.WriteToFile(out,0,MAX_FSIZE);
    return OK;
}

status_t CNativeProcess::UpdateCmdLine()
{
    LOCAL_MEM_WITH_SIZE(cmdline,32*1024);
    ASSERT(this->LoadProcFile("cmdline",&cmdline));

    int_ptr_t size = cmdline.GetSize();
    if(size <= 0)return ERROR;

    m_CmdLine.Destroy();
    m_CmdLine.Init(32);

    LOCAL_MEM(tmp);

    const char *p = cmdline.GetRawBuf();
    char last_char = 0;

    for(int i = 0; i < size; i++)
    {
        char ch = p[i];
        if(ch != 0)
        {
            tmp.Putc(p[i]);
        }
        else if(last_char != 0)
        {
            tmp.Trim();
            m_CmdLine.Push(&tmp);
            tmp.SetSize(0);
        }
        last_char = ch;
    }

    return OK;
}

status_t CNativeProcess::UpdateExeName()
{
    LOCAL_MEM(fn);
    this->MakeProcPath("exe",&fn);
    char buf[1024];
    int size = (int)readlink(fn.CStr(),buf,sizeof(buf));
    if(size > 0)
    {
        ASSERT(size < (int)sizeof(buf));
        buf[size] = 0;
        this->SetExeName(buf);
    }
    return OK;
}

int_ptr_t CNativeProcess::GetDirtyMemoryKb(char *tmp_buf,int_ptr_t tmp_buf_size)
{
    CMem tmp_info;

    tmp_info.Init();

    if(tmp_buf == NULL)
    {   
        tmp_info.Malloc(1024*1024); 
    }
    else
    {
        tmp_info.SetRawBuf(tmp_buf,tmp_buf_size,false);
    }

    this->LoadProcFile("smaps",&tmp_info);

    const char *p = tmp_info.CStr();
    fsize_t offset = 0,len = tmp_info.StrLen();
    char ch,line[4096];
    int line_offset = 0;
    int_ptr_t shared_dirty = 0,private_dirty = 0;
    
    while(offset < len)
    {
        ch = p[offset];
        
        if(ch == '\n')
        {
            line[line_offset] = 0;
            line_offset = 0;
            
            if(IS_PRIVATE_DIRTY_LINE(line))
            {
                private_dirty += atoi(line+14);
            }
            else if(IS_SHARED_DIRTY_LINE(line))
            {           
                shared_dirty += atoi(line+13);
            }           
        }
        else if(line_offset < (int)(sizeof(line)-1))
        {
            line[line_offset++] = ch;
        }
        offset++;
    }
    
    return shared_dirty + private_dirty;
}

status_t CNativeProcess::UpdateMemoryUsage()
{
    m_MemoryUsage = this->GetDirtyMemoryKb();
    return OK;
}

status_t CNativeProcess::Kill(int sig)
{
    return kill(m_Pid,sig) == 0;
}

bool CNativeProcess::IsExist()
{
    LOCAL_MEM(fn);
    this->MakeProcPath("exe",&fn);
    return CDirMgr::IsFileExist(&fn);
}

status_t CNativeProcess::StopCpu(bool stop)
{
    if(stop)
    {       
        if(!IsCpuStopped())
        {
            SetIsCpuStopped(true);
            return this->Kill(SIGSTOP);
        }
    }
    else
    {
        if(IsCpuStopped())
        {
            SetIsCpuStopped(false);
            return this->Kill(SIGCONT);
        }
    }

    return OK;
}

bool CNativeProcess::IsCpuUsageLimited()
{
    return (m_MaxCpuUsage > 0) && (m_MaxCpuUsage > m_CpuUsageLimit);
}

status_t CNativeProcess::LimitCpu(int cpu_limit, int max_cpu_usage)
{
    this->SetCpuUsageLimit(cpu_limit);
    this->SetMaxCpuUsage(max_cpu_usage);
    return OK;
}

int CNativeProcess::SchedGetPriorityMax(int sched_type)
{
    return sched_get_priority_max(sched_type);
}

int CNativeProcess::SchedGetPriorityMin(int sched_type)
{
    return sched_get_priority_min(sched_type);
}

status_t CNativeProcess::SchedSetScheduler(int policy, int priority)
{
    CHECK_PID();
    struct sched_param param;
    param.sched_priority = priority;
    return sched_setscheduler(m_Pid, policy, &param) == 0;
}

int CNativeProcess::SchedGetScheduler()
{
    CHECK_PID();
    return sched_getscheduler(m_Pid);
}

int CNativeProcess::SchedGetSchedPriority()
{
    CHECK_PID();
    struct sched_param param;
    memset(&param,0,sizeof(param));
    sched_getparam(m_Pid,&param);
    return param.sched_priority;
}

int CNativeProcess::SchedGetNiceValue()
{
    CHECK_PID();
    return getpriority(PRIO_PROCESS,m_Pid);
}

int CNativeProcess::SchedSetNiceValue(int nice)
{
    CHECK_PID();
    return setpriority(PRIO_PROCESS,m_Pid,nice) == 0;
}

uint32_t CNativeProcess::SchedGetAffinity()
{
    CHECK_PID();
    cpu_set_t mask;
    CPU_ZERO(&mask);
    sched_getaffinity(m_Pid,sizeof(mask),&mask);
    uint32_t *p = (uint32_t*)&mask;
    return p[0];
}

status_t CNativeProcess::SchedSetAffinity(uint32_t mask)
{
    CHECK_PID();
    cpu_set_t tmp;
    CPU_ZERO(&tmp);
    memcpy(&tmp,&mask,sizeof(mask));
    return sched_setaffinity(m_Pid,sizeof(tmp),&tmp) == 0;
}

