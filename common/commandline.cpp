#include "commandline.h"
#include "syslog.h"
#include "mem_tool.h"
#include "memfile.h"

CCommandLine::CCommandLine()
{
    this->InitBasic();
}
CCommandLine::~CCommandLine()
{
    this->Destroy();
}
status_t CCommandLine::InitBasic()
{
    WEAK_REF_CLEAR();
    m_CmdEntries.InitBasic();
    m_EntryTypes.InitBasic();
    return OK;
}
status_t CCommandLine::Init()
{
    this->InitBasic();
    m_CmdEntries.Init(1024);
    m_EntryTypes.Init(1024);
    return OK;
}
status_t CCommandLine::Destroy()
{
    WEAK_REF_DESTROY();
    m_CmdEntries.Destroy();
    m_EntryTypes.Destroy();
    this->InitBasic();
    return OK;
}
int CCommandLine::Comp(CCommandLine *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    ASSERT(0);
    return 0;
}
status_t CCommandLine::Copy(CCommandLine *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    return OK;
}
status_t CCommandLine::Print(CFileBase *_buf)
{
    _buf->Log("EntryTypes={");
    _buf->IncLogLevel(1);
    m_EntryTypes.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("}");

    _buf->Log("CmdEntries={");
    _buf->IncLogLevel(1);
    m_CmdEntries.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("}");
    return OK;
}

status_t CCommandLine::AddKeyType(const char *key, int type, int option,const char *help)
{
    ASSERT(key && help);
    CCmdEntry *tmp;
    NEW(tmp,CCmdEntry);
    tmp->Init();
    tmp->SetKey(key);
    tmp->SetKeyType(type);
    tmp->SetOption(option);
    tmp->SetHelp(help);
    return m_EntryTypes.PushPtr(tmp);
}

status_t CCommandLine::LoadFromArgv(int argc, char **argv)
{
    ASSERT(argv);
    
    m_CmdEntries.Clear();

    for(int i = 0; i < argc; i++)
    {
        CMem mem_argv(argv[i]);
        CMem mem_argv_2;
        mem_argv_2.Init();

        if(i < argc - 1)
        {           
            mem_argv_2.SetStr(argv[i+1]);
        }

        CCmdEntry *entry_type = NULL;

        int type = this->GetArgvType(&mem_argv,&entry_type);

        CCmdEntry *entry;
        NEW(entry,CCmdEntry);
        entry->Init();
        entry->SetKeyType(type);
        if(entry_type)
        {
            entry->SetKey(entry_type->GetKey());
        }

        int need_argv_2 = 0;
        entry->ParseArgv(&mem_argv,&mem_argv_2,&need_argv_2);
        m_CmdEntries.PushPtr(entry);

        if(need_argv_2)
            i++;
    }

    return OK;
}

int CCommandLine::GetArgvType(CMem *argv, CCmdEntry **typeEntry)
{
    ASSERT(argv && typeEntry);

    *typeEntry = NULL;

    for(int i = 0; i < m_EntryTypes.GetLen(); i++)
    {
        CCmdEntry *type = m_EntryTypes.GetElem(i);

        if(type->IsMatchKeyType(argv))
        {
            *typeEntry = type;
            return type->GetKeyType();
        }
    }

    return CCmdEntry::TYPE_VALUE;
}

CCmdEntry* CCommandLine::GetByKey(CMem *key)
{
    ASSERT(key);
    for(int i = 0; i < m_CmdEntries.GetLen(); i++)
    {
        CCmdEntry *e = m_CmdEntries.GetElem(i);
        if(e->GetKey()->StrEqu(key,true))
        {
            return e;
        }
    }
    return NULL;
}

bool CCommandLine::HasKey(CMem *key)
{
    ASSERT(key);
    return this->GetByKey(key) != NULL;
}

bool CCommandLine::HasKey(const char *key)
{
    ASSERT(key);
    CMem mem_key(key);
    return this->GetByKey(&mem_key) != NULL;    
}

CMem *CCommandLine::GetValueByKey(CMem *key)
{
    ASSERT(key);
    CCmdEntry *entry = this->GetByKey(key);
    if(entry)
    {
        return entry->GetValue();
    }
    return NULL;
}

CMem * CCommandLine::GetValueByKey(const char *key)
{
    ASSERT(key);
    CMem mem_key(key);
    return this->GetValueByKey(&mem_key);
}

status_t CCommandLine::CheckForErrors()
{
    status_t has_errors = FALSE;

    for(int i = 0; i < m_EntryTypes.GetLen(); i++)
    {
        CCmdEntry *e = m_EntryTypes.GetElem(i);
        if(e->GetOption() == CCmdEntry::MUST)
        {           
            if(this->GetByKey(e->GetKey()) == NULL)
            {
                has_errors = TRUE;
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                    "error: %s is missing.",e->GetKeyStr()
                );
            }
        }
    }

    if(has_errors)
    {
        this->PrintHelp();
    }

    return has_errors;
}


status_t CCommandLine::PrintHelp()
{
    CMemFile tmp;
    tmp.Init();
    this->PrintHelp(&tmp);
    tmp.Dump();
    return OK;
}

status_t CCommandLine::PrintHelp(CFileBase *out)
{
    for(int i = 0; i < m_EntryTypes.GetLen(); i++)
    {
        CCmdEntry *e = m_EntryTypes.GetElem(i);     
        const char *opt_str = "optional";
        if(e->GetOption() == CCmdEntry::MUST)
            opt_str = "must";
        out->Log(
            "%s: %s, %s",e->GetKeyStr(),opt_str,e->GetHelpStr()
        );
    }
    return OK;
}

status_t CCommandLine::DelByKey(CMem *key)
{
    ASSERT(key);
    for(int i = 0; i < m_CmdEntries.GetLen(); i++)
    {
        CCmdEntry *e = m_CmdEntries.GetElem(i);
        if(e->GetKey()->StrEqu(key,true))
        {
            m_CmdEntries.DelElem(i);
            i--;
        }
    }
    return OK;
}

status_t CCommandLine::SaveToArgv(int *argc, char **argv)
{
    ASSERT(argc && argv);
    int max = *argc;
    *argc = 0;  
    for(int i = 0; i < m_CmdEntries.GetLen(); i++)
    {
        ASSERT(i < max-2);
        CCmdEntry *e = m_CmdEntries.GetElem(i);
        e->SaveToArgv(argc,argv);
    }
    return OK;
}

status_t CCommandLine::SaveToCmdLine(CMem *cmdline)
{
    ASSERT(cmdline);
    const int MAX = 1024;

    int argc = MAX;
    char *argv[MAX];

    this->SaveToArgv(&argc,argv);

    for(int i = 0; i < argc; i++)
    {
        cmdline->Puts(argv[i]);
        cmdline->Puts(" ");
        free(argv[i]);
    }

    return OK;
}

status_t CCommandLine::DelByIndex(int index)
{
    return m_CmdEntries.DelElem(index);
}

status_t CCommandLine::LoadFromString(CMem *str)
{
    ASSERT(str);

    LOCAL_MEM(mem);
    str->Seek(0);
    
    LOCAL_MEM(tmp);

    const int MAX=1024;

    int argc = 0;
    char *argv[MAX];

    while(!str->IsEnd())
    {
        char ch = str->Getc();
        if(ch == '\"' || ch == '\'')
        {
            while(!str->IsEnd())
            {
                ch = str->Getc();
                if(ch == '\"' || ch == '\'')
                    break;
                tmp.Putc(ch);
            }           
        }
        else if(!CFileBase::IsEmptyChar(ch))
        {
            tmp.Putc(ch);
        }
        else
        {           
            if(tmp.StrLen() > 0)
            {
                ASSERT(argc < MAX);
                argv[argc++] = strdup(tmp.CStr());
            }           
            tmp.SetSize(0);
        }
    }

    if(tmp.StrLen() > 0)
    {       
        ASSERT(argc < MAX);
        argv[argc++] = strdup(tmp.CStr());
    }

    status_t ret = this->LoadFromArgv(argc,argv);

    for(int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    
    return ret;
}

status_t CCommandLine::LoadFromString(const char *str)
{
    ASSERT(str);
    CMem mem(str);
    return this->LoadFromString(&mem);
}

CCmdEntry* CCommandLine::GetCmdEntry(int index)
{
    return m_CmdEntries.GetElem(index);
}

int CCommandLine::GetCmdEntriesLen()
{
    return m_CmdEntries.GetLen();
}

status_t CCommandLine::GetAllValuesByKey(const char *key,CMemStk *values)
{
    ASSERT(key && values);
    CMem mem_key(key);
    return this->GetAllValuesByKey(&mem_key,values);
}

status_t CCommandLine::GetAllValuesByKey(CMem *key,CMemStk *values)
{
    ASSERT(key && values);
    for(int i = 0; i < m_CmdEntries.GetLen(); i++)
    {
        CCmdEntry *e = m_CmdEntries.GetElem(i);
        if(e->GetKey()->StrEqu(key,true))
        {
            values->Push(e->GetValue());
        }
    }
    return OK;
}
