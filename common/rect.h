// Rect.h: interface for the CRect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECT_H__CA7AABD2_1D15_40F6_8D72_DB2611734BA3__INCLUDED_)
#define AFX_RECT_H__CA7AABD2_1D15_40F6_8D72_DB2611734BA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"

class CRect{
public:
    int32_t left,top;
    int32_t right,bottom;
public:
    status_t Get(int32_t *l, int32_t *t, int32_t *r, int32_t *b);   
    status_t GetCenter(int32_t *x, int32_t *y);
    status_t SetPos(int32_t x, int32_t y);
    status_t SetWH(int32_t left,int32_t top,int32_t width,int32_t height);
    status_t CenterRect(int32_t w,int32_t h,CRect *rc);
    status_t Offset(int32_t x,int32_t y);
    int32_t GetHeight();
    int32_t GetWidth();
    status_t PtInRect(int32_t x,int32_t y);
    status_t Subtract(CRect* psrc1, CRect* rc);
    status_t GetBound( CRect *psrc1, CRect *psrc2);
    status_t Set(int32_t l,int32_t t,int32_t r,int32_t b);
    status_t Union(CRect* psrc1, CRect* psrc2);
    status_t IsIntersect( CRect* psrc2);
    status_t Intersect(CRect *psrc1,CRect *psrc2);
    status_t CoveredBy(CRect *pRect);
    status_t Normalize();
    status_t Equal(CRect *p);
    status_t IsEmpty();
    status_t Empty();
    CRect();
    virtual ~CRect();
    status_t Init();
    status_t Destroy();
    status_t Copy(CRect *p);
    status_t Print();
    status_t InitBasic();
};

#endif // !defined(AFX_RECT_H__CA7AABD2_1D15_40F6_8D72_DB2611734BA3__INCLUDED_)
