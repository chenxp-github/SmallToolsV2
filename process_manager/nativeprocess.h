#ifndef __S2_NATIVEPROCESS_H
#define __S2_NATIVEPROCESS_H

#include "cruntime.h"
#include "filebase.h"
#include "mem.h"
#include "memstk.h"

#define NATIVE_PROCESS_FLAG_FUNC(func,bit) FLAG_FUNC(m_Flags,func,bit)

class CNativeProcess{
public:
    WEAK_REF_ID_DEFINE();
public:
    int_ptr_t m_Pid;
    CMem m_ExeName;
    CMemStk m_CmdLine;
    int_ptr_t m_MemoryUsage; //KB
    int m_MaxCpuUsage;
    int m_CpuUsageLimit;
    int m_CpuUsageTick;
    int32_t m_Flags;
public:
    status_t SchedSetAffinity(uint32_t mask);
    uint32_t SchedGetAffinity();
    int SchedSetNiceValue(int nice);
    int SchedGetNiceValue();
    int SchedGetSchedPriority();
    int SchedGetScheduler();
    status_t SchedSetScheduler(int policy, int priority);
    static int SchedGetPriorityMax(int sched_type);
    static int SchedGetPriorityMin(int sched_type);
    status_t LimitCpu(int cpu_limit, int max_cpu_usage);
    bool IsCpuUsageLimited();
    status_t StopCpu(bool stop);
    bool IsExist();
    status_t Kill(int sig);
    status_t UpdateMemoryUsage();
    int_ptr_t GetDirtyMemoryKb(char *tmp_buf=NULL,int_ptr_t tmp_buf_size=0);
    status_t UpdateExeName();
    status_t UpdateCmdLine();
    status_t LoadProcFile(const char *name, CFileBase *out);
    status_t MakeProcPath(const char *name, CMem *out);
    CNativeProcess();
    virtual ~CNativeProcess();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CNativeProcess *_p);
    int Comp(CNativeProcess *_p);
    status_t Print(CFileBase *_buf);
    int_ptr_t GetPid();
    CMem* GetExeName();
    CMemStk* GetCmdLine();
    int_ptr_t GetMemoryUsage();
    int GetMaxCpuUsage();
    int GetCpuUsageLimit();
    int GetCpuUsageTick();
    status_t SetPid(int_ptr_t _pid);
    status_t SetExeName(CMem* _exename);
    status_t SetMemoryUsage(int_ptr_t _memoryusage);
    status_t SetMaxCpuUsage(int _maxcpuusage);
    status_t SetCpuUsageLimit(int _cpuusagelimit);
    status_t SetCpuUsageTick(int _cpuusagetick);
    const char* GetExeNameStr();
    status_t SetExeName(const char *_exename);

    NATIVE_PROCESS_FLAG_FUNC(IsCpuStopped,0x00000001);
};

#endif
