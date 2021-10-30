#include "ret_val_xfs_get_info.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_XfsGetInfo::CRetVal_XfsGetInfo()
{
    this->InitBasic();
}

CRetVal_XfsGetInfo::~CRetVal_XfsGetInfo()
{
    this->Destroy();
}

status_t CRetVal_XfsGetInfo::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_result = 0;
    this->m_info.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_XfsGetInfo::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_info.Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_XfsGetInfo::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_info.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_XfsGetInfo::Copy(CRetVal_XfsGetInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_result = _p->m_result;
    this->m_info.Copy(&_p->m_info);
/*##End Copy##*/
    return OK;
}

int CRetVal_XfsGetInfo::Comp(CRetVal_XfsGetInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_XfsGetInfo::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("result = %d",m_result);
    _buf->Log("info = {");
    _buf->IncLogLevel(1);
    this->m_info.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetInfo@@*/
CxFileInfo* CRetVal_XfsGetInfo::GetInfo()
{
    return &m_info;
}
/*@@End  Function GetInfo@@*/

/*@@Begin Function SetInfo@@*/
status_t CRetVal_XfsGetInfo::SetInfo(CxFileInfo *_info)
{
    ASSERT(_info);
    return this->m_info.Copy(_info);
}
/*@@End  Function SetInfo@@*/

status_t CRetVal_XfsGetInfo::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    /******info begin*******/{
    fsize_t _off;
    _bson->StartDocument("info",&_off);
    m_info.SaveBson(_bson);
    _bson->EndDocument(_off);
    /*******info end********/}
    _bson->PutInt32("result",m_result);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_XfsGetInfo::SaveBson(CMem *_mem)
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

status_t CRetVal_XfsGetInfo::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******info begin*******/{
    CMiniBson _doc;
    BSON_CHECK(_bson->GetDocument("info",&_doc));
    m_info.LoadBson(&_doc);
    /*******info end********/}
    BSON_CHECK(_bson->GetInt32("result",&m_result));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_XfsGetInfo::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}


/*@@Begin Function GetResult@@*/
int CRetVal_XfsGetInfo::GetResult()
{
    return m_result;
}
/*@@End  Function GetResult@@*/

/*@@Begin Function SetResult@@*/
status_t CRetVal_XfsGetInfo::SetResult(int _result)
{
    this->m_result = _result;
    return OK;
}
/*@@End  Function SetResult@@*/
/*@@ Insert Function Here @@*/
