#ifndef __REMOTE_DESKTOP_SNAPSHOTTOR_X11_H
#define __REMOTE_DESKTOP_SNAPSHOTTOR_X11_H

/*##Begin Inlcudes##*/
#include "remote_desktop_snapshottor.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "xdisplay.h"
#include "xwindow.h"
#include "remote_desktop_pixel_buffer.h"

class CRemoteDesktopSnapshottor_X11
/*##Begin Bases##*/
:public CRemoteDesktopSnapshottor
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_src_width;
    int m_src_height;
    CxDisplay m_xDisplay;
    CxWindow m_xWindow;
/*##End Members##*/

public:
    CRemoteDesktopSnapshottor_X11();
    virtual ~CRemoteDesktopSnapshottor_X11();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

    status_t Copy(CRemoteDesktopSnapshottor_X11 *_p);
    int Comp(CRemoteDesktopSnapshottor_X11 *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetSrcWidth();
    int GetSrcHeight();
    CxDisplay* GetXDisplay();
    CxWindow* GetXWindow();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetSrcWidth(int _src_width);
    status_t SetSrcHeight(int _src_height);
    status_t SetXDisplay(CxDisplay *_xdisplay);
    status_t SetXWindow(CxWindow *_xwindow);
/*##End Setter_H##*/

    virtual status_t TakeSnapshot(CRemoteDesktopPixelBuffer *pixelBuf) override;
};

#endif
