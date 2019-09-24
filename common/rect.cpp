// Rect.cpp: implementation of the CRect class.
//
//////////////////////////////////////////////////////////////////////
#include "rect.h"
#include "syslog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRect::CRect()
{
    this->InitBasic();
}
CRect::~CRect()
{
    this->Destroy();
}
status_t CRect::InitBasic()
{
    //default is an empty rect
    this->left = 0;
    this->top = 0;
    this->right = -1;
    this->bottom = -1;
    return OK;
}
status_t CRect::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
status_t  CRect::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
status_t  CRect::Copy(CRect *p)
{
    this->left = p->left;
    this->top = p->top;
    this->right = p->right;
    this->bottom = p->bottom;
    return OK;
}
status_t  CRect::Print()
{
    syslog_printf("[%d,%d,%d,%d]\n",left,top,right,bottom);
    return TRUE;
}
status_t CRect::Empty()
{
    return this->InitBasic();
}
status_t CRect::IsEmpty()
{
    if( this->left > this->right )
        return TRUE;
    if(this->top > this->bottom ) 
        return TRUE;
    return FALSE;
}
status_t CRect::Equal(CRect *p)
{
    if( this->left != p->left )
        return FALSE;
    if( this->top != p->top )
        return FALSE;
    if( this->right != p->right )
        return FALSE;
    if( this->bottom != p->bottom )
        return FALSE;
 
    return TRUE;
}
status_t CRect::Normalize()
{
    int32_t iTemp;
    if(this->left > this->right)
    {
         iTemp = this->left;
         this->left = this->right;
         this->right = iTemp;
    }
    if(this->top > this->bottom)
    {
         iTemp = this->top;
         this->top = this->bottom;
         this->bottom = iTemp;
    }
    return OK;
}
status_t CRect::CoveredBy(CRect *pRect)
{
    if (this->left < pRect->left) 
        return FALSE;
    if (this->top < pRect->top) 
        return FALSE;
    if (this->right > pRect->right) 
        return FALSE;
    if (this->bottom > pRect->bottom) 
        return FALSE;
    return TRUE;
}
status_t CRect::Intersect(CRect *psrc1, CRect *psrc2)
{
    this->left = (psrc1->left > psrc2->left) ? psrc1->left : psrc2->left;
    this->top  = (psrc1->top > psrc2->top) ? psrc1->top : psrc2->top;
    this->right = (psrc1->right < psrc2->right) ? psrc1->right : psrc2->right;
    this->bottom = (psrc1->bottom < psrc2->bottom) ? psrc1->bottom : psrc2->bottom;
    if(this->IsEmpty())
        return FALSE;

    return TRUE;
}
status_t CRect::IsIntersect(CRect *psrc2)
{
    CRect tmp;
    return tmp.Intersect(this,psrc2);
}
status_t CRect::Union(CRect *psrc1, CRect *psrc2)
{
    CRect src1, src2;
    src1.Copy(psrc1);
    src2.Copy(psrc2);
    src1.Normalize();
    src2.Normalize();
    if (src1.left == src2.left && src1.right == src2.right) 
    {
        if (src1.top <= src2.top && src2.top <= src1.bottom) 
        {
            this->left = src1.left;
            this->right = src1.right;
            this->top = src1.top;            
            this->bottom = (src1.bottom > src2.bottom) ? src1.bottom : src2.bottom;
            return TRUE;
        }
        else if (src1.top >= src2.top && src2.bottom >= src1.top) 
        {
            this->left = src1.left;
            this->right = src1.right;
            this->top = src2.top;
            this->bottom = (src1.bottom > src2.bottom) ? src1.bottom : src2.bottom;
            return TRUE;
       }
       return FALSE;
    }
    if (src1.top == src2.top && src1.bottom == src2.bottom) 
    {
        if (src1.left <= src2.left && src2.left <= src1.right) 
        {
            this->top = src1.top;
            this->bottom = src1.bottom;
            this->left = src1.left;
            this->right = (src1.right > src2.right) ? src1.right : src2.right;
            return TRUE;
        }
        else if (src1.left >= src2.left && src2.right >= src1.left) 
        {
            this->top = src1.top;
            this->bottom = src1.bottom;
            this->left = src2.left;
            this->right = (src1.right > src2.right) ? src1.right : src2.right;
            return TRUE;
       }
       return FALSE;
    }
    return FALSE;
}
status_t CRect::Set(int32_t l, int32_t t, int32_t r, int32_t b)
{
    this->left = l;
    this->top = t;
    this->right = r;
    this->bottom = b;
    
    return OK;
}
status_t CRect::SetWH(int32_t left, int32_t top, int32_t width, int32_t height)
{
    return this->Set(left,top,left + width - 1, top + height - 1);
}
status_t CRect::GetBound(CRect *psrc1, CRect *psrc2)
{
    CRect src1, src2;
    src1.Copy(psrc1);
    src2.Copy(psrc2);
    src1.Normalize();
    src2.Normalize();
    this->left = (src1.left < src2.left) ? src1.left : src2.left;
    this->top  = (src1.top < src2.top) ? src1.top : src2.top;
    this->right = (src1.right > src2.right) ? src1.right : src2.right;
    this->bottom = (src1.bottom > src2.bottom) ? src1.bottom : src2.bottom;
    return OK;
}
status_t CRect::Subtract(CRect *psrc2,CRect *rc)
{
    CRect src, rcExpect, *prcExpect;
    int32_t nCount = 0;
    
    src.Copy(this);
    rcExpect.Copy(psrc2);
    src.Normalize();
    rcExpect.Normalize();
    prcExpect = &rcExpect;
    if (!src.IsIntersect(prcExpect)) 
    {       
        nCount = 1;
        rc[0].Copy(&src);
    }
    else 
    {
        if(prcExpect->top > src.top)
        {
            rc[nCount].left  = src.left;
            rc[nCount].top   = src.top;
            rc[nCount].right = src.right;
            rc[nCount].bottom = prcExpect->top - 1;
            nCount++;
            src.top = prcExpect->top;
        }
        if(prcExpect->bottom < src.bottom)
        {
            rc[nCount].top  = prcExpect->bottom + 1;
            rc[nCount].left   = src.left;
            rc[nCount].right = src.right;
            rc[nCount].bottom = src.bottom;
            nCount++;
            src.bottom = prcExpect->bottom;
        }
        if(prcExpect->left > src.left)
        {
            rc[nCount].left  = src.left;
            rc[nCount].top   = src.top;
            rc[nCount].right = prcExpect->left - 1;
            rc[nCount].bottom = src.bottom;
            nCount++;
        }
        if(prcExpect->right < src.right)
        {
            rc[nCount].left  = prcExpect->right + 1;
            rc[nCount].top   = src.top;
            rc[nCount].right = src.right;
            rc[nCount].bottom = src.bottom;
            nCount++;
        }
    }
    return nCount;
}
status_t CRect::PtInRect(int32_t x, int32_t y)
{
    if(    x >= this->left 
        && x <= this->right 
        && y >= this->top 
        && y <= this->bottom) 
        return TRUE;
    
    return FALSE;
}
int32_t CRect::GetWidth()
{
    return this->right - this->left + 1;
}
int32_t CRect::GetHeight()
{
    return this->bottom - this->top + 1;
}
status_t CRect::Offset(int32_t x, int32_t y)
{
    this->left += x;
    this->top += y;
    this->right += x;
    this->bottom += y;
    return OK;
}
status_t CRect::CenterRect(int32_t w, int32_t h, CRect *rc)
{
    ASSERT(rc);
    rc->left = this->left + ( (this->GetWidth() - w) >> 1 );
    rc->right = rc->left + w - 1;
    rc->top = this->top + ( (this->GetHeight() - h) >> 1 );
    rc->bottom = rc->top + h - 1;
    return OK;
}

status_t CRect::SetPos(int32_t x, int32_t y)
{
    int32_t w = this->GetWidth();
    int32_t h = this->GetHeight();
    return this->SetWH(x,y,w,h);
}

status_t CRect::GetCenter(int32_t *x, int32_t *y)
{
    ASSERT(x && y);
    *x = (this->left + this->right) >> 1;
    *y = (this->top + this->bottom) >> 1;
    return OK;
}

status_t CRect::Get(int32_t *l, int32_t *t, int32_t *r, int32_t *b)
{
    ASSERT(l && t && r && b);   
    *l = this->left;
    *t = this->top;
    *r = this->right;   
    *b = this->bottom;
    return OK;
}
