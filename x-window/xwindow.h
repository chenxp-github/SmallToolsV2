#ifndef __S2_XWINDOW_H
#define __S2_XWINDOW_H

#include "cruntime.h"
#include "filebase.h"
#include "weak_pointer.h"
#include "xcommon.h"
#include "closure.h"
#include "mem.h"

#define WINDOW_FLAG_FUNC(func,bit) FLAG_FUNC(m_Flags,func,bit)

class CxDisplay;
class CxWindow{
public:
    WEAK_REF_DEFINE();
public:
    NativeXWindow m_NativeXWindow;
    uint32_t m_Flags;
    CWeakPointer<CxDisplay> m_Display;
    NativeXWindow m_RootWindow;
    NativeXWindow m_ParentWindow;
    NativeXWindow *m_ChildWindows;
    int m_ChildWindowsLen;
public:
    status_t SetNetWMName(const char *name);
    status_t GetNetWMName(CMem *name);
    status_t Move(int x, int y);
    int GetMapState();
    status_t StayAbove(bool above);
    status_t StayBelow(bool below);
    status_t SetFunctions(uint32_t functions);
    status_t SetDecorations(uint32_t decorations);
    status_t SetMotifWmHints(MotifWmHints *new_hints);
    status_t SetWindowType(const char *wm_type,bool on);
    status_t GetWindowProperty(const char *atom_name, int offset, int items, CMem *out);
    status_t Close();
    status_t GetWMName(CMem *name);
    status_t SetWMName(CMem *name);
    status_t FetchName(CMem *name);
    status_t StoreName(const char *name);
    status_t MapRaised();
    status_t MapWindow();
    status_t UnmapWindow();
    status_t WithdrawWindow();
    status_t FullScreen(bool fullscreen);
    status_t Minimize(bool minm);
    status_t Maximize(bool maxm);
    status_t GetTopLevelWindow(CxWindow *toplevel);
    static status_t PrintWindowTree(CxWindow *root);
    status_t SetNormalHints(XSizeHints *hints);
    status_t Lower();
    status_t Raise();
    status_t SetBounds(int x, int y, int width, int height);
    status_t GetBounds(int *x, int *y, int *width, int *height);
    status_t Hide();
    status_t Show();
    CxDisplay* GetDisplay();
    static status_t TraverseWindowTree(CxWindow *root,CClosure *func,int level=0);
    status_t SetDockMode(bool dock);
    status_t GetWindowAttributes(NativeXWindowAttributes *attributes);
    int GetChildWindowsLen();
    status_t GetChildWindow(int index, CxWindow *child);
    NativeXWindow GetChildWindow(int index);
    status_t Clear();
    status_t GetParentWindow(CxWindow *parent);
    NativeXWindow GetParentWindow();
    status_t GetRootWindow(CxWindow *root);
    NativeXWindow GetRootWindow();
    status_t UpdateWindowTree();
    status_t FreeChildWindows();
    status_t GetHostPid(pid_t *pid_ret);
    status_t SetDisplay(CxDisplay *display);
    CxWindow();
    virtual ~CxWindow();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CxWindow *_p);
    int Comp(CxWindow *_p);
    status_t Print(CFileBase *_buf);
    NativeXWindow GetNativeXWindow();
    status_t SetNativeXWindow(NativeXWindow _nativexwindow);
    status_t Attach(NativeXWindow _nativexwindow);

    WINDOW_FLAG_FUNC(IsAttached,0x00000001);
};

#endif
