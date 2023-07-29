#include "ret_val_rd_login.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_RdLogin::CRetVal_RdLogin()
{
    this->InitBasic();
}

CRetVal_RdLogin::~CRetVal_RdLogin()
{
    this->Destroy();
}

status_t CRetVal_RdLogin::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_RetVal0 = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_RdLogin::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_RdLogin::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_RdLogin::Copy(CRetVal_RdLogin *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_RetVal0 = _p->m_RetVal0;
/*##End Copy##*/
    return OK;
}

int CRetVal_RdLogin::Comp(CRetVal_RdLogin *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_RdLogin::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("RetVal0 = %d",m_RetVal0);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetRetVal0@@*/
status_t CRetVal_RdLogin::GetRetVal0()
{
    return m_RetVal0;
}
/*@@End  Function GetRetVal0@@*/

/*@@Begin Function SetRetVal0@@*/
status_t CRetVal_RdLogin::SetRetVal0(status_t _retval0)
{
    this->m_RetVal0 = _retval0;
    return OK;
}
/*@@End  Function SetRetVal0@@*/

status_t CRetVal_RdLogin::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("RetVal0",m_RetVal0);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_RdLogin::SaveBson(CMem *_mem)
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

status_t CRetVal_RdLogin::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("RetVal0",&m_RetVal0));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_RdLogin::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
