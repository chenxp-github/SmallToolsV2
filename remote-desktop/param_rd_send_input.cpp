#include "param_rd_send_input.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_RdSendInput::CParam_RdSendInput()
{
    this->InitBasic();
}

CParam_RdSendInput::~CParam_RdSendInput()
{
    this->Destroy();
}

status_t CParam_RdSendInput::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_input.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CParam_RdSendInput::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
    this->m_input.Init();
/*##End Init##*/
    return OK;
}

status_t CParam_RdSendInput::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    this->m_input.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_RdSendInput::Copy(CParam_RdSendInput *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    this->m_input.Copy(&_p->m_input);
/*##End Copy##*/
    return OK;
}

int CParam_RdSendInput::Comp(CParam_RdSendInput *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_RdSendInput::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    _buf->Log("input = {");
    _buf->IncLogLevel(1);
    this->m_input.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetInput@@*/
CRemoteDesktopInput* CParam_RdSendInput::GetInput()
{
    return &m_input;
}
/*@@End  Function GetInput@@*/

/*@@Begin Function SetInput@@*/
status_t CParam_RdSendInput::SetInput(CRemoteDesktopInput *_input)
{
    ASSERT(_input);
    return this->m_input.Copy(_input);
}
/*@@End  Function SetInput@@*/

status_t CParam_RdSendInput::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    /******input begin*******/{
    fsize_t _off;
    _bson->StartDocument("input",&_off);
    m_input.SaveBson(_bson);
    _bson->EndDocument(_off);
    /*******input end********/}
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_RdSendInput::SaveBson(CMem *_mem)
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

status_t CParam_RdSendInput::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******input begin*******/{
    CMiniBson _doc;
    BSON_CHECK(_bson->GetDocument("input",&_doc));
    m_input.LoadBson(&_doc);
    /*******input end********/}
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_RdSendInput::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
