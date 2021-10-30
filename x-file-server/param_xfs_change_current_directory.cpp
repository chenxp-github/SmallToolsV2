#include "param_xfs_change_current_directory.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_XfsChangeCurrentDirectory::CParam_XfsChangeCurrentDirectory()
{
    this->InitBasic();
}

CParam_XfsChangeCurrentDirectory::~CParam_XfsChangeCurrentDirectory()
{
    this->Destroy();
}

status_t CParam_XfsChangeCurrentDirectory::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_dir.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CParam_XfsChangeCurrentDirectory::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_dir.Init();
/*##End Init##*/
    return OK;
}

status_t CParam_XfsChangeCurrentDirectory::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_dir.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_XfsChangeCurrentDirectory::Copy(CParam_XfsChangeCurrentDirectory *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_dir.Copy(&_p->m_dir);
/*##End Copy##*/
    return OK;
}

int CParam_XfsChangeCurrentDirectory::Comp(CParam_XfsChangeCurrentDirectory *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_XfsChangeCurrentDirectory::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);

    _buf->Log("dir = %s",
        m_dir.StrLen()>0?m_dir.CStr():"<null>"
    );
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetDir@@*/
CMem* CParam_XfsChangeCurrentDirectory::GetDir()
{
    return &m_dir;
}
/*@@End  Function GetDir@@*/

/*@@Begin Function GetDirStr@@*/
const char* CParam_XfsChangeCurrentDirectory::GetDirStr()
{
    return m_dir.CStr();
}
/*@@End  Function GetDirStr@@*/

/*@@Begin Function SetDir@@*/
status_t CParam_XfsChangeCurrentDirectory::SetDir(CMem *_dir)
{
    ASSERT(_dir);
    return this->m_dir.Copy(_dir);
}
/*@@End  Function SetDir@@*/

/*@@Begin Function SetDir-CStr@@*/
status_t CParam_XfsChangeCurrentDirectory::SetDir(const char *_dir)
{
    CMem tmp(_dir);
    return this->SetDir(&tmp);
}
/*@@End  Function SetDir-CStr@@*/

status_t CParam_XfsChangeCurrentDirectory::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutString("dir",&m_dir);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_XfsChangeCurrentDirectory::SaveBson(CMem *_mem)
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

status_t CParam_XfsChangeCurrentDirectory::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******dir begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("dir",&_tmp_str));
    this->SetDir(&_tmp_str);
    /******dir end*******/}
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_XfsChangeCurrentDirectory::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
