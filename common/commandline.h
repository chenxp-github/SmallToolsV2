#ifndef __S2_COMMANDLINE_H
#define __S2_COMMANDLINE_H

#include "cruntime.h"
#include "filebase.h"
#include "cmdentry.h"
#include "commonarray.h"
#include "memstk.h"

class CCommandLine{
public:
    WEAK_REF_DEFINE();
public:
    CCommonArray<CCmdEntry> m_EntryTypes;
    CCommonArray<CCmdEntry> m_CmdEntries;
public:
    int GetCmdEntriesLen();
    CCmdEntry* GetCmdEntry(int index);
    status_t LoadFromString(const char *str);
    status_t LoadFromString(CMem *str);
    status_t DelByIndex(int index);
    status_t SaveToCmdLine(CMem *cmdline);
    status_t SaveToArgv(int *argc, char**argv);
    status_t DelByKey(CMem *key);
    status_t PrintHelp();
    status_t PrintHelp(CFileBase *out);
    status_t CheckForErrors();
    CMem * GetValueByKey(const char *key);
    CMem* GetValueByKey(CMem *key);
    bool HasKey(const char *str);
    bool HasKey(CMem *key);
    CCmdEntry* GetByKey(CMem *key);
    int GetArgvType(CMem *argv,CCmdEntry** typeEntry);
    status_t LoadFromArgv(int argc, char **argv);
    status_t AddKeyType(const char *key, int type, int option,const char *help);
    CCommandLine();
    virtual ~CCommandLine();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CCommandLine *_p);
    int Comp(CCommandLine *_p);
    status_t Print(CFileBase *_buf);

    status_t GetAllValuesByKey(const char *key,CMemStk *values);
    status_t GetAllValuesByKey(CMem *key,CMemStk *values);
};

#endif
