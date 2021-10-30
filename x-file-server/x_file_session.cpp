#include "x_file_session.h"
#include "sys_log.h"
#include "mem_tool.h"

CxFileSession::CxFileSession()
{
    this->InitBasic();
}

CxFileSession::~CxFileSession()
{
    this->Destroy();
}

status_t CxFileSession::InitBasic()
{
/*##Begin InitBasic##*/
    this->m_name.InitBasic();
    this->m_cur_dir.InitBasic();
    this->m_cur_timer = 0;
    this->m_opened_file_map.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CxFileSession::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_name.Init();
    this->m_cur_dir.Init();
    this->m_opened_file_map.Init();
/*##End Init##*/
    return OK;
}

status_t CxFileSession::Destroy()
{
/*##Begin Destroy##*/
    this->m_name.Destroy();
    this->m_cur_dir.Destroy();
    this->m_opened_file_map.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CxFileSession::Copy(CxFileSession *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_name.Copy(&_p->m_name);
    this->m_cur_dir.Copy(&_p->m_cur_dir);
    this->m_cur_timer = _p->m_cur_timer;
    this->m_opened_file_map.Copy(&_p->m_opened_file_map);
/*##End Copy##*/
    return OK;
}

int CxFileSession::Comp(CxFileSession *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    return m_name.StrCmp(&_p->m_name);
}

status_t CxFileSession::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);

    _buf->Log("name = %s",
        m_name.StrLen()>0?m_name.CStr():"<null>"
    );

    _buf->Log("cur_dir = %s",
        m_cur_dir.StrLen()>0?m_cur_dir.CStr():"<null>"
    );
    _buf->Log("cur_timer = %u",m_cur_timer);
    _buf->Log("opened_file_map = {");
    _buf->IncLogLevel(1);
    this->m_opened_file_map.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetName@@*/
CMem* CxFileSession::GetName()
{
    return &m_name;
}
/*@@End  Function GetName@@*/

/*@@Begin Function GetNameStr@@*/
const char* CxFileSession::GetNameStr()
{
    return m_name.CStr();
}
/*@@End  Function GetNameStr@@*/

/*@@Begin Function GetCurDir@@*/
CMem* CxFileSession::GetCurDir()
{
    return &m_cur_dir;
}
/*@@End  Function GetCurDir@@*/

/*@@Begin Function GetCurDirStr@@*/
const char* CxFileSession::GetCurDirStr()
{
    return m_cur_dir.CStr();
}
/*@@End  Function GetCurDirStr@@*/

/*@@Begin Function SetName@@*/
status_t CxFileSession::SetName(CMem *_name)
{
    ASSERT(_name);
    return this->m_name.Copy(_name);
}
/*@@End  Function SetName@@*/

/*@@Begin Function SetName-CStr@@*/
status_t CxFileSession::SetName(const char *_name)
{
    CMem tmp(_name);
    return this->SetName(&tmp);
}
/*@@End  Function SetName-CStr@@*/

/*@@Begin Function SetCurDir@@*/
status_t CxFileSession::SetCurDir(CMem *_cur_dir)
{
    ASSERT(_cur_dir);
    return this->m_cur_dir.Copy(_cur_dir);
}
/*@@End  Function SetCurDir@@*/

/*@@Begin Function SetCurDir-CStr@@*/
status_t CxFileSession::SetCurDir(const char *_cur_dir)
{
    CMem tmp(_cur_dir);
    return this->SetCurDir(&tmp);
}
/*@@End  Function SetCurDir-CStr@@*/

status_t CxFileSession::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutString("cur_dir",&m_cur_dir);
    _bson->PutUInt32("cur_timer",m_cur_timer);
    _bson->PutString("name",&m_name);
/*##End SaveBson_1##*/
    return OK;
}

status_t CxFileSession::SaveBson(CMem *_mem)
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

status_t CxFileSession::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******cur_dir begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("cur_dir",&_tmp_str));
    this->SetCurDir(&_tmp_str);
    /******cur_dir end*******/}
    BSON_CHECK(_bson->GetUInt32("cur_timer",&m_cur_timer));
    /******name begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("name",&_tmp_str));
    this->SetName(&_tmp_str);
    /******name end*******/}
/*##End LoadBson_1##*/
    return OK;
}

status_t CxFileSession::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}


/*@@Begin Function GetCurTimer@@*/
uint32_t CxFileSession::GetCurTimer()
{
    return m_cur_timer;
}
/*@@End  Function GetCurTimer@@*/

/*@@Begin Function SetCurTimer@@*/
status_t CxFileSession::SetCurTimer(uint32_t _cur_timer)
{
    this->m_cur_timer = _cur_timer;
    return OK;
}
/*@@End  Function SetCurTimer@@*/

/*@@Begin Function GetOpenedFileMap@@*/
CxOpenedFileMap* CxFileSession::GetOpenedFileMap()
{
    return &m_opened_file_map;
}
/*@@End  Function GetOpenedFileMap@@*/
/*@@ Insert Function Here @@*/
