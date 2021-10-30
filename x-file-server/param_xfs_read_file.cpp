#include "param_xfs_read_file.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_XfsReadFile::CParam_XfsReadFile()
{
    this->InitBasic();
}

CParam_XfsReadFile::~CParam_XfsReadFile()
{
    this->Destroy();
}

status_t CParam_XfsReadFile::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_handle = 0;
    this->m_offset = 0;
    this->m_size = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CParam_XfsReadFile::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CParam_XfsReadFile::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_XfsReadFile::Copy(CParam_XfsReadFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_handle = _p->m_handle;
    this->m_offset = _p->m_offset;
    this->m_size = _p->m_size;
/*##End Copy##*/
    return OK;
}

int CParam_XfsReadFile::Comp(CParam_XfsReadFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_XfsReadFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("handle = %d",m_handle);
    _buf->Log("offset = %lld",m_offset);
    _buf->Log("size = %d",m_size);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetHandle@@*/
int CParam_XfsReadFile::GetHandle()
{
    return m_handle;
}
/*@@End  Function GetHandle@@*/

/*@@Begin Function GetOffset@@*/
int64_t CParam_XfsReadFile::GetOffset()
{
    return m_offset;
}
/*@@End  Function GetOffset@@*/

/*@@Begin Function GetSize@@*/
int CParam_XfsReadFile::GetSize()
{
    return m_size;
}
/*@@End  Function GetSize@@*/

/*@@Begin Function SetHandle@@*/
status_t CParam_XfsReadFile::SetHandle(int _handle)
{
    this->m_handle = _handle;
    return OK;
}
/*@@End  Function SetHandle@@*/

/*@@Begin Function SetOffset@@*/
status_t CParam_XfsReadFile::SetOffset(int64_t _offset)
{
    this->m_offset = _offset;
    return OK;
}
/*@@End  Function SetOffset@@*/

/*@@Begin Function SetSize@@*/
status_t CParam_XfsReadFile::SetSize(int _size)
{
    this->m_size = _size;
    return OK;
}
/*@@End  Function SetSize@@*/

status_t CParam_XfsReadFile::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("handle",m_handle);
    _bson->PutInt64("offset",m_offset);
    _bson->PutInt32("size",m_size);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_XfsReadFile::SaveBson(CMem *_mem)
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

status_t CParam_XfsReadFile::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("handle",&m_handle));
    BSON_CHECK(_bson->GetInt64("offset",&m_offset));
    BSON_CHECK(_bson->GetInt32("size",&m_size));
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_XfsReadFile::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
