#ifndef __S2_XEVENT_H
#define __S2_XEVENT_H

#include "cruntime.h"
#include "filebase.h"
#include "xcommon.h"

class CXEvent{
public:
    WEAK_REF_ID_DEFINE();
public:
    NativeXEvent m_Event;
public:
    int Size();
    NativeXEvent* GetNativeXEvent();
    CXEvent();
    virtual ~CXEvent();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CXEvent *_p);
    status_t Copy(NativeXEvent *event);
    int Comp(CXEvent *_p);
    status_t Print(CFileBase *_buf);
    int GetType();
    uint32_t GetSerial();
    uint32_t GetSendEvent();
    uint32_t GetWindow();
    uint32_t GetRootWindow();
};

#endif
