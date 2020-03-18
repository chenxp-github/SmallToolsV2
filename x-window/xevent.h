#ifndef __S2_XEVENT_H
#define __S2_XEVENT_H

#include "cruntime.h"
#include "filebase.h"
#include "xcommon.h"

class CxEvent{
public:
    WEAK_REF_DEFINE();
public:
    NativeXEvent m_Event;
public:
    int Size();
    NativeXEvent* GetNativeXEvent();
    CxEvent();
    virtual ~CxEvent();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CxEvent *_p);
    status_t Copy(NativeXEvent *event);
    int Comp(CxEvent *_p);
    status_t Print(CFileBase *_buf);
    int GetType();
    uint32_t GetSerial();
    uint32_t GetSendEvent();
    uint32_t GetWindow();
    uint32_t GetRootWindow();
};

#endif
