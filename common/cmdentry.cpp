#include "cmdentry.h"
#include "syslog.h"
#include "mem_tool.h"

CCmdEntry::CCmdEntry()
{
    this->InitBasic();
}
CCmdEntry::~CCmdEntry()
{
    this->Destroy();
}
status_t CCmdEntry::InitBasic()
{
    WEAK_REF_CLEAR();
    this->m_KeyType = 0;
    this->m_Option = OPTIONAL;
    this->m_Key.InitBasic();
    this->m_Value.InitBasic();
    this->m_Help.InitBasic();
    return OK;
}
status_t CCmdEntry::Init()
{
    this->InitBasic();
    this->m_Key.Init();
    this->m_Value.Init();
    this->m_Help.Init();
    return OK;
}
status_t CCmdEntry::Destroy()
{
    WEAK_REF_DESTROY();
    this->m_Key.Destroy();
    this->m_Value.Destroy();
    this->m_Help.Destroy();
    this->InitBasic();
    return OK;
}
int CCmdEntry::Comp(CCmdEntry *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CCmdEntry::Copy(CCmdEntry *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_KeyType = _p->m_KeyType;
    this->m_Key.Copy(&_p->m_Key);
    this->m_Value.Copy(&_p->m_Value);
    this->m_Help.Copy(&_p->m_Help);
    this->m_Option = _p->m_Option;
    return OK;
}
status_t CCmdEntry::Print(CFileBase *_buf)
{
    _buf->Log("KeyType = %d",this->m_KeyType);
    const char* _str_key = "<null>";
    if(this->m_Key.StrLen() > 0)
        _str_key = m_Key.CStr();
    _buf->Log("Key = %s",_str_key);
    const char* _str_value = "<null>";
    if(this->m_Value.StrLen() > 0)
        _str_value = m_Value.CStr();
    _buf->Log("Value = %s",_str_value);
    _buf->Log("Option = %d",m_Option);
    const char* _str_help = "<null>";
    if(this->m_Help.StrLen() > 0)
        _str_help = m_Help.CStr();
    _buf->Log("Help = %s",_str_help);
    return OK;
}
int CCmdEntry::GetKeyType()
{
    return this->m_KeyType;
}
CMem* CCmdEntry::GetKey()
{
    return &this->m_Key;
}
CMem* CCmdEntry::GetValue()
{
    return &this->m_Value;
}
const char* CCmdEntry::GetKeyStr()
{
    return this->m_Key.CStr();
}
const char* CCmdEntry::GetValueStr()
{
    return this->m_Value.CStr();
}
status_t CCmdEntry::SetKeyType(int _keytype)
{
    this->m_KeyType = _keytype;
    return OK;
}
status_t CCmdEntry::SetKey(CMem* _key)
{
    this->m_Key.Copy(_key);
    return OK;
}
status_t CCmdEntry::SetValue(CMem* _value)
{
    this->m_Value.Copy(_value);
    return OK;
}
status_t CCmdEntry::SetKey(const char *_key)
{
    CMem _mem(_key);
    return this->SetKey(&_mem);
}
status_t CCmdEntry::SetValue(const char *_value)
{
    CMem _mem(_value);
    return this->SetValue(&_mem);
}
CMem* CCmdEntry::GetHelp()
{
    return &this->m_Help;
}
const char* CCmdEntry::GetHelpStr()
{
    return this->m_Help.CStr();
}
status_t CCmdEntry::SetHelp(CMem* _help)
{
    this->m_Help.Copy(_help);
    return OK;
}
status_t CCmdEntry::SetHelp(const char *_help)
{
    CMem _mem(_help);
    return this->SetHelp(&_mem);
}
bool CCmdEntry::IsMatchKeyType(CMem *str)
{
    ASSERT(str);

    if(m_KeyType == TYPE_KEY)
    {
        return str->StrEqu(&m_Key,false);
    }

    if(m_KeyType == TYPE_KEY_VALUE)
    {
        return str->StartWith(&m_Key,1,0) != 0;
    }

    if(m_KeyType == TYPE_KEY_SPACE_VALUE)
    {
        return str->StrEqu(&m_Key,false);
    }

    if(m_KeyType == TYPE_KEY_EQUAL_VALUE)
    {
        LOCAL_MEM(tmp);
        tmp.Puts(&m_Key);
        tmp.Puts("=");
        return str->StartWith(&tmp,1,0) != 0;
    }

    return false;
}

status_t CCmdEntry::ParseArgv(CMem *argv1, CMem *argv2,int *need_argv2)
{
    ASSERT(argv1 && need_argv2); //argv2 can be NULL

    LOCAL_MEM(tmp);
    *need_argv2 = 0;

    if(m_KeyType == TYPE_KEY)
    {
        *need_argv2 = 0;
        return OK;
    }

    if(m_KeyType == TYPE_KEY_VALUE)
    {
        *need_argv2 = 0;
        argv1->WriteToFile(&tmp,m_Key.StrLen(),argv1->GetSize());
        this->SetValue(&tmp);
        return OK;
    }

    if(m_KeyType == TYPE_KEY_SPACE_VALUE)
    {
        *need_argv2 = 1;
        this->SetValue(argv2);
        return OK;
    }

    if(m_KeyType == TYPE_KEY_EQUAL_VALUE)
    {
        *need_argv2 = 0;
        argv1->WriteToFile(&tmp,m_Key.StrLen()+1,argv1->GetSize());
        this->SetValue(&tmp);
        return OK;
    }

    if(m_KeyType == TYPE_VALUE)
    {
        *need_argv2 = 0;
        this->SetValue(argv1);
        return OK;
    }

    return ERROR;
}

int CCmdEntry::GetOption()
{
    return m_Option;
}

status_t CCmdEntry::SetOption(int option)
{
    m_Option = option;
    return OK;
}

static char* cmd_strdup(const char *str)
{
    bool has_empty_char = false;
    int len = strlen(str);
    for(int i = 0; i < len; i++)
    {
        if(str[i]==' ' ||str[i]=='\t')
        {
            has_empty_char = true;
            break;
        }
    }

    if(has_empty_char)
    {
        LOCAL_MEM(tmp);
        tmp.Puts("\"");
        tmp.Puts(str);
        tmp.Puts("\"");
        return strdup(tmp.CStr());
    }
    else
    {
        return strdup(str);
    }
}

static void cmd_strdup(const char *str, CMem *mem)
{
    char *tmp = cmd_strdup(str);
    mem->Puts(tmp);
    free(tmp);
}
status_t CCmdEntry::SaveToArgv(int *argc, char **argv)
{
    ASSERT(argc && argv);

    if(m_KeyType == TYPE_KEY)
    {
        argv[(*argc)++] = cmd_strdup(GetKeyStr());
        return OK;
    }
    
    if(m_KeyType == TYPE_KEY_VALUE)
    {
        LOCAL_MEM(tmp);
        cmd_strdup(m_Key.CStr(),&tmp);
        cmd_strdup(m_Value.CStr(),&tmp);
        argv[(*argc)++] = strdup(tmp.CStr());       
        return OK;
    }
    
    if(m_KeyType == TYPE_KEY_SPACE_VALUE)
    {
        argv[(*argc)++] = cmd_strdup(m_Key.CStr());
        argv[(*argc)++] = cmd_strdup(m_Value.CStr());
        return OK;
    }
    
    if(m_KeyType == TYPE_KEY_EQUAL_VALUE)
    {
        LOCAL_MEM(tmp);
        cmd_strdup(m_Key.CStr(),&tmp);
        tmp.Puts("=");
        cmd_strdup(m_Value.CStr(),&tmp);
        argv[(*argc)++] = strdup(tmp.CStr());           
        return OK;
    }
    
    if(m_KeyType == TYPE_VALUE)
    {
        argv[(*argc)++] = cmd_strdup(m_Value.CStr());
        return OK;
    }
    
    return ERROR;
}
