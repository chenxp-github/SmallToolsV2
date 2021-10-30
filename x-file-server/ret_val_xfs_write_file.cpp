#include "ret_val_xfs_write_file.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_XfsWriteFile::CRetVal_XfsWriteFile()
{
    this->InitBasic();
}

CRetVal_XfsWriteFile::~CRetVal_XfsWriteFile()
{
    this->Destroy();
}

status_t CRetVal_XfsWriteFile::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_result = 0;
    this->m_ws = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_XfsWriteFile::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_XfsWriteFile::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_XfsWriteFile::Copy(CRetVal_XfsWriteFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_result = _p->m_result;
    this->m_ws = _p->m_ws;
/*##End Copy##*/
    return OK;
}

int CRetVal_XfsWriteFile::Comp(CRetVal_XfsWriteFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_XfsWriteFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("result = %d",m_result);
    _buf->Log("ws = %d",m_ws);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetResult@@*/
int CRetVal_XfsWriteFile::GetResult()
{
    return m_result;
}
/*@@End  Function GetResult@@*/

/*@@Begin Function GetWs@@*/
int CRetVal_XfsWriteFile::GetWs()
{
    return m_ws;
}
/*@@End  Function GetWs@@*/

/*@@Begin Function SetResult@@*/
status_t CRetVal_XfsWriteFile::SetResult(int _result)
{
    this->m_result = _result;
    return OK;
}
/*@@End  Function SetResult@@*/

/*@@Begin Function SetWs@@*/
status_t CRetVal_XfsWriteFile::SetWs(int _ws)
{
    this->m_ws = _ws;
    return OK;
}
/*@@End  Function SetWs@@*/

status_t CRetVal_XfsWriteFile::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("result",m_result);
    _bson->PutInt32("ws",m_ws);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_XfsWriteFile::SaveBson(CMem *_mem)
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

status_t CRetVal_XfsWriteFile::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("result",&m_result));
    BSON_CHECK(_bson->GetInt32("ws",&m_ws));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_XfsWriteFile::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
