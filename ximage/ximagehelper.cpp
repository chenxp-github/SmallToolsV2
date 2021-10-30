#include "ximagehelper.h"
#include "sys_log.h"
#include "ximagebmp.h"
#include "ximageico.h"
#include "ximagejpg.h"
#include "ximagepng.h"
#include "ximagegif.h"
#include "memfile.h"
#include "dirmgr.h"
#include "misc.h"

status_t CxImageHelper::LoadImage(CFileBase *file, CxImage *out)
{
    ASSERT(file && out);
    
    SAVE_WEAK_REF_ID(*out,w);
    out->DestroyAll();
    RESTORE_WEAK_REF_ID(*out,w);

    int32_t type = CxImage::GetTypeByFileHeader(file);
    if(type == CXIMAGE_FORMAT_BMP)
    {
        CxImageBMP bmp;
        if(bmp.Decode(file))
        {
            out->Transfer(&bmp,false);
            return OK;
        }
    }

    if(type == CXIMAGE_FORMAT_PNG)
    {
        CxImagePNG png;
        if(png.Decode(file))
        {
            out->Transfer(&png,false);
            return OK;
        }
    }

    if(type == CXIMAGE_FORMAT_JPG)
    {
        CxImageJPG jpg;
        if(jpg.Decode(file))
        {
            out->Transfer(&jpg,false);
            return OK;
        }
    }

    if(type == CXIMAGE_FORMAT_ICO)
    {
        CxImageICO ico;
        if(ico.Decode(file))
        {
            out->Transfer(&ico,false);
            return OK;
        }
    }

    return ERROR;
}

status_t CxImageHelper::LoadImage(const char *filename, CxImage *out)
{
    ASSERT(filename && out);
    CMemFile mf;
    mf.Init();
    ASSERT(mf.LoadFile(filename));
    return LoadImage(&mf,out);
}


status_t CxImageHelper::SaveImage(CxImage *img, const char *type, CFileBase *dst)
{
    ASSERT(img && dst);
    if(crt_stricmp(type,"bmp") == 0)
    {
        CxImageBMP bmp;
        bmp.Ghost(img);
        bmp.Encode(dst);
        return OK;
    }
    
    if(crt_stricmp(type,"png") == 0)
    {
        CxImagePNG png;
        png.Ghost(img);
        png.Encode(dst);
        return OK;
    }
    
    if(crt_stricmp(type,"gif") == 0)
    {
        CxImageGIF gif;
        gif.Ghost(img);
        gif.DecreaseBpp(8,true,NULL,0);
        gif.Encode(dst);            
        return OK;
    }

    if(crt_stricmp(type,"jpg") == 0 || crt_stricmp(type,"jpeg")==0)
    {
        CxImageJPG jpg;
        jpg.Ghost(img);
        jpg.Encode(dst);            
        return OK;
    }

    if(crt_stricmp(type,"ico")== 0)
    {
        CxImageICO ico;
        ico.Ghost(img);
        ico.Encode(dst,(BOOL)FALSE,0);          
        return OK;
    }

    return ERROR;
}

status_t CxImageHelper::SaveImage(CxImage *img, const char *filename)
{
    ASSERT(img && filename);

    LOCAL_MEM(ext);

    CDirMgr::GetFileName(filename,&ext,FN_EXT);

    CMemFile mf;
    mf.Init();
    if(SaveImage(img,ext.CStr(),&mf))
    {
        if(mf.WriteToFile(filename) > 0)
            return OK;
        else
            return ERROR;
    }
    else
    {
        return ERROR;
    }
}
status_t CxImageHelper::ImageToRawRgba(CxImage *img, CMem *rgba)
{
    ASSERT(img && rgba);

    int32_t w,h,tw,th,i,j,offset;
    RGBQUAD c;
    uint8_t *out;

    w = img->GetWidth();
    h = img->GetHeight();
    tw = next_pot(w);
    th = next_pot(h);

    rgba->Destroy();
    rgba->Init();
    rgba->Malloc(tw*th*4);
    rgba->Zero();
    
    offset = (tw-w)*4;
    out = ((uint8_t*)rgba->GetRawBuf()) + (th-h)*4*tw;

    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            c = img->GetPixelColor(i,j,1);
            *(out++) = c.rgbRed; 
            *(out++) = c.rgbGreen; 
            *(out++) = c.rgbBlue;
            *(out++) = c.rgbReserved; 
        }
        out += offset;
    }

    out = ((uint8_t*)rgba->GetRawBuf()) + (th-h)*4*tw;
    if(!img->AlphaIsValid())
    {
        for (j = 0; j < h; j++)
        {
            for (i = 0; i < w; i++)
            {
                out ++;
                out ++;
                out ++;
                *(out++) = 0xff;
            }
            out += offset;
        }       
    }

    rgba->SetSize(rgba->GetMaxSize());
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_WINDOWS
/////////////////////////////////////////////////////////////////////////////////////////////
#include "memdc.h"

status_t CxImageHelper::MeasureTextSize(const wchar_t *text, LOGFONTW *log_font, int *w, int *h)
{
    ASSERT(text && log_font && w &&h);
    HDC hdc = ::GetDC(0);
    HFONT hfont = ::CreateFontIndirectW(log_font);
    HFONT hold = (HFONT)::SelectObject(hdc,hfont);
    
    SIZE size;
    ::GetTextExtentPoint32W( hdc,text, crt_strlen_w(text),&size);
    *w = size.cx;
    *h = size.cy;
    ::SelectObject(hdc,hold);
    ::ReleaseDC(0,hdc);
    return OK;
}
status_t CxImageHelper::DrawString(CxImage *img, LOGFONTW *log_font, CRect *layout, COLORREF color, const wchar_t *text)
{
    CMemDC memDc;

    int w = img->GetWidth();
    int h = img->GetHeight();

    memDc.Init();
    memDc.SetParent(0); 
    memDc.CreateBitmap(w,h,24); 
    memDc.SetSolidBrush(RGB(0,0,0));
    memDc.SetFont(log_font);
    memDc.SetTransparentBkMode();
    memDc.SetTextColor(RGB(255,255,255));

    RECT r;
    r.left = layout->left;
    r.right = layout->right;
    r.top = layout->top;
    r.bottom = layout->bottom;

    memDc.DrawText(text,&r,DT_WORDBREAK|DT_NOPREFIX);
    
    RGBQUAD qc;
    COLORREF c;
    qc.rgbBlue = GetBValue(color);
    qc.rgbGreen = GetGValue(color);
    qc.rgbRed = GetRValue(color);

    for(int j = 0; j < h; j++)
    {
        for(int i = 0; i < w; i++)
        {
            c = memDc.GetPixel(i,j);
            if(c != 0)
            {
                qc.rgbReserved = GetBValue(c);
                img->SetPixelColor(i,h-j-1,qc,TRUE);
            }
        }
    }
    return OK;
}

status_t CxImageHelper::CreateImageFromHdc(
		CxImage *img,
		int nXOriginDest, int nYOriginDest, 
		int nWidthDest, int nHeightDest, 
		HDC hdcSrc, 
		int nXOriginSrc, int nYOriginSrc, 
		int nWidthSrc, int nHeightSrc)
{
	ASSERT(img);
	ASSERT(nWidthDest > 0 && nHeightDest > 0);
	ASSERT(nWidthSrc > 0 && nHeightSrc > 0);

	HBITMAP hbmWnd = ::CreateCompatibleBitmap(hdcSrc, nWidthSrc, nHeightSrc);
	ASSERT(hbmWnd);
	
	HDC hdcMem = ::CreateCompatibleDC(hdcSrc);
	ASSERT(hdcMem);

	::SelectObject(hdcMem, hbmWnd);
	::StretchBlt(hdcMem, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 
		hdcSrc, nXOriginSrc, nYOriginSrc,nWidthSrc,nHeightSrc,SRCCOPY);


	SAVE_WEAK_REF_ID(*img,w);
	img->DestroyAll();
	img->Create(nWidthDest,nHeightDest,24,CXIMAGE_FORMAT_BMP);
	RESTORE_WEAK_REF_ID(*img,w);

	::GetDIBits(hdcMem, hbmWnd, 0,nHeightDest,
		img->GetBits(0),
		(LPBITMAPINFO)img->pDib,
		DIB_RGB_COLORS);

	::DeleteDC(hdcMem);
	::DeleteObject(hbmWnd);
	return OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////////////////////

// order is left-top-right-bottom
status_t CxImageHelper::Convert9Png(CxImage *nineImg, CxImage *outImg, int *info, int *info_size)
{
    ASSERT(nineImg && outImg && info && info_size);
    
    int x,y,nw, nh;

    *info_size = 0;
    nw = nineImg->GetWidth();
    nh = nineImg->GetHeight();
    nineImg->Crop(1,1,nw-1,nh-1,outImg);

    int start,end;
    int k = 0;

    //left
    start = -1; end = -1;
    x = 0;
    for(y = nh-1; y >= 0; y--)
    {
        RGBQUAD c = nineImg->GetPixelColor(x,y,true);
        if(c.rgbReserved == 255)
        {
            if(start < 0) start = y;
            end = y;
        }
        else
        {
            if(start >= 0)
            {
                info[k++] = nh - start - 2;
                info[k++] = nh - end - 2;
                start = -1;
                end = -1;
            }
        }
    }
    info[k++] = -1;

    //top
    start = -1; end = -1;
    y = nh-1;
    for(x = 0; x < nw; x++)
    {
        RGBQUAD c = nineImg->GetPixelColor(x,y,true);
        if(c.rgbReserved == 255)
        {
            if(start < 0) start = x;
            end = x;
        }
        else
        {
            if(start >= 0)
            {
                info[k++] = start - 1;
                info[k++] = end - 1;
                start = -1;
                end = -1;
            }
        }
    }
    info[k++] = -1;

    //right
    start = -1; end = -1;
    x = nw - 1;
    for(y = nh-1; y >= 0; y--)
    {
        RGBQUAD c = nineImg->GetPixelColor(x,y,true);
        if(c.rgbReserved == 255)
        {
            if(start < 0) start = y;
            end = y;
        }
        else
        {
            if(start >= 0)
            {
                info[k++] = nh - start - 2;
                info[k++] = nh - end - 2;
                start = -1;
                end = -1;
            }
        }
    }
    info[k++] = -1;

    //bottom
    start = -1; end = -1;
    y = 0;
    for(x = 0; x < nw; x++)
    {
        RGBQUAD c = nineImg->GetPixelColor(x,y,true);
        if(c.rgbReserved == 255)
        {
            if(start < 0) start = x;
            end = x;
        }
        else
        {
            if(start >= 0)
            {
                info[k++] = start - 1;
                info[k++] = end - 1;
                start = -1;
                end = -1;
            }
        }
    }
    info[k++] = -1;
    *info_size = k;

    return OK;
}

static bool is_pixel_same(RGBQUAD c1, RGBQUAD c2, int threshold)
{
    int r = c1.rgbRed - c2.rgbRed;
    if(r < 0) r = -r;
    
    int g = c1.rgbGreen - c2.rgbGreen;
    if(g < 0) g = -g;
    
    int b = c1.rgbBlue - c2.rgbBlue;
    if(b < 0) b = -b;
    
    int t = r+g+b;
    
    return t < threshold;
}


float CxImageHelper::CalcuSimilarity(CxImage *img1, CxImage *img2, int threshold, CxImage *diff_img)
{
    ASSERT(img1 && img2);

    int w1 = img1->GetWidth();
    int h1 = img1->GetHeight();
    int w2 = img2->GetWidth();
    int h2 = img2->GetHeight();
    
    if(w1==0 || h1==0 || w2==0 || h2==0)
        return 0;
    
    if(w1 != w2 || h1 != h2)
        return 0;   

    if(diff_img)
    {
        if(w1 != diff_img->GetWidth() || h1 != diff_img->GetHeight())
        {
            SAVE_WEAK_REF_ID(*diff_img,w);
            diff_img->DestroyAll();
            diff_img->Init();
            diff_img->Create(w1,h1,24,CXIMAGE_FORMAT_BMP);
            RESTORE_WEAK_REF_ID(*diff_img,w);
        }
    }
    
    int same_pixel = 0;    
    
    for(int x = 0; x < w1; x++)
    {
        for(int y = 0; y < h1;  y++)
        {
            RGBQUAD c1 = img1->GetPixelColor(x,y,TRUE);
            RGBQUAD c2 = img2->GetPixelColor(x,y,TRUE);
            
            if(is_pixel_same(c1,c2,threshold))
            {
                if(diff_img)
                {
                    diff_img->SetPixelColor(x,y,RGB(0,0,0));
                }
                same_pixel ++;
            }
            else
            {
                if(diff_img)
                {
                    diff_img->SetPixelColor(x,y,RGB(255,0,0));                
                }
            }
        }
    }
    
    return (float)same_pixel / (float)(w1*h1);
}
