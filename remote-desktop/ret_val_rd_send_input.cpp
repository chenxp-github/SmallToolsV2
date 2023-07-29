#include "ret_val_rd_send_input.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_RdSendInput::CRetVal_RdSendInput()
{
    this->InitBasic();
}

CRetVal_RdSendInput::~CRetVal_RdSendInput()
{
    this->Destroy();
}

status_t CRetVal_RdSendInput::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_res = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_RdSendInput::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_RdSendInput::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_RdSendInput::Copy(CRetVal_RdSendInput *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_res = _p->m_res;
/*##End Copy##*/
    return OK;
}

int CRetVal_RdSendInput::Comp(CRetVal_RdSendInput *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_RdSendInput::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("res = %d",m_res);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetRes@@*/
int CRetVal_RdSendInput::GetRes()
{
    return m_res;
}
/*@@End  Function GetRes@@*/

/*@@Begin Function SetRes@@*/
status_t CRetVal_RdSendInput::SetRes(int _res)
{
    this->m_res = _res;
    return OK;
}
/*@@End  Function SetRes@@*/

status_t CRetVal_RdSendInput::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("res",m_res);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_RdSendInput::SaveBson(CMem *_mem)
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

status_t CRetVal_RdSendInput::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("res",&m_res));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_RdSendInput::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
