#include "remote_desktop_session.h"
#include "sys_log.h"
#include "mem_tool.h"

CRemoteDesktopSession::CRemoteDesktopSession()
{
    this->InitBasic();
}

CRemoteDesktopSession::~CRemoteDesktopSession()
{
    this->Destroy();
}

status_t CRemoteDesktopSession::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();
    this->m_name.InitBasic();
    this->m_flags = 0;
    this->m_lifetime = 0;
    this->m_cur_screen.InitBasic();
    this->m_frameNo = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopSession::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
/*##Begin Init##*/
    TASK_CONTAINER_INIT(_taskmgr);
    this->m_name.Init();
    this->m_cur_screen.Init();
/*##End Init##*/
    return OK;
}

status_t CRemoteDesktopSession::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    this->m_name.Destroy();
    this->m_cur_screen.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopSession::Copy(CRemoteDesktopSession *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_name.Copy(&_p->m_name);
    this->m_flags = _p->m_flags;
    this->m_lifetime = _p->m_lifetime;
    this->m_cur_screen.Copy(&_p->m_cur_screen);
    this->m_frameNo = _p->m_frameNo;
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopSession::Comp(CRemoteDesktopSession *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    return m_name.StrCmp(&_p->m_name);
}

status_t CRemoteDesktopSession::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);

    _buf->Log("name = %s",
        m_name.StrLen()>0?m_name.CStr():"<null>"
    );
    _buf->Log("flags = %u",m_flags);
    _buf->Log("lifetime = %u",m_lifetime);
    _buf->Log("cur_screen = {");
    _buf->IncLogLevel(1);
    this->m_cur_screen.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("frameNo = %d",m_frameNo);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetName@@*/
CMem* CRemoteDesktopSession::GetName()
{
    return &m_name;
}
/*@@End  Function GetName@@*/

/*@@Begin Function GetNameStr@@*/
const char* CRemoteDesktopSession::GetNameStr()
{
    return m_name.CStr();
}
/*@@End  Function GetNameStr@@*/

/*@@Begin Function GetFlags@@*/
uint32_t CRemoteDesktopSession::GetFlags()
{
    return m_flags;
}
/*@@End  Function GetFlags@@*/

/*@@Begin Function SetName@@*/
status_t CRemoteDesktopSession::SetName(CMem *_name)
{
    ASSERT(_name);
    return this->m_name.Copy(_name);
}
/*@@End  Function SetName@@*/

/*@@Begin Function SetName-CStr@@*/
status_t CRemoteDesktopSession::SetName(const char *_name)
{
    CMem tmp(_name);
    return this->SetName(&tmp);
}
/*@@End  Function SetName-CStr@@*/

/*@@Begin Function SetFlags@@*/
status_t CRemoteDesktopSession::SetFlags(uint32_t _flags)
{
    this->m_flags = _flags;
    return OK;
}
/*@@End  Function SetFlags@@*/


/*@@Begin Function GetLifetime@@*/
uint32_t CRemoteDesktopSession::GetLifetime()
{
    return m_lifetime;
}
/*@@End  Function GetLifetime@@*/

/*@@Begin Function SetLifetime@@*/
status_t CRemoteDesktopSession::SetLifetime(uint32_t _lifetime)
{
    this->m_lifetime = _lifetime;
    return OK;
}
/*@@End  Function SetLifetime@@*/

/*@@Begin Function GetCurScreen@@*/
CRemoteDesktopPixelBuffer* CRemoteDesktopSession::GetCurScreen()
{
    return &m_cur_screen;
}
/*@@End  Function GetCurScreen@@*/

/*@@Begin Function SetCurScreen@@*/
status_t CRemoteDesktopSession::SetCurScreen(CRemoteDesktopPixelBuffer *_cur_screen)
{
    ASSERT(_cur_screen);
    return this->m_cur_screen.Copy(_cur_screen);
}
/*@@End  Function SetCurScreen@@*/

/*@@Begin Function GetFrameNo@@*/
int CRemoteDesktopSession::GetFrameNo()
{
    return m_frameNo;
}
/*@@End  Function GetFrameNo@@*/

/*@@Begin Function SetFrameNo@@*/
status_t CRemoteDesktopSession::SetFrameNo(int _frameno)
{
    this->m_frameNo = _frameno;
    return OK;
}
/*@@End  Function SetFrameNo@@*/
/*@@ Insert Function Here @@*/
status_t CRemoteDesktopSession::IncLifetime(uint32_t ms)
{
	m_lifetime += ms;
	return OK;
}
