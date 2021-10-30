#include "ret_val_xfs_list_folder.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_XfsListFolder::CRetVal_XfsListFolder()
{
    this->InitBasic();
}

CRetVal_XfsListFolder::~CRetVal_XfsListFolder()
{
    this->Destroy();
}

status_t CRetVal_XfsListFolder::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_result = 0;
    this->m_list.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_XfsListFolder::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_list.Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_XfsListFolder::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_list.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_XfsListFolder::Copy(CRetVal_XfsListFolder *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_result = _p->m_result;
    this->m_list.Copy(&_p->m_list);
/*##End Copy##*/
    return OK;
}

int CRetVal_XfsListFolder::Comp(CRetVal_XfsListFolder *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_XfsListFolder::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("result = %d",m_result);
    _buf->Log("list = {");
    _buf->IncLogLevel(1);
    this->m_list.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetResult@@*/
int CRetVal_XfsListFolder::GetResult()
{
    return m_result;
}
/*@@End  Function GetResult@@*/

/*@@Begin Function GetList@@*/
CxFileInfoList* CRetVal_XfsListFolder::GetList()
{
    return &m_list;
}
/*@@End  Function GetList@@*/

/*@@Begin Function SetResult@@*/
status_t CRetVal_XfsListFolder::SetResult(int _result)
{
    this->m_result = _result;
    return OK;
}
/*@@End  Function SetResult@@*/

/*@@Begin Function SetList@@*/
status_t CRetVal_XfsListFolder::SetList(CxFileInfoList *_list)
{
    ASSERT(_list);
    return this->m_list.Copy(_list);
}
/*@@End  Function SetList@@*/

status_t CRetVal_XfsListFolder::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    /******list begin*******/{
    fsize_t _off;
    _bson->StartDocument("list",&_off);
    m_list.SaveBson(_bson);
    _bson->EndDocument(_off);
    /*******list end********/}
    _bson->PutInt32("result",m_result);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_XfsListFolder::SaveBson(CMem *_mem)
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

status_t CRetVal_XfsListFolder::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******list begin*******/{
    CMiniBson _doc;
    BSON_CHECK(_bson->GetDocument("list",&_doc));
    m_list.LoadBson(&_doc);
    /*******list end********/}
    BSON_CHECK(_bson->GetInt32("result",&m_result));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_XfsListFolder::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
