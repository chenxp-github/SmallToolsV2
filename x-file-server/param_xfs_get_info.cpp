#include "param_xfs_get_info.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_XfsGetInfo::CParam_XfsGetInfo()
{
    this->InitBasic();
}

CParam_XfsGetInfo::~CParam_XfsGetInfo()
{
    this->Destroy();
}

status_t CParam_XfsGetInfo::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_path.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CParam_XfsGetInfo::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_path.Init();
/*##End Init##*/
    return OK;
}

status_t CParam_XfsGetInfo::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_path.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_XfsGetInfo::Copy(CParam_XfsGetInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_path.Copy(&_p->m_path);
/*##End Copy##*/
    return OK;
}

int CParam_XfsGetInfo::Comp(CParam_XfsGetInfo *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_XfsGetInfo::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);

    _buf->Log("path = %s",
        m_path.StrLen()>0?m_path.CStr():"<null>"
    );
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetPath@@*/
CMem* CParam_XfsGetInfo::GetPath()
{
    return &m_path;
}
/*@@End  Function GetPath@@*/

/*@@Begin Function GetPathStr@@*/
const char* CParam_XfsGetInfo::GetPathStr()
{
    return m_path.CStr();
}
/*@@End  Function GetPathStr@@*/

/*@@Begin Function SetPath@@*/
status_t CParam_XfsGetInfo::SetPath(CMem *_path)
{
    ASSERT(_path);
    return this->m_path.Copy(_path);
}
/*@@End  Function SetPath@@*/

/*@@Begin Function SetPath-CStr@@*/
status_t CParam_XfsGetInfo::SetPath(const char *_path)
{
    CMem tmp(_path);
    return this->SetPath(&tmp);
}
/*@@End  Function SetPath-CStr@@*/

status_t CParam_XfsGetInfo::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutString("path",&m_path);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_XfsGetInfo::SaveBson(CMem *_mem)
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

status_t CParam_XfsGetInfo::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******path begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("path",&_tmp_str));
    this->SetPath(&_tmp_str);
    /******path end*******/}
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_XfsGetInfo::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
