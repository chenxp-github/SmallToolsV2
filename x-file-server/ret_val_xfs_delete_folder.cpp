#include "ret_val_xfs_delete_folder.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_XfsDeleteFolder::CRetVal_XfsDeleteFolder()
{
    this->InitBasic();
}

CRetVal_XfsDeleteFolder::~CRetVal_XfsDeleteFolder()
{
    this->Destroy();
}

status_t CRetVal_XfsDeleteFolder::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_result = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_XfsDeleteFolder::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_XfsDeleteFolder::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_XfsDeleteFolder::Copy(CRetVal_XfsDeleteFolder *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_result = _p->m_result;
/*##End Copy##*/
    return OK;
}

int CRetVal_XfsDeleteFolder::Comp(CRetVal_XfsDeleteFolder *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_XfsDeleteFolder::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("result = %d",m_result);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetResult@@*/
int CRetVal_XfsDeleteFolder::GetResult()
{
    return m_result;
}
/*@@End  Function GetResult@@*/

/*@@Begin Function SetResult@@*/
status_t CRetVal_XfsDeleteFolder::SetResult(int _result)
{
    this->m_result = _result;
    return OK;
}
/*@@End  Function SetResult@@*/

status_t CRetVal_XfsDeleteFolder::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("result",m_result);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_XfsDeleteFolder::SaveBson(CMem *_mem)
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

status_t CRetVal_XfsDeleteFolder::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("result",&m_result));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_XfsDeleteFolder::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
