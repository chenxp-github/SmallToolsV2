#include "param_rd_get_next_screen.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_RdGetNextScreen::CParam_RdGetNextScreen()
{
    this->InitBasic();
}

CParam_RdGetNextScreen::~CParam_RdGetNextScreen()
{
    this->Destroy();
}

status_t CParam_RdGetNextScreen::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_monitorName.InitBasic();
    this->m_curFrameNo = 0;
    this->m_bytePerPixel = 0;
    this->m_full = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CParam_RdGetNextScreen::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_monitorName.Init();
/*##End Init##*/
    return OK;
}

status_t CParam_RdGetNextScreen::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_monitorName.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_RdGetNextScreen::Copy(CParam_RdGetNextScreen *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_monitorName.Copy(&_p->m_monitorName);
    this->m_curFrameNo = _p->m_curFrameNo;
    this->m_bytePerPixel = _p->m_bytePerPixel;
    this->m_full = _p->m_full;
/*##End Copy##*/
    return OK;
}

int CParam_RdGetNextScreen::Comp(CParam_RdGetNextScreen *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_RdGetNextScreen::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);

    _buf->Log("monitorName = %s",
        m_monitorName.StrLen()>0?m_monitorName.CStr():"<null>"
    );
    _buf->Log("curFrameNo = %d",m_curFrameNo);
    _buf->Log("bytePerPixel = %d",m_bytePerPixel);
    _buf->Log("full = %d",m_full);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetMonitorName@@*/
CMem* CParam_RdGetNextScreen::GetMonitorName()
{
    return &m_monitorName;
}
/*@@End  Function GetMonitorName@@*/

/*@@Begin Function GetMonitorNameStr@@*/
const char* CParam_RdGetNextScreen::GetMonitorNameStr()
{
    return m_monitorName.CStr();
}
/*@@End  Function GetMonitorNameStr@@*/

/*@@Begin Function GetFull@@*/
int CParam_RdGetNextScreen::GetFull()
{
    return m_full;
}
/*@@End  Function GetFull@@*/

/*@@Begin Function SetMonitorName@@*/
status_t CParam_RdGetNextScreen::SetMonitorName(CMem *_monitorname)
{
    ASSERT(_monitorname);
    return this->m_monitorName.Copy(_monitorname);
}
/*@@End  Function SetMonitorName@@*/

/*@@Begin Function SetMonitorName-CStr@@*/
status_t CParam_RdGetNextScreen::SetMonitorName(const char *_monitorname)
{
    CMem tmp(_monitorname);
    return this->SetMonitorName(&tmp);
}
/*@@End  Function SetMonitorName-CStr@@*/

/*@@Begin Function SetFull@@*/
status_t CParam_RdGetNextScreen::SetFull(int _full)
{
    this->m_full = _full;
    return OK;
}
/*@@End  Function SetFull@@*/

status_t CParam_RdGetNextScreen::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutInt32("bytePerPixel",m_bytePerPixel);
    _bson->PutInt32("curFrameNo",m_curFrameNo);
    _bson->PutInt32("full",m_full);
    _bson->PutString("monitorName",&m_monitorName);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_RdGetNextScreen::SaveBson(CMem *_mem)
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

status_t CParam_RdGetNextScreen::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetInt32("bytePerPixel",&m_bytePerPixel));
    BSON_CHECK(_bson->GetInt32("curFrameNo",&m_curFrameNo));
    BSON_CHECK(_bson->GetInt32("full",&m_full));
    /******monitorName begin*******/{
    CMem _tmp_str;
    BSON_CHECK(_bson->GetString("monitorName",&_tmp_str));
    this->SetMonitorName(&_tmp_str);
    /******monitorName end*******/}
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_RdGetNextScreen::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}


/*@@Begin Function GetBytePerPixel@@*/
int CParam_RdGetNextScreen::GetBytePerPixel()
{
    return m_bytePerPixel;
}
/*@@End  Function GetBytePerPixel@@*/

/*@@Begin Function SetBytePerPixel@@*/
status_t CParam_RdGetNextScreen::SetBytePerPixel(int _byteperpixel)
{
    this->m_bytePerPixel = _byteperpixel;
    return OK;
}
/*@@End  Function SetBytePerPixel@@*/

/*@@Begin Function GetCurFrameNo@@*/
int CParam_RdGetNextScreen::GetCurFrameNo()
{
    return m_curFrameNo;
}
/*@@End  Function GetCurFrameNo@@*/

/*@@Begin Function SetCurFrameNo@@*/
status_t CParam_RdGetNextScreen::SetCurFrameNo(int _curframeno)
{
    this->m_curFrameNo = _curframeno;
    return OK;
}
/*@@End  Function SetCurFrameNo@@*/
/*@@ Insert Function Here @@*/
