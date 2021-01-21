#include "xwindow.h"
#include "syslog.h"
#include "mem_tool.h"
#include "xdisplay.h"
#include "mem.h"

#define COMMON_XWINDOW_CHECK()  do{\
ASSERT(m_Display.IsValid());\
ASSERT(m_NativeXWindow);\
}while(0)\

CxWindow::CxWindow()
{
    this->InitBasic();
}
CxWindow::~CxWindow()
{
    this->Destroy();
}
status_t CxWindow::InitBasic()
{
    WEAK_REF_CLEAR();

    this->m_NativeXWindow = 0;
    m_Flags = 0;
    m_Display.InitBasic();

    m_RootWindow = 0;
    m_ParentWindow = 0;
    m_ChildWindows = NULL;
    m_ChildWindowsLen = 0;

    return OK;
}
status_t CxWindow::Init()
{
    this->InitBasic();
    return OK;
}
status_t CxWindow::Destroy()
{
    WEAK_REF_DESTROY();
    FreeChildWindows();
    m_Display.Destroy();
    this->InitBasic();
    return OK;
}
int CxWindow::Comp(CxWindow *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CxWindow::Copy(CxWindow *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->m_NativeXWindow = _p->m_NativeXWindow;
    m_Flags = _p->m_Flags;
    this->SetDisplay(_p->GetDisplay());
    return OK;
}
status_t CxWindow::Print(CFileBase *_buf)
{
    _buf->Log("NativeXWindow = %x",this->m_NativeXWindow);
    return OK;
}
NativeXWindow CxWindow::GetNativeXWindow()
{
    return this->m_NativeXWindow;
}
status_t CxWindow::SetNativeXWindow(NativeXWindow _nativexwindow)
{
    this->m_NativeXWindow = _nativexwindow;
    return OK;
}
status_t CxWindow::Attach(NativeXWindow _nativexwindow)
{
    this->m_NativeXWindow = _nativexwindow;
    this->SetIsAttached(true);
    return OK;
}

status_t CxWindow::SetDisplay(CxDisplay *display)
{
    ASSERT(display);
    m_Display.WeakRef(display);
    return OK;
}

status_t CxWindow::GetHostPid(pid_t *pid_ret)
{
    COMMON_XWINDOW_CHECK();
    ASSERT(pid_ret);
    *pid_ret = 0;

    NativeXAtom actual_type_return;
    int actual_format_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    unsigned char *prop_return = NULL;

    int ret = XGetWindowProperty(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
        m_Display->GetAtomPid(),
        0,
        1,
        false,
        XA_CARDINAL,
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
        pid_t pid = *((pid_t *)prop_return);
        XFree(prop_return);
        *pid_ret = pid;
        return OK;
    }

    return ERROR;
}

status_t CxWindow::FreeChildWindows()
{
    if(m_ChildWindows)
    {
        XFree(m_ChildWindows);
        m_ChildWindows = NULL;
        m_ChildWindowsLen = 0;
    }
    return OK;
}

status_t CxWindow::UpdateWindowTree()
{
    COMMON_XWINDOW_CHECK();

    this->FreeChildWindows();

    return XQueryTree(m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
        &m_RootWindow,
        &m_ParentWindow,
        &m_ChildWindows,
        (uint32_t*)&m_ChildWindowsLen
    ) != 0;
}

NativeXWindow CxWindow::GetParentWindow()
{
    return m_ParentWindow;
}

status_t CxWindow::GetParentWindow(CxWindow *parent)
{
    ASSERT(parent);
    parent->Clear();
    parent->Attach(m_ParentWindow);
    parent->SetDisplay(m_Display.get());
    return OK;
}

status_t CxWindow::Clear()
{
    this->Destroy();
    this->Init();
    return OK;
}

NativeXWindow CxWindow::GetRootWindow()
{
    return m_RootWindow;
}

status_t CxWindow::GetRootWindow(CxWindow *root)
{
    ASSERT(root);
    root->Clear();
    root->Attach(m_RootWindow);
    root->SetDisplay(m_Display.get());
    return OK;
}

NativeXWindow CxWindow::GetChildWindow(int index)
{
    if(!m_ChildWindows)
        return 0;
    if(index < 0 || index >= (int)m_ChildWindowsLen)
        return 0;
    return m_ChildWindows[index];
}

status_t CxWindow::GetChildWindow(int index, CxWindow *child)
{
    ASSERT(child);
    child->Clear();
    NativeXWindow win = this->GetChildWindow(index);

    if(win)
    {
        child->Attach(win);
        child->SetDisplay(m_Display.get());
    }
    return OK;
}

int CxWindow::GetChildWindowsLen()
{
    return m_ChildWindowsLen;
}

status_t CxWindow::GetWindowAttributes(NativeXWindowAttributes *attributes)
{
    COMMON_XWINDOW_CHECK();
    ASSERT(attributes);

    memset(attributes,0,sizeof(NativeXWindowAttributes));
    return XGetWindowAttributes(m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
        attributes) != 0;
}

status_t CxWindow::SetWindowType(const char *wm_type,bool on)
{
    ASSERT(wm_type && m_Display.IsValid());

    NativeXAtom key_type = m_Display->GetAtomByName("_NET_WM_WINDOW_TYPE");

    NativeXAtom key_dock = 0;

    if(on)
    {
        key_dock = m_Display->GetAtomByName(wm_type);
    }

    status_t ret = XChangeProperty (
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
        key_type,
        XA_ATOM, 32, PropModeReplace,
        (guchar *)&key_dock,
        1
    );

    m_Display->Flush();

    return ret;
}

status_t CxWindow::SetDockMode(bool dock)
{
    return this->SetWindowType("_NET_WM_WINDOW_TYPE_DOCK",dock);
}

status_t CxWindow::TraverseWindowTree(CxWindow *root, CClosure *func, int level)
{
    ASSERT(root && func);

    root->UpdateWindowTree();

    func->SetParamPointer(1,root);
    func->SetParamInt(2,level);
    if(!func->Run())
        return FALSE;

    for(int i = 0; i < root->GetChildWindowsLen(); i++)
    {
        CxWindow tmp;
        root->GetChildWindow(i,&tmp);
        if(!CxWindow::TraverseWindowTree(&tmp,func,level+1))
            return FALSE;
    }
    return TRUE;
}

CxDisplay* CxWindow::GetDisplay()
{
    return m_Display.get();
}

status_t CxWindow::MapWindow()
{
    COMMON_XWINDOW_CHECK();

    if(XMapWindow(m_Display->GetNativeXDisplay(), m_NativeXWindow))
    {
        m_Display->Flush();
        return OK;
    }
    return ERROR;
}

status_t CxWindow::MapRaised()
{
    COMMON_XWINDOW_CHECK();

    if(XMapRaised(m_Display->GetNativeXDisplay(), m_NativeXWindow))
    {
        m_Display->Flush();
        return OK;
    }
    return ERROR;
}

status_t CxWindow::UnmapWindow()
{
    COMMON_XWINDOW_CHECK();

    if(XUnmapWindow(m_Display->GetNativeXDisplay(), m_NativeXWindow))
    {
        m_Display->Flush();
        return OK;
    }
    return ERROR;
}

status_t CxWindow::WithdrawWindow()
{
    COMMON_XWINDOW_CHECK();

    if(XWithdrawWindow(m_Display->GetNativeXDisplay(), m_NativeXWindow,m_Display->DefaultScreenNumber()))
    {
        m_Display->Flush();
        return OK;
    }
    return ERROR;
}

status_t CxWindow::Show()
{
    return this->MapWindow();
}

status_t CxWindow::Hide()
{
    return this->WithdrawWindow();
}

status_t CxWindow::GetBounds(int *x, int *y, int *width, int *height)
{
    COMMON_XWINDOW_CHECK();
    ASSERT(x && y && width && height);

    NativeXWindowAttributes attr;
    this->GetWindowAttributes(&attr);

    *x = attr.x;
    *y = attr.y;
    *width = attr.width;
    *height = attr.height;

    return OK;
}

status_t CxWindow::SetBounds(int x, int y, int width, int height)
{
    COMMON_XWINDOW_CHECK();
    return XMoveResizeWindow(
        m_Display->GetNativeXDisplay() ,
        m_NativeXWindow,
        x,y,width,height
    );
}

status_t CxWindow::Move(int x, int y)
{
    COMMON_XWINDOW_CHECK();
    return XMoveWindow(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
        x,y);
}

status_t CxWindow::Raise()
{
    COMMON_XWINDOW_CHECK();

    status_t ret = XRaiseWindow(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow
    );

    XEvent event = { 0 };
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = m_Display->GetAtomByName("_NET_ACTIVE_WINDOW",False);
    event.xclient.window = m_NativeXWindow;
    event.xclient.format = 32;

    XSendEvent(
        m_Display->GetNativeXDisplay(),
        m_Display->GetDefaultRootWindow(),False,
        SubstructureRedirectMask | SubstructureNotifyMask,
        &event
    );

    m_Display->Flush();
    return ret;
}

status_t CxWindow::Lower()
{
    COMMON_XWINDOW_CHECK();
    status_t ret = XLowerWindow(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow
    );
    m_Display->Flush();
    return ret;
}

status_t CxWindow::SetNormalHints(XSizeHints *hints)
{
    ASSERT(hints);
    COMMON_XWINDOW_CHECK();

    return XSetNormalHints(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
        hints
    );
}

status_t CxWindow::PrintWindowTree(CxWindow *root)
{
    ASSERT(root);

    BEGIN_CLOSURE(func)
    {
        CLOSURE_PARAM_PTR(CxWindow*,win,1);
        CLOSURE_PARAM_INT(level,2);

        LOCAL_MEM(buf);

        for(int i = 0 ; i < level; i++)
        {
            buf.Puts("    ");
        }

        int x,y,w,h;
        win->GetBounds(&x,&y,&w,&h);

        LOCAL_MEM(name);
        win->FetchName(&name);

        LOCAL_MEM(wm_name);
        win->GetWMName(&wm_name);

        pid_t pid = 0;
        win->GetHostPid(&pid);

        buf.Printf("0x%x,[%d,%d,%d,%d],%d,%s,%s",
            win->GetNativeXWindow(),
            x,y,w,h,
            pid,
            name.CStr(),wm_name.CStr()
        );

        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "%s",buf.CStr()
        );
        return TRUE;
    }
    END_CLOSURE(func);

    CxWindow::TraverseWindowTree(root,&func);

    return OK;
}

status_t CxWindow::GetTopLevelWindow(CxWindow *toplevel)
{
    ASSERT(toplevel);
    COMMON_XWINDOW_CHECK();

    toplevel->Clear();
    this->UpdateWindowTree();
    NativeXWindow root = m_Display->GetDefaultRootWindow();

    CxWindow win;
    win.Init();
    win.Copy(this);

    while(win.GetNativeXWindow())
    {
        CxWindow parent;
        win.UpdateWindowTree();
        win.GetParentWindow(&parent);
        if(parent.GetNativeXWindow() == root)
        {
            toplevel->Copy(&win);
            return OK;
        }
        win.Copy(&parent);
    }

    return ERROR;
}

status_t CxWindow::Maximize(bool maxm)
{
    COMMON_XWINDOW_CHECK();

    NativeXAtom vertical_maximize = m_Display->GetAtomByName("_NET_WM_STATE_MAXIMIZED_VERT");
    NativeXAtom horizon_maximize = m_Display->GetAtomByName("_NET_WM_STATE_MAXIMIZED_HORZ");
    NativeXAtom wm_state = m_Display->GetAtomByName("_NET_WM_STATE");

    XClientMessageEvent clientEvent = {0};
    clientEvent.type = ClientMessage;
    clientEvent.window = m_NativeXWindow;
    clientEvent.message_type = wm_state;
    clientEvent.format = 32;
    clientEvent.data.l[0] = maxm;
    clientEvent.data.l[1] = vertical_maximize;
    clientEvent.data.l[2] = horizon_maximize;
    clientEvent.data.l[3] = 1; /* source indication */
    clientEvent.data.l[4] = 0;

    XSendEvent (m_Display->GetNativeXDisplay(),
        m_Display->GetDefaultRootWindow(), 0,
        SubstructureRedirectMask | SubstructureNotifyMask,
        (XEvent *)&clientEvent
    );

    m_Display->Flush();
    return OK;
}

status_t CxWindow::Minimize(bool minm)
{
    COMMON_XWINDOW_CHECK();
    if(minm)
    {
        XIconifyWindow(m_Display->GetNativeXDisplay(),
            m_NativeXWindow, m_Display->DefaultScreenNumber());
        m_Display->Flush();
    }
    else
    {
        this->Raise();
    }

    return OK;
}

status_t CxWindow::FullScreen(bool fullscreen)
{
    COMMON_XWINDOW_CHECK();

    NativeXAtom wm_fullscreen = m_Display->GetAtomByName("_NET_WM_STATE_FULLSCREEN");
    NativeXAtom wm_state = m_Display->GetAtomByName("_NET_WM_STATE");

    XClientMessageEvent clientEvent = {0};
    clientEvent.type = ClientMessage;
    clientEvent.window = m_NativeXWindow;
    clientEvent.message_type = wm_state;
    clientEvent.format = 32;
    clientEvent.data.l[0] = fullscreen;
    clientEvent.data.l[1] = wm_fullscreen;
    clientEvent.data.l[2] = 0;
    clientEvent.data.l[3] = 1;
    clientEvent.data.l[4] = 0;

    XSendEvent (m_Display->GetNativeXDisplay(),
        m_Display->GetDefaultRootWindow(), 0,
        SubstructureRedirectMask | SubstructureNotifyMask,
        (XEvent *)&clientEvent
    );

    m_Display->Flush();

    return OK;
}

status_t CxWindow::StoreName(const char *name)
{
    COMMON_XWINDOW_CHECK();
    status_t ret = XStoreName(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow, name
    );
    m_Display->Flush();
    return ret;
}

status_t CxWindow::FetchName(CMem *name)
{
    ASSERT(name);
    name->SetSize(0);

    COMMON_XWINDOW_CHECK();

    char *pname = NULL;
    status_t ret = XFetchName(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow, &pname
    );

    if(ret && pname)
    {
        name->StrCpy(pname);
        XFree(pname);
        return ret;
    }

    return ERROR;
}

status_t CxWindow::GetWMName(CMem *name)
{
    ASSERT(name);
    name->SetSize(0);

    COMMON_XWINDOW_CHECK();

    XTextProperty text_property;
    memset(&text_property,0,sizeof(text_property));

    status_t ret = XGetWMName(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow, &text_property
    );

    if(ret && text_property.nitems > 0)
    {
        name->Write(text_property.value,text_property.nitems);
        XFree(text_property.value);
        return ret;
    }

    return ERROR;
}

status_t CxWindow::Close()
{
    COMMON_XWINDOW_CHECK();

    XClientMessageEvent clientEvent = {0};
    clientEvent.type = ClientMessage;
    clientEvent.window = m_NativeXWindow;
    clientEvent.message_type = m_Display->GetAtomByName("_NET_CLOSE_WINDOW");;
    clientEvent.format = 32;
    clientEvent.data.l[0] = 0;
    clientEvent.data.l[1] = 0;
    clientEvent.data.l[2] = 0;
    clientEvent.data.l[3] = 0;
    clientEvent.data.l[4] = 0;

    XSendEvent (m_Display->GetNativeXDisplay(),
        m_Display->GetDefaultRootWindow(), 0,
        SubstructureRedirectMask | SubstructureNotifyMask,
        (XEvent *)&clientEvent
    );

    m_Display->Flush();

    return OK;
}

status_t CxWindow::GetWindowProperty(const char *atom_name, int offset, int items, CMem *out)
{
    ASSERT(atom_name && out);

    NativeXAtom actual_type_return;
    int actual_format_return;
    unsigned long nitems_return;
    unsigned long bytes_after_return;
    unsigned char *prop_return = NULL;

    out->SetSize(0);

    int ret = XGetWindowProperty(
        m_Display->GetNativeXDisplay(),
        this->GetNativeXWindow(),
        m_Display->GetAtomByName(atom_name),
        offset,items,
        false,
        AnyPropertyType,
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
        int size = actual_format_return/8*nitems_return;
        ASSERT(out->GetMaxSize() >= size);
        out->Write(prop_return,size);
        out->Seek(0);
        XFree(prop_return);
        return OK;
    }

    return ERROR;

}

status_t CxWindow::SetMotifWmHints(MotifWmHints *new_hints)
{
    ASSERT(new_hints && m_Display.IsValid());

    NativeXAtom hints_atom = m_Display->GetAtomByName("_MOTIF_WM_HINTS");
    guchar *data;
    MotifWmHints *hints;
    NativeXAtom type;
    gint format;
    gulong nitems;
    gulong bytes_after;

    XGetWindowProperty (
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
  		hints_atom, 0, sizeof (MotifWmHints)/sizeof (int32_t),
        False, AnyPropertyType, &type, &format, &nitems,
        &bytes_after, &data
    );

    if (type == None)
    {
        hints = new_hints;
    }
    else
    {
        hints = (MotifWmHints *)data;
        if (new_hints->flags & MWM_HINTS_FUNCTIONS)
        {
            hints->flags |= MWM_HINTS_FUNCTIONS;
            hints->functions = new_hints->functions;
        }

        if (new_hints->flags & MWM_HINTS_DECORATIONS)
        {
            hints->flags |= MWM_HINTS_DECORATIONS;
            hints->decorations = new_hints->decorations;
        }

    }

    status_t ret = XChangeProperty (
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow,
        hints_atom, hints_atom, 32, PropModeReplace,
        (guchar *)hints,sizeof(MotifWmHints)/sizeof(uint32_t)
    );

    if (hints != new_hints)
       XFree (hints);

    m_Display->Flush();

    return ret;
}

status_t CxWindow::SetDecorations(uint32_t decorations)
{
    MotifWmHints hints;
    memset(&hints, 0, sizeof(hints));
    hints.flags = MWM_HINTS_DECORATIONS;
    hints.decorations = decorations;
    return this->SetMotifWmHints(&hints);
}

status_t CxWindow::SetFunctions(uint32_t functions)
{
    MotifWmHints hints;
    memset(&hints, 0, sizeof(hints));
    hints.flags = MWM_HINTS_FUNCTIONS;
    hints.functions = functions;
    return this->SetMotifWmHints(&hints);
}

status_t CxWindow::StayAbove(bool above)
{
    COMMON_XWINDOW_CHECK();

    NativeXAtom wm_above = m_Display->GetAtomByName("_NET_WM_STATE_ABOVE");
    NativeXAtom wm_state = m_Display->GetAtomByName("_NET_WM_STATE");

    XClientMessageEvent clientEvent = {0};
    clientEvent.type = ClientMessage;
    clientEvent.window = m_NativeXWindow;
    clientEvent.message_type = wm_state;
    clientEvent.format = 32;
    clientEvent.data.l[0] = above;
    clientEvent.data.l[1] = wm_above;
    clientEvent.data.l[2] = 0;
    clientEvent.data.l[3] = 1;
    clientEvent.data.l[4] = 0;

    XSendEvent (m_Display->GetNativeXDisplay(),
        m_Display->GetDefaultRootWindow(), 0,
        SubstructureRedirectMask | SubstructureNotifyMask,
        (XEvent *)&clientEvent
    );

    m_Display->Flush();

    return OK;
}

status_t CxWindow::StayBelow(bool below)
{
    COMMON_XWINDOW_CHECK();

    NativeXAtom wm_below = m_Display->GetAtomByName("_NET_WM_STATE_BELOW");
    NativeXAtom wm_state = m_Display->GetAtomByName("_NET_WM_STATE");

    XClientMessageEvent clientEvent = {0};
    clientEvent.type = ClientMessage;
    clientEvent.window = m_NativeXWindow;
    clientEvent.message_type = wm_state;
    clientEvent.format = 32;
    clientEvent.data.l[0] = below;
    clientEvent.data.l[1] = wm_below;
    clientEvent.data.l[2] = 0;
    clientEvent.data.l[3] = 1;
    clientEvent.data.l[4] = 0;

    XSendEvent (m_Display->GetNativeXDisplay(),
        m_Display->GetDefaultRootWindow(), 0,
        SubstructureRedirectMask | SubstructureNotifyMask,
        (XEvent *)&clientEvent
    );

    m_Display->Flush();

    return OK;
}

int CxWindow::GetMapState()
{
    COMMON_XWINDOW_CHECK();
    NativeXWindowAttributes attrs;
    this->GetWindowAttributes(&attrs);
    return attrs.map_state;
}
status_t CxWindow::SetWMName(CMem *name)
{
    ASSERT(name);
    COMMON_XWINDOW_CHECK();

    XTextProperty text_property;
    memset(&text_property,0,sizeof(text_property));

	text_property.value = (unsigned char*)name->CStr();
	text_property.encoding = XA_STRING;
	text_property.format = 8;
	text_property.nitems = name->StrLen();

    XSetWMName(
        m_Display->GetNativeXDisplay(),
        m_NativeXWindow, &text_property
    );

    m_Display->Flush();
    return OK;
}

status_t CxWindow::SetNetWMName(const char *name)
{    
    ASSERT(name);

    COMMON_XWINDOW_CHECK();

    NativeXDisplay display = m_Display->GetNativeXDisplay();
    NativeXWindow win = this->GetNativeXWindow();

    XChangeProperty(display , win ,
        XInternAtom(display, "_NET_WM_NAME", False),
        XInternAtom(display, "UTF8_STRING", False),
        8, PropModeReplace, (unsigned char *) name,strlen(name)
    );

    m_Display->Flush();
    return OK;
}

status_t CxWindow::GetNetWMName(CMem *name)
{
    ASSERT(name);
    this->GetWindowProperty("_NET_WM_NAME",0,256,name);
    return OK;
}
