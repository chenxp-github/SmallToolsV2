#include "remote_desktop_snapshottor_x11.h"
#include "sys_log.h"
#include "mem_tool.h"

CRemoteDesktopSnapshottor_X11::CRemoteDesktopSnapshottor_X11()
{
    this->InitBasic();
}

CRemoteDesktopSnapshottor_X11::~CRemoteDesktopSnapshottor_X11()
{
    this->Destroy();
}

status_t CRemoteDesktopSnapshottor_X11::InitBasic()
{
/*##Begin InitBasic##*/
    CRemoteDesktopSnapshottor::InitBasic();
    this->m_src_width = 0;
    this->m_src_height = 0;
    this->m_xDisplay.InitBasic();
    this->m_xWindow.InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopSnapshottor_X11::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
/*##Begin Init##*/
    this->m_xDisplay.Init();
    this->m_xWindow.Init();
/*##End Init##*/
    CRemoteDesktopSnapshottor::Init(_taskmgr);
    this->SetType(SNAPSHOTTOR_TYPE_X11);
    return OK;
}

status_t CRemoteDesktopSnapshottor_X11::Destroy()
{
/*##Begin Destroy##*/
    CRemoteDesktopSnapshottor::Destroy();
    this->m_xDisplay.Destroy();
    this->m_xWindow.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopSnapshottor_X11::Copy(CRemoteDesktopSnapshottor_X11 *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRemoteDesktopSnapshottor::Copy(_p);
    this->m_src_width = _p->m_src_width;
    this->m_src_height = _p->m_src_height;
    this->m_xDisplay.Copy(&_p->m_xDisplay);
    this->m_xWindow.Copy(&_p->m_xWindow);
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopSnapshottor_X11::Comp(CRemoteDesktopSnapshottor_X11 *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopSnapshottor_X11::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRemoteDesktopSnapshottor::Print(_buf);
    _buf->Log("src_width = %d",m_src_width);
    _buf->Log("src_height = %d",m_src_height);
    _buf->Log("xDisplay = {");
    _buf->IncLogLevel(1);
    this->m_xDisplay.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("xWindow = {");
    _buf->IncLogLevel(1);
    this->m_xWindow.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
/*##End Print##*/
    return OK;
}


/*@@Begin Function GetSrcWidth@@*/
int CRemoteDesktopSnapshottor_X11::GetSrcWidth()
{
    return m_src_width;
}
/*@@End  Function GetSrcWidth@@*/

/*@@Begin Function GetSrcHeight@@*/
int CRemoteDesktopSnapshottor_X11::GetSrcHeight()
{
    return m_src_height;
}
/*@@End  Function GetSrcHeight@@*/

/*@@Begin Function SetSrcWidth@@*/
status_t CRemoteDesktopSnapshottor_X11::SetSrcWidth(int _src_width)
{
    this->m_src_width = _src_width;
    return OK;
}
/*@@End  Function SetSrcWidth@@*/

/*@@Begin Function SetSrcHeight@@*/
status_t CRemoteDesktopSnapshottor_X11::SetSrcHeight(int _src_height)
{
    this->m_src_height = _src_height;
    return OK;
}
/*@@End  Function SetSrcHeight@@*/

/*@@Begin Function GetXDisplay@@*/
CxDisplay* CRemoteDesktopSnapshottor_X11::GetXDisplay()
{
    return &m_xDisplay;
}
/*@@End  Function GetXDisplay@@*/

/*@@Begin Function GetXWindow@@*/
CxWindow* CRemoteDesktopSnapshottor_X11::GetXWindow()
{
    return &m_xWindow;
}
/*@@End  Function GetXWindow@@*/

/*@@Begin Function SetXDisplay@@*/
status_t CRemoteDesktopSnapshottor_X11::SetXDisplay(CxDisplay *_xdisplay)
{
    ASSERT(_xdisplay);
    return this->m_xDisplay.Attach(_xdisplay->GetNativeXDisplay());
}
/*@@End  Function SetXDisplay@@*/

/*@@Begin Function SetXWindow@@*/
status_t CRemoteDesktopSnapshottor_X11::SetXWindow(CxWindow *_xwindow)
{
    ASSERT(_xwindow);
    this->m_xWindow.Copy(_xwindow);
    this->m_xWindow.SetIsAttached(true);
    return OK;
}
/*@@End  Function SetXWindow@@*/
/*@@ Insert Function Here @@*/

status_t CRemoteDesktopSnapshottor_X11::TakeSnapshot(CRemoteDesktopPixelBuffer *pixelBuf)
{
    ASSERT(pixelBuf);

    NativeXDisplay display = m_xDisplay.GetNativeXDisplay();
    NativeXWindow  window = m_xWindow.GetNativeXWindow();
    ASSERT(display && window);

    NativeXWindowAttributes attrs;
    memset(&attrs,0,sizeof(attrs));
    m_xWindow.GetWindowAttributes(&attrs);

    if(attrs.width <= 0 || attrs.height <= 0)
        return ERROR;
    
    m_src_width = attrs.width;
    m_src_height = attrs.height;

    XImage *img = XGetImage(display, window, 0, 0 , m_src_width, m_src_height, AllPlanes, ZPixmap);
    ASSERT(img);

    pixelBuf->Create(img->width,img->height,3);

    unsigned long red_mask = img->red_mask;
    unsigned long green_mask = img->green_mask;
    unsigned long blue_mask = img->blue_mask;

    int w = img->width;
    int h = img->height;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            unsigned long pixel = XGetPixel(img, x, h-y-1);
            uint8_t blue = pixel & blue_mask;
            uint8_t green = (pixel & green_mask) >> 8;
            uint8_t red = (pixel & red_mask) >> 16;
            uint8_t *p = pixelBuf->GetPixelBits_Unsafe(x,y);
            p[0] = red;p[1] = green;p[2] = blue;
        }
    }

    XDestroyImage(img);

    return OK;
}

