#include "remote_desktop_mouse_input.h"
#include "sys_log.h"
#include "mem_tool.h"

CRemoteDesktopMouseInput::CRemoteDesktopMouseInput()
{
    this->InitBasic();
}

CRemoteDesktopMouseInput::~CRemoteDesktopMouseInput()
{
    this->Destroy();
}

status_t CRemoteDesktopMouseInput::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_dx = 0;
    this->m_dy = 0;
    this->m_mouseData = 0;
    this->m_dwFlags = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopMouseInput::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CRemoteDesktopMouseInput::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopMouseInput::Copy(CRemoteDesktopMouseInput *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_dx = _p->m_dx;
    this->m_dy = _p->m_dy;
    this->m_mouseData = _p->m_mouseData;
    this->m_dwFlags = _p->m_dwFlags;
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopMouseInput::Comp(CRemoteDesktopMouseInput *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopMouseInput::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("dx = %d",m_dx);
    _buf->Log("dy = %d",m_dy);
    _buf->Log("mouseData = %u",m_mouseData);
    _buf->Log("dwFlags = %u",m_dwFlags);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetDx@@*/
int CRemoteDesktopMouseInput::GetDx()
{
    return m_dx;
}
/*@@End  Function GetDx@@*/

/*@@Begin Function GetDy@@*/
int CRemoteDesktopMouseInput::GetDy()
{
    return m_dy;
}
/*@@End  Function GetDy@@*/

/*@@Begin Function GetMouseData@@*/
uint32_t CRemoteDesktopMouseInput::GetMouseData()
{
    return m_mouseData;
}
/*@@End  Function GetMouseData@@*/

/*@@Begin Function GetDwFlags@@*/
uint32_t CRemoteDesktopMouseInput::GetDwFlags()
{
    return m_dwFlags;
}
/*@@End  Function GetDwFlags@@*/

/*@@Begin Function SetDx@@*/
status_t CRemoteDesktopMouseInput::SetDx(int _dx)
{
    this->m_dx = _dx;
    return OK;
}
/*@@End  Function SetDx@@*/

/*@@Begin Function SetDy@@*/
status_t CRemoteDesktopMouseInput::SetDy(int _dy)
{
    this->m_dy = _dy;
    return OK;
}
/*@@End  Function SetDy@@*/

/*@@Begin Function SetMouseData@@*/
status_t CRemoteDesktopMouseInput::SetMouseData(uint32_t _mousedata)
{
    this->m_mouseData = _mousedata;
    return OK;
}
/*@@End  Function SetMouseData@@*/

/*@@Begin Function SetDwFlags@@*/
status_t CRemoteDesktopMouseInput::SetDwFlags(uint32_t _dwflags)
{
    this->m_dwFlags = _dwflags;
    return OK;
}
/*@@End  Function SetDwFlags@@*/

status_t CRemoteDesktopMouseInput::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutUInt32("dwFlags",m_dwFlags);
    _bson->PutInt32("dx",m_dx);
    _bson->PutInt32("dy",m_dy);
    _bson->PutUInt32("mouseData",m_mouseData);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRemoteDesktopMouseInput::SaveBson(CMem *_mem)
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

status_t CRemoteDesktopMouseInput::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetUInt32("dwFlags",&m_dwFlags));
    BSON_CHECK(_bson->GetInt32("dx",&m_dx));
    BSON_CHECK(_bson->GetInt32("dy",&m_dy));
    BSON_CHECK(_bson->GetUInt32("mouseData",&m_mouseData));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRemoteDesktopMouseInput::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
