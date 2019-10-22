#include "xevent.h"
#include "syslog.h"
#include "mem_tool.h"

CXEvent::CXEvent()
{
    this->InitBasic();
}
CXEvent::~CXEvent()
{
    this->Destroy();
}
status_t CXEvent::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    memset(&m_Event,0,sizeof(m_Event));
    return OK;
}
status_t CXEvent::Init()
{
    this->InitBasic();
    WEAK_REF_ID_INIT();
    return OK;
}
status_t CXEvent::Destroy()
{
    this->InitBasic();
    return OK;
}
int CXEvent::Comp(CXEvent *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CXEvent::Copy(CXEvent *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    memcpy(&m_Event,&_p->m_Event,sizeof(m_Event));
    return OK;
}
status_t CXEvent::Copy(NativeXEvent *event)
{
    ASSERT(event);
    memcpy(&m_Event,event,sizeof(m_Event));
    return OK;
}
status_t CXEvent::Print(CFileBase *_buf)
{
    return OK;
}
int CXEvent::GetType()
{
    return m_Event.type;
}
uint32_t CXEvent::GetSerial()
{
    return m_Event.xkey.serial;
}
uint32_t CXEvent::GetSendEvent()
{
    return m_Event.xkey.send_event;
}
uint32_t CXEvent::GetWindow()
{
    return m_Event.xkey.window;
}
uint32_t CXEvent::GetRootWindow()
{
    return m_Event.xkey.root;
}

NativeXEvent* CXEvent::GetNativeXEvent()
{
    return &m_Event;
}

int CXEvent::Size()
{
    return sizeof(m_Event);
}
