#include "ret_val_xfs_change_current_directory.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_XfsChangeCurrentDirectory::CRetVal_XfsChangeCurrentDirectory()
{
    this->InitBasic();
}

CRetVal_XfsChangeCurrentDirectory::~CRetVal_XfsChangeCurrentDirectory()
{
    this->Destroy();
}

status_t CRetVal_XfsChangeCurrentDirectory::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_result = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_XfsChangeCurrentDirectory::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_XfsChangeCurrentDirectory::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_XfsChangeCurrentDirectory::Copy(CRetVal_XfsChangeCurrentDirectory *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_result = _p->m_result;
/*##End Copy##*/
    return OK;
}

int CRetVal_XfsChangeCurrentDirectory::Comp(CRetVal_XfsChangeCurrentDirectory *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_XfsChangeCurrentDirectory::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("result = %d",m_result);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetResult@@*/
int CRetVal_XfsChangeCurrentDirectory::GetResult()
{
    return m_result;
}
/*@@End  Function GetResult@@*/

/*@@Begin Function SetResult@@*/
status_t CRetVal_XfsChangeCurrentDirectory::SetResult(int _result)
{
    this->m_result = _result;
    return OK;
}
/*@@End  Function SetResult@@*/

status_t CRetVal_XfsChangeCurrentDirectory::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("result",m_result);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_XfsChangeCurrentDirectory::SaveBson(CMem *_mem)
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

status_t CRetVal_XfsChangeCurrentDirectory::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("result",&m_result));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_XfsChangeCurrentDirectory::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
