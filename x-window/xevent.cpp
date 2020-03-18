#include "xevent.h"
#include "syslog.h"
#include "mem_tool.h"

CxEvent::CxEvent()
{
    this->InitBasic();
}
CxEvent::~CxEvent()
{
    this->Destroy();
}
status_t CxEvent::InitBasic()
{
    WEAK_REF_CLEAR();
    memset(&m_Event,0,sizeof(m_Event));
    return OK;
}
status_t CxEvent::Init()
{
    this->InitBasic();    
    return OK;
}
status_t CxEvent::Destroy()
{
    WEAK_REF_DESTROY();
    this->InitBasic();
    return OK;
}
int CxEvent::Comp(CxEvent *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CxEvent::Copy(CxEvent *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    memcpy(&m_Event,&_p->m_Event,sizeof(m_Event));
    return OK;
}
status_t CxEvent::Copy(NativeXEvent *event)
{
    ASSERT(event);
    memcpy(&m_Event,event,sizeof(m_Event));
    return OK;
}
status_t CxEvent::Print(CFileBase *_buf)
{
    return OK;
}
int CxEvent::GetType()
{
    return m_Event.type;
}
uint32_t CxEvent::GetSerial()
{
    return m_Event.xkey.serial;
}
uint32_t CxEvent::GetSendEvent()
{
    return m_Event.xkey.send_event;
}
uint32_t CxEvent::GetWindow()
{
    return m_Event.xkey.window;
}
uint32_t CxEvent::GetRootWindow()
{
    return m_Event.xkey.root;
}

NativeXEvent* CxEvent::GetNativeXEvent()
{
    return &m_Event;
}

int CxEvent::Size()
{
    return sizeof(m_Event);
}
