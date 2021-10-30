#include "param_xfs_open_file.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_XfsOpenFile::CParam_XfsOpenFile()
{
    this->InitBasic();
}

CParam_XfsOpenFile::~CParam_XfsOpenFile()
{
    this->Destroy();
}

status_t CParam_XfsOpenFile::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_path.InitBasic();
    this->m_mode.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CParam_XfsOpenFile::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_path.Init();
    this->m_mode.Init();
/*##End Init##*/
    return OK;
}

status_t CParam_XfsOpenFile::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_path.Destroy();
    this->m_mode.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_XfsOpenFile::Copy(CParam_XfsOpenFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_path.Copy(&_p->m_path);
    this->m_mode.Copy(&_p->m_mode);
/*##End Copy##*/
    return OK;
}

int CParam_XfsOpenFile::Comp(CParam_XfsOpenFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_XfsOpenFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);

    _buf->Log("path = %s",
        m_path.StrLen()>0?m_path.CStr():"<null>"
    );

    _buf->Log("mode = %s",
        m_mode.StrLen()>0?m_mode.CStr():"<null>"
    );
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetPath@@*/
CMem* CParam_XfsOpenFile::GetPath()
{
    return &m_path;
}
/*@@End  Function GetPath@@*/

/*@@Begin Function GetPathStr@@*/
const char* CParam_XfsOpenFile::GetPathStr()
{
    return m_path.CStr();
}
/*@@End  Function GetPathStr@@*/

/*@@Begin Function GetMode@@*/
CMem* CParam_XfsOpenFile::GetMode()
{
    return &m_mode;
}
/*@@End  Function GetMode@@*/

/*@@Begin Function GetModeStr@@*/
const char* CParam_XfsOpenFile::GetModeStr()
{
    return m_mode.CStr();
}
/*@@End  Function GetModeStr@@*/

/*@@Begin Function SetPath@@*/
status_t CParam_XfsOpenFile::SetPath(CMem *_path)
{
    ASSERT(_path);
    return this->m_path.Copy(_path);
}
/*@@End  Function SetPath@@*/

/*@@Begin Function SetPath-CStr@@*/
status_t CParam_XfsOpenFile::SetPath(const char *_path)
{
    CMem tmp(_path);
    return this->SetPath(&tmp);
}
/*@@End  Function SetPath-CStr@@*/

/*@@Begin Function SetMode@@*/
status_t CParam_XfsOpenFile::SetMode(CMem *_mode)
{
    ASSERT(_mode);
    return this->m_mode.Copy(_mode);
}
/*@@End  Function SetMode@@*/

/*@@Begin Function SetMode-CStr@@*/
status_t CParam_XfsOpenFile::SetMode(const char *_mode)
{
    CMem tmp(_mode);
    return this->SetMode(&tmp);
}
/*@@End  Function SetMode-CStr@@*/

status_t CParam_XfsOpenFile::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutString("mode",&m_mode);
    _bson->PutString("path",&m_path);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_XfsOpenFile::SaveBson(CMem *_mem)
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

status_t CParam_XfsOpenFile::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******mode begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("mode",&_tmp_str));
    this->SetMode(&_tmp_str);
    /******mode end*******/}
    /******path begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("path",&_tmp_str));
    this->SetPath(&_tmp_str);
    /******path end*******/}
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_XfsOpenFile::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
