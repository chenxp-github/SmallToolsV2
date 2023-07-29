#include "param_rd_login.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_RdLogin::CParam_RdLogin()
{
    this->InitBasic();
}

CParam_RdLogin::~CParam_RdLogin()
{
    this->Destroy();
}

status_t CParam_RdLogin::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_version = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CParam_RdLogin::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CParam_RdLogin::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_RdLogin::Copy(CParam_RdLogin *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_version = _p->m_version;
/*##End Copy##*/
    return OK;
}

int CParam_RdLogin::Comp(CParam_RdLogin *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_RdLogin::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("version = %d",m_version);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetVersion@@*/
int CParam_RdLogin::GetVersion()
{
    return m_version;
}
/*@@End  Function GetVersion@@*/

/*@@Begin Function SetVersion@@*/
status_t CParam_RdLogin::SetVersion(int _version)
{
    this->m_version = _version;
    return OK;
}
/*@@End  Function SetVersion@@*/

status_t CParam_RdLogin::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("version",m_version);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_RdLogin::SaveBson(CMem *_mem)
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

status_t CParam_RdLogin::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("version",&m_version));
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_RdLogin::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
