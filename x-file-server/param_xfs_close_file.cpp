#include "param_xfs_close_file.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_XfsCloseFile::CParam_XfsCloseFile()
{
    this->InitBasic();
}

CParam_XfsCloseFile::~CParam_XfsCloseFile()
{
    this->Destroy();
}

status_t CParam_XfsCloseFile::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_handle = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CParam_XfsCloseFile::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CParam_XfsCloseFile::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_XfsCloseFile::Copy(CParam_XfsCloseFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_handle = _p->m_handle;
/*##End Copy##*/
    return OK;
}

int CParam_XfsCloseFile::Comp(CParam_XfsCloseFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_XfsCloseFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("handle = %d",m_handle);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetHandle@@*/
int CParam_XfsCloseFile::GetHandle()
{
    return m_handle;
}
/*@@End  Function GetHandle@@*/

/*@@Begin Function SetHandle@@*/
status_t CParam_XfsCloseFile::SetHandle(int _handle)
{
    this->m_handle = _handle;
    return OK;
}
/*@@End  Function SetHandle@@*/

status_t CParam_XfsCloseFile::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("handle",m_handle);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_XfsCloseFile::SaveBson(CMem *_mem)
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

status_t CParam_XfsCloseFile::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("handle",&m_handle));
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_XfsCloseFile::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
