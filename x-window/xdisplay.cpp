#include "xdisplay.h"
#include "syslog.h"
#include "mem_tool.h"
#include "misc.h"

CxDisplay::CxDisplay()
{
    this->InitBasic();
}
CxDisplay::~CxDisplay()
{
    this->Destroy();
}
status_t CxDisplay::InitBasic()
{
    WEAK_REF_CLEAR();
    this->m_NativeXDisplay = 0;
    m_Flags = 0;
    return OK;
}
status_t CxDisplay::Init()
{
    this->InitBasic();    
    return OK;
}
status_t CxDisplay::Destroy()
{
    WEAK_REF_DESTROY();
    this->CloseDisplay();
    this->InitBasic();
    return OK;
}
int CxDisplay::Comp(CxDisplay *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CxDisplay::Copy(CxDisplay *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_NativeXDisplay = _p->m_NativeXDisplay;
    m_Flags = _p->m_Flags;
    return OK;
}
status_t CxDisplay::Print(CFileBase *_buf)
{
    _buf->Log("NativeXDisplay = %x",this->m_NativeXDisplay);
    return OK;
}
NativeXDisplay CxDisplay::GetNativeXDisplay()
{
    return this->m_NativeXDisplay;
}
status_t CxDisplay::SetNativeXDisplay(NativeXDisplay _nativexdisplay)
{
    this->m_NativeXDisplay = _nativexdisplay;
    return OK;
}

status_t CxDisplay::Attach(NativeXDisplay _nativexdisplay)
{
    ASSERT(_nativexdisplay);
    this->CloseDisplay();
    this->m_NativeXDisplay = _nativexdisplay;
    this->SetIsAttached(true);
    return OK;
}

status_t CxDisplay::OpenDisplay(const char *name)
{
    this->CloseDisplay();
    m_NativeXDisplay = XOpenDisplay(name);
    this->SetIsAttached(false);
    return m_NativeXDisplay != NULL;
}

status_t CxDisplay::CloseDisplay()
{
    if(m_NativeXDisplay)
    {
        if(!IsAttached())
        {
            XCloseDisplay(m_NativeXDisplay);
        }
        m_NativeXDisplay = NULL;
    }
    return OK;
}

NativeXWindow CxDisplay::GetDefaultRootWindow()
{
    ASSERT(m_NativeXDisplay);
    return DefaultRootWindow(m_NativeXDisplay);
}

status_t CxDisplay::GetDefaultRootWindow(CxWindow *window)
{
    ASSERT(window);
    window->Destroy();
    window->Init();

    NativeXWindow win = this->GetDefaultRootWindow();
    if(win)
    {
        window->SetDisplay(this);
        window->SetNativeXWindow(win);
        return OK;
    }
    return ERROR;
}

NativeXAtom CxDisplay::GetAtomPid()
{
    return this->GetAtomByName("_NET_WM_PID");
}

NativeXAtom CxDisplay::GetAtomByName(const char *name,bool only_if_exist)
{
    ASSERT(m_NativeXDisplay);
    return ::XInternAtom(m_NativeXDisplay, name,only_if_exist );
}

status_t CxDisplay::Flush()
{
    ASSERT(m_NativeXDisplay);
    XFlush(m_NativeXDisplay);
    return OK;
}

static int XErrorHandlerImpl(Display *display, XErrorEvent *event)
{
    char error_text[1024];

    XGetErrorText(display, event->error_code,error_text,sizeof(error_text));

    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
         "X error received: %s\n"
         "type:  %d\n"
         "serial: %d\n"
         "error_code: %d\n"
         "request_code: %d\n"
         "minor_code %d\n",
         error_text,
         event->type,
         event->serial,
         event->error_code,
         event->request_code,
         event->minor_code
    );
    return 0;
}

static int XIOErrorHandlerImpl(Display *display)
{
    return 0;
}

status_t CxDisplay::InstallDefaultErrorHandler()
{
    static bool installed = false;

    if(installed)return ERROR;
    installed = true;

    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);
    return OK;
}

int CxDisplay::DefaultScreenNumber()
{
    return XDefaultScreen(m_NativeXDisplay);
}

status_t CxDisplay::GetActiveWindow(CxWindow *act_win)
{
    ASSERT(m_NativeXDisplay);
    ASSERT(act_win);

    NativeXAtom actual_type_return;
    int actual_format_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    unsigned char *prop_return = NULL;

    int ret = XGetWindowProperty(
        m_NativeXDisplay,
        this->GetDefaultRootWindow(),
        this->GetAtomByName("_NET_ACTIVE_WINDOW"),
        0,1,
        false,
        XA_WINDOW,
        &actual_type_return,
        &actual_format_return,
        &nitems_return,
        &bytes_after_return,
        &prop_return
    );

    if(ret != Success)
        return ERROR;

    if(prop_return)
    {
        act_win->SetDisplay(this);
        act_win->SetNativeXWindow(*((NativeXWindow*)prop_return));
        XFree(prop_return);
        return OK;
    }

    return ERROR;

}

int CxDisplay::Pending()
{
    ASSERT(m_NativeXDisplay);
    return XPending(m_NativeXDisplay);
}

status_t CxDisplay::PeekEvent(NativeXEvent *event)
{
    ASSERT(m_NativeXDisplay);
    return XPeekEvent(m_NativeXDisplay,event);
}

status_t CxDisplay::NextEvent(NativeXEvent *event)
{
    ASSERT(m_NativeXDisplay);
    return XNextEvent(m_NativeXDisplay,event);
}
