#include "remote_desktop_input.h"
#include "sys_log.h"
#include "mem_tool.h"

CRemoteDesktopInput::CRemoteDesktopInput()
{
    this->InitBasic();
}

CRemoteDesktopInput::~CRemoteDesktopInput()
{
    this->Destroy();
}

status_t CRemoteDesktopInput::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_type = 0;
    this->m_mouseInput = NULL;
    this->m_keybdInput = NULL;
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopInput::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CRemoteDesktopInput::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    DEL(this->m_mouseInput);
    DEL(this->m_keybdInput);
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopInput::Copy(CRemoteDesktopInput *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_type = _p->m_type;
    this->SetMouseInput(_p->GetMouseInput());
    this->SetKeybdInput(_p->GetKeybdInput());
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopInput::Comp(CRemoteDesktopInput *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopInput::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("type = %u",m_type);
    if(this->m_mouseInput)
    {
        _buf->Log("mouseInput = {");
        _buf->IncLogLevel(1);
        this->m_mouseInput->Print(_buf);
        _buf->IncLogLevel(-1);
        _buf->Log("");
        _buf->Log("},");
    }
    if(this->m_keybdInput)
    {
        _buf->Log("keybdInput = {");
        _buf->IncLogLevel(1);
        this->m_keybdInput->Print(_buf);
        _buf->IncLogLevel(-1);
        _buf->Log("");
        _buf->Log("},");
    }
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetType@@*/
uint32_t CRemoteDesktopInput::GetType()
{
    return m_type;
}
/*@@End  Function GetType@@*/

/*@@Begin Function GetMouseInput@@*/
CRemoteDesktopMouseInput* CRemoteDesktopInput::GetMouseInput()
{
    return m_mouseInput;
}
/*@@End  Function GetMouseInput@@*/

/*@@Begin Function GetKeybdInput@@*/
CRemoteDesktopKeybdInput* CRemoteDesktopInput::GetKeybdInput()
{
    return m_keybdInput;
}
/*@@End  Function GetKeybdInput@@*/

/*@@Begin Function SetType@@*/
status_t CRemoteDesktopInput::SetType(uint32_t _type)
{
    this->m_type = _type;
    return OK;
}
/*@@End  Function SetType@@*/

/*@@Begin Function SetMouseInput-Create@@*/
status_t CRemoteDesktopInput::CreateMouseInput()
{
    if(this->m_mouseInput) return OK;
    NEW(this->m_mouseInput,CRemoteDesktopMouseInput);
    this->m_mouseInput->Init();
    return OK;
}
/*@@End  Function SetMouseInput-Create@@*/

/*@@Begin Function SetMouseInput@@*/
status_t CRemoteDesktopInput::SetMouseInput(CRemoteDesktopMouseInput *_mouseinput)
{
    if(_mouseinput == NULL)
    {
        DEL(this->m_mouseInput);
        return OK;
    }

    this->CreateMouseInput();
    ASSERT(this->m_mouseInput);
    return this->m_mouseInput->Copy(_mouseinput);
}
/*@@End  Function SetMouseInput@@*/

/*@@Begin Function SetKeybdInput-Create@@*/
status_t CRemoteDesktopInput::CreateKeybdInput()
{
    if(this->m_keybdInput) return OK;
    NEW(this->m_keybdInput,CRemoteDesktopKeybdInput);
    this->m_keybdInput->Init();
    return OK;
}
/*@@End  Function SetKeybdInput-Create@@*/

/*@@Begin Function SetKeybdInput@@*/
status_t CRemoteDesktopInput::SetKeybdInput(CRemoteDesktopKeybdInput *_keybdinput)
{
    if(_keybdinput == NULL)
    {
        DEL(this->m_keybdInput);
        return OK;
    }

    this->CreateKeybdInput();
    ASSERT(this->m_keybdInput);
    return this->m_keybdInput->Copy(_keybdinput);
}
/*@@End  Function SetKeybdInput@@*/

status_t CRemoteDesktopInput::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    /******keybdInput begin*******/{
    if(this->m_keybdInput)
    {
        fsize_t _off;
        _bson->StartDocument("keybdInput",&_off);
        m_keybdInput->SaveBson(_bson);
        _bson->EndDocument(_off);
    }
    /*******keybdInput end********/}
    /******mouseInput begin*******/{
    if(this->m_mouseInput)
    {
        fsize_t _off;
        _bson->StartDocument("mouseInput",&_off);
        m_mouseInput->SaveBson(_bson);
        _bson->EndDocument(_off);
    }
    /*******mouseInput end********/}
    _bson->PutUInt32("type",m_type);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRemoteDesktopInput::SaveBson(CMem *_mem)
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

status_t CRemoteDesktopInput::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******keybdInput begin*******/{
    CMiniBson _doc;
    if(_bson->GetDocument("keybdInput",&_doc))
    {
        CRemoteDesktopKeybdInput _tmp_keybdinput;
        _tmp_keybdinput.Init();
        _tmp_keybdinput.LoadBson(&_doc);
        this->SetKeybdInput(&_tmp_keybdinput);
    }
    /*******keybdInput end********/}
    /******mouseInput begin*******/{
    CMiniBson _doc;
    if(_bson->GetDocument("mouseInput",&_doc))
    {
        CRemoteDesktopMouseInput _tmp_mouseinput;
        _tmp_mouseinput.Init();
        _tmp_mouseinput.LoadBson(&_doc);
        this->SetMouseInput(&_tmp_mouseinput);
    }
    /*******mouseInput end********/}
    BSON_CHECK(_bson->GetUInt32("type",&m_type));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRemoteDesktopInput::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
