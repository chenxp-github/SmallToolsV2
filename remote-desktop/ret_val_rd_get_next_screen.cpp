#include "ret_val_rd_get_next_screen.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_RdGetNextScreen::CRetVal_RdGetNextScreen()
{
    this->InitBasic();
}

CRetVal_RdGetNextScreen::~CRetVal_RdGetNextScreen()
{
    this->Destroy();
}

status_t CRetVal_RdGetNextScreen::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_res = 0;
    this->m_full = 0;
    this->m_frameNo = 0;
    this->m_diff.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_RdGetNextScreen::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_diff.Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_RdGetNextScreen::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_diff.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_RdGetNextScreen::Copy(CRetVal_RdGetNextScreen *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_res = _p->m_res;
    this->m_full = _p->m_full;
    this->m_frameNo = _p->m_frameNo;
    this->m_diff.Copy(&_p->m_diff);
/*##End Copy##*/
    return OK;
}

int CRetVal_RdGetNextScreen::Comp(CRetVal_RdGetNextScreen *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_RdGetNextScreen::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("res = %d",m_res);
    _buf->Log("full = %d",m_full);
    _buf->Log("frameNo = %d",m_frameNo);
    _buf->Log("diff = {");
    _buf->IncLogLevel(1);
    this->m_diff.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetRes@@*/
int CRetVal_RdGetNextScreen::GetRes()
{
    return m_res;
}
/*@@End  Function GetRes@@*/

/*@@Begin Function GetDiff@@*/
CRemoteDesktopPixelBufferList* CRetVal_RdGetNextScreen::GetDiff()
{
    return &m_diff;
}
/*@@End  Function GetDiff@@*/

/*@@Begin Function SetRes@@*/
status_t CRetVal_RdGetNextScreen::SetRes(int _res)
{
    this->m_res = _res;
    return OK;
}
/*@@End  Function SetRes@@*/

/*@@Begin Function SetDiff@@*/
status_t CRetVal_RdGetNextScreen::SetDiff(CRemoteDesktopPixelBufferList *_diff)
{
    ASSERT(_diff);
    return this->m_diff.Copy(_diff);
}
/*@@End  Function SetDiff@@*/

status_t CRetVal_RdGetNextScreen::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    /******diff begin*******/{
    fsize_t _off;
    _bson->StartDocument("diff",&_off);
    m_diff.SaveBson(_bson);
    _bson->EndDocument(_off);
    /*******diff end********/}
    _bson->PutInt32("frameNo",m_frameNo);
    _bson->PutInt32("full",m_full);
    _bson->PutInt32("res",m_res);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_RdGetNextScreen::SaveBson(CMem *_mem)
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

status_t CRetVal_RdGetNextScreen::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******diff begin*******/{
    CMiniBson _doc;
    BSON_CHECK(_bson->GetDocument("diff",&_doc));
    m_diff.LoadBson(&_doc);
    /*******diff end********/}
    BSON_CHECK(_bson->GetInt32("frameNo",&m_frameNo));
    BSON_CHECK(_bson->GetInt32("full",&m_full));
    BSON_CHECK(_bson->GetInt32("res",&m_res));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_RdGetNextScreen::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}


/*@@Begin Function GetFull@@*/
int CRetVal_RdGetNextScreen::GetFull()
{
    return m_full;
}
/*@@End  Function GetFull@@*/

/*@@Begin Function SetFull@@*/
status_t CRetVal_RdGetNextScreen::SetFull(int _full)
{
    this->m_full = _full;
    return OK;
}
/*@@End  Function SetFull@@*/

/*@@Begin Function GetFrameNo@@*/
int CRetVal_RdGetNextScreen::GetFrameNo()
{
    return m_frameNo;
}
/*@@End  Function GetFrameNo@@*/

/*@@Begin Function SetFrameNo@@*/
status_t CRetVal_RdGetNextScreen::SetFrameNo(int _frameno)
{
    this->m_frameNo = _frameno;
    return OK;
}
/*@@End  Function SetFrameNo@@*/
/*@@ Insert Function Here @@*/
