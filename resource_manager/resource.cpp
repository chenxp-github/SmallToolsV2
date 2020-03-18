#include "resource.h"
#include "syslog.h"
#include "mem_tool.h"
#include "memfile.h"

CResource::CResource()
{
    this->InitBasic();
}
CResource::~CResource()
{
    this->Destroy();
}
status_t CResource::InitBasic()
{
    HASH_ENTRY_CLEAR();
    WEAK_REF_CLEAR();
    this->m_Type = TYPE_UNKNOWN;
    this->m_Id.InitBasic();
    this->m_Theme.InitBasic();
    this->m_Language.InitBasic();
    this->m_BsonData.InitBasic();
    return OK;
}
status_t CResource::Init()
{
    this->InitBasic();
    
    this->m_Id.Init();
    m_Id.SetRawBuf(_id_buf,sizeof(_id_buf),false);

    this->m_Theme.Init();
    m_Theme.SetRawBuf(_theme_buf,sizeof(_theme_buf),false);

    this->m_Language.Init();
    m_Language.SetRawBuf(_language_buf,sizeof(_language_buf),false);

    this->m_BsonData.Init();

    return OK;
}
status_t CResource::Destroy()
{
    WEAK_REF_DESTROY();
    this->m_Id.Destroy();
    this->m_Theme.Destroy();
    this->m_Language.Destroy();
    this->m_BsonData.Destroy();
    this->InitBasic();
    return OK;
}
int CResource::Comp(CResource *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;

    int cmp;

    cmp = m_Theme.StrCmp(&_p->m_Theme);
    if(cmp) return cmp;

    cmp = m_Language.StrCmp(&_p->m_Language);
    if(cmp) return cmp;

    cmp = m_Id.StrCmp(&_p->m_Id);
    if(cmp) return cmp;

    return 0;
}
status_t CResource::Copy(CResource *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_Type = _p->m_Type;
    this->SetId(_p->GetId());
    this->SetTheme(_p->GetTheme());
    this->SetLanguage(_p->GetLanguage());
    this->m_BsonData.Copy(&_p->m_BsonData);
    return OK;
}
status_t CResource::Print(CFileBase *_buf)
{
    _buf->Log("Type = %d",this->m_Type);
    const char* _str_id = "<null>";
    if(this->m_Id.StrLen() > 0)
        _str_id = m_Id.CStr();
    _buf->Log("Id = %s",_str_id);
    const char* _str_theme = "<null>";
    if(this->m_Theme.StrLen() > 0)
        _str_theme = m_Theme.CStr();
    _buf->Log("Theme = %s",_str_theme);
    const char* _str_language = "<null>";
    if(this->m_Language.StrLen() > 0)
        _str_language = m_Language.CStr();
    _buf->Log("Language = %s",_str_language);
    _buf->Log("BsonData = {");
    _buf->IncLogLevel(1);
	m_BsonData.ToJson(_buf,false);
	_buf->Log("");
    _buf->IncLogLevel(-1);
    _buf->Log("}");
    _buf->Log("next = %d",__next_index);
    return OK;
}
int32_t CResource::GetType()
{
    return this->m_Type;
}
CMem* CResource::GetId()
{
    return &this->m_Id;
}
CMem* CResource::GetTheme()
{
    return &this->m_Theme;
}
CMem* CResource::GetLanguage()
{
    return &this->m_Language;
}
CMiniBson* CResource::GetBsonData()
{
    return &this->m_BsonData;
}
const char* CResource::GetIdStr()
{
    return this->m_Id.CStr();
}
const char* CResource::GetThemeStr()
{
    return this->m_Theme.CStr();
}
const char* CResource::GetLanguageStr()
{
    return this->m_Language.CStr();
}
status_t CResource::SetType(int32_t _type)
{
    this->m_Type = _type;
    return OK;
}
status_t CResource::SetId(CMem* _id)
{
    this->m_Id.StrCpy(_id);
    return OK;
}
status_t CResource::SetTheme(CMem* _theme)
{
    this->m_Theme.StrCpy(_theme);
    return OK;
}
status_t CResource::SetLanguage(CMem* _language)
{
    this->m_Language.StrCpy(_language);
    return OK;
}
status_t CResource::SetBsonData(CMiniBson* _bsondata)
{
    this->m_BsonData.Copy(_bsondata);
    return OK;
}
status_t CResource::SetId(const char *_id)
{
    CMem _mem(_id);
    return this->SetId(&_mem);
}
status_t CResource::SetTheme(const char *_theme)
{
    CMem _mem(_theme);
    return this->SetTheme(&_mem);
}
status_t CResource::SetLanguage(const char *_language)
{
    CMem _mem(_language);
    return this->SetLanguage(&_mem);
}

status_t CResource::Serialize(CFileBase *buf)
{
    ASSERT(buf);

    uint8_t size = 0;

    size = m_Id.StrLen();
    buf->Putc(size);
    buf->Puts(&m_Id);

    size = m_Theme.StrLen();
    buf->Putc(size);
    buf->Puts(&m_Theme);

    size = m_Language.StrLen();
    buf->Putc(size);
    buf->Puts(&m_Language);
    buf->Write(&m_Type,sizeof(m_Type));
        
	CMem *raw_data = m_BsonData.GetRawData();
	ASSERT(raw_data);

    int32_t data_size = (int32_t)raw_data->GetSize();
    buf->Write(&data_size,sizeof(data_size));
    buf->Write(raw_data->GetRawBuf(),data_size);
    return OK;
}

status_t CResource::UnSerialize(CFileBase *buf)
{
    LOCAL_MEM(tmp_str);

    uint8_t size = 0;

    buf->Read(&size,sizeof(size));
    buf->Read(_id_buf,size);
    m_Id.SetSize(size);

    buf->Read(&size,sizeof(size));
    buf->Read(_theme_buf,size);
    m_Theme.SetSize(size);

    buf->Read(&size,sizeof(size));
    buf->Read(_language_buf,size);
    m_Language.SetSize(size);

    buf->Read(&m_Type,sizeof(m_Type));

    int32_t data_size = 0;
    buf->Read(&data_size,sizeof(data_size));
    
    ASSERT(data_size > 0);
    	
	m_BsonData.Destroy();
	m_BsonData.Init();
	m_BsonData.AllocBuf(data_size+4);
    
    buf->Read(m_BsonData.GetRawData()->GetRawBuf(),data_size);
	m_BsonData.GetRawData()->SetSize(data_size);
	m_BsonData.ResetPointer();

    return OK;
}

