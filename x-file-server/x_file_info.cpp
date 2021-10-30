#include "x_file_info.h"
#include "sys_log.h"
#include "mem_tool.h"

CxFileInfo::CxFileInfo()
{
    this->InitBasic();
}

CxFileInfo::~CxFileInfo()
{
    this->Destroy();
}

status_t CxFileInfo::InitBasic()
{
/*##Begin InitBasic##*/
    this->m_size = 0;
    this->m_full_path.InitBasic();
    this->m_file_name.InitBasic();
    this->m_flags = 0;
    this->m_modify_time = 0;
    this->m_operation = 0;
    this->m_current_size = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CxFileInfo::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_full_path.Init();
    this->m_file_name.Init();
/*##End Init##*/
    return OK;
}

status_t CxFileInfo::Destroy()
{
/*##Begin Destroy##*/
    this->m_full_path.Destroy();
    this->m_file_name.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CxFileInfo::Copy(CxFileInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_size = _p->m_size;
    this->m_full_path.Copy(&_p->m_full_path);
    this->m_file_name.Copy(&_p->m_file_name);
    this->m_flags = _p->m_flags;
    this->m_modify_time = _p->m_modify_time;
    this->m_operation = _p->m_operation;
    this->m_current_size = _p->m_current_size;
/*##End Copy##*/
    return OK;
}

int CxFileInfo::Comp(CxFileInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    return m_full_path.StrCmp(&_p->m_full_path);
}

status_t CxFileInfo::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("size = %lld",m_size);

    _buf->Log("full_path = %s",
        m_full_path.StrLen()>0?m_full_path.CStr():"<null>"
    );

    _buf->Log("file_name = %s",
        m_file_name.StrLen()>0?m_file_name.CStr():"<null>"
    );
    _buf->Log("flags = %u",m_flags);
    _buf->Log("modify_time = %lld",m_modify_time);
    _buf->Log("operation = %d",m_operation);
    _buf->Log("current_size = %lld",m_current_size);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetSize@@*/
int64_t CxFileInfo::GetSize()
{
    return m_size;
}
/*@@End  Function GetSize@@*/

/*@@Begin Function GetFullPath@@*/
CMem* CxFileInfo::GetFullPath()
{
    return &m_full_path;
}
/*@@End  Function GetFullPath@@*/

/*@@Begin Function GetFullPathStr@@*/
const char* CxFileInfo::GetFullPathStr()
{
    return m_full_path.CStr();
}
/*@@End  Function GetFullPathStr@@*/

/*@@Begin Function GetFlags@@*/
uint32_t CxFileInfo::GetFlags()
{
    return m_flags;
}
/*@@End  Function GetFlags@@*/

/*@@Begin Function SetSize@@*/
status_t CxFileInfo::SetSize(int64_t _size)
{
    this->m_size = _size;
    return OK;
}
/*@@End  Function SetSize@@*/

/*@@Begin Function SetFullPath@@*/
status_t CxFileInfo::SetFullPath(CMem *_full_path)
{
    ASSERT(_full_path);
    return this->m_full_path.Copy(_full_path);
}
/*@@End  Function SetFullPath@@*/

/*@@Begin Function SetFullPath-CStr@@*/
status_t CxFileInfo::SetFullPath(const char *_full_path)
{
    CMem tmp(_full_path);
    return this->SetFullPath(&tmp);
}
/*@@End  Function SetFullPath-CStr@@*/

/*@@Begin Function SetFlags@@*/
status_t CxFileInfo::SetFlags(uint32_t _flags)
{
    this->m_flags = _flags;
    return OK;
}
/*@@End  Function SetFlags@@*/

status_t CxFileInfo::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt64("current_size",m_current_size);
    _bson->PutString("file_name",&m_file_name);
    _bson->PutUInt32("flags",m_flags);
    _bson->PutString("full_path",&m_full_path);
    _bson->PutInt64("modify_time",m_modify_time);
    _bson->PutInt32("operation",m_operation);
    _bson->PutInt64("size",m_size);
/*##End SaveBson_1##*/
    return OK;
}

status_t CxFileInfo::SaveBson_Javascript(CMiniBson *_bson)
{
    ASSERT(_bson);

    _bson->PutDouble("current_size",(double)m_current_size);
    _bson->PutString("file_name",&m_file_name);
    _bson->PutUInt32("flags",m_flags);
    _bson->PutString("full_path",&m_full_path);
    _bson->PutDouble("modify_time",(double)m_modify_time);
    _bson->PutInt32("operation",m_operation);
    _bson->PutDouble("size",(double)m_size);
    return OK;
}


status_t CxFileInfo::SaveBson(CMem *_mem)
{
    ASSERT(_mem);
    CMiniBson _bson;
    _bson.Init();
    _bson.SetRawBuf(_mem);
    _bson.StartDocument();
    this->SaveBson(&_bson);
    _bson.EndDocument();
    _mem->SetSize(_bson.GetDocumentSize());
    return OK;
}

status_t CxFileInfo::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt64("current_size",&m_current_size));
    /******file_name begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("file_name",&_tmp_str));
    this->SetFileName(&_tmp_str);
    /******file_name end*******/}
    BSON_CHECK(_bson->GetUInt32("flags",&m_flags));
    /******full_path begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("full_path",&_tmp_str));
    this->SetFullPath(&_tmp_str);
    /******full_path end*******/}
    BSON_CHECK(_bson->GetInt64("modify_time",&m_modify_time));
    BSON_CHECK(_bson->GetInt32("operation",&m_operation));
    BSON_CHECK(_bson->GetInt64("size",&m_size));
/*##End LoadBson_1##*/
    return OK;
}

status_t CxFileInfo::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}


/*@@Begin Function GetFileName@@*/
CMem* CxFileInfo::GetFileName()
{
    return &m_file_name;
}
/*@@End  Function GetFileName@@*/

/*@@Begin Function GetFileNameStr@@*/
const char* CxFileInfo::GetFileNameStr()
{
    return m_file_name.CStr();
}
/*@@End  Function GetFileNameStr@@*/

/*@@Begin Function SetFileName@@*/
status_t CxFileInfo::SetFileName(CMem *_file_name)
{
    ASSERT(_file_name);
    return this->m_file_name.Copy(_file_name);
}
/*@@End  Function SetFileName@@*/

/*@@Begin Function SetFileName-CStr@@*/
status_t CxFileInfo::SetFileName(const char *_file_name)
{
    CMem tmp(_file_name);
    return this->SetFileName(&tmp);
}
/*@@End  Function SetFileName-CStr@@*/


/*@@Begin Function GetModifyTime@@*/
int64_t CxFileInfo::GetModifyTime()
{
    return m_modify_time;
}
/*@@End  Function GetModifyTime@@*/

/*@@Begin Function SetModifyTime@@*/
status_t CxFileInfo::SetModifyTime(int64_t _modify_time)
{
    this->m_modify_time = _modify_time;
    return OK;
}
/*@@End  Function SetModifyTime@@*/

/*@@Begin Function GetOperation@@*/
int CxFileInfo::GetOperation()
{
    return m_operation;
}
/*@@End  Function GetOperation@@*/

/*@@Begin Function SetOperation@@*/
status_t CxFileInfo::SetOperation(int _operation)
{
    this->m_operation = _operation;
    return OK;
}
/*@@End  Function SetOperation@@*/

/*@@Begin Function GetCurrentSize@@*/
int64_t CxFileInfo::GetCurrentSize()
{
    return m_current_size;
}
/*@@End  Function GetCurrentSize@@*/

/*@@Begin Function SetCurrentSize@@*/
status_t CxFileInfo::SetCurrentSize(int64_t _current_size)
{
    this->m_current_size = _current_size;
    return OK;
}
/*@@End  Function SetCurrentSize@@*/
/*@@ Insert Function Here @@*/
