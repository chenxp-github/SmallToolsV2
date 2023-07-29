#include "remote_desktop_keybd_input.h"
#include "sys_log.h"
#include "mem_tool.h"

CRemoteDesktopKeybdInput::CRemoteDesktopKeybdInput()
{
    this->InitBasic();
}

CRemoteDesktopKeybdInput::~CRemoteDesktopKeybdInput()
{
    this->Destroy();
}

status_t CRemoteDesktopKeybdInput::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_wVk = 0;
    this->m_wScan = 0;
    this->m_dwFlags = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopKeybdInput::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CRemoteDesktopKeybdInput::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopKeybdInput::Copy(CRemoteDesktopKeybdInput *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_wVk = _p->m_wVk;
    this->m_wScan = _p->m_wScan;
    this->m_dwFlags = _p->m_dwFlags;
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopKeybdInput::Comp(CRemoteDesktopKeybdInput *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopKeybdInput::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("wVk = %d",m_wVk);
    _buf->Log("wScan = %d",m_wScan);
    _buf->Log("dwFlags = %u",m_dwFlags);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetWVk@@*/
uint16_t CRemoteDesktopKeybdInput::GetWVk()
{
    return m_wVk;
}
/*@@End  Function GetWVk@@*/

/*@@Begin Function GetWScan@@*/
uint16_t CRemoteDesktopKeybdInput::GetWScan()
{
    return m_wScan;
}
/*@@End  Function GetWScan@@*/

/*@@Begin Function GetDwFlags@@*/
uint32_t CRemoteDesktopKeybdInput::GetDwFlags()
{
    return m_dwFlags;
}
/*@@End  Function GetDwFlags@@*/

/*@@Begin Function SetWVk@@*/
status_t CRemoteDesktopKeybdInput::SetWVk(uint16_t _wvk)
{
    this->m_wVk = _wvk;
    return OK;
}
/*@@End  Function SetWVk@@*/

/*@@Begin Function SetWScan@@*/
status_t CRemoteDesktopKeybdInput::SetWScan(uint16_t _wscan)
{
    this->m_wScan = _wscan;
    return OK;
}
/*@@End  Function SetWScan@@*/

/*@@Begin Function SetDwFlags@@*/
status_t CRemoteDesktopKeybdInput::SetDwFlags(uint32_t _dwflags)
{
    this->m_dwFlags = _dwflags;
    return OK;
}
/*@@End  Function SetDwFlags@@*/

status_t CRemoteDesktopKeybdInput::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    _bson->PutUInt32("dwFlags",m_dwFlags);
    _bson->PutUInt16("wScan",m_wScan);
    _bson->PutUInt16("wVk",m_wVk);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRemoteDesktopKeybdInput::SaveBson(CMem *_mem)
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

status_t CRemoteDesktopKeybdInput::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetUInt32("dwFlags",&m_dwFlags));
    BSON_CHECK(_bson->GetUInt16("wScan",&m_wScan));
    BSON_CHECK(_bson->GetUInt16("wVk",&m_wVk));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRemoteDesktopKeybdInput::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
