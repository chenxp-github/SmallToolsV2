// Canvas.cpp: implementation of the CMemDC class.
//
//////////////////////////////////////////////////////////////////////
#include "memdc.h"
#include "mem_tool.h"
#include "syslog.h"
#include "file.h"
#include "mem.h"

#define PI 3.14159265358979323846
#define HIMETRIC_INCH   2540    // HIMETRIC units per inch
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemDC::CMemDC()
{
    this->InitBasic();
}
CMemDC::~CMemDC()
{
    this->Destroy();
}
int CMemDC::InitBasic()
{
	WEAK_REF_ID_CLEAR();
    this->hparent = NULL;
    this->hdc = NULL;
    this->hbitmap = NULL;
    this->old_hbitmap = NULL;
    this->hpen = NULL;
    this->old_pen = NULL;
    this->hbrush = NULL;
    this->old_brush = NULL;
    this->hfont = NULL;
    this->old_font = NULL;
    this->offx = 0;
    this->offy = 0;
    this->left = 0;
    this->top = 0;
    this->width = 0;
    this->height = 0;
    this->is_attached_hdc = false;
    this->pbits = NULL;

    return OK;
}
int CMemDC::Init()
{
    this->InitBasic();
    //add your code
	WEAK_REF_ID_INIT();
    return OK;
}
int  CMemDC::Destroy()
{
    if(hbitmap)
    {
        SelectObject(hdc,old_hbitmap);
        DeleteObject(hbitmap);
        hbitmap=NULL;
    }
    if(hpen)
    {
        SelectObject(hdc,old_pen);
        DeleteObject(hpen);
        hpen=NULL;
    }
    if(hfont)
    {
        SelectObject(hdc,old_font);
        DeleteObject(hfont);
        hfont=NULL;
    }
    if(hbrush)
    {
        SelectObject(hdc,this->old_brush);
        DeleteObject(this->hbrush);
        this->hbrush = NULL;
    }
    if(hdc)
    {
        if(!this->is_attached_hdc)
            DeleteDC(hdc);
        hdc=NULL;
        this->is_attached_hdc = false;
    }
    
    this->InitBasic();
    
    return OK;
}
int  CMemDC::Copy(CMemDC *p)
{
    return OK;
}
int CMemDC::Comp(CMemDC *p)
{
    return 0;
}
int  CMemDC::Print()
{
    return TRUE;
}

int CMemDC::CreateCompatibleDC(HDC hdc)
{   
    this->hdc= ::CreateCompatibleDC(hdc);
    this->is_attached_hdc = false;

    return OK;
}

int CMemDC::SetPos(int l, int t)
{
    this->left = l;
    this->top  = t;
    
    return OK;   
}

int CMemDC::SetParent(HWND hp)
{
    if(this->is_attached_hdc)
        return ERROR;
    if(this->hparent)
        return ERROR;

    this->hparent = hp;
    
    HDC hgDC = GetDC(hp);
    this->CreateCompatibleDC(hgDC);
    ::ReleaseDC(hp,hgDC);

    return OK;
}

int CMemDC::Paste(HDC hdc_des)
{
    ::BitBlt(hdc_des,left,top,width,height,hdc,0,0,SRCCOPY);    
    return OK;
}

int CMemDC::Copy(HDC hdc_src)
{
    ::BitBlt(hdc,0,0,width,height,hdc_src,left,top,SRCCOPY);
    return OK;
}

int CMemDC::SetPixel(int x, int y, COLORREF color)
{
    return ::SetPixel(hdc,x-offx,y-offy,color);
}

COLORREF CMemDC::GetPixel(int x,int y)
{
    return ::GetPixel(hdc,x-offx,y-offy);
}
int CMemDC::SetStockFont(int fnObject)
{
    if(hfont)
    {
        SelectObject(hdc,old_font);
        DeleteObject(hfont);
        hfont=NULL;
    }
    hfont=(HFONT)GetStockObject(fnObject);
    old_font=(HFONT)SelectObject(hdc,hfont); 
    return OK;
}
int CMemDC::SetFont(LOGFONTW *logfont)
{
    if(hfont)
    {
        SelectObject(hdc,old_font);
        DeleteObject(hfont);
        hfont=NULL;
    }
    hfont=(HFONT)CreateFontIndirectW(logfont);
    old_font=(HFONT)SelectObject(hdc,hfont); 
    return OK;
}
int CMemDC::SetFontA(LOGFONTA *logfont)
{
    if(hfont)
    {
        SelectObject(hdc,old_font);
        DeleteObject(hfont);
        hfont=NULL;
    }
    hfont=(HFONT)CreateFontIndirectA(logfont);
    old_font=(HFONT)SelectObject(hdc,hfont); 
    return OK;
}
int CMemDC::SetTextColor(COLORREF color)
{
    ::SetTextColor(hdc,color);
    return OK;
}
int CMemDC::SetTextBkColor(COLORREF color)
{   
    ::SetBkColor(hdc,color);
    return OK;
}
int CMemDC::SetPen(int fnPenStyle,int nWidth,COLORREF crColor)
{
    if(hpen)
    {
        SelectObject(hdc,old_pen);
        DeleteObject(hpen);
        hpen=NULL;
    }   
    hpen=CreatePen(fnPenStyle,nWidth,crColor);
    old_pen=(HPEN)SelectObject(hdc,hpen);
    return OK;
}
int CMemDC::SetHatchBrush(int fnStyle,COLORREF clrref)
{
#if _WINCE_
    DWORD BS_HATCHED = BS_SOLID;
#endif
    return this->SetBrush(BS_HATCHED,clrref,fnStyle);   
}
int CMemDC::SetSolidBrush(COLORREF clrref)
{
    return this->SetBrush(BS_SOLID,clrref,0);
}
int CMemDC::TextOut(int x,int y,const WCHAR *str)
{       
    RECT r;

    r.left = -1000;
    r.right = 1000;
    r.top = -1000;
    r.bottom = 1000;

    return this->TextOutEx(x,y,&r,str);
}

int CMemDC::TextOutEx(int x,int y,RECT *r, const WCHAR *str)
{   
    r->left -= offx;
    r->right -= offx;
    r->top -= offy;
    r->bottom -= offy;

    ::ExtTextOutW(hdc,x-offx,y-offy,ETO_CLIPPED,r,str,crt_strlen_w(str),NULL);

    return OK;
}
int CMemDC::FillRect(int l,int t,int w,int h)
{   
    RECT r;

    r.left=l-offx;r.top=t-offy;
    r.right=l-offx+w;r.bottom=t-offy+h;

    if(w < 0) r.right ++;
    if(h < 0) r.bottom ++;

    ::FillRect(hdc,&r,this->hbrush);

    return OK;
}
int CMemDC::DrawRect(COLORREF color)
{
    this->SetPen(PS_SOLID,0,color); 
    this->DrawRect(offx,offy,width,height);
    
    return OK;
}
int CMemDC::DrawRect(int l,int t,int w,int h,COLORREF color)
{
    this->SetPen(PS_SOLID,0,color); 
    this->DrawRect(l,t,w,h);
    
    return OK;
}
int CMemDC::DrawRect(int l,int t,int w,int h)
{
    int r,b;
    
    r = l + w ;
    b = t + h;
    
    POINT p[5];
    
    this->MoveTo(r,t);

    p[0].x = r;p[0].y = t;
    p[1].x = l;p[1].y = t;
    p[2].x = l;p[2].y = b;
    p[3].x = r;p[3].y = b;
    p[4].x = r;p[4].y = t;

    PolylineTo(hdc,p,5);

    return OK;
}

int CMemDC::LoadBitMap(CFileBase *file)
{
    ASSERT(file);
    
    BITMAPFILEHEADER bmfh ;
    BITMAPINFO     * pbmi ;
    BYTE           * pBits ;
    DWORD            dwInfoSize;
    HBITMAP          hBitmap ;
    char __buf[2048];   //enough memory 
    
    file->Seek(0);
    file->Read(&bmfh,sizeof (BITMAPFILEHEADER));
    
    if(bmfh.bfType != * (WCHAR *) "BM")
        return ERROR;
            
    dwInfoSize = bmfh.bfOffBits - sizeof (BITMAPFILEHEADER) ;
    ASSERT(dwInfoSize < 2048);

    pbmi = (BITMAPINFO*)__buf;

    file->Read(pbmi,dwInfoSize);

    hBitmap = CreateDIBSection (NULL, pbmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0) ;
    ASSERT(hBitmap);

    file->Read(pBits,bmfh.bfSize - bmfh.bfOffBits); 

    this->height = pbmi->bmiHeader.biHeight;
    this->width = pbmi->bmiHeader.biWidth;
    
    if(this->hbitmap)
    {
        SelectObject(hdc,old_hbitmap);
        DeleteObject(this->hbitmap);
    }
    
    this->hbitmap = hBitmap;
    old_hbitmap=(HBITMAP)SelectObject(hdc,hbitmap);
    
    this->pbits = (char*)pBits;

    return OK;
}

int CMemDC::LineTo(int x,int y)
{
    return ::LineTo(hdc,x-offx,y-offy);
}
int CMemDC::Ellipse(int l,int t,int r,int b)
{
    return ::Ellipse(hdc,l-offx,t-offy,r-offx,b-offy);
}
int CMemDC::Arc(int nLeftRect,int nTopRect,int nRightRect,int nBottomRect,
                 int nXRadial1,int nYRadial1,int nXRadial2,int nYRadial2)
{

#if _WIN32_ || _WIN64_
    return ::Arc(this->hdc,nLeftRect - offx,nTopRect - offy, 
                 nRightRect - offx, nBottomRect - offy,
                 nXRadial1 - offx,nYRadial1 - offy,
                 nXRadial2 - offx,nYRadial2 - offy);
#endif

    return OK;
}
int CMemDC::MoveTo(int x,int y)
{
    return ::MoveToEx(hdc,x-offx,y-offy,NULL);    
}
int CMemDC::DrawToScreen()
{
    HDC h = GetDC(this->hparent);
    this->Paste(h);
    ReleaseDC (this->hparent,h);

    return OK;
}
int CMemDC::Clear()
{
    RECT r;

    r.left = 0; r.top = 0;
    r.right = this->width;
    r.bottom = this->height;

    return ::FillRect(this->hdc,&r,this->hbrush);
}
int CMemDC::StretchToDC(HDC hdc_des,int width,int height)
{
    StretchBlt(hdc_des,this->left,this->top,width,height,hdc,0,0,this->width,this->height,SRCCOPY);
    return OK;
}
int CMemDC::SaveBmp(const char *fn,int bits)
{
    CFile file;
    file.Init();
    ASSERT( file.OpenFile(fn,"wb+") );
    
    return this->SaveBmp(&file,bits);
}

int CMemDC::SetWindowPosX(int x)
{
    offx=x;
    return OK;
}
int CMemDC::SetWindowPosY(int y)
{
    offy=y;
    return OK;
}
int CMemDC::DrawText(const WCHAR *str,RECT *r,UINT dwDTFormat)
{
    r->left-=offx;
    r->right-=offx;
    r->top-=offy;
    r->bottom-=offy;

    ::DrawTextW(hdc,str,crt_strlen_w(str),r,dwDTFormat);

    return OK;
}

int CMemDC::AttachDC(HDC hdc)
{
    this->hdc = hdc;
    this->is_attached_hdc = true;

    return OK;
}
int CMemDC::SetTransparentBkMode()
{
    return ::SetBkMode(this->hdc,TRANSPARENT);
}
int CMemDC::UpdateParent()
{
    RECT r;

    r.left = this->left;
    r.top = this->top;
    r.right = r.left + this->width;
    r.bottom = r.top + this->height;

    ::InvalidateRect(this->hparent,&r,0);

    return OK;
}

int CMemDC::SetBrush(UINT style, COLORREF color, LONG hatch)
{
    if(hbrush)
    {
        SelectObject(hdc,old_brush);
        DeleteObject(hbrush);
        hbrush=NULL;
    }   

#if _WIN32_ || _WIN64_
    LOGBRUSH lb;

    lb.lbStyle = style;
    lb.lbColor = color;
    lb.lbHatch = hatch;
    hbrush = ::CreateBrushIndirect(&lb);
#endif

#if _WINCE_
    hbrush = ::CreateSolidBrush(color);
#endif

    old_brush=(HBRUSH)SelectObject(hdc,hbrush);
    return OK;
}
int CMemDC::SaveBmp(CFileBase *file,int bits)
{    
    ASSERT(file);
    file->SetSize(0);

    DWORD t;
    int w = this->width;
    int h = this->height;

    file->Putc('B');
    file->Putc('M');
    
    t = 54 + w*h*(bits >> 3);file->Write(&t,4); 
    t = 0; file->Write(&t,4);
    t = 0x36; file->Write(&t,4);
    t = 0x28; file->Write(&t,4);
    t = w; file->Write(&t,4);
    t = h; file->Write(&t,4);
    t = 1; file->Write(&t,2);
    t = bits;file->Write(&t,2);
    t = 0; file->Write(&t,4);
    t = (bits >> 3)*w*h; file->Write(&t,4);
    //BMP has 54 bytes header
    file->FillBlock(54 - file->GetOffset(),0);
    
    file->Write(this->pbits,w*h*(bits>>3));
    
    return OK;
}

int CMemDC::CreateBitmap(int w, int h,int bits)
{
    DWORD t;

    LOCAL_MEM(mem);
    mem.Putc('B');
    mem.Putc('M');
    
    t = 54 + w*h*(bits >> 3);mem.Write(&t,4);   
    t = 0; mem.Write(&t,4);
    t = 0x36; mem.Write(&t,4);
    t = 0x28; mem.Write(&t,4);
    t = w; mem.Write(&t,4);
    t = h; mem.Write(&t,4);
    t = 1; mem.Write(&t,2);
    t = bits;mem.Write(&t,2);
    t = 0; mem.Write(&t,4);
    t = (bits >> 3)*w*h; mem.Write(&t,4);
    //BMP has 54 bytes header
    mem.FillBlock(54 - mem.GetOffset(),0);

    return this->LoadBitMap(&mem);
}

COLORREF CMemDC::GetPixelColorRaw_24(int x, int y)
{
    ASSERT(this->pbits);

    y = this->height-y-1;
    ASSERT(x >= 0 && x < this->width);
    ASSERT(y >= 0 && y < this->height);
    
    int pos;

    BYTE *p = (BYTE*)this->pbits;
    pos = (x +y*width)*3;
    p += pos;
    
    return (p[2]) | (p[1]<<8) | (p[0]<<16);
}

int CMemDC::DrawRect(RECT r, COLORREF color)
{
    return this->DrawRect(r.left,r.top,r.right - r.left+1,r.bottom-r.top+1,color);
}
