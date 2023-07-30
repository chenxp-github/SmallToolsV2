#include "remote_desktop_snapshottor_gdi.h"
#include "sys_log.h"
#include "mem_tool.h"

CRemoteDesktopSnapshottor_GDI::CRemoteDesktopSnapshottor_GDI()
{
    this->InitBasic();
}

CRemoteDesktopSnapshottor_GDI::~CRemoteDesktopSnapshottor_GDI()
{
    this->Destroy();
}

status_t CRemoteDesktopSnapshottor_GDI::InitBasic()
{
/*##Begin InitBasic##*/
    CRemoteDesktopSnapshottor::InitBasic();
    this->m_hdc_to_take = 0;
    this->m_hdc_src = 0;
    this->m_hbmp_to_take = 0;
    this->m_src_origin_x = 0;
    this->m_src_origin_y = 0;
    this->m_src_width = 0;
    this->m_src_height = 0;
    this->m_dst_width = 0;
    this->m_dst_height = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopSnapshottor_GDI::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
/*##Begin Init##*/
    CRemoteDesktopSnapshottor::Init(_taskmgr);
/*##End Init##*/
	this->SetType(SNAPSHOTTOR_TYPE_GDI);
    return OK;
}

status_t CRemoteDesktopSnapshottor_GDI::Destroy()
{
/*##Begin Destroy##*/
    CRemoteDesktopSnapshottor::Destroy();
/*##End Destroy##*/

	this->ReleaseBitmapBuffer();

	if(m_hdc_to_take)
	{
		::DeleteDC(m_hdc_to_take);
		m_hdc_to_take = 0;
	}

    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopSnapshottor_GDI::Copy(CRemoteDesktopSnapshottor_GDI *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    return OK;
}

int CRemoteDesktopSnapshottor_GDI::Comp(CRemoteDesktopSnapshottor_GDI *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    return CRemoteDesktopSnapshottor::Comp(_p);
}

status_t CRemoteDesktopSnapshottor_GDI::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRemoteDesktopSnapshottor::Print(_buf);
    _buf->Log("hdc_to_take = %u",m_hdc_to_take);
    _buf->Log("hdc_src = %u",m_hdc_src);
    _buf->Log("hbmp_to_take = %u",m_hbmp_to_take);
    _buf->Log("src_origin_x = %d",m_src_origin_x);
    _buf->Log("src_origin_y = %d",m_src_origin_y);
    _buf->Log("src_width = %d",m_src_width);
    _buf->Log("src_height = %d",m_src_height);
    _buf->Log("dst_width = %d",m_dst_width);
    _buf->Log("dst_height = %d",m_dst_height);
/*##End Print##*/
    return OK;
}


/*@@Begin Function GetSrcOriginX@@*/
int CRemoteDesktopSnapshottor_GDI::GetSrcOriginX()
{
    return m_src_origin_x;
}
/*@@End  Function GetSrcOriginX@@*/

/*@@Begin Function GetSrcOriginY@@*/
int CRemoteDesktopSnapshottor_GDI::GetSrcOriginY()
{
    return m_src_origin_y;
}
/*@@End  Function GetSrcOriginY@@*/

/*@@Begin Function GetSrcWidth@@*/
int CRemoteDesktopSnapshottor_GDI::GetSrcWidth()
{
    return m_src_width;
}
/*@@End  Function GetSrcWidth@@*/

/*@@Begin Function GetSrcHeight@@*/
int CRemoteDesktopSnapshottor_GDI::GetSrcHeight()
{
    return m_src_height;
}
/*@@End  Function GetSrcHeight@@*/

/*@@Begin Function GetDstWidth@@*/
int CRemoteDesktopSnapshottor_GDI::GetDstWidth()
{
    return m_dst_width;
}
/*@@End  Function GetDstWidth@@*/

/*@@Begin Function GetDstHeight@@*/
int CRemoteDesktopSnapshottor_GDI::GetDstHeight()
{
    return m_dst_height;
}
/*@@End  Function GetDstHeight@@*/


status_t CRemoteDesktopSnapshottor_GDI::UpdateBltSize()
{
	int width = ::GetDeviceCaps(m_hdc_src, DESKTOPHORZRES);
    int height = ::GetDeviceCaps(m_hdc_src, DESKTOPVERTRES);
	m_src_width = width;
	m_src_height = height;
	m_dst_width = width;
	m_dst_height = height;
	return OK;
}

status_t CRemoteDesktopSnapshottor_GDI::CreateBitmapBuffer()
{
	if(m_hbmp_to_take)return OK;

	m_hbmp_to_take = ::CreateCompatibleBitmap(m_hdc_src, m_dst_width,m_dst_height);
	ASSERT(m_hbmp_to_take);
	
	m_hdc_to_take = ::CreateCompatibleDC(m_hdc_src);
	ASSERT(m_hdc_to_take);
	
	::SelectObject(m_hdc_to_take, m_hbmp_to_take);
	
	return OK;
}

status_t CRemoteDesktopSnapshottor_GDI::ReleaseBitmapBuffer()
{
	if(m_hbmp_to_take)
	{
		::DeleteObject(m_hbmp_to_take);
		m_hbmp_to_take = 0;
	}
	return OK;
}

/*@@Begin Function SetSrcOriginX@@*/
status_t CRemoteDesktopSnapshottor_GDI::SetSrcOriginX(int _src_origin_x)
{
    this->m_src_origin_x = _src_origin_x;
    return OK;
}
/*@@End  Function SetSrcOriginX@@*/

/*@@Begin Function SetSrcOriginY@@*/
status_t CRemoteDesktopSnapshottor_GDI::SetSrcOriginY(int _src_origin_y)
{
    this->m_src_origin_y = _src_origin_y;
    return OK;
}
/*@@End  Function SetSrcOriginY@@*/

/*@@Begin Function SetSrcWidth@@*/
status_t CRemoteDesktopSnapshottor_GDI::SetSrcWidth(int _src_width)
{
    this->m_src_width = _src_width;
    return OK;
}
/*@@End  Function SetSrcWidth@@*/

/*@@Begin Function SetSrcHeight@@*/
status_t CRemoteDesktopSnapshottor_GDI::SetSrcHeight(int _src_height)
{
    this->m_src_height = _src_height;
    return OK;
}
/*@@End  Function SetSrcHeight@@*/

/*@@Begin Function SetDstWidth@@*/
status_t CRemoteDesktopSnapshottor_GDI::SetDstWidth(int _dst_width)
{
    this->m_dst_width = _dst_width;
    return OK;
}
/*@@End  Function SetDstWidth@@*/

/*@@Begin Function SetDstHeight@@*/
status_t CRemoteDesktopSnapshottor_GDI::SetDstHeight(int _dst_height)
{
    this->m_dst_height = _dst_height;
    return OK;
}
/*@@End  Function SetDstHeight@@*/
/*@@ Insert Function Here @@*/
status_t CRemoteDesktopSnapshottor_GDI::TakeSnapshot(CRemoteDesktopPixelBuffer *pixelBuf)
{
	ASSERT(pixelBuf);

	int old_width = m_src_width;
	int old_height = m_src_height;

	this->UpdateBltSize();

	if(old_width != m_src_width || old_height !=m_src_height)
	{
		this->ReleaseBitmapBuffer();
		this->CreateBitmapBuffer();
	}

	::StretchBlt(m_hdc_to_take, 0, 0, m_dst_width, m_dst_height, 
		m_hdc_src, m_src_origin_x, m_src_origin_y,m_src_width,m_src_height,SRCCOPY);

	LOCAL_MEM(mem_info);
	mem_info.Zero();
	BITMAPINFO *info = (BITMAPINFO*)mem_info.GetRawBuf();
	info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	
	::GetDIBits(
		m_hdc_to_take, m_hbmp_to_take, 0,0,
		NULL, info, DIB_RGB_COLORS
	);

	pixelBuf->Create(info->bmiHeader.biWidth,info->bmiHeader.biHeight,info->bmiHeader.biBitCount/8);

	::GetDIBits(
		m_hdc_to_take, m_hbmp_to_take, 0,info->bmiHeader.biHeight,
		pixelBuf->GetPixelData(), info, DIB_RGB_COLORS
	);

	return OK;
}

status_t CRemoteDesktopSnapshottor_GDI::SetSrcHdc(HDC hdc)
{
	m_hdc_src = hdc;
	return OK;
}



