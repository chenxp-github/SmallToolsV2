#include "simplediskfsentry.h"
#include "syslog.h"
#include "mem_tool.h"

CSimpleDiskFsEntry::CSimpleDiskFsEntry()
{
    this->InitBasic();
}
CSimpleDiskFsEntry::~CSimpleDiskFsEntry()
{
    this->Destroy();
}
status_t CSimpleDiskFsEntry::InitBasic()
{
    HASH_ENTRY_CLEAR();
    this->m_FileName.InitBasic();
    this->m_Index = 0;
    m_Md5[0] = 0;
    return OK;
}
status_t CSimpleDiskFsEntry::Init()
{
    this->InitBasic();
    this->m_FileName.Init();
    return OK;
}
status_t CSimpleDiskFsEntry::Destroy()
{
    this->m_FileName.Destroy();
    this->InitBasic();
    return OK;
}
int CSimpleDiskFsEntry::Comp(CSimpleDiskFsEntry *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return m_FileName.StrCmp(_p->GetFileName());
}
status_t CSimpleDiskFsEntry::Copy(CSimpleDiskFsEntry *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_FileName.Copy(&_p->m_FileName);
    this->m_Index = _p->m_Index;
    memcpy(m_Md5,_p->m_Md5,sizeof(m_Md5));
    return OK;
}
status_t CSimpleDiskFsEntry::Print(CFileBase *_buf)
{
    const char* _str_filename = "<null>";
    if(this->m_FileName.StrLen() > 0)
        _str_filename = m_FileName.CStr();
    _buf->Log("FileName = %s",_str_filename);
    _buf->Log("Index = %lld",this->m_Index);
    _buf->Log("Md5 = %s",m_Md5);
    return OK;
}
CMem* CSimpleDiskFsEntry::GetFileName()
{
    return &this->m_FileName;
}
int64_t CSimpleDiskFsEntry::GetIndex()
{
    return this->m_Index;
}
const char* CSimpleDiskFsEntry::GetFileNameStr()
{
    return this->m_FileName.CStr();
}
const char* CSimpleDiskFsEntry::GetMd5Str()
{
    return m_Md5;
}
status_t CSimpleDiskFsEntry::SetFileName(CMem* _filename)
{
    this->m_FileName.Copy(_filename);
    return OK;
}
status_t CSimpleDiskFsEntry::SetIndex(int64_t _index)
{
    this->m_Index = _index;
    return OK;
}
status_t CSimpleDiskFsEntry::SetFileName(const char *_filename)
{
    CMem _mem(_filename);
    return this->SetFileName(&_mem);
}
status_t CSimpleDiskFsEntry::SetMd5(const char *_md5)
{
    crt_strncpy(m_Md5,_md5,sizeof(m_Md5));
    m_Md5[32] = 0;
    return OK;
}
status_t CSimpleDiskFsEntry::Serialize(CFileBase *file)
{
    ASSERT(file);
    file->Write(&this->m_Index,sizeof(this->m_Index));
    file->Puts(&this->m_FileName);file->Putc(0);
    file->Puts(this->m_Md5);file->Putc(0);
    return OK;
}
status_t CSimpleDiskFsEntry::UnSerialize(CFileBase *file)
{
    ASSERT(file);
    LOCAL_MEM(tmp_str);
    file->Read(&this->m_Index,sizeof(this->m_Index));
    file->ReadZeroEndString(&tmp_str);
    this->SetFileName(tmp_str.CStr());
    file->ReadZeroEndString(&tmp_str);
    this->SetMd5(tmp_str.CStr());
    return OK;
}

