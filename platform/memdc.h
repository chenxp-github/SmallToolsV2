// Canvas.h: interface for the CMemDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMDC_H__31413FAA_DC0C_4694_AF72_F7B77FEA8316__INCLUDED_)
#define AFX_MEMDC_H__31413FAA_DC0C_4694_AF72_F7B77FEA8316__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"
#include <OLECTL.H>
#include "filebase.h"

class CMemDC{
public:
	WEAK_REF_ID_DEFINE();
public:
    HWND hparent;
    HDC hdc;
    HBITMAP hbitmap,old_hbitmap;
    HPEN hpen,old_pen;
    HBRUSH hbrush,old_brush;
    HFONT hfont,old_font;
    int offx,offy;
    int left,top,width,height;
    bool is_attached_hdc;
    char *pbits;
public:
    int DrawRect(RECT r,COLORREF color);
    COLORREF GetPixelColorRaw_24(int x,int y);
    int CreateBitmap(int w,int h,int bits);
    int LoadBitMap(CFileBase *file);
    int SaveBmp(CFileBase *file,int bits);
    int SetBrush(UINT style,COLORREF color,LONG hatch);
    int UpdateParent();
    int SetTransparentBkMode();
    int AttachDC(HDC hdc);
    COLORREF GetPixel(int x,int y);
    int SetStockFont(int fnObject);
    int SetFont(LOGFONTW *logfont);
    int SetFontA(LOGFONTA *logfont);
    int SetTextColor(COLORREF color);
    int SetTextBkColor(COLORREF color);
    int SetPen(int fnPenStyle,int nWidth,COLORREF crColor);
    int SetHatchBrush(int fnStyle,COLORREF clrref);
    int SetSolidBrush(COLORREF clrref);
    int TextOut(int x,int y,const WCHAR *str);
    int TextOutEx(int x,int y,RECT *r,const WCHAR *str);
    int FillRect(int l,int t,int w,int h);
    int DrawRect(COLORREF color);
    int DrawRect(int l,int t,int w,int h,COLORREF color);
    int DrawRect(int l,int t,int w,int h);
    int LineTo(int x,int y);
    int Ellipse(int l,int t,int r,int b);
    int Arc(int nLeftRect,int nTopRect,int nRightRect,int nBottomRect,int nXRadial1,int nYRadial1,int nXRadial2,int nYRadial2);
    int MoveTo(int x,int y);
    int DrawToScreen();
    int Clear();
    int StretchToDC(HDC hdc_des,int width,int height);
    int SaveBmp(const char *fn,int bits);
    int SetWindowPosX(int x);
    int SetWindowPosY(int y);
    int DrawText(const WCHAR *str,RECT *r,UINT dwDTFormat);
    int SetPixel(int x,int y,COLORREF color);
    int Copy(HDC hdc_src);
    int Paste(HDC hdc_des);
    int SetParent(HWND hp);
    int SetPos(int l,int t);
    int CreateCompatibleDC(HDC hdc);
    CMemDC();
    virtual ~CMemDC();
    int Init();
    int Destroy();
    int Copy(CMemDC *p);
    int Comp(CMemDC *p);
    int Print();
    int InitBasic();
};

/////////////////////////////////////////////
#endif // !defined(AFX_MEMDC_H__31413FAA_DC0C_4694_AF72_F7B77FEA8316__INCLUDED_)
