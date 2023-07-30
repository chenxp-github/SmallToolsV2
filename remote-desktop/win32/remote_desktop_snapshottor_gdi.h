#ifndef __REMOTE_DESKTOP_SNAPSHOTTOR_GDI_H
#define __REMOTE_DESKTOP_SNAPSHOTTOR_GDI_H

/*##Begin Inlcudes##*/
#include "remote_desktop_snapshottor.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"


class CRemoteDesktopSnapshottor_GDI
/*##Begin Bases##*/
:public CRemoteDesktopSnapshottor
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    HDC m_hdc_to_take;
    HDC m_hdc_src;
    HBITMAP m_hbmp_to_take;
    int m_src_origin_x;
    int m_src_origin_y;
    int m_src_width;
    int m_src_height;
    int m_dst_width;
    int m_dst_height;
/*##End Members##*/


public:
    CRemoteDesktopSnapshottor_GDI();
    virtual ~CRemoteDesktopSnapshottor_GDI();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

    status_t Copy(CRemoteDesktopSnapshottor_GDI *_p);
    int Comp(CRemoteDesktopSnapshottor_GDI *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetSrcOriginX();
    int GetSrcOriginY();
    int GetSrcWidth();
    int GetSrcHeight();
    int GetDstWidth();
    int GetDstHeight();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetSrcOriginX(int _src_origin_x);
    status_t SetSrcOriginY(int _src_origin_y);
    status_t SetSrcWidth(int _src_width);
    status_t SetSrcHeight(int _src_height);
    status_t SetDstWidth(int _dst_width);
    status_t SetDstHeight(int _dst_height);
/*##End Setter_H##*/
	status_t UpdateBltSize();
	status_t CreateBitmapBuffer();
	status_t ReleaseBitmapBuffer();
	status_t TakeSnapshot(CRemoteDesktopPixelBuffer *pixelBuf);
	status_t SetSrcHdc(HDC hdc);
};

#endif
