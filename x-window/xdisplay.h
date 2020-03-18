#ifndef __S2_XDISPLAY_H
#define __S2_XDISPLAY_H

#include "cruntime.h"
#include "filebase.h"
#include "xcommon.h"
#include "xwindow.h"

#define DISPLAY_FLAG_FUNC(func,bit) FLAG_FUNC(m_Flags,func,bit)

class CxDisplay{
public:
    WEAK_REF_DEFINE();
public:
    uint32_t m_Flags;
    NativeXDisplay m_NativeXDisplay;
public:
    status_t PeekEvent(NativeXEvent *event);
    status_t NextEvent(NativeXEvent *event);
    int Pending();
    status_t GetActiveWindow(CxWindow *act_win);
    int DefaultScreenNumber();
    static status_t InstallDefaultErrorHandler();
    status_t Flush();
    NativeXAtom GetAtomByName(const char *name,bool only_if_exist=True);
    NativeXAtom GetAtomPid();
    status_t GetDefaultRootWindow(CxWindow *window);
    NativeXWindow GetDefaultRootWindow();
    status_t CloseDisplay();
    status_t OpenDisplay(const char *name=NULL);
    CxDisplay();
    virtual ~CxDisplay();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CxDisplay *_p);
    int Comp(CxDisplay *_p);
    status_t Print(CFileBase *_buf);
    NativeXDisplay GetNativeXDisplay();
    status_t SetNativeXDisplay(NativeXDisplay _nativexdisplay);
    status_t Attach(NativeXDisplay _nativexdisplay);

    DISPLAY_FLAG_FUNC(IsAttached,0x00000001);
};

#endif
