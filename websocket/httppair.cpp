#include "httppair.h"
#include "syslog.h"
#include "mem_tool.h"

CHttpPair::CHttpPair()
{
    this->InitBasic();
}
CHttpPair::~CHttpPair()
{
    this->Destroy();
}
status_t CHttpPair::InitBasic()
{
    this->m_Key.InitBasic();
    this->m_Value.InitBasic();
    return OK;
}
status_t CHttpPair::Init()
{
    this->InitBasic();
    this->m_Key.Init();
    this->m_Value.Init();
    return OK;
}
status_t CHttpPair::Destroy()
{
    this->m_Key.Destroy();
    this->m_Value.Destroy();
    this->InitBasic();
    return OK;
}
int CHttpPair::Comp(CHttpPair *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return m_Key.StrICmp(_p->GetKey());
}
status_t CHttpPair::Copy(CHttpPair *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_Key.Copy(&_p->m_Key);
    this->m_Value.Copy(&_p->m_Value);
    return OK;
}
status_t CHttpPair::Print(CFileBase *_buf)
{
    const char* _str_key = "<null>";
    if(this->m_Key.StrLen() > 0)
        _str_key = m_Key.CStr();
    _buf->Log("Key = %s",_str_key);
    const char* _str_value = "<null>";
    if(this->m_Value.StrLen() > 0)
        _str_value = m_Value.CStr();
    _buf->Log("Value = %s",_str_value);
    return OK;
}
CMem* CHttpPair::GetKey()
{
    return &this->m_Key;
}
CMem* CHttpPair::GetValue()
{
    return &this->m_Value;
}
const char* CHttpPair::GetKeyStr()
{
    return this->m_Key.CStr();
}
const char* CHttpPair::GetValueStr()
{
    return this->m_Value.CStr();
}
status_t CHttpPair::SetKey(CMem* _key)
{
    this->m_Key.Copy(_key);
    return OK;
}
status_t CHttpPair::SetValue(CMem* _value)
{
    this->m_Value.Copy(_value);
    return OK;
}
status_t CHttpPair::SetKey(const char *_key)
{
    CMem _mem(_key);
    return this->SetKey(&_mem);
}
status_t CHttpPair::SetValue(const char *_value)
{
    CMem _mem(_value);
    return this->SetValue(&_mem);
}

status_t CHttpPair::ToString(CMem *out)
{
    ASSERT(out);
    out->SetSize(0);
    out->Puts(&m_Key);
    out->Putc(':');
    out->Puts(&m_Value);
    return OK;
}
