#include "ret_val_xfs_open_file.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_XfsOpenFile::CRetVal_XfsOpenFile()
{
    this->InitBasic();
}

CRetVal_XfsOpenFile::~CRetVal_XfsOpenFile()
{
    this->Destroy();
}

status_t CRetVal_XfsOpenFile::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_result = 0;
    this->m_handle = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_XfsOpenFile::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_XfsOpenFile::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_XfsOpenFile::Copy(CRetVal_XfsOpenFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_result = _p->m_result;
    this->m_handle = _p->m_handle;
/*##End Copy##*/
    return OK;
}

int CRetVal_XfsOpenFile::Comp(CRetVal_XfsOpenFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_XfsOpenFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("result = %d",m_result);
    _buf->Log("handle = %d",m_handle);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetResult@@*/
int CRetVal_XfsOpenFile::GetResult()
{
    return m_result;
}
/*@@End  Function GetResult@@*/

/*@@Begin Function GetHandle@@*/
int CRetVal_XfsOpenFile::GetHandle()
{
    return m_handle;
}
/*@@End  Function GetHandle@@*/

/*@@Begin Function SetResult@@*/
status_t CRetVal_XfsOpenFile::SetResult(int _result)
{
    this->m_result = _result;
    return OK;
}
/*@@End  Function SetResult@@*/

/*@@Begin Function SetHandle@@*/
status_t CRetVal_XfsOpenFile::SetHandle(int _handle)
{
    this->m_handle = _handle;
    return OK;
}
/*@@End  Function SetHandle@@*/

status_t CRetVal_XfsOpenFile::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("handle",m_handle);
    _bson->PutInt32("result",m_result);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_XfsOpenFile::SaveBson(CMem *_mem)
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

status_t CRetVal_XfsOpenFile::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("handle",&m_handle));
    BSON_CHECK(_bson->GetInt32("result",&m_result));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_XfsOpenFile::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
