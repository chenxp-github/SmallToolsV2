// XImage.cpp: implementation of the CxImage class.
//
//////////////////////////////////////////////////////////////////////

#include "ximage.h"
#include "mem_tool.h"
#include "sys_log.h"

#ifndef PI
#define PI 3.141592653589793f
#endif

#define  RBLOCK 64
#define  HSLMAX   255   
#define  RGBMAX   255   
#define  HSLUNDEFINED (HSLMAX*2/3)

static const BYTE __pal256[1024] = 
{0,0,0,0,0,0,128,0,0,128,0,0,0,128,128,0,128,0,0,0,128,0,128,0,128,128,0,0,192,192,192,0,
192,220,192,0,240,202,166,0,212,240,255,0,177,226,255,0,142,212,255,0,107,198,255,0,
72,184,255,0,37,170,255,0,0,170,255,0,0,146,220,0,0,122,185,0,0,98,150,0,0,74,115,0,0,
50,80,0,212,227,255,0,177,199,255,0,142,171,255,0,107,143,255,0,72,115,255,0,37,87,255,0,0,
85,255,0,0,73,220,0,0,61,185,0,0,49,150,0,0,37,115,0,0,25,80,0,212,212,255,0,177,177,255,0,
142,142,255,0,107,107,255,0,72,72,255,0,37,37,255,0,0,0,254,0,0,0,220,0,0,0,185,0,0,0,150,0,
0,0,115,0,0,0,80,0,227,212,255,0,199,177,255,0,171,142,255,0,143,107,255,0,115,72,255,0,
87,37,255,0,85,0,255,0,73,0,220,0,61,0,185,0,49,0,150,0,37,0,115,0,25,0,80,0,240,212,255,0,
226,177,255,0,212,142,255,0,198,107,255,0,184,72,255,0,170,37,255,0,170,0,255,0,146,0,220,0,
122,0,185,0,98,0,150,0,74,0,115,0,50,0,80,0,255,212,255,0,255,177,255,0,255,142,255,0,255,107,255,0,
255,72,255,0,255,37,255,0,254,0,254,0,220,0,220,0,185,0,185,0,150,0,150,0,115,0,115,0,80,0,80,0,
255,212,240,0,255,177,226,0,255,142,212,0,255,107,198,0,255,72,184,0,255,37,170,0,255,0,170,0,
220,0,146,0,185,0,122,0,150,0,98,0,115,0,74,0,80,0,50,0,255,212,227,0,255,177,199,0,255,142,171,0,
255,107,143,0,255,72,115,0,255,37,87,0,255,0,85,0,220,0,73,0,185,0,61,0,150,0,49,0,115,0,37,0,
80,0,25,0,255,212,212,0,255,177,177,0,255,142,142,0,255,107,107,0,255,72,72,0,255,37,37,0,254,0,
0,0,220,0,0,0,185,0,0,0,150,0,0,0,115,0,0,0,80,0,0,0,255,227,212,0,255,199,177,0,255,171,142,0,
255,143,107,0,255,115,72,0,255,87,37,0,255,85,0,0,220,73,0,0,185,61,0,0,150,49,0,0,115,37,0,
0,80,25,0,0,255,240,212,0,255,226,177,0,255,212,142,0,255,198,107,0,255,184,72,0,255,170,37,0,
255,170,0,0,220,146,0,0,185,122,0,0,150,98,0,0,115,74,0,0,80,50,0,0,255,255,212,0,255,255,177,0,
255,255,142,0,255,255,107,0,255,255,72,0,255,255,37,0,254,254,0,0,220,220,0,0,185,185,0,0,150,150,0,
0,115,115,0,0,80,80,0,0,240,255,212,0,226,255,177,0,212,255,142,0,198,255,107,0,184,255,72,0,
170,255,37,0,170,255,0,0,146,220,0,0,122,185,0,0,98,150,0,0,74,115,0,0,50,80,0,0,227,255,212,0,
199,255,177,0,171,255,142,0,143,255,107,0,115,255,72,0,87,255,37,0,85,255,0,0,73,220,0,0,61,185,0,
0,49,150,0,0,37,115,0,0,25,80,0,0,212,255,212,0,177,255,177,0,142,255,142,0,107,255,107,0,72,255,72,0,
37,255,37,0,0,254,0,0,0,220,0,0,0,185,0,0,0,150,0,0,0,115,0,0,0,80,0,0,212,255,227,0,177,255,199,0,
142,255,171,0,107,255,143,0,72,255,115,0,37,255,87,0,0,255,85,0,0,220,73,0,0,185,61,0,0,150,49,0,0,
115,37,0,0,80,25,0,212,255,240,0,177,255,226,0,142,255,212,0,107,255,198,0,72,255,184,0,37,255,170,0,
0,255,170,0,0,220,146,0,0,185,122,0,0,150,98,0,0,115,74,0,0,80,50,0,212,255,255,0,177,255,255,0,
142,255,255,0,107,255,255,0,72,255,255,0,37,255,255,0,0,254,254,0,0,220,220,0,0,185,185,0,0,
150,150,0,0,115,115,0,0,80,80,0,242,242,242,0,230,230,230,0,218,218,218,0,206,206,206,0,194,194,194,0,
182,182,182,0,170,170,170,0,158,158,158,0,146,146,146,0,134,134,134,0,122,122,122,0,110,110,110,0,
98,98,98,0,86,86,86,0,74,74,74,0,62,62,62,0,50,50,50,0,38,38,38,0,26,26,26,0,14,14,14,0,240,251,255,0,
164,160,160,0,128,128,128,0,0,0,255,0,0,255,0,0,0,255,255,0,255,0,0,0,255,0,255,0,255,255,0,0,255,255,255,0};

static const BYTE __pal16[64]=
{0,0,0,0,0,0,128,0,0,128,0,0,0,128,128,0,128,0,0,0,128,0,128,0,128,128,0,0,192,192,192,0,
128,128,128,0,0,0,255,0,0,255,0,0,0,255,255,0,255,0,0,0,255,0,255,0,255,255,0,0,255,255,255,0};

static const BYTE __pal2[8]=
{0,0,0,0,255,255,255,0};
//////////////////////////////////////////////////////////////////////////////////////
// CxPoint2
//////////////////////////////////////////////////////////////////////////////////////
CxPoint2::CxPoint2()
{
  x=y=0.0f;
}

CxPoint2::CxPoint2(float const x_, float const y_)
{
  x=x_;
  y=y_;
}

CxPoint2::CxPoint2(CxPoint2 const &p)
{
  x=p.x;
  y=p.y;
}

float CxPoint2::Distance(CxPoint2 const p2)
{
  return (float)sqrt((x-p2.x)*(x-p2.x)+(y-p2.y)*(y-p2.y));
}

float CxPoint2::Distance(float const x_, float const y_)
{
  return (float)sqrt((x-x_)*(x-x_)+(y-y_)*(y-y_));
}
//////////////////////////////////////////////////////////////////////////////////////
// CxRect2
//////////////////////////////////////////////////////////////////////////////////////
CxRect2::CxRect2()
{
}

CxRect2::CxRect2(float const x1_, float const y1_, float const x2_, float const y2_)
{
  botLeft.x=x1_;
  botLeft.y=y1_;
  topRight.x=x2_;
  topRight.y=y2_;
}

CxRect2::CxRect2(CxRect2 const &p)
{
  botLeft=p.botLeft;
  topRight=p.topRight;
}

float CxRect2::Surface()
{
  return (topRight.x-botLeft.x)*(topRight.y-botLeft.y);
}

CxRect2 CxRect2::CrossSection(CxRect2 const &r2)
{
  CxRect2 cs;
  cs.botLeft.x=max(botLeft.x, r2.botLeft.x);
  cs.botLeft.y=max(botLeft.y, r2.botLeft.y);
  cs.topRight.x=min(topRight.x, r2.topRight.x);
  cs.topRight.y=min(topRight.y, r2.topRight.y);
  if (cs.botLeft.x<=cs.topRight.x && cs.botLeft.y<=cs.topRight.y) {
    return cs;
  } else {
    return CxRect2(0,0,0,0);
  }//if
}

CxPoint2 CxRect2::Center()
{
  return CxPoint2((topRight.x+botLeft.x)/2.0f, (topRight.y+botLeft.y)/2.0f);
}

float CxRect2::Width()
{
  return topRight.x-botLeft.x;
}

float CxRect2::Height()
{
  return topRight.y-botLeft.y;
}
//////////////////////////////////////////////////////////////////////////////////////
// CxImageInfo
//////////////////////////////////////////////////////////////////////////////////////
CxImageInfo::CxImageInfo()
{
    this->InitBasic();
}
CxImageInfo::~CxImageInfo()
{
    this->Destroy();
}
int CxImageInfo::InitBasic()
{
    return this->Clear();
}
int CxImageInfo::Init()
{
    this->InitBasic();
    return OK;
}
int CxImageInfo::Destroy()
{
    this->InitBasic();
    return OK;
}
int CxImageInfo::Copy(CxImageInfo *p)
{
    int i;

    if(this == p)
        return OK;

    this->dwEffWidth = p->dwEffWidth;
    this->pImage = p->pImage;
    this->pGhost = p->pGhost;
    this->pParent = p->pParent;
    this->dwType = p->dwType;

    for(i = 0; i < 256; i++)
        this->szLastError[i] = p->szLastError[i];

    this->nProgress = p->nProgress;
    this->nEscape = p->nEscape;
    this->nBkgndIndex = p->nBkgndIndex;
    this->nBkgndColor = p->nBkgndColor;
    this->fQuality = p->fQuality;
    this->nJpegScale = p->nJpegScale;
    this->nFrame = p->nFrame;
    this->nNumFrames = p->nNumFrames;
    this->dwFrameDelay = p->dwFrameDelay;
    this->xDPI = p->xDPI;
    this->yDPI = p->yDPI;
    this->rSelectionBox = p->rSelectionBox;
    this->nAlphaMax = p->nAlphaMax;
    this->bAlphaPaletteEnabled = p->bAlphaPaletteEnabled;
    this->bEnabled = p->bEnabled;
    this->xOffset = p->xOffset;
    this->yOffset = p->yOffset;

    for(i = 0; i < CMAX_IMAGE_FORMATS; i++)
        this->dwCodecOpt[i] = p->dwCodecOpt[i];
    
    this->last_c = p->last_c;
    this->last_c_index = p->last_c_index;
    this->last_c_isvalid = p->last_c_isvalid;
    this->nNumLayers = p->nNumLayers;
    this->dwFlags = p->dwFlags;
    this->dispmeth = p->dispmeth;
    this->bGetAllFrames = p->bGetAllFrames;
    this->bLittleEndianHost = p->bLittleEndianHost;

    return OK;
}
int CxImageInfo::Comp(CxImageInfo *p)
{
    return 0;
}
int CxImageInfo::Print()
{
    return TRUE;
}

int CxImageInfo::Clear()
{
    int i;

    this->dwEffWidth = 0;
    this->pImage = 0;
    this->pGhost = 0;
    this->pParent = 0;
    this->dwType = 0;
    
    this->nProgress = 0;
    this->nEscape = 0;
    this->fQuality = 0;
    this->nJpegScale = 0;
    this->nFrame = 0;
    this->nNumFrames = 0;
    this->dwFrameDelay = 0;
    this->xDPI = 0;
    this->yDPI = 0;
    this->nAlphaMax = 0;
    this->bAlphaPaletteEnabled = 0;
    this->bEnabled = 0;
    this->xOffset = 0;
    this->yOffset = 0;
    this->last_c_index = 0;
    this->last_c_isvalid = 0;
    this->nNumLayers = 0;
    this->dwFlags = 0;
    this->dispmeth = 0;
    this->bGetAllFrames = 0;
    this->bLittleEndianHost = 0;
    
    for(i = 0; i < 256; i++)
        this->szLastError[i] = 0;
    
    for(i = 0; i < CMAX_IMAGE_FORMATS; i++)
        this->dwCodecOpt[i] = 0;

    return OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// CImageIterator
//////////////////////////////////////////////////////////////////////////////////////

CImageIterator::CImageIterator(void)
{
    ima = 0;
    IterImage = 0;
    Itx = Ity = 0;
    Stepx = Stepy = 0;
}

CImageIterator::CImageIterator(CxImage *imageImpl)
{
    this->ima = imageImpl;
    if (ima) 
        IterImage = ima->GetBits(0);
    Itx = Ity = 0;
    Stepx = Stepy = 0;
}

BOOL CImageIterator::ItOK ()
{
    if (ima) return ima->IsInside(Itx, Ity);
    else     return FALSE;
}

void CImageIterator::Reset()
{
    if (ima) IterImage = ima->GetBits(0);
    else     IterImage=0;
    Itx = Ity = 0;
}

void CImageIterator::Upset()
{
    Itx = 0;
    Ity = ima->GetHeight()-1;
    IterImage = ima->GetBits(0) + ima->GetEffWidth()*(ima->GetHeight()-1);
}

BOOL CImageIterator::NextRow()
{
    if (++Ity >= (int)ima->GetHeight()) return 0;
    IterImage += ima->GetEffWidth();
    return 1;
}

BOOL CImageIterator::PrevRow()
{
    if (--Ity < 0) return 0;
    IterImage -= ima->GetEffWidth();
    return 1;
}

void CImageIterator::SetY(int y)
{
    if ((y < 0) || (y > (int)ima->GetHeight())) return;
    Ity = y;
    IterImage = ima->GetBits(0) + ima->GetEffWidth()*y;
}

void CImageIterator::SetRow(BYTE *buf, int n)
{
    if (n<0) n = (int)ima->GetEffWidth();
    else n = min(n,(int)ima->GetEffWidth());

    if ((IterImage!=NULL)&&(buf!=NULL)&&(n>0)) memcpy(IterImage,buf,n);
}

void CImageIterator::GetRow(BYTE *buf, int n)
{
    if ((IterImage!=NULL)&&(buf!=NULL)&&(n>0))
        memcpy(buf,IterImage,min(n,(int)ima->GetEffWidth()));
}

BYTE* CImageIterator::GetRow()
{
    return IterImage;
}

BYTE* CImageIterator::GetRow(int n)
{
    SetY(n);
    return IterImage;
}

BOOL CImageIterator::NextByte()
{
    if (++Itx < (int)ima->GetEffWidth()) return 1;
    else
        if (++Ity < (int)ima->GetHeight()){
            IterImage += ima->GetEffWidth();
            Itx = 0;
            return 1;
        } else
            return 0;
}

BOOL CImageIterator::PrevByte()
{
  if (--Itx >= 0) return 1;
  else
      if (--Ity >= 0){
          IterImage -= ima->GetEffWidth();
          Itx = 0;
          return 1;
      } else
          return 0;
}

BOOL CImageIterator::NextStep()
{
    Itx += Stepx;
    if (Itx < (int)ima->GetEffWidth()) return 1;
    else {
        Ity += Stepy;
        if (Ity < (int)ima->GetHeight()){
            IterImage += ima->GetEffWidth();
            Itx = 0;
            return 1;
        } else
            return 0;
    }
}

BOOL CImageIterator::PrevStep()
{
    Itx -= Stepx;
    if (Itx >= 0) return 1;
    else {       
        Ity -= Stepy;
        if (Ity >= 0 && Ity < (int)ima->GetHeight()) {
            IterImage -= ima->GetEffWidth();
            Itx = 0;
            return 1;
        } else
            return 0;
    }
}

BOOL CImageIterator::GetCol(BYTE* pCol, DWORD x)
{
    if ((pCol==0)||(ima->GetBpp()<8)||((int)x>=ima->GetWidth()))
        return 0;
    DWORD h = ima->GetHeight();
    //DWORD line = ima->GetEffWidth();
    BYTE bytes = (BYTE)(ima->GetBpp()>>3);
    BYTE* pSrc;
    for (DWORD y=0;y<h;y++){
        pSrc = ima->GetBits(y) + x*bytes;
        for (BYTE w=0;w<bytes;w++){
            *pCol++=*pSrc++;
        }
    }
    return 1;
}

BOOL CImageIterator::SetCol(BYTE* pCol, DWORD x)
{
    if ((pCol==0)||(ima->GetBpp()<8)||((int)x>=ima->GetWidth()))
        return 0;
    DWORD h = ima->GetHeight();
    //DWORD line = ima->GetEffWidth();
    BYTE bytes = (BYTE)(ima->GetBpp()>>3);
    BYTE* pSrc;
    for (DWORD y=0;y<h;y++){
        pSrc = ima->GetBits(y) + x*bytes;
        for (BYTE w=0;w<bytes;w++){
            *pSrc++=*pCol++;
        }
    }
    return 1;
}

//////////////////////////////////////////////////////////////////////////////////////
// CxImage
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_ALPHA
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_ALPHA

int CxImage::AlphaCreate()
{
    if (pAlpha==NULL) 
    {
        MALLOC(pAlpha,BYTE,head.biWidth * head.biHeight);
        memset(pAlpha,255,head.biWidth * head.biHeight);
    }
    return (pAlpha!=0);
}

int CxImage::AlphaSet(BYTE level)
{
    if (pAlpha) memset(pAlpha,level,head.biWidth * head.biHeight);
    return OK;
}

int CxImage::AlphaSet(const long x, const long y, const BYTE level)
{
    if (pAlpha && IsInside(x,y)) pAlpha[x+y*head.biWidth]=level;
    return OK;
}

BYTE CxImage::AlphaGet(const long x, const long y)
{
    if (pAlpha && IsInside(x,y)) return pAlpha[x+y*head.biWidth];
    return 0;
}

BYTE * CxImage::AlphaGetPointer(const long x, const long y)
{
    if (pAlpha && IsInside(x,y)) return pAlpha+x+y*head.biWidth;
    return 0;
}

BYTE CxImage::BlindAlphaGet(const long x, const long y)
{
    if (!IsInside(x,y) || (pAlpha==0))
        return 0;
    return pAlpha[x+y*head.biWidth];
}

int CxImage::AlphaPaletteClear()
{
    RGBQUAD c;
    for(WORD ip=0; ip<head.biClrUsed;ip++){
        c=GetPaletteColor((BYTE)ip);
        c.rgbReserved=0;
        SetPaletteColor((BYTE)ip,c);
    }

    return OK;
}

int CxImage::AlphaDelete()
{
    FREE(pAlpha);
    return OK;
}   

BYTE CxImage::AlphaGetMax()
{
    return info.nAlphaMax;
}

int CxImage::AlphaSetMax(BYTE nAlphaMax)
{
    info.nAlphaMax=nAlphaMax;
    return OK;
}

BOOL CxImage::AlphaIsValid()
{
    return pAlpha!=0;
}

void CxImage::AlphaPaletteEnable(BOOL enable)
{
    info.bAlphaPaletteEnabled=enable;
}

BOOL CxImage::AlphaPaletteIsEnabled()
{
    return info.bAlphaPaletteEnabled;
}

void CxImage::AlphaClear()
{
    if (pAlpha) memset(pAlpha,0,head.biWidth * head.biHeight);
}
void CxImage::AlphaInvert()
{
    if (pAlpha) {
        BYTE *iSrc=pAlpha;
        long n=head.biHeight*head.biWidth;
        for(long i=0; i < n; i++){
            *iSrc=(BYTE)~(*(iSrc));
            iSrc++;
        }
    }
}

BOOL CxImage::AlphaCopy(CxImage *from)
{
    ASSERT(from);

    if (from->pAlpha == NULL || head.biWidth != from->head.biWidth || head.biHeight != from->head.biHeight)
        return FALSE;

    if (pAlpha==NULL) 
    {
        MALLOC(pAlpha,BYTE,head.biWidth * head.biHeight);
    }
    
    memcpy(pAlpha,from->pAlpha,head.biWidth * head.biHeight);
    info.nAlphaMax=from->info.nAlphaMax;
    return TRUE;
}


BOOL CxImage::AlphaSet(CxImage *from)
{
    ASSERT(from);

    if (!from->IsGrayScale() || head.biWidth != from->head.biWidth || head.biHeight != from->head.biHeight) 
        return FALSE;

    if (pAlpha==NULL) 
    {
        MALLOC(pAlpha,BYTE,head.biWidth * head.biHeight);
    }

    BYTE* src = from->info.pImage;
    BYTE* dst = pAlpha;

    if (src==NULL || dst==NULL) 
        return FALSE;

    for (long y=0; y<head.biHeight; y++){
        memcpy(dst,src,head.biWidth);
        dst += head.biWidth;
        src += from->info.dwEffWidth;
    }
    return TRUE;

}
BOOL CxImage::AlphaPaletteIsValid()
{
    RGBQUAD c;
    for(WORD ip=0; ip<head.biClrUsed;ip++){
        c=GetPaletteColor((BYTE)ip);
        if (c.rgbReserved != 0) return TRUE;
    }
    return FALSE;
}

void CxImage::AlphaStrip()
{
    BOOL bAlphaPaletteIsValid = AlphaPaletteIsValid();
    BOOL bAlphaIsValid = AlphaIsValid();
    if (!(bAlphaIsValid || bAlphaPaletteIsValid)) return;
    RGBQUAD c;
    long a, a1;
    if (head.biBitCount==24){
        for(long y=0; y<head.biHeight; y++){
            for(long x=0; x<head.biWidth; x++){
                c = BlindGetPixelColor(x,y,TRUE);
                if (bAlphaIsValid) a=(BlindAlphaGet(x,y)*info.nAlphaMax)/255; else a=info.nAlphaMax;
                a1 = 256-a;
                c.rgbBlue = (BYTE)((c.rgbBlue * a + a1 * info.nBkgndColor.rgbBlue)>>8);
                c.rgbGreen = (BYTE)((c.rgbGreen * a + a1 * info.nBkgndColor.rgbGreen)>>8);
                c.rgbRed = (BYTE)((c.rgbRed * a + a1 * info.nBkgndColor.rgbRed)>>8);
                BlindSetPixelColor(x,y,c,FALSE);
            }
        }
        AlphaDelete();
    } else {
        CxImage tmp;
        tmp.Init();
        tmp.StartUp(0);
        tmp.Create(head.biWidth,head.biHeight,24,0);

        if (!tmp.IsValid()){
            strcpy(info.szLastError,tmp.GetLastError());
            return;
        }

        for(long y=0; y<head.biHeight; y++){
            for(long x=0; x<head.biWidth; x++){
                c = BlindGetPixelColor(x,y,TRUE);
                if (bAlphaIsValid) a=(BlindAlphaGet(x,y)*info.nAlphaMax)/255; else a=info.nAlphaMax;
                if (bAlphaPaletteIsValid) a=(c.rgbReserved*a)/255;
                a1 = 256-a;
                c.rgbBlue = (BYTE)((c.rgbBlue * a + a1 * info.nBkgndColor.rgbBlue)>>8);
                c.rgbGreen = (BYTE)((c.rgbGreen * a + a1 * info.nBkgndColor.rgbGreen)>>8);
                c.rgbRed = (BYTE)((c.rgbRed * a + a1 * info.nBkgndColor.rgbRed)>>8);
                tmp.BlindSetPixelColor(x,y,c,FALSE);
            }
        }
        Transfer(&tmp,TRUE);
    }
    return;
}

BOOL CxImage::AlphaFlip()
{
    if (!pAlpha) return FALSE;

    BYTE *buff;
    MALLOC(buff,BYTE,head.biWidth);
    
    BYTE *iSrc,*iDst;
    iSrc = pAlpha + (head.biHeight-1)*head.biWidth;
    iDst = pAlpha;
    for (long i=0; i<(head.biHeight/2); ++i)
    {
        memcpy(buff, iSrc, head.biWidth);
        memcpy(iSrc, iDst, head.biWidth);
        memcpy(iDst, buff, head.biWidth);
        iSrc-=head.biWidth;
        iDst+=head.biWidth;
    }

    FREE(buff);

    return TRUE;
}

BOOL CxImage::AlphaMirror()
{
    if (!pAlpha) return FALSE;

    BYTE* pAlpha2;
    MALLOC(pAlpha2,BYTE,head.biWidth * head.biHeight);
    
    BYTE *iSrc,*iDst;
    long wdt=head.biWidth-1;
    iSrc=pAlpha + wdt;
    iDst=pAlpha2;
    for(long y=0; y < head.biHeight; y++){
        for(long x=0; x <= wdt; x++)
            *(iDst+x)=*(iSrc-x);
        iSrc+=head.biWidth;
        iDst+=head.biWidth;
    }
    FREE(pAlpha);
    pAlpha=pAlpha2;
    return TRUE;
}

BOOL CxImage::AlphaSplit(CxImage *dest)
{
    if (!pAlpha || !dest) return FALSE;

    CxImage tmp;

    tmp.Init();
    tmp.StartUp(0);
    tmp.Create(head.biWidth,head.biHeight,8,0);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    for(long y=0; y<head.biHeight; y++){
        for(long x=0; x<head.biWidth; x++){
            tmp.BlindSetPixelIndex(x,y,pAlpha[x+y*head.biWidth]);
        }
    }

    tmp.SetGrayPalette();
    dest->Transfer(&tmp,TRUE);

    return TRUE;
}

BOOL CxImage::AlphaPaletteSplit(CxImage *dest)
{
    if (!AlphaPaletteIsValid() || !dest) return FALSE;

    CxImage tmp;

    tmp.Init();
    tmp.StartUp(0);
    tmp.Create(head.biWidth,head.biHeight,8,0);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    for(long y=0; y<head.biHeight; y++){
        for(long x=0; x<head.biWidth; x++){
            tmp.BlindSetPixelIndex(x,y,BlindGetPixelColor(x,y,TRUE).rgbReserved);
        }
    }

    tmp.SetGrayPalette();
    dest->Transfer(&tmp,TRUE);

    return TRUE;

}

BOOL CxImage::AlphaFromTransparency()
{
    if (!IsValid() || !IsTransparent())
        return FALSE;

    AlphaCreate();

    for(long y=0; y<head.biHeight; y++){
        for(long x=0; x<head.biWidth; x++){
            if (IsTransparent(x,y)){
                AlphaSet(x,y,0);
            }
        }
    }
    return TRUE;
}
#endif //#if CXIMAGE_SUPPORT_ALPHA

//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_ALPHA END
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_SELECTION
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_SELECTION

BOOL CxImage::SelectionIsValid()
{
    return pSelection!=0;
}

BOOL CxImage::SelectionGetBox(RECT *r)
{
    ASSERT(r);
    memcpy(r,&info.rSelectionBox,sizeof(RECT));
    return TRUE;
}

BOOL CxImage::SelectionClear(BYTE level)
{
    if (pSelection){
        if (level==0){
            memset(pSelection,0,head.biWidth * head.biHeight);
            info.rSelectionBox.left = head.biWidth;
            info.rSelectionBox.bottom = head.biHeight;
            info.rSelectionBox.right = info.rSelectionBox.top = 0;
        } else {
            memset(pSelection,level,head.biWidth * head.biHeight);
            info.rSelectionBox.right = head.biWidth;
            info.rSelectionBox.top = head.biHeight;
            info.rSelectionBox.left = info.rSelectionBox.bottom = 0;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CxImage::SelectionCreate()
{
    SelectionDelete();
    
    MALLOC(pSelection,BYTE,head.biWidth * head.biHeight);
    memset(pSelection,0,head.biWidth * head.biHeight);

    return (pSelection!=0);
}

int CxImage::SelectionDelete()
{
    FREE(pSelection);
    info.rSelectionBox.left = head.biWidth;
    info.rSelectionBox.bottom = head.biHeight;
    info.rSelectionBox.right = info.rSelectionBox.top = 0;
    return OK;
}

BOOL CxImage::SelectionIsInside(long x, long y)
{
    if (IsInside(x,y)){
        if (pSelection==NULL) return TRUE;
        return pSelection[x+y*head.biWidth]!=0;
    }
    return FALSE;
}

BOOL CxImage::BlindSelectionIsInside(long x, long y)
{
    if (!IsInside(x,y))
        return 0;
    if (pSelection==NULL) return TRUE;
    return pSelection[x+y*head.biWidth]!=0;
}

BOOL CxImage::SelectionAddRect(RECT r, BYTE level)
{
    if (pSelection==NULL) SelectionCreate();
    if (pSelection==NULL) return FALSE;

    RECT r2;
    if (r.left<r.right) {r2.left=r.left; r2.right=r.right; } else {r2.left=r.right ; r2.right=r.left; }
    if (r.bottom<r.top) {r2.bottom=r.bottom; r2.top=r.top; } else {r2.bottom=r.top ; r2.top=r.bottom; }

    if (info.rSelectionBox.top <= r2.top) info.rSelectionBox.top = max(0L,min(head.biHeight,r2.top+1));
    if (info.rSelectionBox.left > r2.left) info.rSelectionBox.left = max(0L,min(head.biWidth,r2.left));
    if (info.rSelectionBox.right <= r2.right) info.rSelectionBox.right = max(0L,min(head.biWidth,r2.right+1));
    if (info.rSelectionBox.bottom > r2.bottom) info.rSelectionBox.bottom = max(0L,min(head.biHeight,r2.bottom));

    long ymin = max(0L,min(head.biHeight,r2.bottom));
    long ymax = max(0L,min(head.biHeight,r2.top+1));
    long xmin = max(0L,min(head.biWidth,r2.left));
    long xmax = max(0L,min(head.biWidth,r2.right+1));

    for (long y=ymin; y<ymax; y++)
        memset(pSelection + xmin + y * head.biWidth, level, xmax-xmin);

    return TRUE;
}

BOOL CxImage::SelectionAddEllipse(RECT r, BYTE level)
{
    if (pSelection==NULL) SelectionCreate();
    if (pSelection==NULL) return FALSE;

    long xradius = abs(r.right - r.left)/2;
    long yradius = abs(r.top - r.bottom)/2;
    if (xradius==0 || yradius==0) return FALSE;

    long xcenter = (r.right + r.left)/2;
    long ycenter = (r.top + r.bottom)/2;

    if (info.rSelectionBox.left > (xcenter - xradius)) info.rSelectionBox.left = max(0L,min(head.biWidth,(xcenter - xradius)));
    if (info.rSelectionBox.right <= (xcenter + xradius)) info.rSelectionBox.right = max(0L,min(head.biWidth,(xcenter + xradius + 1)));
    if (info.rSelectionBox.bottom > (ycenter - yradius)) info.rSelectionBox.bottom = max(0L,min(head.biHeight,(ycenter - yradius)));
    if (info.rSelectionBox.top <= (ycenter + yradius)) info.rSelectionBox.top = max(0L,min(head.biHeight,(ycenter + yradius + 1)));

    long xmin = max(0L,min(head.biWidth,xcenter - xradius));
    long xmax = max(0L,min(head.biWidth,xcenter + xradius + 1));
    long ymin = max(0L,min(head.biHeight,ycenter - yradius));
    long ymax = max(0L,min(head.biHeight,ycenter + yradius + 1));

    long y,yo;
    for (y=ymin; y<min(ycenter,ymax); y++){
        for (long x=xmin; x<xmax; x++){
            yo = (long)(ycenter - yradius * sqrt(1-pow((float)(x - xcenter)/(float)xradius,2)));
            if (yo<y) pSelection[x + y * head.biWidth] = level;
        }
    }
    for (y=ycenter; y<ymax; y++){
        for (long x=xmin; x<xmax; x++){
            yo = (long)(ycenter + yradius * sqrt(1-pow((float)(x - xcenter)/(float)xradius,2)));
            if (yo>y) pSelection[x + y * head.biWidth] = level;
        }
    }
    return TRUE;
}

BOOL CxImage::SelectionInvert()
{
    if (pSelection) {
        BYTE *iSrc=pSelection;
        long n=head.biHeight*head.biWidth;
        for(long i=0; i < n; i++){
            *iSrc=(BYTE)~(*(iSrc));
            iSrc++;
        }
        
        SelectionRebuildBox();
        
        return TRUE;
    }
    return FALSE;
}

void CxImage::SelectionRebuildBox()
{
    info.rSelectionBox.left = head.biWidth;
    info.rSelectionBox.bottom = head.biHeight;
    info.rSelectionBox.right = info.rSelectionBox.top = 0;

    if (!pSelection)
        return;

    long x,y;

    for (y=0; y<head.biHeight; y++){
        for (x=0; x<info.rSelectionBox.left; x++){
            if (pSelection[x+y*head.biWidth]){
                info.rSelectionBox.left = x;
                continue;
            }
        }
    }

    for (y=0; y<head.biHeight; y++){
        for (x=head.biWidth-1; x>=info.rSelectionBox.right; x--){
            if (pSelection[x+y*head.biWidth]){
                info.rSelectionBox.right = x+1;
                continue;
            }
        }
    }

    for (x=0; x<head.biWidth; x++){
        for (y=0; y<info.rSelectionBox.bottom; y++){
            if (pSelection[x+y*head.biWidth]){
                info.rSelectionBox.bottom = y;
                continue;
            }
        }
    }

    for (x=0; x<head.biWidth; x++){
        for (y=head.biHeight-1; y>=info.rSelectionBox.top; y--){
            if (pSelection[x+y*head.biWidth]){
                info.rSelectionBox.top = y+1;
                continue;
            }
        }
    }
}

BOOL CxImage::SelectionCopy(CxImage *from)
{
    ASSERT(from);

    if (from->pSelection == NULL || head.biWidth != from->head.biWidth || head.biHeight != from->head.biHeight) return FALSE;
    
    if (pSelection==NULL)
    {
        MALLOC(pSelection,BYTE,head.biWidth * head.biHeight);
    }
    
    memcpy(pSelection,from->pSelection,head.biWidth * head.biHeight);
    memcpy(&info.rSelectionBox,&from->info.rSelectionBox,sizeof(RECT));

    return TRUE;
}

BOOL CxImage::SelectionAddPolygon(POINT *points, long npoints, BYTE level)
{
    if (points==NULL || npoints<3) return FALSE;

    if (pSelection==NULL) SelectionCreate();
    if (pSelection==NULL) return FALSE;
    
    BYTE *plocal;
    MALLOC(plocal,BYTE,head.biWidth*head.biHeight);

    RECT localbox = {head.biWidth,0,0,head.biHeight};
    long x,y,i=0;
    POINT *current;
    POINT *next = NULL;
    POINT *start = NULL;
    //trace contour
    while (i < npoints){
        current = &points[i];
        if (current->x!=-1){
            if (i==0 || (i>0 && points[i-1].x==-1)) start = &points[i];

            if ((i+1)==npoints || points[i+1].x==-1)
                next = start;
            else
                next = &points[i+1];

            float beta;
            if (current->x != next->x){
                beta = (float)(next->y - current->y)/(float)(next->x - current->x);
                if (current->x < next->x){
                    for (x=current->x; x<=next->x; x++){
                        y = (long)(current->y + (x - current->x) * beta);
                        if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
                    }
                } else {
                    for (x=current->x; x>=next->x; x--){
                        y = (long)(current->y + (x - current->x) * beta);
                        if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
                    }
                }
            }
            if (current->y != next->y){
                beta = (float)(next->x - current->x)/(float)(next->y - current->y);
                if (current->y < next->y){
                    for (y=current->y; y<=next->y; y++){
                        x = (long)(current->x + (y - current->y) * beta);
                        if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
                    }
                } else {
                    for (y=current->y; y>=next->y; y--){
                        x = (long)(current->x + (y - current->y) * beta);
                        if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
                    }
                }
            }
        }

        RECT r2;
        if (current->x < next->x) {r2.left=current->x; r2.right=next->x; } else {r2.left=next->x ; r2.right=current->x; }
        if (current->y < next->y) {r2.bottom=current->y; r2.top=next->y; } else {r2.bottom=next->y ; r2.top=current->y; }
        if (localbox.top < r2.top) localbox.top = max(0L,min(head.biHeight-1,r2.top+1));
        if (localbox.left > r2.left) localbox.left = max(0L,min(head.biWidth-1,r2.left-1));
        if (localbox.right < r2.right) localbox.right = max(0L,min(head.biWidth-1,r2.right+1));
        if (localbox.bottom > r2.bottom) localbox.bottom = max(0L,min(head.biHeight-1,r2.bottom-1));

        i++;
    }

    //fill the outer region
    long npix=(localbox.right - localbox.left)*(localbox.top - localbox.bottom);

    POINT* pix;
    MALLOC(pix,POINT,npix);

    BYTE back=0, mark=1;
    long fx, fy, fxx, fyy, first, last;
    long xmin = 0;
    long xmax = 0;
    long ymin = 0;
    long ymax = 0;

    for (int side=0; side<4; side++){
        switch(side){
        case 0:
            xmin=localbox.left; xmax=localbox.right+1; ymin=localbox.bottom; ymax=localbox.bottom+1;
            break;
        case 1:
            xmin=localbox.right; xmax=localbox.right+1; ymin=localbox.bottom; ymax=localbox.top+1;
            break;
        case 2:
            xmin=localbox.left; xmax=localbox.right+1; ymin=localbox.top; ymax=localbox.top+1;
            break;
        case 3:
            xmin=localbox.left; xmax=localbox.left+1; ymin=localbox.bottom; ymax=localbox.top+1;
            break;
        }
        //fill from the border points
        for(y=ymin;y<ymax;y++){
            for(x=xmin;x<xmax;x++){
                if (plocal[x+y*head.biWidth]==0){
                    // Subject: FLOOD FILL ROUTINE              Date: 12-23-97 (00:57)       
                    // Author:  Petter Holmberg                 Code: QB, QBasic, PDS        
                    // Origin:  petter.holmberg@usa.net         Packet: GRAPHICS.ABC
                    first=0;
                    last=1;
                    while(first!=last){
                        fx = pix[first].x;
                        fy = pix[first].y;
                        fxx = fx + x;
                        fyy = fy + y;
                        for(;;)
                        {
                            if ((plocal[fxx + fyy*head.biWidth] == back) &&
                                fxx>=localbox.left && fxx<=localbox.right && fyy>=localbox.bottom && fyy<=localbox.top )
                            {
                                plocal[fxx + fyy*head.biWidth] = mark;
                                if (fyy > 0 && plocal[fxx + (fyy - 1)*head.biWidth] == back){
                                    pix[last].x = fx;
                                    pix[last].y = fy - 1;
                                    last++;
                                    if (last == npix) last = 0;
                                }
                                if ((fyy + 1)<head.biHeight && plocal[fxx + (fyy + 1)*head.biWidth] == back){
                                    pix[last].x = fx;
                                    pix[last].y = fy + 1;
                                    last++;
                                    if (last == npix) last = 0;
                                }
                            } else {
                                break;
                            }
                            fx++;
                            fxx++;
                        };

                        fx = pix[first].x - 1;
                        fy = pix[first].y;
                        fxx = fx + x;
                        fyy = fy + y;

                        for( ;; )
                        {
                            if ((plocal[fxx + fyy*head.biWidth] == back) &&
                                fxx>=localbox.left && fxx<=localbox.right && fyy>=localbox.bottom && fyy<=localbox.top )
                            {
                                plocal[fxx + (y + fy)*head.biWidth] = mark;
                                if (fyy > 0 && plocal[fxx + (fyy - 1)*head.biWidth] == back){
                                    pix[last].x = fx;
                                    pix[last].y = fy - 1;
                                    last++;
                                    if (last == npix) last = 0;
                                }
                                if ((fyy + 1)<head.biHeight && plocal[fxx + (fyy + 1)*head.biWidth] == back){
                                    pix[last].x = fx;
                                    pix[last].y = fy + 1;
                                    last++;
                                    if (last == npix) last = 0;
                                }
                            } else {
                                break;
                            }
                            fx--;
                            fxx--;
                        }
                        
                        first++;
                        if (first == npix) first = 0;
                    }
                }
            }
        }
    }

    //transfer the region
    long yoffset;
    for (y=localbox.bottom; y<=localbox.top; y++){
        yoffset = y * head.biWidth;
        for (x=localbox.left; x<=localbox.right; x++)
            if (plocal[x + yoffset]!=1) pSelection[x + yoffset]=level;
    }
    if (info.rSelectionBox.top <= localbox.top) info.rSelectionBox.top = min(head.biHeight,localbox.top + 1);
    if (info.rSelectionBox.left > localbox.left) info.rSelectionBox.left = min(head.biWidth,localbox.left);
    if (info.rSelectionBox.right <= localbox.right) info.rSelectionBox.right = min(head.biWidth,localbox.right + 1);
    if (info.rSelectionBox.bottom > localbox.bottom) info.rSelectionBox.bottom = min(head.biHeight,localbox.bottom);

    FREE(plocal);
    FREE(pix);

    return TRUE;
}

BOOL CxImage::SelectionAddColor(RGBQUAD c, BYTE level)
{
    if (pSelection==NULL) SelectionCreate();
    if (pSelection==NULL) return FALSE;

    RECT localbox = {head.biWidth,0,0,head.biHeight};

    for (long y = 0; y < head.biHeight; y++){
        for (long x = 0; x < head.biWidth; x++){
            RGBQUAD color = BlindGetPixelColor(x, y, TRUE);
            if (color.rgbRed   == c.rgbRed &&
                color.rgbGreen == c.rgbGreen &&
                color.rgbBlue  == c.rgbBlue)
            {
                pSelection[x + y * head.biWidth] = level;

                if (localbox.top < y) localbox.top = y;
                if (localbox.left > x) localbox.left = x;
                if (localbox.right < x) localbox.right = x;
                if (localbox.bottom > y) localbox.bottom = y;
            }
        }
    }

    if (info.rSelectionBox.top <= localbox.top) info.rSelectionBox.top = localbox.top + 1;
    if (info.rSelectionBox.left > localbox.left) info.rSelectionBox.left = localbox.left;
    if (info.rSelectionBox.right <= localbox.right) info.rSelectionBox.right = localbox.right + 1;
    if (info.rSelectionBox.bottom > localbox.bottom) info.rSelectionBox.bottom = localbox.bottom;

    return TRUE;
}

BOOL CxImage::SelectionAddPixel(long x, long y, BYTE level)
{
    if (pSelection==NULL) SelectionCreate();
    if (pSelection==NULL) return FALSE;
    
    if (IsInside(x,y)) {
        pSelection[x + y * head.biWidth] = level; // set the correct mask bit
        
        if (info.rSelectionBox.top <= y) info.rSelectionBox.top = y+1;
        if (info.rSelectionBox.left > x) info.rSelectionBox.left = x;
        if (info.rSelectionBox.right <= x) info.rSelectionBox.right = x+1;
        if (info.rSelectionBox.bottom > y) info.rSelectionBox.bottom = y;
        
        return TRUE;
    }
    
    return FALSE;
}

BOOL CxImage::SelectionSplit(CxImage *dest)
{
    if (!pSelection || !dest) return FALSE;
    
    CxImage tmp(head.biWidth,head.biHeight,8,0);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }
    
    for(long y=0; y<head.biHeight; y++){
        for(long x=0; x<head.biWidth; x++){
            tmp.BlindSetPixelIndex(x,y,pSelection[x+y*head.biWidth]);
        }
    }
    
    tmp.SetGrayPalette();
    dest->Transfer(&tmp,TRUE);
    
    return TRUE;
}


BOOL CxImage::SelectionSet(CxImage *from)
{
    ASSERT(from);

    if (!from->IsGrayScale() || head.biWidth != from->head.biWidth || head.biHeight != from->head.biHeight){
        strcpy(info.szLastError,"CxImage::SelectionSet: wrong width or height, or image is not gray scale");
        return FALSE;
    }
    
    if (pSelection==NULL) 
    {
        MALLOC(pSelection,BYTE,head.biWidth * head.biHeight);
    }
    
    BYTE* src = from->info.pImage;
    BYTE* dst = pSelection;
    if (src==NULL || dst==NULL){
        strcpy(info.szLastError,"CxImage::SelectionSet: null pointer");
        return FALSE;
    }
    
    for (long y=0; y<head.biHeight; y++){
        memcpy(dst,src,head.biWidth);
        dst += head.biWidth;
        src += from->info.dwEffWidth;
    }
    
    SelectionRebuildBox();
    
    return TRUE;
}

void CxImage::SelectionSet(const long x, const long y, const BYTE level)
{
    if (pSelection && IsInside(x,y)) pSelection[x+y*head.biWidth]=level;
}

BYTE CxImage::SelectionGet(const long x, const long y)
{
    if (pSelection && IsInside(x,y)) return pSelection[x+y*head.biWidth];
    return 0;
}

BYTE CxImage::BlindSelectionGet(const long x, const long y)
{
    if (!IsInside(x,y) || (pSelection==0))
        return 0;
    return pSelection[x+y*head.biWidth];
}

BYTE * CxImage::SelectionGetPointer(const long x, const long y)
{
    if (pSelection && IsInside(x,y)) return pSelection+x+y*head.biWidth;
    return 0;
}

BOOL CxImage::SelectionFlip()
{
    if (!pSelection) return FALSE;
    
    BYTE *buff;
    MALLOC(buff,BYTE,head.biWidth);
    if (!buff) return FALSE;

    BYTE *iSrc,*iDst;
    iSrc = pSelection + (head.biHeight-1)*head.biWidth;
    iDst = pSelection;
    for (long i=0; i<(head.biHeight/2); ++i)
    {
        memcpy(buff, iSrc, head.biWidth);
        memcpy(iSrc, iDst, head.biWidth);
        memcpy(iDst, buff, head.biWidth);
        iSrc-=head.biWidth;
        iDst+=head.biWidth;
    }

    FREE(buff);

    long top = info.rSelectionBox.top;
    info.rSelectionBox.top = head.biHeight - info.rSelectionBox.bottom;
    info.rSelectionBox.bottom = head.biHeight - top;
    return TRUE;

}

BOOL CxImage::SelectionMirror()
{
    if (!pSelection) return FALSE;

    BYTE* pSelection2;
    MALLOC(pSelection2,BYTE,head.biWidth * head.biHeight);

    if (!pSelection2) return FALSE;
    
    BYTE *iSrc,*iDst;
    long wdt=head.biWidth-1;
    iSrc=pSelection + wdt;
    iDst=pSelection2;
    for(long y=0; y < head.biHeight; y++){
        for(long x=0; x <= wdt; x++)
            *(iDst+x)=*(iSrc-x);
        iSrc+=head.biWidth;
        iDst+=head.biWidth;
    }

    FREE(pSelection);
    pSelection=pSelection2;
    
    long left = info.rSelectionBox.left;
    info.rSelectionBox.left = head.biWidth - info.rSelectionBox.right;
    info.rSelectionBox.right = head.biWidth - left;
    return TRUE;

}

#if CXIMAGE_SUPPORT_WINDOWS
BOOL CxImage::SelectionToHRGN(HRGN &region)
{
    if (pSelection && region){           
        for(int y = 0; y < head.biHeight; y++){
            HRGN hTemp = NULL;
            int iStart = -1;
            int x = 0;
            for(; x < head.biWidth; x++){
                if (pSelection[x + y * head.biWidth] != 0){
                    if (iStart == -1) iStart = x;
                    continue;
                }else{
                    if (iStart >= 0){
                        hTemp = CreateRectRgn(iStart, y, x, y + 1);
                        CombineRgn(region, hTemp, region, RGN_OR);
                        DeleteObject(hTemp);
                        iStart = -1;
                    }
                }
            }
            if (iStart >= 0){
                hTemp = CreateRectRgn(iStart, y, x, y + 1);
                CombineRgn(region, hTemp, region, RGN_OR);
                DeleteObject(hTemp);
                iStart = -1;
            }
        }
        return TRUE;
    }
    return FALSE;
}
#endif //CXIMAGE_SUPPORT_WINDOWS
#endif //CXIMAGE_SUPPORT_SELECTION
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_SELECTION END
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_LAYERS
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_LAYERS

CxImage * CxImage::GetLayer(long position)
{
    if ( ppLayers == NULL) return NULL;
    if ( info.nNumLayers == 0) return NULL;
    if ( position >= info.nNumLayers ) return NULL;
    if ( position < 0) position = info.nNumLayers - 1;
    return ppLayers[position];
}

CxImage * CxImage::GetParent()
{
    return info.pParent;
}

long CxImage::GetNumLayers()
{
    return info.nNumLayers;
}

BOOL CxImage::LayerCreate(long position)
{
    if ( position < 0 || position > info.nNumLayers ) position = info.nNumLayers;
    
    CxImage** ptmp;
    NEW_ARRAY(ptmp,CxImage*,info.nNumLayers + 1);

    if (ptmp==0) return FALSE;
    
    int i=0;
    for (int n=0; n<info.nNumLayers; n++){
        if (position == n){
            NEW(ptmp[n],CxImage);
            i=1;
        }
        ptmp[n+i]=ppLayers[n];
    }
    if (i==0)
    {
        NEW(ptmp[info.nNumLayers],CxImage);
    }
    
    if (ptmp[position]){
        ptmp[position]->info.pParent = this;
    } else {
        DEL_ARRAY(ptmp);
        return FALSE;
    }
    
    info.nNumLayers++;
    DEL_ARRAY(ppLayers);
    ppLayers = ptmp;
    return TRUE;
}

BOOL CxImage::LayerDelete(long position)
{
    if ( position >= info.nNumLayers ) return FALSE;
    if ( position < 0) position = info.nNumLayers - 1;
    if ( position < 0) return FALSE;

    if (info.nNumLayers>1){     
        CxImage **ptmp;
        NEW_ARRAY(ptmp,CxImage*,info.nNumLayers - 1);
        if (ptmp==0) return FALSE;

        int i=0;
        for (int n=0; n<info.nNumLayers; n++){
            if (position == n){
                DEL(ppLayers[n]);
                i=1;
            }
            ptmp[n]=ppLayers[n+i];
        }

        info.nNumLayers--;
        DEL_ARRAY(ppLayers);
        ppLayers = ptmp;

    } else {
        DEL(ppLayers[0]);
        DEL_ARRAY(ppLayers);
        ppLayers = 0;
        info.nNumLayers = 0;
    }
    return TRUE;
}

void CxImage::LayerDeleteAll()
{
    if (ppLayers) 
    { 
        for(long n=0; n<info.nNumLayers;n++)
        { DEL(ppLayers[n]); }
        DEL_ARRAY(ppLayers);
        ppLayers=0; 
        info.nNumLayers = 0;
    }
}
#endif  //CXIMAGE_SUPPORT_LAYERS
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_LAYERS
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_INTERPOLATION
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_INTERPOLATION
void CxImage::OverflowCoordinates(long &x, long &y, int const ofMethod)
{
  if (IsInside(x,y)) return;  
  switch (ofMethod) {
    case OM_REPEAT:
      
      x=max(x,0); x=min(x, head.biWidth-1);
      y=max(y,0); y=min(y, head.biHeight-1);
      break;
    case OM_WRAP:
      
      x = x % head.biWidth;
      y = y % head.biHeight;
      if (x<0) x = head.biWidth + x;
      if (y<0) y = head.biHeight + y;
      break;
    case OM_MIRROR:
      
      if (x<0) x=((-x) % head.biWidth);
      else if (x>=head.biWidth) x=head.biWidth-(x % head.biWidth + 1);
      if (y<0) y=((-y) % head.biHeight);
      else if (y>=head.biHeight) y=head.biHeight-(y % head.biHeight + 1);
      break;
    default:
      return;
  }
}

void CxImage::OverflowCoordinates(float &x, float &y, int const ofMethod)
{
  if (x>=0 && x<head.biWidth && y>=0 && y<head.biHeight) return;  
  switch (ofMethod) {
    case OM_REPEAT:
      
      x=max(x,0); x=min(x, head.biWidth-1);
      y=max(y,0); y=min(y, head.biHeight-1);
      break;
    case OM_WRAP:
      
      x = (float)fmod(x, (float) head.biWidth);
      y = (float)fmod(y, (float) head.biHeight);
      if (x<0) x = head.biWidth + x;
      if (y<0) y = head.biHeight + y;
      break;
    case OM_MIRROR:
      
      if (x<0) x=(float)fmod(-x, (float) head.biWidth);
      else if (x>=head.biWidth) x=head.biWidth-((float)fmod(x, (float) head.biWidth) + 1);
      if (y<0) y=(float)fmod(-y, (float) head.biHeight);
      else if (y>=head.biHeight) y=head.biHeight-((float)fmod(y, (float) head.biHeight) + 1);
      break;
    default:
      return;
  }
}

RGBQUAD CxImage::GetPixelColorWithOverflow(long x, long y, int const ofMethod, RGBQUAD* const rplColor)
{
  RGBQUAD color;          
  if ((!IsInside(x,y)) || pDib==NULL) {     
    
    if (rplColor!=NULL)
      color=*rplColor;
    else {
      color.rgbRed=color.rgbGreen=color.rgbBlue=255; color.rgbReserved=0; 
    }
    if (pDib==NULL) return color;
    
    switch (ofMethod) {
      case OM_TRANSPARENT:
#if CXIMAGE_SUPPORT_ALPHA
        if (AlphaIsValid()) {
          
          color.rgbReserved=0;
        } else {
#endif 
          
          if (GetTransIndex()>=0) {
            color=GetTransColor();    
          }
#if CXIMAGE_SUPPORT_ALPHA
        }
#endif 
        return color;
      case OM_BACKGROUND:
          
          if (info.nBkgndIndex >= 0) {
              if (head.biBitCount<24) color = GetPaletteColor((BYTE)info.nBkgndIndex);
              else color = info.nBkgndColor;
          }
          return color;
      case OM_REPEAT:
      case OM_WRAP:
      case OM_MIRROR:
        OverflowCoordinates(x,y,ofMethod);
        break;
      default:
        
        return color;
    }
  }
  
  return BlindGetPixelColor(x,y,TRUE);
}

RGBQUAD CxImage::GetPixelColorInterpolated(
  float x,float y, 
  int const inMethod, 
  int const ofMethod, 
  RGBQUAD* const rplColor)
{
  
  int xi=(int)(x); if (x<0) xi--;   
  int yi=(int)(y); if (y<0) yi--;
  RGBQUAD color;                    

  switch (inMethod) {
    case IM_NEAREST_NEIGHBOUR:
      return GetPixelColorWithOverflow((long)(x+0.5f), (long)(y+0.5f), ofMethod, rplColor);
    default: {
      
      if (xi<-1 || xi>=head.biWidth || yi<-1 || yi>=head.biHeight) {  
        switch (ofMethod) {
          case OM_COLOR: case OM_TRANSPARENT: case OM_BACKGROUND:
            
            return GetPixelColorWithOverflow(-999, -999, ofMethod, rplColor);
          default:
            
            OverflowCoordinates(x,y,ofMethod);
            xi=(int)(x); if (x<0) xi--;   
            yi=(int)(y); if (y<0) yi--;
        }
      }
      
      if ((xi+1)<head.biWidth && xi>=0 && (yi+1)<head.biHeight && yi>=0 && head.biClrUsed==0) {
        
        WORD wt1=(WORD)((x-xi)*256.0f), wt2=(WORD)((y-yi)*256.0f);
        WORD wd=wt1*wt2>>8;
        WORD wb=wt1-wd;
        WORD wc=wt2-wd;
        WORD wa=256-wt1-wc;
        WORD wrr,wgg,wbb;
        BYTE *pxptr=(BYTE*)info.pImage+yi*info.dwEffWidth+xi*3;
        wbb=wa*(*pxptr++); wgg=wa*(*pxptr++); wrr=wa*(*pxptr++);
        wbb+=wb*(*pxptr++); wgg+=wb*(*pxptr++); wrr+=wb*(*pxptr);
        pxptr+=(info.dwEffWidth-5); 
        wbb+=wc*(*pxptr++); wgg+=wc*(*pxptr++); wrr+=wc*(*pxptr++); 
        wbb+=wd*(*pxptr++); wgg+=wd*(*pxptr++); wrr+=wd*(*pxptr); 
        color.rgbRed=(BYTE) (wrr>>8); color.rgbGreen=(BYTE) (wgg>>8); color.rgbBlue=(BYTE) (wbb>>8);
#if CXIMAGE_SUPPORT_ALPHA
        if (pAlpha) {
          WORD waa;
          
          pxptr=AlphaGetPointer(xi,yi);                           
          waa=wa*(*pxptr++); waa+=wb*(*pxptr);   
          pxptr+=(head.biWidth-1);                                
          waa+=wc*(*pxptr++); waa+=wd*(*pxptr);   
          color.rgbReserved=(BYTE) (waa>>8);
        } else
#endif
        { 
            color.rgbReserved = 0;
        }
        return color;
      } else {
        
        float t1=x-xi, t2=y-yi;
        float d=t1*t2;
        float b=t1-d;
        float c=t2-d;
        float a=1-t1-c;
        RGBQUAD rgb11,rgb21,rgb12,rgb22;
        rgb11=GetPixelColorWithOverflow(xi, yi, ofMethod, rplColor);
        rgb21=GetPixelColorWithOverflow(xi+1, yi, ofMethod, rplColor);
        rgb12=GetPixelColorWithOverflow(xi, yi+1, ofMethod, rplColor);
        rgb22=GetPixelColorWithOverflow(xi+1, yi+1, ofMethod, rplColor);
        
        color.rgbRed=(BYTE) (a*rgb11.rgbRed+b*rgb21.rgbRed+c*rgb12.rgbRed+d*rgb22.rgbRed);
        color.rgbGreen=(BYTE) (a*rgb11.rgbGreen+b*rgb21.rgbGreen+c*rgb12.rgbGreen+d*rgb22.rgbGreen);
        color.rgbBlue=(BYTE) (a*rgb11.rgbBlue+b*rgb21.rgbBlue+c*rgb12.rgbBlue+d*rgb22.rgbBlue);
#if CXIMAGE_SUPPORT_ALPHA
        if (AlphaIsValid())
            color.rgbReserved=(BYTE) (a*rgb11.rgbReserved+b*rgb21.rgbReserved+c*rgb12.rgbReserved+d*rgb22.rgbReserved);
        else
#endif
        { 
            color.rgbReserved = 0;
        }
        return color;
      }
    }
    case IM_BICUBIC: 
    case IM_BICUBIC2:
    case IM_BSPLINE:
    case IM_BOX:
    case IM_HERMITE:
    case IM_HAMMING:
    case IM_SINC:
    case IM_BLACKMAN:
    case IM_BESSEL:
    case IM_GAUSSIAN:
    case IM_QUADRATIC:
    case IM_MITCHELL:
    case IM_CATROM:
    case IM_HANNING:
    case IM_POWER:
      
      if (((xi+2)<0) || ((xi-1)>=head.biWidth) || ((yi+2)<0) || ((yi-1)>=head.biHeight)) { 
        switch (ofMethod) {
          case OM_COLOR: case OM_TRANSPARENT: case OM_BACKGROUND:
            
            return GetPixelColorWithOverflow(-999, -999, ofMethod, rplColor);
            break;
          default:
            
            OverflowCoordinates(x,y,ofMethod);
            xi=(int)(x); if (x<0) xi--;   
            yi=(int)(y); if (y<0) yi--;
        }
      }

      
      int xii,yii;                      
      float kernel, kernelyc;           
      float kernelx[12], kernely[4];    
      float rr,gg,bb,aa;                
      
      int i;
      switch (inMethod) {
        case IM_BICUBIC:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelCubic((float)(xi+i-1-x));
            kernely[i]=KernelCubic((float)(yi+i-1-y));
          }
          break;
        case IM_BICUBIC2:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelGeneralizedCubic((float)(xi+i-1-x), -0.5);
            kernely[i]=KernelGeneralizedCubic((float)(yi+i-1-y), -0.5);
          }
          break;
        case IM_BSPLINE:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelBSpline((float)(xi+i-1-x));
            kernely[i]=KernelBSpline((float)(yi+i-1-y));
          }
          break;
        case IM_BOX:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelBox((float)(xi+i-1-x));
            kernely[i]=KernelBox((float)(yi+i-1-y));
          }
          break;
        case IM_HERMITE:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelHermite((float)(xi+i-1-x));
            kernely[i]=KernelHermite((float)(yi+i-1-y));
          }
          break;
        case IM_HAMMING:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelHamming((float)(xi+i-1-x));
            kernely[i]=KernelHamming((float)(yi+i-1-y));
          }
          break;
        case IM_SINC:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelSinc((float)(xi+i-1-x));
            kernely[i]=KernelSinc((float)(yi+i-1-y));
          }
          break;
        case IM_BLACKMAN:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelBlackman((float)(xi+i-1-x));
            kernely[i]=KernelBlackman((float)(yi+i-1-y));
          }
          break;
        case IM_BESSEL:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelBessel((float)(xi+i-1-x));
            kernely[i]=KernelBessel((float)(yi+i-1-y));
          }
          break;
        case IM_GAUSSIAN:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelGaussian((float)(xi+i-1-x));
            kernely[i]=KernelGaussian((float)(yi+i-1-y));
          }
          break;
        case IM_QUADRATIC:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelQuadratic((float)(xi+i-1-x));
            kernely[i]=KernelQuadratic((float)(yi+i-1-y));
          }
          break;
        case IM_MITCHELL:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelMitchell((float)(xi+i-1-x));
            kernely[i]=KernelMitchell((float)(yi+i-1-y));
          }
          break;
        case IM_CATROM:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelCatrom((float)(xi+i-1-x));
            kernely[i]=KernelCatrom((float)(yi+i-1-y));
          }
          break;
        case IM_HANNING:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelHanning((float)(xi+i-1-x));
            kernely[i]=KernelHanning((float)(yi+i-1-y));
          }
          break;
        case IM_POWER:
          for (i=0; i<4; i++) {
            kernelx[i]=KernelPower((float)(xi+i-1-x),2);
            kernely[i]=KernelPower((float)(yi+i-1-y),2);
          }
          break;
      }
      rr=gg=bb=aa=0;
      if (((xi+2)<head.biWidth) && xi>=1 && ((yi+2)<head.biHeight) && (yi>=1) && !IsIndexed()) {
        
        BYTE *pxptr, *pxptra;
        for (yii=yi-1; yii<yi+3; yii++) {
          pxptr=(BYTE *)BlindGetPixelPointer(xi-1, yii);    
          kernelyc=kernely[yii-(yi-1)];
#if CXIMAGE_SUPPORT_ALPHA
          if (AlphaIsValid()) {
            
            pxptra=AlphaGetPointer(xi-1, yii);
            kernel=kernelyc*kernelx[0];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++); aa+=kernel*(*pxptra++);
            kernel=kernelyc*kernelx[1];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++); aa+=kernel*(*pxptra++);
            kernel=kernelyc*kernelx[2];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++); aa+=kernel*(*pxptra++);
            kernel=kernelyc*kernelx[3];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr); aa+=kernel*(*pxptra);
          } else
#endif
          
          {
            kernel=kernelyc*kernelx[0];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++);
            kernel=kernelyc*kernelx[1];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++);
            kernel=kernelyc*kernelx[2];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++);
            kernel=kernelyc*kernelx[3];
            bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr);
          }
        }
      } else {
        
        RGBQUAD rgbs;
        for (yii=yi-1; yii<yi+3; yii++) {
          kernelyc=kernely[yii-(yi-1)];
          for (xii=xi-1; xii<xi+3; xii++) {
            kernel=kernelyc*kernelx[xii-(xi-1)];
            rgbs=GetPixelColorWithOverflow(xii, yii, ofMethod, rplColor);
            rr+=kernel*rgbs.rgbRed;
            gg+=kernel*rgbs.rgbGreen;
            bb+=kernel*rgbs.rgbBlue;
#if CXIMAGE_SUPPORT_ALPHA
            aa+=kernel*rgbs.rgbReserved;
#endif
          }
        }
      }
      
      if (rr>255) rr=255; if (rr<0) rr=0; color.rgbRed=(BYTE) rr;
      if (gg>255) gg=255; if (gg<0) gg=0; color.rgbGreen=(BYTE) gg;
      if (bb>255) bb=255; if (bb<0) bb=0; color.rgbBlue=(BYTE) bb;
#if CXIMAGE_SUPPORT_ALPHA
      if (AlphaIsValid()) {
        if (aa>255) aa=255; if (aa<0) aa=0; color.rgbReserved=(BYTE) aa;
      } else
#endif
        { 
            color.rgbReserved = 0;
        }
      return color;
    case IM_LANCZOS:
      
      if (((xi+6)<0) || ((xi-5)>=head.biWidth) || ((yi+6)<0) || ((yi-5)>=head.biHeight)) {
        
        switch (ofMethod) {
          case OM_COLOR: case OM_TRANSPARENT: case OM_BACKGROUND:
            
            return GetPixelColorWithOverflow(-999, -999, ofMethod, rplColor);
            break;
          default:
            
            OverflowCoordinates(x,y,ofMethod);
            xi=(int)(x); if (x<0) xi--;   
            yi=(int)(y); if (y<0) yi--;
        }
      }

      for (xii=xi-5; xii<xi+7; xii++) kernelx[xii-(xi-5)]=KernelLanczosSinc((float)(xii-x), 6.0f);
      rr=gg=bb=aa=0;

      if (((xi+6)<head.biWidth) && ((xi-5)>=0) && ((yi+6)<head.biHeight) && ((yi-5)>=0) && !IsIndexed()) {
        
        BYTE *pxptr, *pxptra;
        for (yii=yi-5; yii<yi+7; yii++) {
          pxptr=(BYTE *)BlindGetPixelPointer(xi-5, yii);    
          kernelyc=KernelLanczosSinc((float)(yii-y),6.0f);
#if CXIMAGE_SUPPORT_ALPHA
          if (AlphaIsValid()) {
            
            pxptra=AlphaGetPointer(xi-1, yii);
            for (xii=0; xii<12; xii++) {
              kernel=kernelyc*kernelx[xii];
              bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++); aa+=kernel*(*pxptra++);
            }
          } else
#endif
          
          {
            for (xii=0; xii<12; xii++) {
              kernel=kernelyc*kernelx[xii];
              bb+=kernel*(*pxptr++); gg+=kernel*(*pxptr++); rr+=kernel*(*pxptr++);
            }
          }
        }
      } else {
        
        RGBQUAD rgbs;
        for (yii=yi-5; yii<yi+7; yii++) {
          kernelyc=KernelLanczosSinc((float)(yii-y),6.0f);
          for (xii=xi-5; xii<xi+7; xii++) {
            kernel=kernelyc*kernelx[xii-(xi-5)];
            rgbs=GetPixelColorWithOverflow(xii, yii, ofMethod, rplColor);
            rr+=kernel*rgbs.rgbRed;
            gg+=kernel*rgbs.rgbGreen;
            bb+=kernel*rgbs.rgbBlue;
#if CXIMAGE_SUPPORT_ALPHA
            aa+=kernel*rgbs.rgbReserved;
#endif
          }
        }
      }
      
      if (rr>255) rr=255; if (rr<0) rr=0; color.rgbRed=(BYTE) rr;
      if (gg>255) gg=255; if (gg<0) gg=0; color.rgbGreen=(BYTE) gg;
      if (bb>255) bb=255; if (bb<0) bb=0; color.rgbBlue=(BYTE) bb;
#if CXIMAGE_SUPPORT_ALPHA
      if (AlphaIsValid()) {
        if (aa>255) aa=255; if (aa<0) aa=0; color.rgbReserved=(BYTE) aa;   
      } else
#endif
        { 
            color.rgbReserved = 0;
        }
      return color;
  }
}


void CxImage::AddAveragingCont(RGBQUAD const &color, float const surf, float &rr, float &gg, float &bb, float &aa)
{
  rr+=color.rgbRed*surf;
  gg+=color.rgbGreen*surf;
  bb+=color.rgbBlue*surf;
#if CXIMAGE_SUPPORT_ALPHA
  aa+=color.rgbReserved*surf;
#endif
}


RGBQUAD CxImage::GetAreaColorInterpolated(
  float const xc, float const yc, float const w, float const h, 
  int const inMethod, 
  int const ofMethod, 
  RGBQUAD* const rplColor)
{
    RGBQUAD color;      
    
    if (h<=1 && w<=1) {
        
        return GetPixelColorInterpolated(xc, yc, inMethod, ofMethod, rplColor);
    } else {
        
        CxRect2 area(xc-w/2.0f, yc-h/2.0f, xc+w/2.0f, yc+h/2.0f);   
        int xi1=(int)(area.botLeft.x+0.49999999f);                
        int yi1=(int)(area.botLeft.y+0.49999999f);                
        
        
        int xi2=(int)(area.topRight.x+0.5f);                      
        int yi2=(int)(area.topRight.y+0.5f);                      
        
        float rr,gg,bb,aa;                                        
        rr=gg=bb=aa=0;
        int x,y;                                                  
        float s=0;                                                
        float cps;                                                
        if (h>1 && w>1) {
            
            CxRect2 intBL, intTR;     
            intBL=area.CrossSection(CxRect2(((float)xi1)-0.5f, ((float)yi1)-0.5f, ((float)xi1)+0.5f, ((float)yi1)+0.5f));
            intTR=area.CrossSection(CxRect2(((float)xi2)-0.5f, ((float)yi2)-0.5f, ((float)xi2)+0.5f, ((float)yi2)+0.5f));
            float wBL, wTR, hBL, hTR;
            wBL=intBL.Width();            
            hBL=intBL.Height();           
            wTR=intTR.Width();            
            hTR=intTR.Height();           
            
            AddAveragingCont(GetPixelColorWithOverflow(xi1,yi1,ofMethod,rplColor), wBL*hBL, rr, gg, bb, aa);    
            AddAveragingCont(GetPixelColorWithOverflow(xi2,yi1,ofMethod,rplColor), wTR*hBL, rr, gg, bb, aa);    
            AddAveragingCont(GetPixelColorWithOverflow(xi1,yi2,ofMethod,rplColor), wBL*hTR, rr, gg, bb, aa);    
            AddAveragingCont(GetPixelColorWithOverflow(xi2,yi2,ofMethod,rplColor), wTR*hTR, rr, gg, bb, aa);    
            
            for (x=xi1+1; x<xi2; x++) {
                AddAveragingCont(GetPixelColorWithOverflow(x,yi1,ofMethod,rplColor), hBL, rr, gg, bb, aa);    
                AddAveragingCont(GetPixelColorWithOverflow(x,yi2,ofMethod,rplColor), hTR, rr, gg, bb, aa);    
            }
            
            for (y=yi1+1; y<yi2; y++) {
                AddAveragingCont(GetPixelColorWithOverflow(xi1,y,ofMethod,rplColor), wBL, rr, gg, bb, aa);    
                AddAveragingCont(GetPixelColorWithOverflow(xi2,y,ofMethod,rplColor), wTR, rr, gg, bb, aa);    
            }
            for (y=yi1+1; y<yi2; y++) {
                for (x=xi1+1; x<xi2; x++) { 
                    color=GetPixelColorWithOverflow(x,y,ofMethod,rplColor);
                    rr+=color.rgbRed;
                    gg+=color.rgbGreen;
                    bb+=color.rgbBlue;
#if CXIMAGE_SUPPORT_ALPHA
                    aa+=color.rgbReserved;
#endif
                }
            }
        } else {
            
            CxRect2 intersect;                                          
            CxPoint2 center;
            for (y=yi1; y<=yi2; y++) {
                for (x=xi1; x<=xi2; x++) {
                    intersect=area.CrossSection(CxRect2(((float)x)-0.5f, ((float)y)-0.5f, ((float)x)+0.5f, ((float)y)+0.5f));
                    center=intersect.Center();
                    color=GetPixelColorInterpolated(center.x, center.y, inMethod, ofMethod, rplColor);
                    cps=intersect.Surface();
                    rr+=color.rgbRed*cps;
                    gg+=color.rgbGreen*cps;
                    bb+=color.rgbBlue*cps;
#if CXIMAGE_SUPPORT_ALPHA
                    aa+=color.rgbReserved*cps;
#endif
                }
            }
        }
        
        s=area.Surface();
        rr/=s; gg/=s; bb/=s; aa/=s;
        if (rr>255) rr=255; if (rr<0) rr=0; color.rgbRed=(BYTE) rr;
        if (gg>255) gg=255; if (gg<0) gg=0; color.rgbGreen=(BYTE) gg;
        if (bb>255) bb=255; if (bb<0) bb=0; color.rgbBlue=(BYTE) bb;
#if CXIMAGE_SUPPORT_ALPHA
        if (AlphaIsValid()) {
            if (aa>255) aa=255; if (aa<0) aa=0; color.rgbReserved=(BYTE) aa;
        }
#endif
    }
    return color;
}


float CxImage::KernelBSpline(const float x)
{
    if (x>2.0f) return 0.0f;
    
    float a, b, c, d;
    float xm1 = x - 1.0f; 
    float xp1 = x + 1.0f;
    float xp2 = x + 2.0f;

    if ((xp2) <= 0.0f) a = 0.0f; else a = xp2*xp2*xp2; 
    if ((xp1) <= 0.0f) b = 0.0f; else b = xp1*xp1*xp1;
    if (x <= 0) c = 0.0f; else c = x*x*x;  
    if ((xm1) <= 0.0f) d = 0.0f; else d = xm1*xm1*xm1;

    return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));
}

float CxImage::KernelLinear(const float t)
{
    if (t < -1.0f)
        return 0.0f;
    if (t < 0.0f)
        return 1.0f+t;
    if (t < 1.0f)
        return 1.0f-t;
    return 0.0f;
}



float CxImage::KernelCubic(const float t)
{
  float abs_t = (float)fabs(t);
  float abs_t_sq = abs_t * abs_t;
  if (abs_t<1) return 1-2*abs_t_sq+abs_t_sq*abs_t;
  if (abs_t<2) return 4 - 8*abs_t +5*abs_t_sq - abs_t_sq*abs_t;
  return 0;
}



float CxImage::KernelGeneralizedCubic(const float t, const float a)
{
  float abs_t = (float)fabs(t);
  float abs_t_sq = abs_t * abs_t;
  if (abs_t<1) return (a+2)*abs_t_sq*abs_t - (a+3)*abs_t_sq + 1;
  if (abs_t<2) return a*abs_t_sq*abs_t - 5*a*abs_t_sq + 8*a*abs_t - 4*a;
  return 0;
}



float CxImage::KernelLanczosSinc(const float t, const float r)
{
  if (fabs(t) > r) return 0;
  if (t==0) return 1;
  float pit=PI*t;
  float pitd=pit/r;
  return (float)((sin(pit)/pit) * (sin(pitd)/pitd));
}


float CxImage::KernelBox(const float x)
{
    if (x < -0.5f)
        return 0.0f;
    if (x < 0.5f)
        return 1.0f;
    return 0.0f;
}

float CxImage::KernelHermite(const float x)
{
    if (x < -1.0f)
        return 0.0f;
    if (x < 0.0f)
        return (-2.0f*x-3.0f)*x*x+1.0f;
    if (x < 1.0f)
        return (2.0f*x-3.0f)*x*x+1.0f;
    return 0.0f;


}

float CxImage::KernelHanning(const float x)
{
    if (fabs(x)>1) return 0.0f;
    return (0.5f+0.5f*(float)cos(PI*x))*((float)sin(PI*x)/(PI*x));
}

float CxImage::KernelHamming(const float x)
{
    if (x < -1.0f)
        return 0.0f;
    if (x < 0.0f)
        return 0.92f*(-2.0f*x-3.0f)*x*x+1.0f;
    if (x < 1.0f)
        return 0.92f*(2.0f*x-3.0f)*x*x+1.0f;
    return 0.0f;


}

float CxImage::KernelSinc(const float x)
{
    if (x == 0.0)
        return(1.0);
    return((float)sin(PI*x)/(PI*x));
}

float CxImage::KernelBlackman(const float x)
{
    
    return (0.42f+0.5f*(float)cos(PI*x)+0.08f*(float)cos(2.0f*PI*x));
}

float CxImage::KernelBessel_J1(const float x)
{
    double p, q;
    
    register long i;
    
    static const double
    Pone[] =
    {
        0.581199354001606143928050809e+21,
        -0.6672106568924916298020941484e+20,
        0.2316433580634002297931815435e+19,
        -0.3588817569910106050743641413e+17,
        0.2908795263834775409737601689e+15,
        -0.1322983480332126453125473247e+13,
        0.3413234182301700539091292655e+10,
        -0.4695753530642995859767162166e+7,
        0.270112271089232341485679099e+4
    },
    Qone[] =
    {
        0.11623987080032122878585294e+22,
        0.1185770712190320999837113348e+20,
        0.6092061398917521746105196863e+17,
        0.2081661221307607351240184229e+15,
        0.5243710262167649715406728642e+12,
        0.1013863514358673989967045588e+10,
        0.1501793594998585505921097578e+7,
        0.1606931573481487801970916749e+4,
        0.1e+1
    };
        
    p = Pone[8];
    q = Qone[8];
    for (i=7; i >= 0; i--)
    {
        p = p*x*x+Pone[i];
        q = q*x*x+Qone[i];
    }
    return (float)(p/q);
}

float CxImage::KernelBessel_P1(const float x)
{
    double p, q;
    
    register long i;
    
    static const double
    Pone[] =
    {
        0.352246649133679798341724373e+5,
        0.62758845247161281269005675e+5,
        0.313539631109159574238669888e+5,
        0.49854832060594338434500455e+4,
        0.2111529182853962382105718e+3,
        0.12571716929145341558495e+1
    },
    Qone[] =
    {
        0.352246649133679798068390431e+5,
        0.626943469593560511888833731e+5,
        0.312404063819041039923015703e+5,
        0.4930396490181088979386097e+4,
        0.2030775189134759322293574e+3,
        0.1e+1
    };
        
    p = Pone[5];
    q = Qone[5];
    for (i=4; i >= 0; i--)
    {
        p = p*(8.0/x)*(8.0/x)+Pone[i];
        q = q*(8.0/x)*(8.0/x)+Qone[i];
    }
    return (float)(p/q);
}

float CxImage::KernelBessel_Q1(const float x)
{
    double p, q;
    
    register long i;
    
    static const double
    Pone[] =
    {
        0.3511751914303552822533318e+3,
        0.7210391804904475039280863e+3,
        0.4259873011654442389886993e+3,
        0.831898957673850827325226e+2,
        0.45681716295512267064405e+1,
        0.3532840052740123642735e-1
    },
    Qone[] =
    {
        0.74917374171809127714519505e+4,
        0.154141773392650970499848051e+5,
        0.91522317015169922705904727e+4,
        0.18111867005523513506724158e+4,
        0.1038187585462133728776636e+3,
        0.1e+1
    };
        
    p = Pone[5];
    q = Qone[5];
    for (i=4; i >= 0; i--)
    {
        p = p*(8.0/x)*(8.0/x)+Pone[i];
        q = q*(8.0/x)*(8.0/x)+Qone[i];
    }
    return (float)(p/q);
}

float CxImage::KernelBessel_Order1(float x)
{
    float p, q;
    
    if (x == 0.0)
        return (0.0f);
    p = x;
    if (x < 0.0)
        x=(-x);
    if (x < 8.0)
        return(p*KernelBessel_J1(x));
    q = (float)sqrt(2.0f/(PI*x))*(float)(KernelBessel_P1(x)*(1.0f/sqrt(2.0f)*(sin(x)-cos(x)))-8.0f/x*KernelBessel_Q1(x)*
        (-1.0f/sqrt(2.0f)*(sin(x)+cos(x))));
    if (p < 0.0f)
        q = (-q);
    return (q);
}

float CxImage::KernelBessel(const float x)
{
    if (x == 0.0f)
        return(PI/4.0f);
    return(KernelBessel_Order1(PI*x)/(2.0f*x));
}

float CxImage::KernelGaussian(const float x)
{
    return (float)(exp(-2.0f*x*x)*0.79788456080287f);
}

float CxImage::KernelQuadratic(const float x)
{
    if (x < -1.5f)
        return(0.0f);
    if (x < -0.5f)
        return(0.5f*(x+1.5f)*(x+1.5f));
    if (x < 0.5f)
        return(0.75f-x*x);
    if (x < 1.5f)
        return(0.5f*(x-1.5f)*(x-1.5f));
    return(0.0f);
}

float CxImage::KernelMitchell(const float x)
{
#define KM_B (1.0f/3.0f)
#define KM_C (1.0f/3.0f)
#define KM_P0 ((  6.0f - 2.0f * KM_B ) / 6.0f)
#define KM_P2 ((-18.0f + 12.0f * KM_B + 6.0f * KM_C) / 6.0f)
#define KM_P3 (( 12.0f - 9.0f  * KM_B - 6.0f * KM_C) / 6.0f)
#define KM_Q0 ((  8.0f * KM_B + 24.0f * KM_C) / 6.0f)
#define KM_Q1 ((-12.0f * KM_B - 48.0f * KM_C) / 6.0f)
#define KM_Q2 ((  6.0f * KM_B + 30.0f * KM_C) / 6.0f)
#define KM_Q3 (( -1.0f * KM_B -  6.0f * KM_C) / 6.0f)
    
    if (x < -2.0)
        return(0.0f);
    if (x < -1.0)
        return(KM_Q0-x*(KM_Q1-x*(KM_Q2-x*KM_Q3)));
    if (x < 0.0f)
        return(KM_P0+x*x*(KM_P2-x*KM_P3));
    if (x < 1.0f)
        return(KM_P0+x*x*(KM_P2+x*KM_P3));
    if (x < 2.0f)
        return(KM_Q0+x*(KM_Q1+x*(KM_Q2+x*KM_Q3)));
    return(0.0f);
}

float CxImage::KernelCatrom(const float x)
{
    if (x < -2.0)
        return(0.0f);
    if (x < -1.0)
        return(0.5f*(4.0f+x*(8.0f+x*(5.0f+x))));
    if (x < 0.0)
        return(0.5f*(2.0f+x*x*(-5.0f-3.0f*x)));
    if (x < 1.0)
        return(0.5f*(2.0f+x*x*(-5.0f+3.0f*x)));
    if (x < 2.0)
        return(0.5f*(4.0f+x*(-8.0f+x*(5.0f-x))));
    return(0.0f);
}

float CxImage::KernelPower(const float x, const float a)
{
    if (fabs(x)>1) return 0.0f;
    return (1.0f - (float)fabs(pow(x,a)));
}
#endif  //CXIMAGE_SUPPORT_INTERPOLATION
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_INTERPOLATION END
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_BASICTRANSFORMATIONS
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_BASICTRANSFORMATIONS
BOOL CxImage::GrayScale()
{
    if (!pDib) return FALSE;
    if (head.biBitCount<=8){
        RGBQUAD* ppal=GetPalette();
        int gray;
        //converts the colors to gray, use the blue channel only
        for(DWORD i=0;i<head.biClrUsed;i++){
            gray=(int)RGB2GRAY(ppal[i].rgbRed,ppal[i].rgbGreen,ppal[i].rgbBlue);
            ppal[i].rgbBlue = (BYTE)gray;
        }
        // preserve transparency
        if (info.nBkgndIndex >= 0) info.nBkgndIndex = ppal[info.nBkgndIndex].rgbBlue;
        //create a "real" 8 bit gray scale image
        if (head.biBitCount==8){
            BYTE *img=info.pImage;
            for(DWORD i=0;i<head.biSizeImage;i++) img[i]=ppal[img[i]].rgbBlue;
            SetGrayPalette();
        }
        //transform to 8 bit gray scale
        if (head.biBitCount==4 || head.biBitCount==1){
            CxImage ima;
            ima.CopyInfo(this);
            if (!ima.Create(head.biWidth,head.biHeight,8,info.dwType)) return FALSE;
            ima.SetGrayPalette();
#if CXIMAGE_SUPPORT_SELECTION
            ima.SelectionCopy(this);
#endif //CXIMAGE_SUPPORT_SELECTION
#if CXIMAGE_SUPPORT_ALPHA
            ima.AlphaCopy(this);
#endif //CXIMAGE_SUPPORT_ALPHA
            for (long y=0;y<head.biHeight;y++){
                BYTE *iDst = ima.GetBits(y);
                BYTE *iSrc = GetBits(y);
                for (long x=0;x<head.biWidth; x++){
                    //iDst[x]=ppal[BlindGetPixelIndex(x,y)].rgbBlue;
                    if (head.biBitCount==4){
                        BYTE pos = (BYTE)(4*(1-x%2));
                        iDst[x]= ppal[(BYTE)((iSrc[x >> 1]&((BYTE)0x0F<<pos)) >> pos)].rgbBlue;
                    } else {
                        BYTE pos = (BYTE)(7-x%8);
                        iDst[x]= ppal[(BYTE)((iSrc[x >> 3]&((BYTE)0x01<<pos)) >> pos)].rgbBlue;
                    }
                }
            }
            Transfer(&ima,TRUE);
        }
    } else { //from RGB to 8 bit gray scale
        BYTE *iSrc=info.pImage;
        CxImage ima;
        ima.CopyInfo(this);
        if (!ima.Create(head.biWidth,head.biHeight,8,info.dwType)) return FALSE;
        ima.SetGrayPalette();
#if CXIMAGE_SUPPORT_SELECTION
        ima.SelectionCopy(this);
#endif //CXIMAGE_SUPPORT_SELECTION
#if CXIMAGE_SUPPORT_ALPHA
        ima.AlphaCopy(this);
#endif //CXIMAGE_SUPPORT_ALPHA
        BYTE *img=ima.GetBits(0);
        long l8=ima.GetEffWidth();
        long l=head.biWidth * 3;
        for(long y=0; y < head.biHeight; y++) {
            for(long x=0,x8=0; x < l; x+=3,x8++) {
                img[x8+y*l8]=(BYTE)RGB2GRAY(*(iSrc+x+2),*(iSrc+x+1),*(iSrc+x+0));
            }
            iSrc+=info.dwEffWidth;
        }
        Transfer(&ima,TRUE);
    }
    return TRUE;
}

BOOL CxImage::Flip(BOOL bFlipSelection, BOOL bFlipAlpha)
{
    if (!pDib) return FALSE;

    BYTE *buff;
    MALLOC(buff,BYTE,info.dwEffWidth);
    if (!buff) return FALSE;

    BYTE *iSrc,*iDst;
    iSrc = GetBits(head.biHeight-1);
    iDst = GetBits(0);
    for (long i=0; i<(head.biHeight/2); ++i)
    {
        memcpy(buff, iSrc, info.dwEffWidth);
        memcpy(iSrc, iDst, info.dwEffWidth);
        memcpy(iDst, buff, info.dwEffWidth);
        iSrc-=info.dwEffWidth;
        iDst+=info.dwEffWidth;
    }

    FREE(buff);

    if (bFlipSelection){
#if CXIMAGE_SUPPORT_SELECTION
        SelectionFlip();
#endif //CXIMAGE_SUPPORT_SELECTION
    }

    if (bFlipAlpha){
#if CXIMAGE_SUPPORT_ALPHA
        AlphaFlip();
#endif //CXIMAGE_SUPPORT_ALPHA
    }

    return TRUE;
}

BOOL CxImage::Mirror(BOOL bMirrorSelection, BOOL bMirrorAlpha)
{
    if (!pDib) return FALSE;

    CxImage* imatmp;
    NEW(imatmp,CxImage);

    imatmp->StartUp(this->GetType());
    imatmp->Copy(this,FALSE,TRUE,TRUE);

    if (!imatmp) return FALSE;
    if (!imatmp->IsValid()){
        DEL(imatmp);
        return FALSE;
    }

    BYTE *iSrc,*iDst;
    long wdt=(head.biWidth-1) * (head.biBitCount==24 ? 3:1);
    iSrc=info.pImage + wdt;
    iDst=imatmp->info.pImage;
    long x,y;
    switch (head.biBitCount){
    case 24:
        for(y=0; y < head.biHeight; y++){
            for(x=0; x <= wdt; x+=3){
                *(iDst+x)=*(iSrc-x);
                *(iDst+x+1)=*(iSrc-x+1);
                *(iDst+x+2)=*(iSrc-x+2);
            }
            iSrc+=info.dwEffWidth;
            iDst+=info.dwEffWidth;
        }
        break;
    case 8:
        for(y=0; y < head.biHeight; y++){
            for(x=0; x <= wdt; x++)
                *(iDst+x)=*(iSrc-x);
            iSrc+=info.dwEffWidth;
            iDst+=info.dwEffWidth;
        }
        break;
    default:
        for(y=0; y < head.biHeight; y++){
            for(x=0; x <= wdt; x++)
                imatmp->SetPixelIndex(x,y,GetPixelIndex(wdt-x,y));
        }
    }

    if (bMirrorSelection){
#if CXIMAGE_SUPPORT_SELECTION
        imatmp->SelectionMirror();
#endif //CXIMAGE_SUPPORT_SELECTION
    }

    if (bMirrorAlpha){
#if CXIMAGE_SUPPORT_ALPHA
        imatmp->AlphaMirror();
#endif //CXIMAGE_SUPPORT_ALPHA
    }

    Transfer(imatmp,TRUE);
    DEL(imatmp);
    return TRUE;
}

BOOL CxImage::RotateLeft(CxImage *iDst)
{
    if (!pDib) return FALSE;

    long newWidth = GetHeight();
    long newHeight = GetWidth();

    CxImage imgDest;
    imgDest.Init();
    imgDest.CopyInfo(this);
    imgDest.Create(newWidth,newHeight,GetBpp(),GetType());
    imgDest.SetPalette(GetPalette(),256);

#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()) imgDest.AlphaCreate();
#endif

#if CXIMAGE_SUPPORT_SELECTION
    if (SelectionIsValid()) imgDest.SelectionCreate();
#endif

    long x,x2,y,dlineup;
    
    // Speedy rotate for BW images <Robert Abram>
    if (head.biBitCount == 1) {
    
        BYTE *sbits, *dbits, *dbitsmax, bitpos, *nrow,*srcdisp;
        ldiv_t div_r;

        BYTE *bsrc = GetBits(0), *bdest = imgDest.GetBits(0);
        dbitsmax = bdest + imgDest.head.biSizeImage - 1;
        dlineup = 8 * imgDest.info.dwEffWidth - imgDest.head.biWidth;

        imgDest.Clear(0);
        for (y = 0; y < head.biHeight; y++) {
            // Figure out the Column we are going to be copying to
            div_r = ldiv(y + dlineup, (long)8);
            // set bit pos of src column byte               
            bitpos = (BYTE)(1 << div_r.rem);
            srcdisp = bsrc + y * info.dwEffWidth;
            for (x = 0; x < (long)info.dwEffWidth; x++) {
                // Get Source Bits
                sbits = srcdisp + x;
                // Get destination column
                nrow = bdest + (x * 8) * imgDest.info.dwEffWidth + imgDest.info.dwEffWidth - 1 - div_r.quot;
                for (long z = 0; z < 8; z++) {
                   // Get Destination Byte
                    dbits = nrow + z * imgDest.info.dwEffWidth;
                    if ((dbits < bdest) || (dbits > dbitsmax)) break;
                    if (*sbits & (128 >> z)) *dbits |= bitpos;
                }
            }
        }//for y

#if CXIMAGE_SUPPORT_ALPHA
        if (AlphaIsValid()) {
            for (x = 0; x < newWidth; x++){
                x2=newWidth-x-1;
                for (y = 0; y < newHeight; y++){
                    imgDest.AlphaSet(x,y,BlindAlphaGet(y, x2));
                }//for y
            }//for x
        }
#endif //CXIMAGE_SUPPORT_ALPHA

#if CXIMAGE_SUPPORT_SELECTION
        if (SelectionIsValid()) {
            imgDest.info.rSelectionBox.left = newWidth-info.rSelectionBox.top;
            imgDest.info.rSelectionBox.right = newWidth-info.rSelectionBox.bottom;
            imgDest.info.rSelectionBox.bottom = info.rSelectionBox.left;
            imgDest.info.rSelectionBox.top = info.rSelectionBox.right;
            for (x = 0; x < newWidth; x++){
                x2=newWidth-x-1;
                for (y = 0; y < newHeight; y++){
                    imgDest.SelectionSet(x,y,BlindSelectionGet(y, x2));
                }//for y
            }//for x
        }
#endif //CXIMAGE_SUPPORT_SELECTION

    } else {
    //anything other than BW:
    //bd, 10. 2004: This optimized version of rotation rotates image by smaller blocks. It is quite
    //a bit faster than obvious algorithm, because it produces much less CPU cache misses.
    //This optimization can be tuned by changing block size (RBLOCK). 96 is good value for current
    //CPUs (tested on Athlon XP and Celeron D). Larger value (if CPU has enough cache) will increase
    //speed somehow, but once you drop out of CPU's cache, things will slow down drastically.
    //For older CPUs with less cache, lower value would yield better results.
        
        BYTE *srcPtr, *dstPtr;                        //source and destionation for 24-bit version
        int xs, ys;                                   //x-segment and y-segment
        for (xs = 0; xs < newWidth; xs+=RBLOCK) {       //for all image blocks of RBLOCK*RBLOCK pixels
            for (ys = 0; ys < newHeight; ys+=RBLOCK) {
                if (head.biBitCount==24) {
                    //RGB24 optimized pixel access:
                    for (x = xs; x < min(newWidth, xs+RBLOCK); x++){    //do rotation
                        info.nProgress = (long)(100*x/newWidth);
                        x2=newWidth-x-1;
                        dstPtr = (BYTE*) imgDest.BlindGetPixelPointer(x,ys);
                        srcPtr = (BYTE*) BlindGetPixelPointer(ys, x2);
                        for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                            //imgDest.SetPixelColor(x, y, GetPixelColor(y, x2));
                            *(dstPtr) = *(srcPtr);
                            *(dstPtr+1) = *(srcPtr+1);
                            *(dstPtr+2) = *(srcPtr+2);
                            srcPtr += 3;
                            dstPtr += imgDest.info.dwEffWidth;
                        }//for y
                    }//for x
                } else {
                    //anything else than 24bpp (and 1bpp): palette
                    for (x = xs; x < min(newWidth, xs+RBLOCK); x++){
                        info.nProgress = (long)(100*x/newWidth); //<Anatoly Ivasyuk>
                        x2=newWidth-x-1;
                        for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                            imgDest.SetPixelIndex(x, y, BlindGetPixelIndex(y, x2));
                        }//for y
                    }//for x
                }//if (version selection)
#if CXIMAGE_SUPPORT_ALPHA
                if (AlphaIsValid()) {
                    for (x = xs; x < min(newWidth, xs+RBLOCK); x++){
                        x2=newWidth-x-1;
                        for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                            imgDest.AlphaSet(x,y,BlindAlphaGet(y, x2));
                        }//for y
                    }//for x
                }//if (alpha channel)
#endif //CXIMAGE_SUPPORT_ALPHA

#if CXIMAGE_SUPPORT_SELECTION
                if (SelectionIsValid()) {
                    imgDest.info.rSelectionBox.left = newWidth-info.rSelectionBox.top;
                    imgDest.info.rSelectionBox.right = newWidth-info.rSelectionBox.bottom;
                    imgDest.info.rSelectionBox.bottom = info.rSelectionBox.left;
                    imgDest.info.rSelectionBox.top = info.rSelectionBox.right;
                    for (x = xs; x < min(newWidth, xs+RBLOCK); x++){
                        x2=newWidth-x-1;
                        for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                            imgDest.SelectionSet(x,y,BlindSelectionGet(y, x2));
                        }//for y
                    }//for x
                }//if (selection)
#endif //CXIMAGE_SUPPORT_SELECTION
            }//for ys
        }//for xs
    }//if

    //select the destination
    if (iDst) iDst->Transfer(&imgDest,TRUE);
    else Transfer(&imgDest,TRUE);
    return TRUE;
}

BOOL CxImage::RotateRight(CxImage *iDst)
{
    if (!pDib) return FALSE;

    long newWidth = GetHeight();
    long newHeight = GetWidth();

    CxImage imgDest;
    imgDest.Init();
    imgDest.CopyInfo(this);
    imgDest.Create(newWidth,newHeight,GetBpp(),GetType());
    imgDest.SetPalette(GetPalette(),256);

#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()) imgDest.AlphaCreate();
#endif

#if CXIMAGE_SUPPORT_SELECTION
    if (SelectionIsValid()) imgDest.SelectionCreate();
#endif

    long x,y,y2;
    // Speedy rotate for BW images <Robert Abram>
    if (head.biBitCount == 1) {
    
        BYTE *sbits, *dbits, *dbitsmax, bitpos, *nrow,*srcdisp;
        ldiv_t div_r;

        BYTE *bsrc = GetBits(0), *bdest = imgDest.GetBits(0);
        dbitsmax = bdest + imgDest.head.biSizeImage - 1;

        imgDest.Clear(0);
        for (y = 0; y < head.biHeight; y++) {
            // Figure out the Column we are going to be copying to
            div_r = ldiv(y, (long)8);
            // set bit pos of src column byte               
            bitpos = (BYTE)(128 >> div_r.rem);
            srcdisp = bsrc + y * info.dwEffWidth;
            for (x = 0; x < (long)info.dwEffWidth; x++) {
                // Get Source Bits
                sbits = srcdisp + x;
                // Get destination column
                nrow = bdest + (imgDest.head.biHeight-1-(x*8)) * imgDest.info.dwEffWidth + div_r.quot;
                for (long z = 0; z < 8; z++) {
                   // Get Destination Byte
                    dbits = nrow - z * imgDest.info.dwEffWidth;
                    if ((dbits < bdest) || (dbits > dbitsmax)) break;
                    if (*sbits & (128 >> z)) *dbits |= bitpos;
                }
            }
        }

#if CXIMAGE_SUPPORT_ALPHA
        if (AlphaIsValid()){
            for (y = 0; y < newHeight; y++){
                y2=newHeight-y-1;
                for (x = 0; x < newWidth; x++){
                    imgDest.AlphaSet(x,y,BlindAlphaGet(y2, x));
                }
            }
        }
#endif //CXIMAGE_SUPPORT_ALPHA

#if CXIMAGE_SUPPORT_SELECTION
        if (SelectionIsValid()){
            imgDest.info.rSelectionBox.left = info.rSelectionBox.bottom;
            imgDest.info.rSelectionBox.right = info.rSelectionBox.top;
            imgDest.info.rSelectionBox.bottom = newHeight-info.rSelectionBox.right;
            imgDest.info.rSelectionBox.top = newHeight-info.rSelectionBox.left;
            for (y = 0; y < newHeight; y++){
                y2=newHeight-y-1;
                for (x = 0; x < newWidth; x++){
                    imgDest.SelectionSet(x,y,BlindSelectionGet(y2, x));
                }
            }
        }
#endif //CXIMAGE_SUPPORT_SELECTION

    } else {
        //anything else but BW
        BYTE *srcPtr, *dstPtr;                        //source and destionation for 24-bit version
        int xs, ys;                                   //x-segment and y-segment
        for (xs = 0; xs < newWidth; xs+=RBLOCK) {
            for (ys = 0; ys < newHeight; ys+=RBLOCK) {
                if (head.biBitCount==24) {
                    //RGB24 optimized pixel access:
                    for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                        info.nProgress = (long)(100*y/newHeight); //<Anatoly Ivasyuk>
                        y2=newHeight-y-1;
                        dstPtr = (BYTE*) imgDest.BlindGetPixelPointer(xs,y);
                        srcPtr = (BYTE*) BlindGetPixelPointer(y2, xs);
                        for (x = xs; x < min(newWidth, xs+RBLOCK); x++){
                            //imgDest.SetPixelColor(x, y, GetPixelColor(y2, x));
                            *(dstPtr) = *(srcPtr);
                            *(dstPtr+1) = *(srcPtr+1);
                            *(dstPtr+2) = *(srcPtr+2);
                            dstPtr += 3;
                            srcPtr += info.dwEffWidth;
                        }//for x
                    }//for y
                } else {
                    //anything else than BW & RGB24: palette
                    for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                        info.nProgress = (long)(100*y/newHeight); //<Anatoly Ivasyuk>
                        y2=newHeight-y-1;
                        for (x = xs; x < min(newWidth, xs+RBLOCK); x++){
                            imgDest.SetPixelIndex(x, y, BlindGetPixelIndex(y2, x));
                        }//for x
                    }//for y
                }//if
#if CXIMAGE_SUPPORT_ALPHA
                if (AlphaIsValid()){
                    for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                        y2=newHeight-y-1;
                        for (x = xs; x < min(newWidth, xs+RBLOCK); x++){
                            imgDest.AlphaSet(x,y,BlindAlphaGet(y2, x));
                        }//for x
                    }//for y
                }//if (has alpha)
#endif //CXIMAGE_SUPPORT_ALPHA

#if CXIMAGE_SUPPORT_SELECTION
                if (SelectionIsValid()){
                    imgDest.info.rSelectionBox.left = info.rSelectionBox.bottom;
                    imgDest.info.rSelectionBox.right = info.rSelectionBox.top;
                    imgDest.info.rSelectionBox.bottom = newHeight-info.rSelectionBox.right;
                    imgDest.info.rSelectionBox.top = newHeight-info.rSelectionBox.left;
                    for (y = ys; y < min(newHeight, ys+RBLOCK); y++){
                        y2=newHeight-y-1;
                        for (x = xs; x < min(newWidth, xs+RBLOCK); x++){
                            imgDest.SelectionSet(x,y,BlindSelectionGet(y2, x));
                        }//for x
                    }//for y
                }//if (has alpha)
#endif //CXIMAGE_SUPPORT_SELECTION
            }//for ys
        }//for xs
    }//if

    //select the destination
    if (iDst) iDst->Transfer(&imgDest,TRUE);
    else Transfer(&imgDest,TRUE);
    return TRUE;
}

BOOL CxImage::Negative()
{
    if (!pDib) return FALSE;

    if (head.biBitCount<=8){
        if (IsGrayScale()){ //GRAYSCALE, selection
            if (pSelection){
                for(long y=info.rSelectionBox.bottom; y<info.rSelectionBox.top; y++){
                    for(long x=info.rSelectionBox.left; x<info.rSelectionBox.right; x++){
#if CXIMAGE_SUPPORT_SELECTION
                        if (BlindSelectionIsInside(x,y))
#endif //CXIMAGE_SUPPORT_SELECTION
                        {
                            BlindSetPixelIndex(x,y,(BYTE)(255-BlindGetPixelIndex(x,y)));
                        }
                    }
                }
            } else {
                BYTE *iSrc=info.pImage;
                for(unsigned long i=0; i < head.biSizeImage; i++){
                    *iSrc=(BYTE)~(*(iSrc));
                    iSrc++;
                }
            }
        } else { //PALETTE, full image
            RGBQUAD* ppal=GetPalette();
            for(DWORD i=0;i<head.biClrUsed;i++){
                ppal[i].rgbBlue =(BYTE)(255-ppal[i].rgbBlue);
                ppal[i].rgbGreen =(BYTE)(255-ppal[i].rgbGreen);
                ppal[i].rgbRed =(BYTE)(255-ppal[i].rgbRed);
            }
        }
    } else {
        if (pSelection==NULL){ //RGB, full image
            BYTE *iSrc=info.pImage;
            for(unsigned long i=0; i < head.biSizeImage; i++){
                *iSrc=(BYTE)~(*(iSrc));
                iSrc++;
            }
        } else { // RGB with selection
            RGBQUAD color;
            for(long y=info.rSelectionBox.bottom; y<info.rSelectionBox.top; y++){
                for(long x=info.rSelectionBox.left; x<info.rSelectionBox.right; x++){
#if CXIMAGE_SUPPORT_SELECTION
                    if (BlindSelectionIsInside(x,y))
#endif //CXIMAGE_SUPPORT_SELECTION
                    {
                        color = BlindGetPixelColor(x,y,TRUE);
                        color.rgbRed = (BYTE)(255-color.rgbRed);
                        color.rgbGreen = (BYTE)(255-color.rgbGreen);
                        color.rgbBlue = (BYTE)(255-color.rgbBlue);
                        BlindSetPixelColor(x,y,color,TRUE);
                    }
                }
            }
        }
        //<DP> invert transparent color too
        info.nBkgndColor.rgbBlue = (BYTE)(255-info.nBkgndColor.rgbBlue);
        info.nBkgndColor.rgbGreen = (BYTE)(255-info.nBkgndColor.rgbGreen);
        info.nBkgndColor.rgbRed = (BYTE)(255-info.nBkgndColor.rgbRed);
    }
    return TRUE;
}
#endif //CXIMAGE_SUPPORT_BASICTRANSFORMATIONS
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_BASICTRANSFORMATIONS END
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_TRANSFORMATION
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_TRANSFORMATION
BOOL CxImage::Rotate(float angle, CxImage *iDst)
{
    if (!pDib) return FALSE;

    //  Copyright (c) 1996-1998 Ulrich von Zadow

    // Negative the angle, because the y-axis is negative.
    double ang = -angle*acos((float)0)/90;
    int newWidth, newHeight;
    int nWidth = GetWidth();
    int nHeight= GetHeight();
    double cos_angle = cos(ang);
    double sin_angle = sin(ang);

    // Calculate the size of the new bitmap
    POINT p1={0,0};
    POINT p2={nWidth,0};
    POINT p3={0,nHeight};
    POINT p4={nWidth,nHeight};

    CxPoint2 newP1,newP2,newP3,newP4, leftTop, rightTop, leftBottom, rightBottom;

    newP1.x = (float)p1.x;
    newP1.y = (float)p1.y;
    newP2.x = (float)(p2.x*cos_angle - p2.y*sin_angle);
    newP2.y = (float)(p2.x*sin_angle + p2.y*cos_angle);
    newP3.x = (float)(p3.x*cos_angle - p3.y*sin_angle);
    newP3.y = (float)(p3.x*sin_angle + p3.y*cos_angle);
    newP4.x = (float)(p4.x*cos_angle - p4.y*sin_angle);
    newP4.y = (float)(p4.x*sin_angle + p4.y*cos_angle);

    leftTop.x = min(min(newP1.x,newP2.x),min(newP3.x,newP4.x));
    leftTop.y = min(min(newP1.y,newP2.y),min(newP3.y,newP4.y));
    rightBottom.x = max(max(newP1.x,newP2.x),max(newP3.x,newP4.x));
    rightBottom.y = max(max(newP1.y,newP2.y),max(newP3.y,newP4.y));
    leftBottom.x = leftTop.x;
    leftBottom.y = rightBottom.y;
    rightTop.x = rightBottom.x;
    rightTop.y = leftTop.y;

    newWidth = (int) floor(0.5f + rightTop.x - leftTop.x);
    newHeight= (int) floor(0.5f + leftBottom.y - leftTop.y);
    CxImage imgDest;
    imgDest.Init();
    imgDest.CopyInfo(this);
    imgDest.Create(newWidth,newHeight,GetBpp(),GetType());
    imgDest.SetPalette(GetPalette(),256);

#if CXIMAGE_SUPPORT_ALPHA
    if(AlphaIsValid())  //MTA: Fix for rotation problem when the image has an alpha channel
    {
        imgDest.AlphaCreate();
        imgDest.AlphaClear();
    }
#endif //CXIMAGE_SUPPORT_ALPHA

    int x,y,newX,newY,oldX,oldY;

    if (head.biClrUsed==0){ //RGB
        for (y = (int)leftTop.y, newY = 0; y<=(int)leftBottom.y; y++,newY++){
            info.nProgress = (long)(100*newY/newHeight);
            if (info.nEscape) break;
            for (x = (int)leftTop.x, newX = 0; x<=(int)rightTop.x; x++,newX++){
                oldX = (long)(x*cos_angle + y*sin_angle + 0.5);
                oldY = (long)(y*cos_angle - x*sin_angle + 0.5);
                imgDest.SetPixelColor(newX,newY,GetPixelColor(oldX,oldY,TRUE),FALSE);
#if CXIMAGE_SUPPORT_ALPHA
                imgDest.AlphaSet(newX,newY,AlphaGet(oldX,oldY));                //MTA: copy the alpha value
#endif //CXIMAGE_SUPPORT_ALPHA
            }
        }
    } else { //PALETTE
        for (y = (int)leftTop.y, newY = 0; y<=(int)leftBottom.y; y++,newY++){
            info.nProgress = (long)(100*newY/newHeight);
            if (info.nEscape) break;
            for (x = (int)leftTop.x, newX = 0; x<=(int)rightTop.x; x++,newX++){
                oldX = (long)(x*cos_angle + y*sin_angle + 0.5);
                oldY = (long)(y*cos_angle - x*sin_angle + 0.5);
                imgDest.SetPixelIndex(newX,newY,GetPixelIndex(oldX,oldY));
#if CXIMAGE_SUPPORT_ALPHA
                imgDest.AlphaSet(newX,newY,AlphaGet(oldX,oldY));                //MTA: copy the alpha value
#endif //CXIMAGE_SUPPORT_ALPHA
            }
        }
    }
    //select the destination
    if (iDst) iDst->Transfer(&imgDest,TRUE);
    else Transfer(&imgDest,TRUE);

    return TRUE;
}

BOOL CxImage::Rotate2(float angle, CxImage *iDst, int inMethod, int ofMethod, RGBQUAD *replColor, const BOOL optimizeRightAngles, const BOOL bKeepOriginalSize)
{
    if (!pDib) return FALSE;                    //no dib no go
    
    double ang = -angle*acos(0.0f)/90.0f;       //convert angle to radians and invert (positive angle performs clockwise rotation)
    float cos_angle = (float) cos(ang);         //these two are needed later (to rotate)
    float sin_angle = (float) sin(ang);
    
    //Calculate the size of the new bitmap (rotate corners of image)
    CxPoint2 p[4];                              //original corners of the image
    p[0]=CxPoint2(-0.5f,-0.5f);
    p[1]=CxPoint2(GetWidth()-0.5f,-0.5f);
    p[2]=CxPoint2(-0.5f,GetHeight()-0.5f);
    p[3]=CxPoint2(GetWidth()-0.5f,GetHeight()-0.5f);
    CxPoint2 newp[4];                               //rotated positions of corners
    //(rotate corners)
    if (bKeepOriginalSize){
        for (int i=0; i<4; i++) {
            newp[i].x = p[i].x;
            newp[i].y = p[i].y;
        }//for
    } else {
        for (int i=0; i<4; i++) {
            newp[i].x = (p[i].x*cos_angle - p[i].y*sin_angle);
            newp[i].y = (p[i].x*sin_angle + p[i].y*cos_angle);
        }//for i
        
        if (optimizeRightAngles) { 
            //For rotations of 90, -90 or 180 or 0 degrees, call faster routines
            if (newp[3].Distance(CxPoint2(GetHeight()-0.5f, 0.5f-GetWidth())) < 0.25) 
                //rotation right for circa 90 degrees (diagonal pixels less than 0.25 pixel away from 90 degree rotation destination)
                return RotateRight(iDst);
            if (newp[3].Distance(CxPoint2(0.5f-GetHeight(), -0.5f+GetWidth())) < 0.25) 
                //rotation left for ~90 degrees
                return RotateLeft(iDst);
            if (newp[3].Distance(CxPoint2(0.5f-GetWidth(), 0.5f-GetHeight())) < 0.25) 
                //rotation left for ~180 degrees
                return Rotate180(iDst);
            if (newp[3].Distance(p[3]) < 0.25) {
                //rotation not significant
                if (iDst) iDst->Copy(this);     //copy image to iDst, if required
                return TRUE;                        //and we're done
            }//if
        }//if
    }//if

    //(read new dimensions from location of corners)
    float minx = (float) min(min(newp[0].x,newp[1].x),min(newp[2].x,newp[3].x));
    float miny = (float) min(min(newp[0].y,newp[1].y),min(newp[2].y,newp[3].y));
    float maxx = (float) max(max(newp[0].x,newp[1].x),max(newp[2].x,newp[3].x));
    float maxy = (float) max(max(newp[0].y,newp[1].y),max(newp[2].y,newp[3].y));
    int newWidth = (int) floor(maxx-minx+0.5f);
    int newHeight= (int) floor(maxy-miny+0.5f);
    float ssx=((maxx+minx)- ((float) newWidth-1))/2.0f;   //start for x
    float ssy=((maxy+miny)- ((float) newHeight-1))/2.0f;  //start for y

    float newxcenteroffset = 0.5f * newWidth;
    float newycenteroffset = 0.5f * newHeight;
    if (bKeepOriginalSize){
        ssx -= 0.5f * GetWidth();
        ssy -= 0.5f * GetHeight();
    }

    //create destination image
    CxImage imgDest;
    imgDest.Init();
    imgDest.CopyInfo(this);
    imgDest.Create(newWidth,newHeight,GetBpp(),GetType());
    imgDest.SetPalette(GetPalette(),256);
#if CXIMAGE_SUPPORT_ALPHA
    if(AlphaIsValid()) imgDest.AlphaCreate(); //MTA: Fix for rotation problem when the image has an alpha channel
#endif //CXIMAGE_SUPPORT_ALPHA
    
    RGBQUAD rgb;            //pixel colour
    RGBQUAD rc;
    if (replColor!=0) 
        rc=*replColor; 
    else {
        rc.rgbRed=255; rc.rgbGreen=255; rc.rgbBlue=255; rc.rgbReserved=0;
    }//if
    float x,y;              //destination location (float, with proper offset)
    float origx, origy;     //origin location
    int destx, desty;       //destination location
    
    y=ssy;                  //initialize y
    if (!IsIndexed()){ //RGB24
        //optimized RGB24 implementation (direct write to destination):
        BYTE *pxptr;
#if CXIMAGE_SUPPORT_ALPHA
        BYTE *pxptra=0;
#endif //CXIMAGE_SUPPORT_ALPHA
        for (desty=0; desty<newHeight; desty++) {
            info.nProgress = (long)(100*desty/newHeight);
            if (info.nEscape) break;
            //initialize x
            x=ssx;
            //calculate pointer to first byte in row
            pxptr=(BYTE *)imgDest.BlindGetPixelPointer(0, desty);
#if CXIMAGE_SUPPORT_ALPHA
            //calculate pointer to first byte in row
            if (AlphaIsValid()) pxptra=imgDest.AlphaGetPointer(0, desty);
#endif //CXIMAGE_SUPPORT_ALPHA
            for (destx=0; destx<newWidth; destx++) {
                //get source pixel coordinate for current destination point
                //origx = (cos_angle*(x-head.biWidth/2)+sin_angle*(y-head.biHeight/2))+newWidth/2;
                //origy = (cos_angle*(y-head.biHeight/2)-sin_angle*(x-head.biWidth/2))+newHeight/2;
                origx = cos_angle*x+sin_angle*y;
                origy = cos_angle*y-sin_angle*x;
                if (bKeepOriginalSize){
                    origx += newxcenteroffset;
                    origy += newycenteroffset;
                }
                rgb = GetPixelColorInterpolated(origx, origy, inMethod, ofMethod, &rc);   //get interpolated colour value
                //copy alpha and colour value to destination
#if CXIMAGE_SUPPORT_ALPHA
                if (pxptra) *pxptra++ = rgb.rgbReserved;
#endif //CXIMAGE_SUPPORT_ALPHA
                *pxptr++ = rgb.rgbBlue;
                *pxptr++ = rgb.rgbGreen;
                *pxptr++ = rgb.rgbRed;
                x++;
            }//for destx
            y++;
        }//for desty
    } else { 
        //non-optimized implementation for paletted images
        for (desty=0; desty<newHeight; desty++) {
            info.nProgress = (long)(100*desty/newHeight);
            if (info.nEscape) break;
            x=ssx;
            for (destx=0; destx<newWidth; destx++) {
                //get source pixel coordinate for current destination point
                origx=(cos_angle*x+sin_angle*y);
                origy=(cos_angle*y-sin_angle*x);
                if (bKeepOriginalSize){
                    origx += newxcenteroffset;
                    origy += newycenteroffset;
                }
                rgb = GetPixelColorInterpolated(origx, origy, inMethod, ofMethod, &rc);
                //***!*** SetPixelColor is slow for palleted images
#if CXIMAGE_SUPPORT_ALPHA
                if (AlphaIsValid()) 
                    imgDest.SetPixelColor(destx,desty,rgb,TRUE);
                else 
#endif //CXIMAGE_SUPPORT_ALPHA     
                    imgDest.SetPixelColor(destx,desty,rgb,FALSE);
                x++;
            }//for destx
            y++;
        }//for desty
    }
    //select the destination
    
    if (iDst) iDst->Transfer(&imgDest,TRUE);
    else Transfer(&imgDest,TRUE);
    
    return TRUE;
}


BOOL CxImage::Rotate180(CxImage *iDst)
{
    if (!pDib) return FALSE;

    long wid = GetWidth();
    long ht = GetHeight();

    CxImage imgDest;
    imgDest.Init();
    imgDest.CopyInfo(this);
    imgDest.Create(wid,ht,GetBpp(),GetType());
    imgDest.SetPalette(GetPalette(),256);

#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()) imgDest.AlphaCreate();
#endif //CXIMAGE_SUPPORT_ALPHA

    long x,y,y2;
    for (y = 0; y < ht; y++){
        info.nProgress = (long)(100*y/ht); //<Anatoly Ivasyuk>
        y2=ht-y-1;
        for (x = 0; x < wid; x++){
            if(head.biClrUsed==0)//RGB
                imgDest.SetPixelColor(wid-x-1, y2, BlindGetPixelColor(x, y,TRUE),FALSE);
            else  //PALETTE
                imgDest.SetPixelIndex(wid-x-1, y2, BlindGetPixelIndex(x, y));

#if CXIMAGE_SUPPORT_ALPHA
            if (AlphaIsValid()) imgDest.AlphaSet(wid-x-1, y2,BlindAlphaGet(x, y));
#endif //CXIMAGE_SUPPORT_ALPHA

        }
    }

    //select the destination
    if (iDst) iDst->Transfer(&imgDest,TRUE);
    else Transfer(&imgDest,TRUE);
    return TRUE;
}

BOOL CxImage::Resample(long newx, long newy, int mode, CxImage *iDst)
{
    if (newx==0 || newy==0) return FALSE;

    if (head.biWidth==newx && head.biHeight==newy){
        if (iDst) iDst->Copy(this);
        return TRUE;
    }

    float xScale, yScale, fX, fY;
    xScale = (float)head.biWidth  / (float)newx;
    yScale = (float)head.biHeight / (float)newy;

    CxImage newImage;
    newImage.Init();
    newImage.CopyInfo(this);
    newImage.Create(newx,newy,head.biBitCount,GetType());
    newImage.SetPalette(GetPalette(),256);
    if (!newImage.IsValid()){
        strcpy(info.szLastError,newImage.GetLastError());
        return FALSE;
    }

    switch (mode) {
    case 1: // nearest pixel
    { 
        for(long y=0; y<newy; y++){
            info.nProgress = (long)(100*y/newy);
            if (info.nEscape) break;
            fY = y * yScale;
            for(long x=0; x<newx; x++){
                fX = x * xScale;
                newImage.SetPixelColor(x,y,GetPixelColor((long)fX,(long)fY,TRUE),FALSE);
            }
        }
        break;
    }
    case 2: // bicubic interpolation by Blake L. Carlson <blake-carlson(at)uiowa(dot)edu
    {
        float f_x, f_y, a, b, rr, gg, bb, r1, r2;
        int   i_x, i_y, xx, yy;
        RGBQUAD rgb;
        BYTE* iDst;
        for(long y=0; y<newy; y++){
            info.nProgress = (long)(100*y/newy);
            if (info.nEscape) break;
            f_y = (float) y * yScale - 0.5f;
            i_y = (int) floor(f_y);
            a   = f_y - (float)floor(f_y);
            for(long x=0; x<newx; x++){
                f_x = (float) x * xScale - 0.5f;
                i_x = (int) floor(f_x);
                b   = f_x - (float)floor(f_x);

                rr = gg = bb = 0.0f;
                for(int m=-1; m<3; m++) {
                    r1 = KernelBSpline((float) m - a);
                    yy = i_y+m;
                    if (yy<0) yy=0;
                    if (yy>=head.biHeight) yy = head.biHeight-1;
                    for(int n=-1; n<3; n++) {
                        r2 = r1 * KernelBSpline(b - (float)n);
                        xx = i_x+n;
                        if (xx<0) xx=0;
                        if (xx>=head.biWidth) xx=head.biWidth-1;

                        if (head.biClrUsed){
                            rgb = GetPixelColor(xx,yy,TRUE);
                        } else {
                            iDst  = info.pImage + yy*info.dwEffWidth + xx*3;
                            rgb.rgbBlue = *iDst++;
                            rgb.rgbGreen= *iDst++;
                            rgb.rgbRed  = *iDst;
                        }

                        rr += rgb.rgbRed * r2;
                        gg += rgb.rgbGreen * r2;
                        bb += rgb.rgbBlue * r2;
                    }
                }

                if (head.biClrUsed)
                    newImage.SetPixelColor(x,y,RGB(rr,gg,bb));
                else {
                    iDst = newImage.info.pImage + y*newImage.info.dwEffWidth + x*3;
                    *iDst++ = (BYTE)bb;
                    *iDst++ = (BYTE)gg;
                    *iDst   = (BYTE)rr;
                }

            }
        }
        break;
    }
    default: // bilinear interpolation
        if (!(head.biWidth>newx && head.biHeight>newy && head.biBitCount==24)) {
            // (c) 1999 Steve McMahon (steve@dogma.demon.co.uk)
            long ifX, ifY, ifX1, ifY1, xmax, ymax;
            float ir1, ir2, ig1, ig2, ib1, ib2, dx, dy;
            BYTE r,g,b;
            RGBQUAD rgb1, rgb2, rgb3, rgb4;
            xmax = head.biWidth-1;
            ymax = head.biHeight-1;
            for(long y=0; y<newy; y++){
                info.nProgress = (long)(100*y/newy);
                if (info.nEscape) break;
                fY = y * yScale;
                ifY = (int)fY;
                ifY1 = min(ymax, ifY+1);
                dy = fY - ifY;
                for(long x=0; x<newx; x++){
                    fX = x * xScale;
                    ifX = (int)fX;
                    ifX1 = min(xmax, ifX+1);
                    dx = fX - ifX;
                    // Interpolate using the four nearest pixels in the source
                    if (head.biClrUsed){
                        rgb1=GetPaletteColor(GetPixelIndex(ifX,ifY));
                        rgb2=GetPaletteColor(GetPixelIndex(ifX1,ifY));
                        rgb3=GetPaletteColor(GetPixelIndex(ifX,ifY1));
                        rgb4=GetPaletteColor(GetPixelIndex(ifX1,ifY1));
                    }
                    else {
                        BYTE* iDst;
                        iDst = info.pImage + ifY*info.dwEffWidth + ifX*3;
                        rgb1.rgbBlue = *iDst++; rgb1.rgbGreen= *iDst++; rgb1.rgbRed =*iDst;
                        iDst = info.pImage + ifY*info.dwEffWidth + ifX1*3;
                        rgb2.rgbBlue = *iDst++; rgb2.rgbGreen= *iDst++; rgb2.rgbRed =*iDst;
                        iDst = info.pImage + ifY1*info.dwEffWidth + ifX*3;
                        rgb3.rgbBlue = *iDst++; rgb3.rgbGreen= *iDst++; rgb3.rgbRed =*iDst;
                        iDst = info.pImage + ifY1*info.dwEffWidth + ifX1*3;
                        rgb4.rgbBlue = *iDst++; rgb4.rgbGreen= *iDst++; rgb4.rgbRed =*iDst;
                    }
                    // Interplate in x direction:
                    ir1 = rgb1.rgbRed   + (rgb3.rgbRed   - rgb1.rgbRed)   * dy;
                    ig1 = rgb1.rgbGreen + (rgb3.rgbGreen - rgb1.rgbGreen) * dy;
                    ib1 = rgb1.rgbBlue  + (rgb3.rgbBlue  - rgb1.rgbBlue)  * dy;
                    ir2 = rgb2.rgbRed   + (rgb4.rgbRed   - rgb2.rgbRed)   * dy;
                    ig2 = rgb2.rgbGreen + (rgb4.rgbGreen - rgb2.rgbGreen) * dy;
                    ib2 = rgb2.rgbBlue  + (rgb4.rgbBlue  - rgb2.rgbBlue)  * dy;
                    // Interpolate in y:
                    r = (BYTE)(ir1 + (ir2-ir1) * dx);
                    g = (BYTE)(ig1 + (ig2-ig1) * dx);
                    b = (BYTE)(ib1 + (ib2-ib1) * dx);
                    // Set output
                    newImage.SetPixelColor(x,y,RGB(r,g,b));
                }
            } 
        } else {
            //high resolution shrink, thanks to Henrik Stellmann <henrik.stellmann@volleynet.de>
            const long ACCURACY = 1000;
            long i,j; // index for faValue
            long x,y; // coordinates in  source image
            BYTE* pSource;
            BYTE* pDest = newImage.info.pImage;

            long* naAccu;
            long* naCarry;

            NEW_ARRAY(naAccu,long,3 * newx + 3);
            NEW_ARRAY(naCarry,long,3 * newx + 3);

            long* naTemp;
            long  nWeightX,nWeightY;
            float fEndX;
            long nScale = (long)(ACCURACY * xScale * yScale);

            memset(naAccu,  0, sizeof(long) * 3 * newx);
            memset(naCarry, 0, sizeof(long) * 3 * newx);

            int u, v = 0; // coordinates in dest image
            float fEndY = yScale - 1.0f;
            for (y = 0; y < head.biHeight; y++){
                info.nProgress = (long)(100*y/head.biHeight); //<Anatoly Ivasyuk>
                if (info.nEscape) break;
                pSource = info.pImage + y * info.dwEffWidth;
                u = i = 0;
                fEndX = xScale - 1.0f;
                if ((float)y < fEndY) {       // complete source row goes into dest row
                    for (x = 0; x < head.biWidth; x++){
                        if ((float)x < fEndX){       // complete source pixel goes into dest pixel
                            for (j = 0; j < 3; j++) naAccu[i + j] += (*pSource++) * ACCURACY;
                        } else {       // source pixel is splitted for 2 dest pixels
                            nWeightX = (long)(((float)x - fEndX) * ACCURACY);
                            for (j = 0; j < 3; j++){
                                naAccu[i] += (ACCURACY - nWeightX) * (*pSource);
                                naAccu[3 + i++] += nWeightX * (*pSource++);
                            }
                            fEndX += xScale;
                            u++;
                        }
                    }
                } else {       // source row is splitted for 2 dest rows       
                    nWeightY = (long)(((float)y - fEndY) * ACCURACY);
                    for (x = 0; x < head.biWidth; x++){
                        if ((float)x < fEndX){       // complete source pixel goes into 2 pixel
                            for (j = 0; j < 3; j++){
                                naAccu[i + j] += ((ACCURACY - nWeightY) * (*pSource));
                                naCarry[i + j] += nWeightY * (*pSource++);
                            }
                        } else {       // source pixel is splitted for 4 dest pixels
                            nWeightX = (int)(((float)x - fEndX) * ACCURACY);
                            for (j = 0; j < 3; j++) {
                                naAccu[i] += ((ACCURACY - nWeightY) * (ACCURACY - nWeightX)) * (*pSource) / ACCURACY;
                                *pDest++ = (BYTE)(naAccu[i] / nScale);
                                naCarry[i] += (nWeightY * (ACCURACY - nWeightX) * (*pSource)) / ACCURACY;
                                naAccu[i + 3] += ((ACCURACY - nWeightY) * nWeightX * (*pSource)) / ACCURACY;
                                naCarry[i + 3] = (nWeightY * nWeightX * (*pSource)) / ACCURACY;
                                i++;
                                pSource++;
                            }
                            fEndX += xScale;
                            u++;
                        }
                    }
                    if (u < newx){ // possibly not completed due to rounding errors
                        for (j = 0; j < 3; j++) *pDest++ = (BYTE)(naAccu[i++] / nScale);
                    }
                    naTemp = naCarry;
                    naCarry = naAccu;
                    naAccu = naTemp;
                    memset(naCarry, 0, sizeof(int) * 3);    // need only to set first pixel zero
                    pDest = newImage.info.pImage + (++v * newImage.info.dwEffWidth);
                    fEndY += yScale;
                }
            }
            if (v < newy){  // possibly not completed due to rounding errors
                for (i = 0; i < 3 * newx; i++) *pDest++ = (BYTE)(naAccu[i] / nScale);
            }
            DEL_ARRAY(naAccu);
            DEL_ARRAY(naCarry);
        }
    }

#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()){
        newImage.AlphaCreate();
        for(long y=0; y<newy; y++){
            fY = y * yScale;
            for(long x=0; x<newx; x++){
                fX = x * xScale;
                newImage.AlphaSet(x,y,AlphaGet((long)fX,(long)fY));
            }
        }
    }
#endif //CXIMAGE_SUPPORT_ALPHA

    //select the destination
    if (iDst) iDst->Transfer(&newImage,TRUE);
    else Transfer(&newImage,TRUE);

    return TRUE;
}

BOOL CxImage::Resample2(long newx, long newy, int inMethod, int ofMethod, CxImage *iDst, BOOL disableAveraging)
{
    if (newx<=0 || newy<=0 || !pDib) return FALSE;
    
    if (head.biWidth==newx && head.biHeight==newy) {
        //image already correct size (just copy and return)
        if (iDst) iDst->Copy(this);
        return TRUE;
    }//if
    
    //calculate scale of new image (less than 1 for enlarge)
    float xScale, yScale;
    xScale = (float)head.biWidth  / (float)newx;    
    yScale = (float)head.biHeight / (float)newy;
    
    //create temporary destination image
    CxImage newImage;
    newImage.Init();
    newImage.CopyInfo(this);
    newImage.Create(newx,newy,head.biBitCount,GetType());
    newImage.SetPalette(GetPalette(),256);
    if (!newImage.IsValid()){
        strcpy(info.szLastError,newImage.GetLastError());
        return FALSE;
    }
    
    //and alpha channel if required
#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()) newImage.AlphaCreate();
    BYTE *pxptra = 0;   // destination alpha data
#endif
    
    float sX, sY;         //source location
    long dX,dY;           //destination pixel (int value)
    if ((xScale<=1 && yScale<=1) || disableAveraging) {
        //image is being enlarged (or interpolation on demand)
        if (!IsIndexed()) {
            //RGB24 image (optimized version with direct writes)
            RGBQUAD q;              //pixel colour
            BYTE *pxptr;            //pointer to destination pixel
            for(dY=0; dY<newy; dY++){
                info.nProgress = (long)(100*dY/newy);
                if (info.nEscape) break;
                sY = (dY + 0.5f) * yScale - 0.5f;
                pxptr=(BYTE*)(newImage.BlindGetPixelPointer(0,dY));
#if CXIMAGE_SUPPORT_ALPHA
                pxptra=newImage.AlphaGetPointer(0,dY);
#endif
                for(dX=0; dX<newx; dX++){
                    sX = (dX + 0.5f) * xScale - 0.5f;
                    q=GetPixelColorInterpolated(sX,sY,inMethod,ofMethod,0);
                    *pxptr++=q.rgbBlue;
                    *pxptr++=q.rgbGreen;
                    *pxptr++=q.rgbRed;
#if CXIMAGE_SUPPORT_ALPHA
                    if (pxptra) *pxptra++=q.rgbReserved;
#endif
                }//for dX
            }//for dY
        } else {
            //enlarge paletted image. Slower method.
            for(dY=0; dY<newy; dY++){
                info.nProgress = (long)(100*dY/newy);
                if (info.nEscape) break;
                sY = (dY + 0.5f) * yScale - 0.5f;
                for(dX=0; dX<newx; dX++){
                    sX = (dX + 0.5f) * xScale - 0.5f;
                    newImage.SetPixelColor(dX,dY,GetPixelColorInterpolated(sX,sY,inMethod,ofMethod,0),TRUE);
                }//for x
            }//for y
        }//if
    } else {
        //image size is being reduced (averaging enabled)
        for(dY=0; dY<newy; dY++){
            info.nProgress = (long)(100*dY/newy); if (info.nEscape) break;
            sY = (dY+0.5f) * yScale - 0.5f;
            for(dX=0; dX<newx; dX++){
                sX = (dX+0.5f) * xScale - 0.5f;
                newImage.SetPixelColor(dX,dY,GetAreaColorInterpolated(sX, sY, xScale, yScale, inMethod, ofMethod,0),TRUE);
            }//for x
        }//for y
    }//if

#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid() && pxptra == 0){
        for(long y=0; y<newy; y++){
            dY = (long)(y * yScale);
            for(long x=0; x<newx; x++){
                dX = (long)(x * xScale);
                newImage.AlphaSet(x,y,AlphaGet(dX,dY));
            }
        }
    }
#endif //CXIMAGE_SUPPORT_ALPHA

    //copy new image to the destination
    if (iDst) 
        iDst->Transfer(&newImage,TRUE);
    else 
        Transfer(&newImage,TRUE);
    return TRUE;
}

BOOL CxImage::DecreaseBpp(DWORD nbit, BOOL errordiffusion, RGBQUAD *ppal, DWORD clrimportant)
{
    if (!pDib) return FALSE;
    if (head.biBitCount <  nbit){
        strcpy(info.szLastError,"DecreaseBpp: target BPP greater than source BPP");
        return FALSE;
    }
    if (head.biBitCount == nbit){
        if (clrimportant==0) return TRUE;
        if (head.biClrImportant && (head.biClrImportant<clrimportant)) return TRUE;
    }

    long er,eg,eb;
    RGBQUAD c,ce;

    CxImage tmp;
    tmp.Init();
    tmp.CopyInfo(this);
    tmp.Create(head.biWidth,head.biHeight,(WORD)nbit,info.dwType);
    if (clrimportant) tmp.SetClrImportant(clrimportant);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

#if CXIMAGE_SUPPORT_SELECTION
    tmp.SelectionCopy(this);
#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
    tmp.AlphaCopy(this);
#endif //CXIMAGE_SUPPORT_ALPHA

    if (ppal) {
        if (clrimportant) {
            tmp.SetPalette(ppal,clrimportant);
        } else {
            tmp.SetPalette(ppal,1<<tmp.head.biBitCount);
        }
    } else {
        tmp.SetStdPalette();
    }

    for (long y=0;y<head.biHeight;y++){
        if (info.nEscape) break;
        info.nProgress = (long)(100*y/head.biHeight);
        for (long x=0;x<head.biWidth;x++){
            if (!errordiffusion){
                tmp.BlindSetPixelColor(x,y,BlindGetPixelColor(x,y,TRUE),FALSE);
            } else {
                c = BlindGetPixelColor(x,y,TRUE);
                tmp.BlindSetPixelColor(x,y,c,FALSE);

                ce = tmp.BlindGetPixelColor(x,y,TRUE);
                er=(long)c.rgbRed - (long)ce.rgbRed;
                eg=(long)c.rgbGreen - (long)ce.rgbGreen;
                eb=(long)c.rgbBlue - (long)ce.rgbBlue;

                c = GetPixelColor(x+1,y,TRUE);
                c.rgbRed = (BYTE)min(255L,max(0L,(long)c.rgbRed + ((er*7)/16)));
                c.rgbGreen = (BYTE)min(255L,max(0L,(long)c.rgbGreen + ((eg*7)/16)));
                c.rgbBlue = (BYTE)min(255L,max(0L,(long)c.rgbBlue + ((eb*7)/16)));
                SetPixelColor(x+1,y,c,TRUE);
                int coeff=1;
                for(int i=-1; i<2; i++){
                    switch(i){
                    case -1:
                        coeff=2; break;
                    case 0:
                        coeff=4; break;
                    case 1:
                        coeff=1; break;
                    }
                    c = GetPixelColor(x+i,y+1,TRUE);
                    c.rgbRed = (BYTE)min(255L,max(0L,(long)c.rgbRed + ((er * coeff)/16)));
                    c.rgbGreen = (BYTE)min(255L,max(0L,(long)c.rgbGreen + ((eg * coeff)/16)));
                    c.rgbBlue = (BYTE)min(255L,max(0L,(long)c.rgbBlue + ((eb * coeff)/16)));
                    SetPixelColor(x+i,y+1,c,FALSE);
                }
            }
        }
    }

    Transfer(&tmp,TRUE);
    return TRUE;
}

BOOL CxImage::IncreaseBpp(DWORD nbit)
{
    if (!pDib) return FALSE;
    switch (nbit){
    case 4:
        {
            if (head.biBitCount==4) return TRUE;
            if (head.biBitCount>4) return FALSE;

            CxImage tmp;
            tmp.Init();
            tmp.CopyInfo(this);
            tmp.Create(head.biWidth,head.biHeight,4,info.dwType);
            tmp.SetPalette(GetPalette(),GetNumColors());
            if (!tmp.IsValid()){
                strcpy(info.szLastError,tmp.GetLastError());
                return FALSE;
            }


#if CXIMAGE_SUPPORT_SELECTION
            tmp.SelectionCopy(this);
#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
            tmp.AlphaCopy(this);
#endif //CXIMAGE_SUPPORT_ALPHA

            for (long y=0;y<head.biHeight;y++){
                if (info.nEscape) break;
                for (long x=0;x<head.biWidth;x++){
                    tmp.BlindSetPixelIndex(x,y,BlindGetPixelIndex(x,y));
                }
            }
            Transfer(&tmp,TRUE);
            return TRUE;
        }
    case 8:
        {
            if (head.biBitCount==8) return TRUE;
            if (head.biBitCount>8) return FALSE;

            CxImage tmp;
            tmp.CopyInfo(this);
            tmp.Create(head.biWidth,head.biHeight,8,info.dwType);
            tmp.SetPalette(GetPalette(),GetNumColors());
            if (!tmp.IsValid()){
                strcpy(info.szLastError,tmp.GetLastError());
                return FALSE;
            }

#if CXIMAGE_SUPPORT_SELECTION
            tmp.SelectionCopy(this);
#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
            tmp.AlphaCopy(this);
#endif //CXIMAGE_SUPPORT_ALPHA

            for (long y=0;y<head.biHeight;y++){
                if (info.nEscape) break;
                for (long x=0;x<head.biWidth;x++){
                    tmp.BlindSetPixelIndex(x,y,BlindGetPixelIndex(x,y));
                }
            }
            Transfer(&tmp,TRUE);
            return TRUE;
        }
    case 24:
        {
            if (head.biBitCount==24) return TRUE;
            if (head.biBitCount>24) return FALSE;

            CxImage tmp;
            tmp.Init();
            tmp.CopyInfo(this);
            tmp.Create(head.biWidth,head.biHeight,24,info.dwType);
            if (!tmp.IsValid()){
                strcpy(info.szLastError,tmp.GetLastError());
                return FALSE;
            }

            if (info.nBkgndIndex>=0) //translate transparency
                tmp.info.nBkgndColor=GetPaletteColor((BYTE)info.nBkgndIndex);

#if CXIMAGE_SUPPORT_SELECTION
            tmp.SelectionCopy(this);
#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
            tmp.AlphaCopy(this);
            if (AlphaPaletteIsValid() && !AlphaIsValid()) tmp.AlphaCreate();
#endif //CXIMAGE_SUPPORT_ALPHA

            for (long y=0;y<head.biHeight;y++){
                if (info.nEscape) break;
                for (long x=0;x<head.biWidth;x++){
                    tmp.BlindSetPixelColor(x,y,BlindGetPixelColor(x,y,TRUE),TRUE);
                }
            }
            Transfer(&tmp,TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CxImage::Dither(long method)
{
    if (!pDib) return FALSE;
    if (head.biBitCount == 1) return TRUE;
    
    GrayScale();

    CxImage tmp;
    tmp.CopyInfo(this);
    tmp.Create(head.biWidth, head.biHeight, 1, info.dwType);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

#if CXIMAGE_SUPPORT_SELECTION
    tmp.SelectionCopy(this);
#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
    tmp.AlphaCopy(this);
#endif //CXIMAGE_SUPPORT_ALPHA

    switch (method){
    case 1:
    {
        // Multi-Level Ordered-Dithering by Kenny Hoff (Oct. 12, 1995)
        #define dth_NumRows 4
        #define dth_NumCols 4
        #define dth_NumIntensityLevels 2
        #define dth_NumRowsLessOne (dth_NumRows-1)
        #define dth_NumColsLessOne (dth_NumCols-1)
        #define dth_RowsXCols (dth_NumRows*dth_NumCols)
        #define dth_MaxIntensityVal 255
        #define dth_MaxDitherIntensityVal (dth_NumRows*dth_NumCols*(dth_NumIntensityLevels-1))

        int DitherMatrix[dth_NumRows][dth_NumCols] = {{0,8,2,10}, {12,4,14,6}, {3,11,1,9}, {15,7,13,5} };
        
        unsigned char Intensity[dth_NumIntensityLevels] = { 0,1 };                       // 2 LEVELS B/W
        //unsigned char Intensity[NumIntensityLevels] = { 0,255 };                       // 2 LEVELS
        //unsigned char Intensity[NumIntensityLevels] = { 0,127,255 };                   // 3 LEVELS
        //unsigned char Intensity[NumIntensityLevels] = { 0,85,170,255 };                // 4 LEVELS
        //unsigned char Intensity[NumIntensityLevels] = { 0,63,127,191,255 };            // 5 LEVELS
        //unsigned char Intensity[NumIntensityLevels] = { 0,51,102,153,204,255 };        // 6 LEVELS
        //unsigned char Intensity[NumIntensityLevels] = { 0,42,85,127,170,213,255 };     // 7 LEVELS
        //unsigned char Intensity[NumIntensityLevels] = { 0,36,73,109,145,182,219,255 }; // 8 LEVELS
        int DitherIntensity, DitherMatrixIntensity, Offset, DeviceIntensity;
        unsigned char DitherValue;
  
        for (long y=0;y<head.biHeight;y++){
            info.nProgress = (long)(100*y/head.biHeight);
            if (info.nEscape) break;
            for (long x=0;x<head.biWidth;x++){

                DeviceIntensity = BlindGetPixelIndex(x,y);
                DitherIntensity = DeviceIntensity*dth_MaxDitherIntensityVal/dth_MaxIntensityVal;
                DitherMatrixIntensity = DitherIntensity % dth_RowsXCols;
                Offset = DitherIntensity / dth_RowsXCols;
                if (DitherMatrix[y&dth_NumRowsLessOne][x&dth_NumColsLessOne] < DitherMatrixIntensity)
                    DitherValue = Intensity[1+Offset];
                else
                    DitherValue = Intensity[0+Offset];

                tmp.BlindSetPixelIndex(x,y,DitherValue);
            }
        }
        break;
    }
    case 2:
    {
        //Burkes error diffusion (Thanks to Franco Gerevini)
        int TotalCoeffSum = 32;
        long error, nlevel, coeff=1;
        BYTE level;

        for (long y = 0; y < head.biHeight; y++) {
            info.nProgress = (long)(100 * y / head.biHeight);
            if (info.nEscape) 
                break;
            for (long x = 0; x < head.biWidth; x++) {
                level = BlindGetPixelIndex(x, y);
                if (level > 128) {
                    tmp.SetPixelIndex(x, y, 1);
                    error = level - 255;
                } else {
                    tmp.SetPixelIndex(x, y, 0);
                    error = level;
                }

                nlevel = GetPixelIndex(x + 1, y) + (error * 8) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 1, y, level);
                nlevel = GetPixelIndex(x + 2, y) + (error * 4) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 2, y, level);
                int i;
                for (i = -2; i < 3; i++) {
                    switch (i) {
                    case -2:
                        coeff = 2;
                        break;
                    case -1:
                        coeff = 4;
                        break;
                    case 0:
                        coeff = 8; 
                        break;
                    case 1:
                        coeff = 4; 
                        break;
                    case 2:
                        coeff = 2; 
                        break;
                    }
                    nlevel = GetPixelIndex(x + i, y + 1) + (error * coeff) / TotalCoeffSum;
                    level = (BYTE)min(255, max(0, (int)nlevel));
                    SetPixelIndex(x + i, y + 1, level);
                }
            }
        }
        break;
    }
    case 3:
    {
        //Stucki error diffusion (Thanks to Franco Gerevini)
        int TotalCoeffSum = 42;
        long error, nlevel, coeff=1;
        BYTE level;

        for (long y = 0; y < head.biHeight; y++) {
            info.nProgress = (long)(100 * y / head.biHeight);
            if (info.nEscape) 
                break;
            for (long x = 0; x < head.biWidth; x++) {
                level = BlindGetPixelIndex(x, y);
                if (level > 128) {
                    tmp.SetPixelIndex(x, y, 1);
                    error = level - 255;
                } else {
                    tmp.SetPixelIndex(x, y, 0);
                    error = level;
                }

                nlevel = GetPixelIndex(x + 1, y) + (error * 8) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 1, y, level);
                nlevel = GetPixelIndex(x + 2, y) + (error * 4) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 2, y, level);
                int i;
                for (i = -2; i < 3; i++) {
                    switch (i) {
                    case -2:
                        coeff = 2;
                        break;
                    case -1:
                        coeff = 4;
                        break;
                    case 0:
                        coeff = 8; 
                        break;
                    case 1:
                        coeff = 4; 
                        break;
                    case 2:
                        coeff = 2; 
                        break;
                    }
                    nlevel = GetPixelIndex(x + i, y + 1) + (error * coeff) / TotalCoeffSum;
                    level = (BYTE)min(255, max(0, (int)nlevel));
                    SetPixelIndex(x + i, y + 1, level);
                }
                for (i = -2; i < 3; i++) {
                    switch (i) {
                    case -2:
                        coeff = 1;
                        break;
                    case -1:
                        coeff = 2;
                        break;
                    case 0:
                        coeff = 4; 
                        break;
                    case 1:
                        coeff = 2; 
                        break;
                    case 2:
                        coeff = 1; 
                        break;
                    }
                    nlevel = GetPixelIndex(x + i, y + 2) + (error * coeff) / TotalCoeffSum;
                    level = (BYTE)min(255, max(0, (int)nlevel));
                    SetPixelIndex(x + i, y + 2, level);
                }
            }
        }
        break;
    }
    case 4:
    {
        //Jarvis, Judice and Ninke error diffusion (Thanks to Franco Gerevini)
        int TotalCoeffSum = 48;
        long error, nlevel, coeff=1;
        BYTE level;

        for (long y = 0; y < head.biHeight; y++) {
            info.nProgress = (long)(100 * y / head.biHeight);
            if (info.nEscape) 
                break;
            for (long x = 0; x < head.biWidth; x++) {
                level = BlindGetPixelIndex(x, y);
                if (level > 128) {
                    tmp.SetPixelIndex(x, y, 1);
                    error = level - 255;
                } else {
                    tmp.SetPixelIndex(x, y, 0);
                    error = level;
                }

                nlevel = GetPixelIndex(x + 1, y) + (error * 7) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 1, y, level);
                nlevel = GetPixelIndex(x + 2, y) + (error * 5) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 2, y, level);
                int i;
                for (i = -2; i < 3; i++) {
                    switch (i) {
                    case -2:
                        coeff = 3;
                        break;
                    case -1:
                        coeff = 5;
                        break;
                    case 0:
                        coeff = 7; 
                        break;
                    case 1:
                        coeff = 5; 
                        break;
                    case 2:
                        coeff = 3; 
                        break;
                    }
                    nlevel = GetPixelIndex(x + i, y + 1) + (error * coeff) / TotalCoeffSum;
                    level = (BYTE)min(255, max(0, (int)nlevel));
                    SetPixelIndex(x + i, y + 1, level);
                }
                for (i = -2; i < 3; i++) {
                    switch (i) {
                    case -2:
                        coeff = 1;
                        break;
                    case -1:
                        coeff = 3;
                        break;
                    case 0:
                        coeff = 5; 
                        break;
                    case 1:
                        coeff = 3; 
                        break;
                    case 2:
                        coeff = 1; 
                        break;
                    }
                    nlevel = GetPixelIndex(x + i, y + 2) + (error * coeff) / TotalCoeffSum;
                    level = (BYTE)min(255, max(0, (int)nlevel));
                    SetPixelIndex(x + i, y + 2, level);
                }
            }
        }
        break;
    }
    case 5:
    {
        //Sierra error diffusion (Thanks to Franco Gerevini)
        int TotalCoeffSum = 32;
        long error, nlevel, coeff=1;
        BYTE level;

        for (long y = 0; y < head.biHeight; y++) {
            info.nProgress = (long)(100 * y / head.biHeight);
            if (info.nEscape) 
                break;
            for (long x = 0; x < head.biWidth; x++) {
                level = BlindGetPixelIndex(x, y);
                if (level > 128) {
                    tmp.SetPixelIndex(x, y, 1);
                    error = level - 255;
                } else {
                    tmp.SetPixelIndex(x, y, 0);
                    error = level;
                }

                nlevel = GetPixelIndex(x + 1, y) + (error * 5) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 1, y, level);
                nlevel = GetPixelIndex(x + 2, y) + (error * 3) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(x + 2, y, level);
                int i;
                for (i = -2; i < 3; i++) {
                    switch (i) {
                    case -2:
                        coeff = 2;
                        break;
                    case -1:
                        coeff = 4;
                        break;
                    case 0:
                        coeff = 5; 
                        break;
                    case 1:
                        coeff = 4; 
                        break;
                    case 2:
                        coeff = 2; 
                        break;
                    }
                    nlevel = GetPixelIndex(x + i, y + 1) + (error * coeff) / TotalCoeffSum;
                    level = (BYTE)min(255, max(0, (int)nlevel));
                    SetPixelIndex(x + i, y + 1, level);
                }
                for (i = -1; i < 2; i++) {
                    switch (i) {
                    case -1:
                        coeff = 2;
                        break;
                    case 0:
                        coeff = 3; 
                        break;
                    case 1:
                        coeff = 2; 
                        break;
                    }
                    nlevel = GetPixelIndex(x + i, y + 2) + (error * coeff) / TotalCoeffSum;
                    level = (BYTE)min(255, max(0, (int)nlevel));
                    SetPixelIndex(x + i, y + 2, level);
                }
            }
        }
        break;
    }
    case 6:
    {
        //Stevenson and Arce error diffusion (Thanks to Franco Gerevini)
        int TotalCoeffSum = 200;
        long error, nlevel;
        BYTE level;

        for (long y = 0; y < head.biHeight; y++) {
            info.nProgress = (long)(100 * y / head.biHeight);
            if (info.nEscape) 
                break;
            for (long x = 0; x < head.biWidth; x++) {
                level = BlindGetPixelIndex(x, y);
                if (level > 128) {
                    tmp.SetPixelIndex(x, y, 1);
                    error = level - 255;
                } else {
                    tmp.SetPixelIndex(x, y, 0);
                    error = level;
                }

                int tmp_index_x = x + 2;
                int tmp_index_y = y;
                int tmp_coeff = 32;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x - 3;
                tmp_index_y = y + 1;
                tmp_coeff = 12;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x - 1;
                tmp_coeff = 26;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x + 1;
                tmp_coeff = 30;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x + 3;
                tmp_coeff = 16;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x - 2;
                tmp_index_y = y + 2;
                tmp_coeff = 12;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x;
                tmp_coeff = 26;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x + 2;
                tmp_coeff = 12;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x - 3;
                tmp_index_y = y + 3;
                tmp_coeff = 5;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x - 1;
                tmp_coeff = 12;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x + 1;
                tmp_coeff = 12;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);

                tmp_index_x = x + 3;
                tmp_coeff = 5;
                nlevel = GetPixelIndex(tmp_index_x, tmp_index_y) + (error * tmp_coeff) / TotalCoeffSum;
                level = (BYTE)min(255, max(0, (int)nlevel));
                SetPixelIndex(tmp_index_x, tmp_index_y, level);
            }
        }
        break;
    }
    case 7:
    {
        // Bayer ordered dither
        int order = 4;
        //create Bayer matrix
        if (order>4) order = 4;
        int size = (1 << (2*order));
        
        BYTE* Bmatrix;
        MALLOC(Bmatrix,BYTE,size);

        for(int i = 0; i < size; i++) {
            int n = order;
            int x = i / n;
            int y = i % n;
            int dither = 0;
            while (n-- > 0){
                dither = (((dither<<1)|((x&1) ^ (y&1)))<<1) | (y&1);
                x >>= 1;
                y >>= 1;
            }
            Bmatrix[i] = (BYTE)(dither);
        }

        int scale = max(0,(8-2*order));
        int level;
        for (long y=0;y<head.biHeight;y++){
            info.nProgress = (long)(100*y/head.biHeight);
            if (info.nEscape) break;
            for (long x=0;x<head.biWidth;x++){
                level = BlindGetPixelIndex(x,y) >> scale;
                if(level > Bmatrix[ (x % order) + order * (y % order) ]){
                    tmp.SetPixelIndex(x,y,1);
                } else {
                    tmp.SetPixelIndex(x,y,0);
                }
            }
        }

        FREE(Bmatrix);

        break;
    }
    default:
    {
        // Floyd-Steinberg error diffusion (Thanks to Steve McMahon)
        long error,nlevel,coeff=1;
        BYTE level;

        for (long y=0;y<head.biHeight;y++){
            info.nProgress = (long)(100*y/head.biHeight);
            if (info.nEscape) break;
            for (long x=0;x<head.biWidth;x++){

                level = BlindGetPixelIndex(x,y);
                if (level > 128){
                    tmp.SetPixelIndex(x,y,1);
                    error = level-255;
                } else {
                    tmp.SetPixelIndex(x,y,0);
                    error = level;
                }

                nlevel = GetPixelIndex(x+1,y) + (error * 7)/16;
                level = (BYTE)min(255,max(0,(int)nlevel));
                SetPixelIndex(x+1,y,level);
                for(int i=-1; i<2; i++){
                    switch(i){
                    case -1:
                        coeff=3; break;
                    case 0:
                        coeff=5; break;
                    case 1:
                        coeff=1; break;
                    }
                    nlevel = GetPixelIndex(x+i,y+1) + (error * coeff)/16;
                    level = (BYTE)min(255,max(0,(int)nlevel));
                    SetPixelIndex(x+i,y+1,level);
                }
            }
        }
    }
    }

    tmp.SetPaletteColor(0,0,0,0,0);
    tmp.SetPaletteColor(1,255,255,255,0);
    Transfer(&tmp,TRUE);

    return TRUE;
}

BOOL CxImage::CropRotatedRectangle(long topx, long topy, long width, long height, float angle, CxImage *iDst)
{
    if (!pDib) return FALSE;
    
    long startx,starty,endx,endy;
    double cos_angle = cos(angle/*/57.295779513082320877*/);
    double sin_angle = sin(angle/*/57.295779513082320877*/);

    // if there is nothing special, call the original Crop():
    if ( fabs(angle)<0.0002 )
        return Crop( topx, topy, topx+width, topy+height, iDst);

    startx = min(topx, topx - (long)(sin_angle*(double)height));
    endx   = topx + (long)(cos_angle*(double)width);
    endy   = topy + (long)(cos_angle*(double)height + sin_angle*(double)width);
    // check: corners of the rectangle must be inside
    if ( IsInside( startx, topy )==FALSE ||
         IsInside( endx, endy ) == FALSE )
         return FALSE;

    // first crop to bounding rectangle
    CxImage tmp(this, TRUE, FALSE, TRUE);
    // tmp.Copy(*this, TRUE, FALSE, TRUE);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }
    if (!tmp.Crop( startx, topy, endx, endy,NULL)){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }
    
    // the midpoint of the image now became the same as the midpoint of the rectangle
    // rotate new image with minus angle amount
    if ( FALSE == tmp.Rotate( (float)(-angle*57.295779513082320877), NULL ) ) // Rotate expects angle in degrees
        return FALSE;

    // crop rotated image to the original selection rectangle
    endx   = (tmp.head.biWidth+width)/2;
    startx = (tmp.head.biWidth-width)/2;
    starty = (tmp.head.biHeight+height)/2;
    endy   = (tmp.head.biHeight-height)/2;
    if ( FALSE == tmp.Crop( startx, starty, endx, endy ,NULL) )
        return FALSE;

    if (iDst) iDst->Transfer(&tmp,1);
    else Transfer(&tmp,1);

    return TRUE;
}

BOOL CxImage::Crop(const RECT &rect, CxImage *iDst)
{
    return Crop(rect.left, rect.top, rect.right, rect.bottom, iDst);
}

BOOL CxImage::Crop(long left, long top, long right, long bottom, CxImage *iDst)
{
    if (!pDib) return FALSE;

    long startx = max(0L,min(left,head.biWidth));
    long endx = max(0L,min(right,head.biWidth));
    long starty = head.biHeight - max(0L,min(top,head.biHeight));
    long endy = head.biHeight - max(0L,min(bottom,head.biHeight));

    if (startx==endx || starty==endy) return FALSE;

    if (startx>endx) {long tmp=startx; startx=endx; endx=tmp;}
    if (starty>endy) {long tmp=starty; starty=endy; endy=tmp;}

    CxImage tmp(endx-startx,endy-starty,head.biBitCount,info.dwType);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    tmp.SetPalette(GetPalette(),head.biClrUsed);
    tmp.info.nBkgndIndex = info.nBkgndIndex;
    tmp.info.nBkgndColor = info.nBkgndColor;

    switch (head.biBitCount) {
    case 1:
    case 4:
    {
        for(long y=starty, yd=0; y<endy; y++, yd++){
            info.nProgress = (long)(100*(y-starty)/(endy-starty)); //<Anatoly Ivasyuk>
            for(long x=startx, xd=0; x<endx; x++, xd++){
                tmp.SetPixelIndex(xd,yd,GetPixelIndex(x,y));
            }
        }
        break;
    }
    case 8:
    case 24:
    {
        int linelen = tmp.head.biWidth * tmp.head.biBitCount >> 3;
        BYTE* pDest = tmp.info.pImage;
        BYTE* pSrc = info.pImage + starty * info.dwEffWidth + (startx*head.biBitCount >> 3);
        for(long y=starty; y<endy; y++){
            info.nProgress = (long)(100*(y-starty)/(endy-starty)); //<Anatoly Ivasyuk>
            memcpy(pDest,pSrc,linelen);
            pDest+=tmp.info.dwEffWidth;
            pSrc+=info.dwEffWidth;
        }
    }
    }

#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()){ //<oboolo>
        tmp.AlphaCreate();
        if (!tmp.AlphaIsValid()) return FALSE;
        BYTE* pDest = tmp.pAlpha;
        BYTE* pSrc = pAlpha + startx + starty*head.biWidth;
        for (long y=starty; y<endy; y++){
            memcpy(pDest,pSrc,endx-startx);
            pDest+=tmp.head.biWidth;
            pSrc+=head.biWidth;
        }
    }
#endif //CXIMAGE_SUPPORT_ALPHA

    //select the destination
    if (iDst) iDst->Transfer(&tmp,TRUE);
    else Transfer(&tmp,TRUE);

    return TRUE;

}

BOOL CxImage::Skew(float xgain, float ygain, long xpivot, long ypivot, BOOL bEnableInterpolation)
{
    if (!pDib) return FALSE;
    float nx,ny;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }
    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif //CXIMAGE_SUPPORT_SELECTION
            {
                nx = x + (xgain*(y - ypivot));
                ny = y + (ygain*(x - xpivot));
#if CXIMAGE_SUPPORT_INTERPOLATION
                if (bEnableInterpolation){
                    tmp.SetPixelColor(x,y,GetPixelColorInterpolated(nx, ny, IM_BILINEAR, OM_BACKGROUND,0),TRUE);
                } else
#endif //CXIMAGE_SUPPORT_INTERPOLATION
                {
                    if (head.biClrUsed==0){
                        tmp.SetPixelColor(x,y,GetPixelColor((long)nx,(long)ny,TRUE),FALSE);
                    } else {
                        tmp.SetPixelIndex(x,y,GetPixelIndex((long)nx,(long)ny));
                    }
#if CXIMAGE_SUPPORT_ALPHA
                    tmp.AlphaSet(x,y,AlphaGet((long)nx,(long)ny));
#endif //CXIMAGE_SUPPORT_ALPHA
                }
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}

BOOL CxImage::Expand(long left, long top, long right, long bottom, RGBQUAD canvascolor, CxImage *iDst)
{
   if (!pDib) return FALSE;

    if ((left < 0) || (right < 0) || (bottom < 0) || (top < 0)) return FALSE;

    long newWidth = head.biWidth + left + right;
    long newHeight = head.biHeight + top + bottom;

    right = left + head.biWidth - 1;
    top = bottom + head.biHeight - 1;
    
    CxImage tmp;
    tmp.CopyInfo(this);
    if (!tmp.Create(newWidth, newHeight, head.biBitCount, info.dwType)){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    tmp.SetPalette(GetPalette(),head.biClrUsed);

    switch (head.biBitCount) {
    case 1:
    case 4:
        {
            BYTE pixel = tmp.GetNearestIndex(canvascolor);
            for(long y=0; y < newHeight; y++){
                info.nProgress = (long)(100*y/newHeight);
                for(long x=0; x < newWidth; x++){
                    if ((y < bottom) || (y > top) || (x < left) || (x > right)) {
                        tmp.SetPixelIndex(x,y, pixel);
                    } else {
                        tmp.SetPixelIndex(x,y,GetPixelIndex(x-left,y-bottom));
                    }
                }
            }
            break;
        }
    case 8:
    case 24:
        {
            if (head.biBitCount == 8) {
                BYTE pixel = tmp.GetNearestIndex( canvascolor);
                memset(tmp.info.pImage, pixel,  + (tmp.info.dwEffWidth * newHeight));
            } else {
                for (long y = 0; y < newHeight; ++y) {
                    BYTE *pDest = tmp.info.pImage + (y * tmp.info.dwEffWidth);
                    for (long x = 0; x < newWidth; ++x) {
                        *pDest++ = canvascolor.rgbBlue;
                        *pDest++ = canvascolor.rgbGreen;
                        *pDest++ = canvascolor.rgbRed;
                    }
                }
            }

            BYTE* pDest = tmp.info.pImage + (tmp.info.dwEffWidth * bottom) + (left*(head.biBitCount >> 3));
            BYTE* pSrc = info.pImage;
            for(long y=bottom; y <= top; y++){
                info.nProgress = (long)(100*y/(1 + top - bottom));
                memcpy(pDest,pSrc,(head.biBitCount >> 3) * (right - left + 1));
                pDest+=tmp.info.dwEffWidth;
                pSrc+=info.dwEffWidth;
            }
        }
    }

#if CXIMAGE_SUPPORT_SELECTION
    if (SelectionIsValid()){
        if (!tmp.SelectionCreate())
            return FALSE;
        BYTE* pSrc = SelectionGetPointer(0,0);
        BYTE* pDst = tmp.SelectionGetPointer(left,bottom);
        for(long y=bottom; y <= top; y++){
            memcpy(pDst,pSrc, (right - left + 1));
            pSrc+=head.biWidth;
            pDst+=tmp.head.biWidth;
        }
        tmp.info.rSelectionBox.left = info.rSelectionBox.left + left;
        tmp.info.rSelectionBox.right = info.rSelectionBox.right + left;
        tmp.info.rSelectionBox.top = info.rSelectionBox.top + bottom;
        tmp.info.rSelectionBox.bottom = info.rSelectionBox.bottom + bottom;
    }
#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()){
        if (!tmp.AlphaCreate())
            return FALSE;
        tmp.AlphaSet(canvascolor.rgbReserved);
        BYTE* pSrc = AlphaGetPointer(0,0);
        BYTE* pDst = tmp.AlphaGetPointer(left,bottom);
        for(long y=bottom; y <= top; y++){
            memcpy(pDst,pSrc, (right - left + 1));
            pSrc+=head.biWidth;
            pDst+=tmp.head.biWidth;
        }
    }
#endif //CXIMAGE_SUPPORT_ALPHA

    //select the destination
    if (iDst) iDst->Transfer(&tmp,TRUE);
    else Transfer(&tmp,TRUE);

    return TRUE;
}

BOOL CxImage::Expand(long newx, long newy, RGBQUAD canvascolor, CxImage *iDst)
{
    //thanks to <Colin Urquhart>

    if (!pDib) return FALSE;

    if ((newx < head.biWidth) || (newy < head.biHeight)) return FALSE;

    int nAddLeft = (newx - head.biWidth) / 2;
    int nAddTop = (newy - head.biHeight) / 2;

    return Expand(nAddLeft, nAddTop, newx - (head.biWidth + nAddLeft), newy - (head.biHeight + nAddTop), canvascolor, iDst);
}

BOOL CxImage::Thumbnail(long newx, long newy, RGBQUAD canvascolor, CxImage *iDst)
{
   if (!pDib) return FALSE;

    if ((newx <= 0) || (newy <= 0)) return FALSE;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    // determine whether we need to shrink the image
    if ((head.biWidth > newx) || (head.biHeight > newy)) {
        float fScale;
        float fAspect = (float) newx / (float) newy;
        if (fAspect * head.biHeight > head.biWidth) {
            fScale = (float) newy / head.biHeight;
        } else {
            fScale = (float) newx / head.biWidth;
        }
        tmp.Resample((long) (fScale * head.biWidth), (long) (fScale * head.biHeight), 0,NULL);
    }

    // expand the frame
    tmp.Expand(newx, newy, canvascolor, iDst);

    //select the destination
    if (iDst) iDst->Transfer(&tmp,TRUE);
    else Transfer(&tmp,TRUE);
    return TRUE;
}

BOOL CxImage::CircleTransform(int type, long rmax, float Koeff)
{
    if (!pDib) return FALSE;

    long nx,ny;
    double angle,radius,rnew;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax,xmid,ymid;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }
    
    xmid = (long) (tmp.GetWidth()/2);
    ymid = (long) (tmp.GetHeight()/2);

    if (!rmax) rmax=(long)sqrt((float)((xmid-xmin)*(xmid-xmin)+(ymid-ymin)*(ymid-ymin)));
    if (Koeff==0.0f) Koeff=1.0f;

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif //CXIMAGE_SUPPORT_SELECTION
            {
                nx=xmid-x;
                ny=ymid-y;
                radius=sqrt((float)(nx*nx+ny*ny));
                if (radius<rmax) {
                    angle=atan2((double)ny,(double)nx);
                    if (type==0)      rnew=radius*radius/rmax;
                    else if (type==1) rnew=sqrt(radius*rmax);
                    else if (type==2) {rnew=radius;angle += radius / Koeff;}
                    else rnew = 1; // potentially uninitialized
                    if (type<3){
                        nx = xmid + (long)(rnew * cos(angle));
                        ny = ymid - (long)(rnew * sin(angle));
                    }
                    else if (type==3){
                        nx = (long)fabs((angle*xmax/6.2831852));
                        ny = (long)fabs((radius*ymax/rmax));
                    }
                    else {
                        nx=x+(x%32)-16;
                        ny=y;
                    }
//                  nx=max(xmin,min(nx,xmax));
//                  ny=max(ymin,min(ny,ymax));
                }
                else { nx=-1;ny=-1;}
                if (head.biClrUsed==0){
                    tmp.SetPixelColor(x,y,GetPixelColor(nx,ny,TRUE),FALSE);
                } else {
                    tmp.SetPixelIndex(x,y,GetPixelIndex(nx,ny));
                }
#if CXIMAGE_SUPPORT_ALPHA
                tmp.AlphaSet(x,y,AlphaGet(nx,ny));
#endif //CXIMAGE_SUPPORT_ALPHA
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}

BOOL CxImage::QIShrink(long newx, long newy, CxImage *const iDst, BOOL bChangeBpp)
{
    if (!pDib) return FALSE;
    
    if (newx>head.biWidth || newy>head.biHeight) { 
        //let me repeat... this method can't enlarge image
        strcpy(info.szLastError,"QIShrink can't enlarge image");
        return FALSE;
    }

    if (newx==head.biWidth && newy==head.biHeight) {
        //image already correct size (just copy and return)
        if (iDst) iDst->Copy(this);
        return TRUE;
    }//if
    
    //create temporary destination image
    CxImage newImage;
    newImage.CopyInfo(this);
    newImage.Create(newx,newy,(bChangeBpp)?24:head.biBitCount,GetType());
    newImage.SetPalette(GetPalette(),256);
    if (!newImage.IsValid()){
        strcpy(info.szLastError,newImage.GetLastError());
        return FALSE;
    }

    //and alpha channel if required
#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()) newImage.AlphaCreate();
#endif

    const int oldx = head.biWidth;
    const int oldy = head.biHeight;

    int accuCellSize = 4;
#if CXIMAGE_SUPPORT_ALPHA
    BYTE *alphaPtr;
    if (AlphaIsValid()) accuCellSize=5;
#endif

    unsigned int *accu; //array for suming pixels... one pixel for every destination column
    NEW_ARRAY(accu,unsigned int,newx*accuCellSize);
    unsigned int *accuPtr;                              //pointer for walking through accu
    //each cell consists of blue, red, green component and count of pixels summed in this cell
    memset(accu, 0, newx * accuCellSize * sizeof(unsigned int));  //clear accu

    if (!IsIndexed()) {
        //RGB24 version with pointers
        BYTE *destPtr, *srcPtr, *destPtrS, *srcPtrS;        //destination and source pixel, and beginnings of current row
        srcPtrS=(BYTE*)BlindGetPixelPointer(0,0);
        destPtrS=(BYTE*)newImage.BlindGetPixelPointer(0,0);
        int ex=0, ey=0;                                               //ex and ey replace division... 
        int dy=0;
        //(we just add pixels, until by adding newx or newy we get a number greater than old size... then
        // it's time to move to next pixel)
        
        for(int y=0; y<oldy; y++){                                    //for all source rows
            info.nProgress = (long)(100*y/oldy); if (info.nEscape) break;
            ey += newy;                                                   
            ex = 0;                                                       //restart with ex = 0
            accuPtr=accu;                                                 //restart from beginning of accu
            srcPtr=srcPtrS;                                               //and from new source line
#if CXIMAGE_SUPPORT_ALPHA
            alphaPtr = AlphaGetPointer(0, y);
#endif

            for(int x=0; x<oldx; x++){                                    //for all source columns
                ex += newx;
                *accuPtr     += *(srcPtr++);                                  //add current pixel to current accu slot
                *(accuPtr+1) += *(srcPtr++);
                *(accuPtr+2) += *(srcPtr++);
                (*(accuPtr+3)) ++;
#if CXIMAGE_SUPPORT_ALPHA
                if (alphaPtr) *(accuPtr+4) += *(alphaPtr++);
#endif
                if (ex>oldx) {                                                //when we reach oldx, it's time to move to new slot
                    accuPtr += accuCellSize;
                    ex -= oldx;                                                   //(substract oldx from ex and resume from there on)
                }//if (ex overflow)
            }//for x

            if (ey>=oldy) {                                                 //now when this happens
                ey -= oldy;                                                     //it's time to move to new destination row
                destPtr = destPtrS;                                             //reset pointers to proper initial values
                accuPtr = accu;
#if CXIMAGE_SUPPORT_ALPHA
                alphaPtr = newImage.AlphaGetPointer(0, dy++);
#endif
                for (int k=0; k<newx; k++) {                                    //copy accu to destination row (divided by number of pixels in each slot)
                    *(destPtr++) = (BYTE)(*(accuPtr) / *(accuPtr+3));
                    *(destPtr++) = (BYTE)(*(accuPtr+1) / *(accuPtr+3));
                    *(destPtr++) = (BYTE)(*(accuPtr+2) / *(accuPtr+3));
#if CXIMAGE_SUPPORT_ALPHA
                    if (alphaPtr) *(alphaPtr++) = (BYTE)(*(accuPtr+4) / *(accuPtr+3));
#endif
                    accuPtr += accuCellSize;
                }//for k
                memset(accu, 0, newx * accuCellSize * sizeof(unsigned int));                   //clear accu
                destPtrS += newImage.info.dwEffWidth;
            }//if (ey overflow)

            srcPtrS += info.dwEffWidth;                                     //next round we start from new source row
        }//for y
    } else {
        //standard version with GetPixelColor...
        int ex=0, ey=0;                                               //ex and ey replace division... 
        int dy=0;
        //(we just add pixels, until by adding newx or newy we get a number greater than old size... then
        // it's time to move to next pixel)
        RGBQUAD rgb;
        
        for(int y=0; y<oldy; y++){                                    //for all source rows
            info.nProgress = (long)(100*y/oldy); if (info.nEscape) break;
            ey += newy;                                                   
            ex = 0;                                                       //restart with ex = 0
            accuPtr=accu;                                                 //restart from beginning of accu
            for(int x=0; x<oldx; x++){                                    //for all source columns
                ex += newx;
                rgb = GetPixelColor(x, y, TRUE);
                *accuPtr     += rgb.rgbBlue;                                  //add current pixel to current accu slot
                *(accuPtr+1) += rgb.rgbRed;
                *(accuPtr+2) += rgb.rgbGreen;
                (*(accuPtr+3)) ++;
#if CXIMAGE_SUPPORT_ALPHA
                if (pAlpha) *(accuPtr+4) += rgb.rgbReserved;
#endif
                if (ex>oldx) {                                                //when we reach oldx, it's time to move to new slot
                    accuPtr += accuCellSize;
                    ex -= oldx;                                                   //(substract oldx from ex and resume from there on)
                }//if (ex overflow)
            }//for x

            if (ey>=oldy) {                                                 //now when this happens
                ey -= oldy;                                                     //it's time to move to new destination row
                accuPtr = accu;
                for (int dx=0; dx<newx; dx++) {                                 //copy accu to destination row (divided by number of pixels in each slot)
                    rgb.rgbBlue = (BYTE)(*(accuPtr) / *(accuPtr+3));
                    rgb.rgbRed  = (BYTE)(*(accuPtr+1) / *(accuPtr+3));
                    rgb.rgbGreen= (BYTE)(*(accuPtr+2) / *(accuPtr+3));
#if CXIMAGE_SUPPORT_ALPHA
                    if (pAlpha) rgb.rgbReserved = (BYTE)(*(accuPtr+4) / *(accuPtr+3));
#endif
                    newImage.SetPixelColor(dx, dy, rgb, pAlpha!=0);
                    accuPtr += accuCellSize;
                }//for dx
                memset(accu, 0, newx * accuCellSize * sizeof(unsigned int));                   //clear accu
                dy++;
            }//if (ey overflow)
        }//for y
    }//if

    DEL_ARRAY(accu);                                                 //delete helper array
    
    //copy new image to the destination
    if (iDst) 
        iDst->Transfer(&newImage,TRUE);
    else 
        Transfer(&newImage,TRUE);
    return TRUE;

}
#endif  //CXIMAGE_SUPPORT_TRANSFORMATION
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_TRANSFORMATION END
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_DSP
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_DSP

BOOL CxImage::Threshold(BYTE level)
{
    if (!pDib) return FALSE;
    if (head.biBitCount == 1) return TRUE;

    GrayScale();

    CxImage tmp(head.biWidth,head.biHeight,1,0);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    for (long y=0;y<head.biHeight;y++){
        info.nProgress = (long)(100*y/head.biHeight);
        if (info.nEscape) break;
        for (long x=0;x<head.biWidth;x++){
            if (BlindGetPixelIndex(x,y)>level)
                tmp.BlindSetPixelIndex(x,y,1);
            else
                tmp.BlindSetPixelIndex(x,y,0);
        }
    }
    tmp.SetPaletteColor(0,0,0,0,0);
    tmp.SetPaletteColor(1,255,255,255,0);
    Transfer(&tmp,TRUE);
    return TRUE;
}


BOOL CxImage::Threshold(CxImage* pThresholdMask)
{
    if (!pDib) return FALSE;
    if (head.biBitCount == 1) return TRUE;

    if (!pThresholdMask) return FALSE;
    
    if (!pThresholdMask->IsValid() ||
        !pThresholdMask->IsGrayScale() ||
        pThresholdMask->GetWidth() != GetWidth() ||
        pThresholdMask->GetHeight() != GetHeight()){
        strcpy(info.szLastError,"invalid ThresholdMask");
        return FALSE;
    }

    GrayScale();

    CxImage tmp(head.biWidth,head.biHeight,1,0);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    for (long y=0;y<head.biHeight;y++){
        info.nProgress = (long)(100*y/head.biHeight);
        if (info.nEscape) break;
        for (long x=0;x<head.biWidth;x++){
            if (BlindGetPixelIndex(x,y)>pThresholdMask->BlindGetPixelIndex(x,y))
                tmp.BlindSetPixelIndex(x,y,1);
            else
                tmp.BlindSetPixelIndex(x,y,0);
        }
    }
    tmp.SetPaletteColor(0,0,0,0,0);
    tmp.SetPaletteColor(1,255,255,255,0);
    Transfer(&tmp,TRUE);
    return TRUE;
}



BOOL CxImage::Threshold2(BYTE level, BOOL bDirection, RGBQUAD nBkgndColor, BOOL bSetAlpha)
{
    if (!pDib) return FALSE;
    if (head.biBitCount == 1) return TRUE;

    CxImage tmp(this, TRUE, FALSE, FALSE);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    tmp.GrayScale();

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*y/head.biHeight);
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                BYTE i = tmp.BlindGetPixelIndex(x,y);
                if (!bDirection && i<level) BlindSetPixelColor(x,y,nBkgndColor,bSetAlpha);
                if (bDirection && i>=level) BlindSetPixelColor(x,y,nBkgndColor,bSetAlpha);
            }
        }
    }

    return TRUE;
}


BOOL CxImage::SplitRGB(CxImage* r,CxImage* g,CxImage* b)
{
    if (!pDib) return FALSE;
    if (r==NULL && g==NULL && b==NULL) return FALSE;

    CxImage tmpr(head.biWidth,head.biHeight,8,0);
    CxImage tmpg(head.biWidth,head.biHeight,8,0);
    CxImage tmpb(head.biWidth,head.biHeight,8,0);

    RGBQUAD color;
    for(long y=0; y<head.biHeight; y++){
        for(long x=0; x<head.biWidth; x++){
            color = BlindGetPixelColor(x,y,TRUE);
            if (r) tmpr.BlindSetPixelIndex(x,y,color.rgbRed);
            if (g) tmpg.BlindSetPixelIndex(x,y,color.rgbGreen);
            if (b) tmpb.BlindSetPixelIndex(x,y,color.rgbBlue);
        }
    }

    if (r) tmpr.SetGrayPalette();
    if (g) tmpg.SetGrayPalette();
    if (b) tmpb.SetGrayPalette();

    if (r) r->Transfer(&tmpr,TRUE);
    if (g) g->Transfer(&tmpg,TRUE);
    if (b) b->Transfer(&tmpb,TRUE);

    return TRUE;
}

BOOL CxImage::SplitCMYK(CxImage* c,CxImage* m,CxImage* y,CxImage* k)
{
    if (!pDib) return FALSE;
    if (c==NULL && m==NULL && y==NULL && k==NULL) return FALSE;

    CxImage tmpc(head.biWidth,head.biHeight,8,0);
    CxImage tmpm(head.biWidth,head.biHeight,8,0);
    CxImage tmpy(head.biWidth,head.biHeight,8,0);
    CxImage tmpk(head.biWidth,head.biHeight,8,0);

    RGBQUAD color;
    for(long yy=0; yy<head.biHeight; yy++){
        for(long xx=0; xx<head.biWidth; xx++){
            color = BlindGetPixelColor(xx,yy,TRUE);
            if (c) tmpc.BlindSetPixelIndex(xx,yy,(BYTE)(255-color.rgbRed));
            if (m) tmpm.BlindSetPixelIndex(xx,yy,(BYTE)(255-color.rgbGreen));
            if (y) tmpy.BlindSetPixelIndex(xx,yy,(BYTE)(255-color.rgbBlue));
            if (k) tmpk.BlindSetPixelIndex(xx,yy,(BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue));
        }
    }

    if (c) tmpc.SetGrayPalette();
    if (m) tmpm.SetGrayPalette();
    if (y) tmpy.SetGrayPalette();
    if (k) tmpk.SetGrayPalette();

    if (c) c->Transfer(&tmpc,TRUE);
    if (m) m->Transfer(&tmpm,TRUE);
    if (y) y->Transfer(&tmpy,TRUE);
    if (k) k->Transfer(&tmpk,TRUE);

    return TRUE;
}


BOOL CxImage::SplitYUV(CxImage* y,CxImage* u,CxImage* v)
{
    if (!pDib) return FALSE;
    if (y==NULL && u==NULL && v==NULL) return FALSE;

    CxImage tmpy(head.biWidth,head.biHeight,8,0);
    CxImage tmpu(head.biWidth,head.biHeight,8,0);
    CxImage tmpv(head.biWidth,head.biHeight,8,0);

    RGBQUAD color;
    for(long yy=0; yy<head.biHeight; yy++){
        for(long x=0; x<head.biWidth; x++){
            color = RGBtoYUV(BlindGetPixelColor(x,yy,TRUE));
            if (y) tmpy.BlindSetPixelIndex(x,yy,color.rgbRed);
            if (u) tmpu.BlindSetPixelIndex(x,yy,color.rgbGreen);
            if (v) tmpv.BlindSetPixelIndex(x,yy,color.rgbBlue);
        }
    }

    if (y) tmpy.SetGrayPalette();
    if (u) tmpu.SetGrayPalette();
    if (v) tmpv.SetGrayPalette();

    if (y) y->Transfer(&tmpy,TRUE);
    if (u) u->Transfer(&tmpu,TRUE);
    if (v) v->Transfer(&tmpv,TRUE);

    return TRUE;
}


BOOL CxImage::SplitYIQ(CxImage* y,CxImage* i,CxImage* q)
{
    if (!pDib) return FALSE;
    if (y==NULL && i==NULL && q==NULL) return FALSE;

    CxImage tmpy(head.biWidth,head.biHeight,8,0);
    CxImage tmpi(head.biWidth,head.biHeight,8,0);
    CxImage tmpq(head.biWidth,head.biHeight,8,0);

    RGBQUAD color;
    for(long yy=0; yy<head.biHeight; yy++){
        for(long x=0; x<head.biWidth; x++){
            color = RGBtoYIQ(BlindGetPixelColor(x,yy,TRUE));
            if (y) tmpy.BlindSetPixelIndex(x,yy,color.rgbRed);
            if (i) tmpi.BlindSetPixelIndex(x,yy,color.rgbGreen);
            if (q) tmpq.BlindSetPixelIndex(x,yy,color.rgbBlue);
        }
    }

    if (y) tmpy.SetGrayPalette();
    if (i) tmpi.SetGrayPalette();
    if (q) tmpq.SetGrayPalette();

    if (y) y->Transfer(&tmpy,TRUE);
    if (i) i->Transfer(&tmpi,TRUE);
    if (q) q->Transfer(&tmpq,TRUE);

    return TRUE;
}


BOOL CxImage::SplitXYZ(CxImage* x,CxImage* y,CxImage* z)
{
    if (!pDib) return FALSE;
    if (x==NULL && y==NULL && z==NULL) return FALSE;

    CxImage tmpx(head.biWidth,head.biHeight,8,0);
    CxImage tmpy(head.biWidth,head.biHeight,8,0);
    CxImage tmpz(head.biWidth,head.biHeight,8,0);

    RGBQUAD color;
    for(long yy=0; yy<head.biHeight; yy++){
        for(long xx=0; xx<head.biWidth; xx++){
            color = RGBtoXYZ(BlindGetPixelColor(xx,yy,TRUE));
            if (x) tmpx.BlindSetPixelIndex(xx,yy,color.rgbRed);
            if (y) tmpy.BlindSetPixelIndex(xx,yy,color.rgbGreen);
            if (z) tmpz.BlindSetPixelIndex(xx,yy,color.rgbBlue);
        }
    }

    if (x) tmpx.SetGrayPalette();
    if (y) tmpy.SetGrayPalette();
    if (z) tmpz.SetGrayPalette();

    if (x) x->Transfer(&tmpx,TRUE);
    if (y) y->Transfer(&tmpy,TRUE);
    if (z) z->Transfer(&tmpz,TRUE);

    return TRUE;
}


BOOL CxImage::SplitHSL(CxImage* h,CxImage* s,CxImage* l)
{
    if (!pDib) return FALSE;
    if (h==NULL && s==NULL && l==NULL) return FALSE;

    CxImage tmph(head.biWidth,head.biHeight,8,0);
    CxImage tmps(head.biWidth,head.biHeight,8,0);
    CxImage tmpl(head.biWidth,head.biHeight,8,0);

    RGBQUAD color;
    for(long y=0; y<head.biHeight; y++){
        for(long x=0; x<head.biWidth; x++){
            color = RGBtoHSL(BlindGetPixelColor(x,y,TRUE));
            if (h) tmph.BlindSetPixelIndex(x,y,color.rgbRed);
            if (s) tmps.BlindSetPixelIndex(x,y,color.rgbGreen);
            if (l) tmpl.BlindSetPixelIndex(x,y,color.rgbBlue);
        }
    }

    if (h) tmph.SetGrayPalette();
    if (s) tmps.SetGrayPalette();
    if (l) tmpl.SetGrayPalette();

    

    if (h) h->Transfer(&tmph,TRUE);
    if (s) s->Transfer(&tmps,TRUE);
    if (l) l->Transfer(&tmpl,TRUE);

    return TRUE;
}

RGBQUAD CxImage::RGBtoHSL(RGBQUAD lRGBColor)
{
    BYTE R,G,B;                 
    BYTE H,L,S;                 
    BYTE cMax,cMin;             
    WORD Rdelta,Gdelta,Bdelta;  

    R = lRGBColor.rgbRed;   
    G = lRGBColor.rgbGreen;
    B = lRGBColor.rgbBlue;

    cMax = max( max(R,G), B);   
    cMin = min( min(R,G), B);
    L = (BYTE)((((cMax+cMin)*HSLMAX)+RGBMAX)/(2*RGBMAX));

    if (cMax==cMin){            
        S = 0;                  
        H = HSLUNDEFINED;       
    } else {                    
        if (L <= (HSLMAX/2))    
            S = (BYTE)((((cMax-cMin)*HSLMAX)+((cMax+cMin)/2))/(cMax+cMin));
        else
            S = (BYTE)((((cMax-cMin)*HSLMAX)+((2*RGBMAX-cMax-cMin)/2))/(2*RGBMAX-cMax-cMin));
        
        Rdelta = (WORD)((((cMax-R)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
        Gdelta = (WORD)((((cMax-G)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));
        Bdelta = (WORD)((((cMax-B)*(HSLMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin));

        if (R == cMax)
            H = (BYTE)(Bdelta - Gdelta);
        else if (G == cMax)
            H = (BYTE)((HSLMAX/3) + Rdelta - Bdelta);
        else 
            H = (BYTE)(((2*HSLMAX)/3) + Gdelta - Rdelta);


        if (H > HSLMAX) H -= HSLMAX;
    }
    RGBQUAD hsl={L,S,H,0};
    return hsl;
}

float CxImage::HueToRGB(float n1,float n2, float hue)
{
    
    float rValue;

    if (hue > 360)
        hue = hue - 360;
    else if (hue < 0)
        hue = hue + 360;

    if (hue < 60)
        rValue = n1 + (n2-n1)*hue/60.0f;
    else if (hue < 180)
        rValue = n2;
    else if (hue < 240)
        rValue = n1+(n2-n1)*(240-hue)/60;
    else
        rValue = n1;

    return rValue;
}

RGBQUAD CxImage::HSLtoRGB(COLORREF cHSLColor)
{
    return HSLtoRGB(RGBtoRGBQUAD(cHSLColor));
}

RGBQUAD CxImage::HSLtoRGB(RGBQUAD lHSLColor)
{ 
    
    float h,s,l;
    float m1,m2;
    BYTE r,g,b;

    h = (float)lHSLColor.rgbRed * 360.0f/255.0f;
    s = (float)lHSLColor.rgbGreen/255.0f;
    l = (float)lHSLColor.rgbBlue/255.0f;

    if (l <= 0.5)   m2 = l * (1+s);
    else            m2 = l + s - l*s;

    m1 = 2 * l - m2;

    if (s == 0) {
        r=g=b=(BYTE)(l*255.0f);
    } else {
        r = (BYTE)(HueToRGB(m1,m2,h+120) * 255.0f);
        g = (BYTE)(HueToRGB(m1,m2,h) * 255.0f);
        b = (BYTE)(HueToRGB(m1,m2,h-120) * 255.0f);
    }

    RGBQUAD rgb = {b,g,r,0};
    return rgb;
}

RGBQUAD CxImage::YUVtoRGB(RGBQUAD lYUVColor)
{
    int U,V,R,G,B;
    float Y = lYUVColor.rgbRed;
    U = lYUVColor.rgbGreen - 128;
    V = lYUVColor.rgbBlue - 128;




    R = (int)( Y + 1.403f * V);
    G = (int)( Y - 0.344f * U - 0.714f * V);
    B = (int)( Y + 1.770f * U);

    R= min(255,max(0,R));
    G= min(255,max(0,G));
    B= min(255,max(0,B));
    RGBQUAD rgb={(BYTE)B,(BYTE)G,(BYTE)R,0};
    return rgb;
}

RGBQUAD CxImage::RGBtoYUV(RGBQUAD lRGBColor)
{
    int Y,U,V,R,G,B;
    R = lRGBColor.rgbRed;
    G = lRGBColor.rgbGreen;
    B = lRGBColor.rgbBlue;




    Y = (int)(0.299f * R + 0.587f * G + 0.114f * B);
    U = (int)((B-Y) * 0.565f + 128);
    V = (int)((R-Y) * 0.713f + 128);

    Y= min(255,max(0,Y));
    U= min(255,max(0,U));
    V= min(255,max(0,V));
    RGBQUAD yuv={(BYTE)V,(BYTE)U,(BYTE)Y,0};
    return yuv;
}

RGBQUAD CxImage::YIQtoRGB(RGBQUAD lYIQColor)
{
    int I,Q,R,G,B;
    float Y = lYIQColor.rgbRed;
    I = lYIQColor.rgbGreen - 128;
    Q = lYIQColor.rgbBlue - 128;

    R = (int)( Y + 0.956f * I + 0.621f * Q);
    G = (int)( Y - 0.273f * I - 0.647f * Q);
    B = (int)( Y - 1.104f * I + 1.701f * Q);

    R= min(255,max(0,R));
    G= min(255,max(0,G));
    B= min(255,max(0,B));
    RGBQUAD rgb={(BYTE)B,(BYTE)G,(BYTE)R,0};
    return rgb;
}

RGBQUAD CxImage::RGBtoYIQ(RGBQUAD lRGBColor)
{
    int Y,I,Q,R,G,B;
    R = lRGBColor.rgbRed;
    G = lRGBColor.rgbGreen;
    B = lRGBColor.rgbBlue;

    Y = (int)( 0.2992f * R + 0.5868f * G + 0.1140f * B);
    I = (int)( 0.5960f * R - 0.2742f * G - 0.3219f * B + 128);
    Q = (int)( 0.2109f * R - 0.5229f * G + 0.3120f * B + 128);

    Y= min(255,max(0,Y));
    I= min(255,max(0,I));
    Q= min(255,max(0,Q));
    RGBQUAD yiq={(BYTE)Q,(BYTE)I,(BYTE)Y,0};
    return yiq;
}

RGBQUAD CxImage::XYZtoRGB(RGBQUAD lXYZColor)
{
    int X,Y,Z,R,G,B;
    X = lXYZColor.rgbRed;
    Y = lXYZColor.rgbGreen;
    Z = lXYZColor.rgbBlue;
    double k=1.088751;

    R = (int)(  3.240479f * X - 1.537150f * Y - 0.498535f * Z * k);
    G = (int)( -0.969256f * X + 1.875992f * Y + 0.041556f * Z * k);
    B = (int)(  0.055648f * X - 0.204043f * Y + 1.057311f * Z * k);

    R= min(255,max(0,R));
    G= min(255,max(0,G));
    B= min(255,max(0,B));
    RGBQUAD rgb={(BYTE)B,(BYTE)G,(BYTE)R,0};
    return rgb;
}

RGBQUAD CxImage::RGBtoXYZ(RGBQUAD lRGBColor)
{
    int X,Y,Z,R,G,B;
    R = lRGBColor.rgbRed;
    G = lRGBColor.rgbGreen;
    B = lRGBColor.rgbBlue;

    X = (int)( 0.412453f * R + 0.357580f * G + 0.180423f * B);
    Y = (int)( 0.212671f * R + 0.715160f * G + 0.072169f * B);
    Z = (int)((0.019334f * R + 0.119193f * G + 0.950227f * B)*0.918483657f);

    
    
    
    RGBQUAD xyz={(BYTE)Z,(BYTE)Y,(BYTE)X,0};
    return xyz;
}


void CxImage::HuePalette(float correction)
{
    if (head.biClrUsed==0) return;

    for(DWORD j=0; j<head.biClrUsed; j++){
        BYTE i=(BYTE)(j*correction*(255/(head.biClrUsed-1)));
        RGBQUAD hsl={120,240,i,0};
        SetPaletteColor((BYTE)j,HSLtoRGB(hsl));
    }
}


BOOL CxImage::Colorize(BYTE hue, BYTE sat, float blend)
{
    if (!pDib) return FALSE;

    if (blend < 0.0f) blend = 0.0f;
    if (blend > 1.0f) blend = 1.0f;
    int a0 = (int)(256*blend);
    int a1 = 256 - a0;

    BOOL bFullBlend = FALSE;
    if (blend > 0.999f) bFullBlend = TRUE;

    RGBQUAD color,hsl;
    if (head.biClrUsed==0){

        long xmin,xmax,ymin,ymax;
        if (pSelection){
            xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
            ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
        } else {
            xmin = ymin = 0;
            xmax = head.biWidth; ymax=head.biHeight;
        }

        for(long y=ymin; y<ymax; y++){
            info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
            if (info.nEscape) break;
            for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                if (BlindSelectionIsInside(x,y))
#endif 
                {
                    if (bFullBlend){
                        color = RGBtoHSL(BlindGetPixelColor(x,y,TRUE));
                        color.rgbRed=hue;
                        color.rgbGreen=sat;
                        BlindSetPixelColor(x,y,HSLtoRGB(color),FALSE);
                    } else {
                        color = BlindGetPixelColor(x,y,TRUE);
                        hsl.rgbRed=hue;
                        hsl.rgbGreen=sat;
                        hsl.rgbBlue = (BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue);
                        hsl = HSLtoRGB(hsl);
                        
                        
                        
                        
                        color.rgbRed = (BYTE)((hsl.rgbRed * a0 + color.rgbRed * a1)>>8);
                        color.rgbBlue = (BYTE)((hsl.rgbBlue * a0 + color.rgbBlue * a1)>>8);
                        color.rgbGreen = (BYTE)((hsl.rgbGreen * a0 + color.rgbGreen * a1)>>8);
                        BlindSetPixelColor(x,y,color,FALSE);
                    }
                }
            }
        }
    } else {
        for(DWORD j=0; j<head.biClrUsed; j++){
            if (bFullBlend){
                color = RGBtoHSL(GetPaletteColor((BYTE)j));
                color.rgbRed=hue;
                color.rgbGreen=sat;
                SetPaletteColor((BYTE)j,HSLtoRGB(color));
            } else {
                color = GetPaletteColor((BYTE)j);
                hsl.rgbRed=hue;
                hsl.rgbGreen=sat;
                hsl.rgbBlue = (BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue);
                hsl = HSLtoRGB(hsl);
                color.rgbRed = (BYTE)(hsl.rgbRed * blend + color.rgbRed * (1.0f - blend));
                color.rgbBlue = (BYTE)(hsl.rgbBlue * blend + color.rgbBlue * (1.0f - blend));
                color.rgbGreen = (BYTE)(hsl.rgbGreen * blend + color.rgbGreen * (1.0f - blend));
                SetPaletteColor((BYTE)j,color);
            }
        }
    }

    return TRUE;
}


BOOL CxImage::Light(long brightness, long contrast)
{
    if (!pDib) return FALSE;
    float c=(100 + contrast)/100.0f;
    brightness+=128;

    BYTE cTable[256]; 
    for (int i=0;i<256;i++) {
        cTable[i] = (BYTE)max(0,min(255,(int)((i-128)*c + brightness + 0.5f)));
    }

    return Lut(cTable);
}


float CxImage::Mean()
{
    if (!pDib) return 0;

    CxImage tmp(this,TRUE,TRUE,TRUE);
    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    tmp.GrayScale();
    float sum=0;

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }
    if (xmin==xmax || ymin==ymax) return (float)0.0;

    BYTE *iSrc=tmp.info.pImage;
    iSrc += tmp.info.dwEffWidth*ymin; 

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin)); 
        for(long x=xmin; x<xmax; x++){
            sum+=iSrc[x];
        }
        iSrc+=tmp.info.dwEffWidth;
    }
    return sum/(xmax-xmin)/(ymax-ymin);
}


BOOL CxImage::Filter(long* kernel, long Ksize, long Kfactor, long Koffset)
{
    if (!pDib) return FALSE;

    long k2 = Ksize/2;
    long kmax= Ksize-k2;
    long r,g,b,i;
    long ksumcur,ksumtot;
    RGBQUAD c;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    ksumtot = 0;
    for(long j=-k2;j<kmax;j++){
        for(long k=-k2;k<kmax;k++){
            ksumtot += kernel[(j+k2)+Ksize*(k+k2)];
        }
    }

    if ((head.biBitCount==8) && IsGrayScale())
    {
        unsigned char* cPtr;
        unsigned char* cPtr2;      
        int iCount;
        int iY, iY2, iY1;
        cPtr = info.pImage;
        cPtr2 = (unsigned char *)tmp.info.pImage;
        for(long y=ymin; y<ymax; y++){
            info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
            if (info.nEscape) break;
            iY1 = y*info.dwEffWidth+xmin;
            for(long x=xmin; x<xmax; x++, iY1++){
#if CXIMAGE_SUPPORT_SELECTION
                if (BlindSelectionIsInside(x,y))
#endif 
                {
                    b=ksumcur=0;
                    iCount = 0;
                    iY2 = ((y-k2)*info.dwEffWidth);
                    for(long j=-k2;j<kmax;j++, iY2+=info.dwEffWidth)
                    {
                        if (0>(y+j) || (y+j)>=head.biHeight) continue;
                        iY = iY2+x;
                        for(long k=-k2;k<kmax;k++, iCount++)
                        {
                            if (0>(x+k) || (x+k)>=head.biWidth) continue;
                            i=kernel[iCount];
                            b += cPtr[iY+k] * i;
                            ksumcur += i;
                        }
                    }
                    if (Kfactor==0 || ksumcur==0){
                        cPtr2[iY1] = (BYTE)min(255, max(0,(int)(b + Koffset)));
                    } else if (ksumtot == ksumcur) {
                        cPtr2[iY1] = (BYTE)min(255, max(0,(int)(b/Kfactor + Koffset)));
                    } else {
                        cPtr2[iY1] = (BYTE)min(255, max(0,(int)((b*ksumtot)/(ksumcur*Kfactor) + Koffset)));
                    }
                }
            }
        }
    }
    else
    {
        for(long y=ymin; y<ymax; y++){
            info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
            if (info.nEscape) break;
            for(long x=xmin; x<xmax; x++){
    #if CXIMAGE_SUPPORT_SELECTION
                if (BlindSelectionIsInside(x,y))
    #endif 
                    {
                    r=b=g=ksumcur=0;
                    for(long j=-k2;j<kmax;j++){
                        for(long k=-k2;k<kmax;k++){
                            if (!IsInside(x+j,y+k)) continue;
                            c = BlindGetPixelColor(x+j,y+k,TRUE);
                            i = kernel[(j+k2)+Ksize*(k+k2)];
                            r += c.rgbRed * i;
                            g += c.rgbGreen * i;
                            b += c.rgbBlue * i;
                            ksumcur += i;
                        }
                    }
                    if (Kfactor==0 || ksumcur==0){
                        c.rgbRed   = (BYTE)min(255, max(0,(int)(r + Koffset)));
                        c.rgbGreen = (BYTE)min(255, max(0,(int)(g + Koffset)));
                        c.rgbBlue  = (BYTE)min(255, max(0,(int)(b + Koffset)));
                    } else if (ksumtot == ksumcur) {
                        c.rgbRed   = (BYTE)min(255, max(0,(int)(r/Kfactor + Koffset)));
                        c.rgbGreen = (BYTE)min(255, max(0,(int)(g/Kfactor + Koffset)));
                        c.rgbBlue  = (BYTE)min(255, max(0,(int)(b/Kfactor + Koffset)));
                    } else {
                        c.rgbRed   = (BYTE)min(255, max(0,(int)((r*ksumtot)/(ksumcur*Kfactor) + Koffset)));
                        c.rgbGreen = (BYTE)min(255, max(0,(int)((g*ksumtot)/(ksumcur*Kfactor) + Koffset)));
                        c.rgbBlue  = (BYTE)min(255, max(0,(int)((b*ksumtot)/(ksumcur*Kfactor) + Koffset)));
                    }
                    tmp.BlindSetPixelColor(x,y,c,FALSE);
                }
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}


BOOL CxImage::Erode(long Ksize)
{
    if (!pDib) return FALSE;

    long k2 = Ksize/2;
    long kmax= Ksize-k2;
    BYTE r,g,b;
    RGBQUAD c;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                r=b=g=255;
                for(long j=-k2;j<kmax;j++){
                    for(long k=-k2;k<kmax;k++){
                        if (!IsInside(x+j,y+k)) continue;
                        c = BlindGetPixelColor(x+j,y+k,TRUE);
                        if (c.rgbRed < r) r=c.rgbRed;
                        if (c.rgbGreen < g) g=c.rgbGreen;
                        if (c.rgbBlue < b) b=c.rgbBlue;
                    }
                }
                c.rgbRed   = r;
                c.rgbGreen = g;
                c.rgbBlue  = b;
                tmp.BlindSetPixelColor(x,y,c,FALSE);
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}


BOOL CxImage::Dilate(long Ksize)
{
    if (!pDib) return FALSE;

    long k2 = Ksize/2;
    long kmax= Ksize-k2;
    BYTE r,g,b;
    RGBQUAD c;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                r=b=g=0;
                for(long j=-k2;j<kmax;j++){
                    for(long k=-k2;k<kmax;k++){
                        if (!IsInside(x+j,y+k)) continue;
                        c = BlindGetPixelColor(x+j,y+k,TRUE);
                        if (c.rgbRed > r) r=c.rgbRed;
                        if (c.rgbGreen > g) g=c.rgbGreen;
                        if (c.rgbBlue > b) b=c.rgbBlue;
                    }
                }
                c.rgbRed   = r;
                c.rgbGreen = g;
                c.rgbBlue  = b;
                tmp.BlindSetPixelColor(x,y,c,FALSE);
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}


BOOL CxImage::Edge(long Ksize)
{
    if (!pDib) return FALSE;

    long k2 = Ksize/2;
    long kmax= Ksize-k2;
    BYTE r,g,b,rr,gg,bb;
    RGBQUAD c;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                r=b=g=0;
                rr=bb=gg=255;
                for(long j=-k2;j<kmax;j++){
                    for(long k=-k2;k<kmax;k++){
                        if (!IsInside(x+j,y+k)) continue;
                        c = BlindGetPixelColor(x+j,y+k,TRUE);
                        if (c.rgbRed > r) r=c.rgbRed;
                        if (c.rgbGreen > g) g=c.rgbGreen;
                        if (c.rgbBlue > b) b=c.rgbBlue;

                        if (c.rgbRed < rr) rr=c.rgbRed;
                        if (c.rgbGreen < gg) gg=c.rgbGreen;
                        if (c.rgbBlue < bb) bb=c.rgbBlue;
                    }
                }
                c.rgbRed   = (BYTE)(255-abs(r-rr));
                c.rgbGreen = (BYTE)(255-abs(g-gg));
                c.rgbBlue  = (BYTE)(255-abs(b-bb));
                tmp.BlindSetPixelColor(x,y,c,FALSE);
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}

void CxImage::Mix(CxImage & imgsrc2, ImageOpType op, long lXOffset, long lYOffset, BOOL bMixAlpha)
{
    long lWide = min(GetWidth(),imgsrc2.GetWidth()-lXOffset);
    long lHeight = min(GetHeight(),imgsrc2.GetHeight()-lYOffset);

    BOOL bEditAlpha = imgsrc2.AlphaIsValid() & (BOOL)bMixAlpha;

    if (bEditAlpha && AlphaIsValid()==FALSE){
        AlphaCreate();
    }

    RGBQUAD rgbBackgrnd1 = GetTransColor();
    RGBQUAD rgb1, rgb2, rgbDest;

    for(long lY=0;lY<lHeight;lY++)
    {
        info.nProgress = (long)(100*lY/head.biHeight);
        if (info.nEscape) break;

        for(long lX=0;lX<lWide;lX++)
        {
#if CXIMAGE_SUPPORT_SELECTION
            if (SelectionIsInside(lX,lY) && imgsrc2.SelectionIsInside(lX+lXOffset,lY+lYOffset))
#endif 
            {
                rgb1 = GetPixelColor(lX,lY,TRUE);
                rgb2 = imgsrc2.GetPixelColor(lX+lXOffset,lY+lYOffset,TRUE);
                switch(op)
                {
                    case OpAvg:
                        rgbDest.rgbBlue =  (BYTE)((rgb1.rgbBlue+rgb2.rgbBlue)/2);
                        rgbDest.rgbGreen = (BYTE)((rgb1.rgbGreen+rgb2.rgbGreen)/2);
                        rgbDest.rgbRed =   (BYTE)((rgb1.rgbRed+rgb2.rgbRed)/2);
                        if (bEditAlpha) rgbDest.rgbReserved = (BYTE)((rgb1.rgbReserved+rgb2.rgbReserved)/2);
                    break;
                    case OpAdd:
                        rgbDest.rgbBlue = (BYTE)max(0,min(255,rgb1.rgbBlue+rgb2.rgbBlue));
                        rgbDest.rgbGreen = (BYTE)max(0,min(255,rgb1.rgbGreen+rgb2.rgbGreen));
                        rgbDest.rgbRed = (BYTE)max(0,min(255,rgb1.rgbRed+rgb2.rgbRed));
                        if (bEditAlpha) rgbDest.rgbReserved = (BYTE)max(0,min(255,rgb1.rgbReserved+rgb2.rgbReserved));
                    break;
                    case OpSub:
                        rgbDest.rgbBlue = (BYTE)max(0,min(255,rgb1.rgbBlue-rgb2.rgbBlue));
                        rgbDest.rgbGreen = (BYTE)max(0,min(255,rgb1.rgbGreen-rgb2.rgbGreen));
                        rgbDest.rgbRed = (BYTE)max(0,min(255,rgb1.rgbRed-rgb2.rgbRed));
                        if (bEditAlpha) rgbDest.rgbReserved = (BYTE)max(0,min(255,rgb1.rgbReserved-rgb2.rgbReserved));
                    break;
                    case OpAnd:
                        rgbDest.rgbBlue = (BYTE)(rgb1.rgbBlue&rgb2.rgbBlue);
                        rgbDest.rgbGreen = (BYTE)(rgb1.rgbGreen&rgb2.rgbGreen);
                        rgbDest.rgbRed = (BYTE)(rgb1.rgbRed&rgb2.rgbRed);
                        if (bEditAlpha) rgbDest.rgbReserved = (BYTE)(rgb1.rgbReserved&rgb2.rgbReserved);
                    break;
                    case OpXor:
                        rgbDest.rgbBlue = (BYTE)(rgb1.rgbBlue^rgb2.rgbBlue);
                        rgbDest.rgbGreen = (BYTE)(rgb1.rgbGreen^rgb2.rgbGreen);
                        rgbDest.rgbRed = (BYTE)(rgb1.rgbRed^rgb2.rgbRed);
                        if (bEditAlpha) rgbDest.rgbReserved = (BYTE)(rgb1.rgbReserved^rgb2.rgbReserved);
                    break;
                    case OpOr:
                        rgbDest.rgbBlue = (BYTE)(rgb1.rgbBlue|rgb2.rgbBlue);
                        rgbDest.rgbGreen = (BYTE)(rgb1.rgbGreen|rgb2.rgbGreen);
                        rgbDest.rgbRed = (BYTE)(rgb1.rgbRed|rgb2.rgbRed);
                        if (bEditAlpha) rgbDest.rgbReserved = (BYTE)(rgb1.rgbReserved|rgb2.rgbReserved);
                    break;
                    case OpMask:
                        if(rgb2.rgbBlue==0 && rgb2.rgbGreen==0 && rgb2.rgbRed==0)
                            rgbDest = rgbBackgrnd1;
                        else
                            rgbDest = rgb1;
                        break;
                    case OpSrcCopy:
                        if(IsTransparent(lX,lY))
                            rgbDest = rgb2;
                        else 
                            rgbDest = rgb1;
                        break;
                    case OpDstCopy:
                        if(imgsrc2.IsTransparent(lX+lXOffset,lY+lYOffset))
                            rgbDest = rgb1;
                        else 
                            rgbDest = rgb2;
                        break;
                    case OpScreen:
                        { 
                            BYTE a,a1; 
                            
                            if (imgsrc2.IsTransparent(lX+lXOffset,lY+lYOffset)){
                                a=0;
                            } else if (imgsrc2.AlphaIsValid()){
                                a=imgsrc2.AlphaGet(lX+lXOffset,lY+lYOffset);
                                a =(BYTE)((a*imgsrc2.info.nAlphaMax)/255);
                            } else {
                                a=255;
                            }

                            if (a==0){ 
                                rgbDest = rgb1; 
                            } else if (a==255){ 
                                rgbDest = rgb2; 
                            } else { 
                                a1 = (BYTE)~a; 
                                rgbDest.rgbBlue = (BYTE)((rgb1.rgbBlue*a1+rgb2.rgbBlue*a)/255); 
                                rgbDest.rgbGreen = (BYTE)((rgb1.rgbGreen*a1+rgb2.rgbGreen*a)/255); 
                                rgbDest.rgbRed = (BYTE)((rgb1.rgbRed*a1+rgb2.rgbRed*a)/255);  
                            }

                            if (bEditAlpha) rgbDest.rgbReserved = (BYTE)((rgb1.rgbReserved*a)/255);
                        } 
                        break; 
                    case OpSrcBlend:
                        if(IsTransparent(lX,lY))
                            rgbDest = rgb2;
                        else
                        {
                            long lBDiff = abs(rgb1.rgbBlue - rgbBackgrnd1.rgbBlue);
                            long lGDiff = abs(rgb1.rgbGreen - rgbBackgrnd1.rgbGreen);
                            long lRDiff = abs(rgb1.rgbRed - rgbBackgrnd1.rgbRed);

                            double lAverage = (lBDiff+lGDiff+lRDiff)/3;
                            double lThresh = 16;
                            double dLarge = lAverage/lThresh;
                            double dSmall = (lThresh-lAverage)/lThresh;
                            double dSmallAmt = dSmall*((double)rgb2.rgbBlue);

                            if( lAverage < lThresh+1){
                                rgbDest.rgbBlue = (BYTE)max(0,min(255,(int)(dLarge*((double)rgb1.rgbBlue) +
                                                dSmallAmt)));
                                rgbDest.rgbGreen = (BYTE)max(0,min(255,(int)(dLarge*((double)rgb1.rgbGreen) +
                                                dSmallAmt)));
                                rgbDest.rgbRed = (BYTE)max(0,min(255,(int)(dLarge*((double)rgb1.rgbRed) +
                                                dSmallAmt)));
                            }
                            else
                                rgbDest = rgb1;
                        }
                        break;
                        default:
                        return;
                }
                SetPixelColor(lX,lY,rgbDest,bEditAlpha);
            }
        }
    }
}


void CxImage::MixFrom(CxImage & imagesrc2, long lXOffset, long lYOffset)
{
    long width = imagesrc2.GetWidth();
    long height = imagesrc2.GetHeight();

    int x, y;

    if (imagesrc2.IsTransparent()) {
        for(x = 0; x < width; x++) {
            for(y = 0; y < height; y++) {
                if(!imagesrc2.IsTransparent(x,y)){
                    SetPixelColor(x + lXOffset, y + lYOffset, imagesrc2.BlindGetPixelColor(x, y,TRUE),FALSE);
                }
            }
        }
    } else { 
        for(x = 0; x < width; x++) {
            for(y = 0; y < height; y++) {
                SetPixelColor(x + lXOffset, y + lYOffset, imagesrc2.BlindGetPixelColor(x, y,TRUE),FALSE); 
            }
        }
    }
}


BOOL CxImage::ShiftRGB(long r, long g, long b)
{
    if (!pDib) return FALSE;
    RGBQUAD color;
    if (head.biClrUsed==0){

        long xmin,xmax,ymin,ymax;
        if (pSelection){
            xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
            ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
        } else {
            xmin = ymin = 0;
            xmax = head.biWidth; ymax=head.biHeight;
        }

        for(long y=ymin; y<ymax; y++){
            for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                if (BlindSelectionIsInside(x,y))
#endif 
                {
                    color = BlindGetPixelColor(x,y,TRUE);
                    color.rgbRed = (BYTE)max(0,min(255,(int)(color.rgbRed + r)));
                    color.rgbGreen = (BYTE)max(0,min(255,(int)(color.rgbGreen + g)));
                    color.rgbBlue = (BYTE)max(0,min(255,(int)(color.rgbBlue + b)));
                    BlindSetPixelColor(x,y,color,FALSE);
                }
            }
        }
    } else {
        for(DWORD j=0; j<head.biClrUsed; j++){
            color = GetPaletteColor((BYTE)j);
            color.rgbRed = (BYTE)max(0,min(255,(int)(color.rgbRed + r)));
            color.rgbGreen = (BYTE)max(0,min(255,(int)(color.rgbGreen + g)));
            color.rgbBlue = (BYTE)max(0,min(255,(int)(color.rgbBlue + b)));
            SetPaletteColor((BYTE)j,color);
        }
    }
    return TRUE;
}


BOOL CxImage::Gamma(float gamma)
{
    if (!pDib) return FALSE;

    if (gamma <= 0.0f) return FALSE;

    double dinvgamma = 1/gamma;
    double dMax = pow(255.0, dinvgamma) / 255.0;

    BYTE cTable[256]; 
    for (int i=0;i<256;i++) {
        cTable[i] = (BYTE)max(0,min(255,(int)( pow((double)i, dinvgamma) / dMax)));
    }

    return Lut(cTable);
}


BOOL CxImage::GammaRGB(float gammaR, float gammaG, float gammaB)
{
    if (!pDib) return FALSE;

    if (gammaR <= 0.0f) return FALSE;
    if (gammaG <= 0.0f) return FALSE;
    if (gammaB <= 0.0f) return FALSE;

    double dinvgamma, dMax;
    int i;

    dinvgamma = 1/gammaR;
    dMax = pow(255.0, dinvgamma) / 255.0;
    BYTE cTableR[256];
    for (i=0;i<256;i++) {
        cTableR[i] = (BYTE)max(0,min(255,(int)( pow((double)i, dinvgamma) / dMax)));
    }

    dinvgamma = 1/gammaG;
    dMax = pow(255.0, dinvgamma) / 255.0;
    BYTE cTableG[256];
    for (i=0;i<256;i++) {
        cTableG[i] = (BYTE)max(0,min(255,(int)( pow((double)i, dinvgamma) / dMax)));
    }

    dinvgamma = 1/gammaB;
    dMax = pow(255.0, dinvgamma) / 255.0;
    BYTE cTableB[256];
    for (i=0;i<256;i++) {
        cTableB[i] = (BYTE)max(0,min(255,(int)( pow((double)i, dinvgamma) / dMax)));
    }

    return Lut(cTableR, cTableG, cTableB,NULL);
}




BOOL CxImage::Median(long Ksize)
{
    if (!pDib) return FALSE;

    long k2 = Ksize/2;
    long kmax= Ksize-k2;
    long i,j,k;

    RGBQUAD* kernel;
    MALLOC(kernel,RGBQUAD,Ksize*Ksize);

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
                {
                for(j=-k2, i=0;j<kmax;j++)
                    for(k=-k2;k<kmax;k++)
                        if (IsInside(x+j,y+k))
                            kernel[i++]=BlindGetPixelColor(x+j,y+k,TRUE);

                qsort(kernel, i, sizeof(RGBQUAD), CompareColors);
                tmp.SetPixelColor(x,y,kernel[i/2],FALSE);
            }
        }
    }
    FREE(kernel);
    Transfer(&tmp,TRUE);
    return TRUE;
}



BOOL CxImage::Noise(long level)
{
    if (!pDib) return FALSE;
    RGBQUAD color;

    long xmin,xmax,ymin,ymax,n;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin)); 
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                color = BlindGetPixelColor(x,y,TRUE);
                n=(long)((rand()/(float)RAND_MAX - 0.5)*level);
                color.rgbRed = (BYTE)max(0,min(255,(int)(color.rgbRed + n)));
                n=(long)((rand()/(float)RAND_MAX - 0.5)*level);
                color.rgbGreen = (BYTE)max(0,min(255,(int)(color.rgbGreen + n)));
                n=(long)((rand()/(float)RAND_MAX - 0.5)*level);
                color.rgbBlue = (BYTE)max(0,min(255,(int)(color.rgbBlue + n)));
                BlindSetPixelColor(x,y,color,FALSE);
            }
        }
    }
    return TRUE;
}
static double _cabs(_complex c)
{
    return sqrt(c.x * c.x + c.y * c.y);
}

BOOL CxImage::FFT2(CxImage* srcReal, CxImage* srcImag, CxImage* dstReal, CxImage* dstImag,
                   long direction, BOOL bForceFFT, BOOL bMagnitude)
{
    
    if (srcReal==NULL && srcImag==NULL) return FALSE;

    long w,h;
    
    if (srcReal) {
        w=srcReal->GetWidth();
        h=srcReal->GetHeight();
    } else {
        w=srcImag->GetWidth();
        h=srcImag->GetHeight();
    }

    BOOL bXpow2 = IsPowerof2(w);
    BOOL bYpow2 = IsPowerof2(h);
    
    if (bForceFFT && !(bXpow2 && bYpow2)) {
        long i;
        
        i=0;
        while((1<<i)<w) i++;
        w=1<<i;
        bXpow2=TRUE;

        i=0;
        while((1<<i)<h) i++;
        h=1<<i;
        bYpow2=TRUE;
    }

    
    CxImage *tmpReal,*tmpImag;

    
    tmpReal = (dstReal) ? dstReal : srcReal;
    tmpImag = (dstImag) ? dstImag : srcImag;

    
    if (srcReal && dstReal) tmpReal->Copy(srcReal,TRUE,FALSE,FALSE);
    if (srcImag && dstImag) tmpImag->Copy(srcImag,TRUE,FALSE,FALSE);

    
    if (srcReal==0 && dstReal==0){      
        NEW(tmpReal,CxImage(w,h,8,0));      
        tmpReal->Clear(0);
        tmpReal->SetGrayPalette();
    } else {
        if (!tmpReal->IsGrayScale()) tmpReal->GrayScale();
    }
    if (srcImag==0 && dstImag==0){      
        NEW(tmpImag,CxImage(w,h,8,0));
        tmpImag->Clear(0);
        tmpImag->SetGrayPalette();
    } else {
        if (!tmpImag->IsGrayScale()) tmpImag->GrayScale();
    }

    if (!(tmpReal->IsValid() && tmpImag->IsValid())){
        if (srcReal==0 && dstReal==0) DEL(tmpReal);
        if (srcImag==0 && dstImag==0) DEL(tmpImag);
        return FALSE;
    }

    
    tmpReal->Resample(w,h,0,NULL);
    tmpImag->Resample(w,h,0,NULL);

    

    double* real;
    double* imag;
    long j,k,m;

    _complex **grid;
    
    MALLOC(grid,_complex *,w);
    for (k=0;k<w;k++) {     
        MALLOC(grid[k],_complex,h);
    }
    for (j=0;j<h;j++) {
        for (k=0;k<w;k++) {
            grid[k][j].x = tmpReal->GetPixelIndex(k,j)-128;
            grid[k][j].y = tmpImag->GetPixelIndex(k,j)-128;
        }
    }

    
    double *real2,*imag2;
    MALLOC(real2,double,max(w,h));
    MALLOC(imag2,double,max(w,h));
    
    MALLOC(real,double,w);
    MALLOC(imag,double,w);

    m=0;
    while((1<<m)<w) m++;

    for (j=0;j<h;j++) {
        for (k=0;k<w;k++) {
            real[k] = grid[k][j].x;
            imag[k] = grid[k][j].y;
        }

        if (bXpow2) FFT(direction,m,real,imag);
        else        DFT(direction,w,real,imag,real2,imag2);

        for (k=0;k<w;k++) {
            grid[k][j].x = real[k];
            grid[k][j].y = imag[k];
        }
    }
    FREE(real);
    FREE(imag);

    MALLOC(real,double,h);
    MALLOC(imag,double,h);
    
    m=0;
    while((1<<m)<h) m++;

    for (k=0;k<w;k++) {
        for (j=0;j<h;j++) {
            real[j] = grid[k][j].x;
            imag[j] = grid[k][j].y;
        }

        if (bYpow2) FFT(direction,m,real,imag);
        else        DFT(direction,h,real,imag,real2,imag2);

        for (j=0;j<h;j++) {
            grid[k][j].x = real[j];
            grid[k][j].y = imag[j];
        }
    }
    FREE(real);
    FREE(imag);

    FREE(real2);
    FREE(imag2);

    
    double nn=pow((double)2,(double)log((double)max(w,h))/(double)log((double)2)-4);
    
    if (direction==-1) nn=1/nn;
    
    if (bMagnitude) nn*=4;

    for (j=0;j<h;j++) {
        for (k=0;k<w;k++) {
            if (bMagnitude){
                tmpReal->SetPixelIndex(k,j,(BYTE)max(0,min(255,(nn*(3+log(_cabs(grid[k][j])))))));
                if (grid[k][j].x==0){
                    tmpImag->SetPixelIndex(k,j,(BYTE)max(0,min(255,(128+(atan(grid[k][j].y/0.0000000001)*nn)))));
                } else {
                    tmpImag->SetPixelIndex(k,j,(BYTE)max(0,min(255,(128+(atan(grid[k][j].y/grid[k][j].x)*nn)))));
                }
            } else {
                tmpReal->SetPixelIndex(k,j,(BYTE)max(0,min(255,(128 + grid[k][j].x*nn))));
                tmpImag->SetPixelIndex(k,j,(BYTE)max(0,min(255,(128 + grid[k][j].y*nn))));
            }
        }
    }

    for (k=0;k<w;k++) FREE(grid[k]);
    FREE (grid);

    if (srcReal==0 && dstReal==0) DEL(tmpReal);
    if (srcImag==0 && dstImag==0) DEL(tmpImag);
    
    return FALSE;
}

BOOL CxImage::IsPowerof2(long x)
{
    long i=0;
    while ((1<<i)<x) i++;
    if (x==(1<<i)) return TRUE;
    return FALSE;
}


BOOL CxImage::FFT(int dir,int m,double *x,double *y)
{
    long nn,i,i1,j,k,i2,l,l1,l2;
    double c1,c2,tx,ty,t1,t2,u1,u2,z;

    
    nn = 1<<m;

    
    i2 = nn >> 1;
    j = 0;
    for (i=0;i<nn-1;i++) {
        if (i < j) {
            tx = x[i];
            ty = y[i];
            x[i] = x[j];
            y[i] = y[j];
            x[j] = tx;
            y[j] = ty;
        }
        k = i2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    
    c1 = -1.0;
    c2 = 0.0;
    l2 = 1;
    for (l=0;l<m;l++) {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0;
        u2 = 0.0;
        for (j=0;j<l1;j++) {
            for (i=j;i<nn;i+=l2) {
                i1 = i + l1;
                t1 = u1 * x[i1] - u2 * y[i1];
                t2 = u1 * y[i1] + u2 * x[i1];
                x[i1] = x[i] - t1;
                y[i1] = y[i] - t2;
                x[i] += t1;
                y[i] += t2;
            }
            z =  u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (dir == 1)
            c2 = -c2;
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    
    if (dir == 1) {
        for (i=0;i<nn;i++) {
            x[i] /= (double)nn;
            y[i] /= (double)nn;
        }
    }

   return TRUE;
}


BOOL CxImage::DFT(int dir,long m,double *x1,double *y1,double *x2,double *y2)
{
   long i,k;
   double arg;
   double cosarg,sinarg;
   
   for (i=0;i<m;i++) {
      x2[i] = 0;
      y2[i] = 0;
      arg = - dir * 2.0 * PI * i / (double)m;
      for (k=0;k<m;k++) {
         cosarg = cos(k * arg);
         sinarg = sin(k * arg);
         x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
         y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
      }
   }
   
   
   if (dir == 1) {
      for (i=0;i<m;i++) {
         x1[i] = x2[i] / m;
         y1[i] = y2[i] / m;
      }
   } else {
      for (i=0;i<m;i++) {
         x1[i] = x2[i];
         y1[i] = y2[i];
      }
   }
   
   return TRUE;
}


BOOL CxImage::Combine(CxImage* r,CxImage* g,CxImage* b,CxImage* a, long colorspace)
{
    if (r==0 || g==0 || b==0) return FALSE;

    long w = r->GetWidth();
    long h = r->GetHeight();

    Create(w,h,24,0);

    g->Resample(w,h,1,NULL);
    b->Resample(w,h,1,NULL);

    if (a) {
        a->Resample(w,h,1,NULL);
#if CXIMAGE_SUPPORT_ALPHA
        AlphaCreate();
#endif 
    }

    RGBQUAD c;
    for (long y=0;y<h;y++){
        info.nProgress = (long)(100*y/h); 
        for (long x=0;x<w;x++){
            c.rgbRed=r->GetPixelIndex(x,y);
            c.rgbGreen=g->GetPixelIndex(x,y);
            c.rgbBlue=b->GetPixelIndex(x,y);
            switch (colorspace){
            case 1:
                BlindSetPixelColor(x,y,HSLtoRGB(c),FALSE);
                break;
            case 2:
                BlindSetPixelColor(x,y,YUVtoRGB(c),FALSE);
                break;
            case 3:
                BlindSetPixelColor(x,y,YIQtoRGB(c),FALSE);
                break;
            case 4:
                BlindSetPixelColor(x,y,XYZtoRGB(c),FALSE);
                break;
            default:
                BlindSetPixelColor(x,y,c,FALSE);
            }
#if CXIMAGE_SUPPORT_ALPHA
            if (a) AlphaSet(x,y,a->GetPixelIndex(x,y));
#endif 
        }
    }

    return TRUE;
}


BOOL CxImage::Repair(float radius, long niterations, long colorspace)
{
    if (!IsValid()) return FALSE;

    long w = GetWidth();
    long h = GetHeight();

    CxImage r,g,b;

    r.Create(w,h,8,0);
    g.Create(w,h,8,0);
    b.Create(w,h,8,0);

    switch (colorspace){
    case 1:
        SplitHSL(&r,&g,&b);
        break;
    case 2:
        SplitYUV(&r,&g,&b);
        break;
    case 3:
        SplitYIQ(&r,&g,&b);
        break;
    case 4:
        SplitXYZ(&r,&g,&b);
        break;
    default:
        SplitRGB(&r,&g,&b);
    }
    
    for (int i=0; i<niterations; i++){
        RepairChannel(&r,radius);
        RepairChannel(&g,radius);
        RepairChannel(&b,radius);
    }

    CxImage* a=NULL;
#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()){
        NEW(a,CxImage());
        AlphaSplit(a);
    }
#endif

    Combine(&r,&g,&b,a,colorspace);

    DEL(a);

    return TRUE;
}

BOOL CxImage::RepairChannel(CxImage *ch, float radius)
{
    if (ch==NULL) return FALSE;

    CxImage tmp;

    tmp.Copy(ch);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long w = ch->GetWidth()-1;
    long h = ch->GetHeight()-1;

    double correction,ix,iy,ixx,ixy,iyy;
    int x,y,xy0,xp1,xm1,yp1,ym1;

    for(x=1; x<w; x++){
        for(y=1; y<h; y++){

            xy0 = ch->BlindGetPixelIndex(x,y);
            xm1 = ch->BlindGetPixelIndex(x-1,y);
            xp1 = ch->BlindGetPixelIndex(x+1,y);
            ym1 = ch->BlindGetPixelIndex(x,y-1);
            yp1 = ch->BlindGetPixelIndex(x,y+1);

            ix= (xp1-xm1)/2.0;
            iy= (yp1-ym1)/2.0;
            ixx= xp1 - 2.0 * xy0 + xm1;
            iyy= yp1 - 2.0 * xy0 + ym1;
            ixy=(ch->BlindGetPixelIndex(x+1,y+1) + ch->BlindGetPixelIndex(x-1,y-1) -
                 ch->BlindGetPixelIndex(x-1,y+1) - ch->BlindGetPixelIndex(x+1,y-1))/4.0;

            correction = ((1.0+iy*iy)*ixx - ix*iy*ixy + (1.0+ix*ix)*iyy)/(1.0+ix*ix+iy*iy);

            tmp.BlindSetPixelIndex(x,y,(BYTE)min(255,max(0,(xy0 + radius * correction + 0.5))));
        }
    }

    for (x=0;x<=w;x++){
        for(y=0; y<=h; y+=h){
            xy0 = ch->BlindGetPixelIndex(x,y);
            xm1 = ch->GetPixelIndex(x-1,y);
            xp1 = ch->GetPixelIndex(x+1,y);
            ym1 = ch->GetPixelIndex(x,y-1);
            yp1 = ch->GetPixelIndex(x,y+1);

            ix= (xp1-xm1)/2.0;
            iy= (yp1-ym1)/2.0;
            ixx= xp1 - 2.0 * xy0 + xm1;
            iyy= yp1 - 2.0 * xy0 + ym1;
            ixy=(ch->GetPixelIndex(x+1,y+1) + ch->GetPixelIndex(x-1,y-1) -
                 ch->GetPixelIndex(x-1,y+1) - ch->GetPixelIndex(x+1,y-1))/4.0;

            correction = ((1.0+iy*iy)*ixx - ix*iy*ixy + (1.0+ix*ix)*iyy)/(1.0+ix*ix+iy*iy);

            tmp.BlindSetPixelIndex(x,y,(BYTE)min(255,max(0,(xy0 + radius * correction + 0.5))));
        }
    }
    for (x=0;x<=w;x+=w){
        for (y=0;y<=h;y++){
            xy0 = ch->BlindGetPixelIndex(x,y);
            xm1 = ch->GetPixelIndex(x-1,y);
            xp1 = ch->GetPixelIndex(x+1,y);
            ym1 = ch->GetPixelIndex(x,y-1);
            yp1 = ch->GetPixelIndex(x,y+1);

            ix= (xp1-xm1)/2.0;
            iy= (yp1-ym1)/2.0;
            ixx= xp1 - 2.0 * xy0 + xm1;
            iyy= yp1 - 2.0 * xy0 + ym1;
            ixy=(ch->GetPixelIndex(x+1,y+1) + ch->GetPixelIndex(x-1,y-1) -
                 ch->GetPixelIndex(x-1,y+1) - ch->GetPixelIndex(x+1,y-1))/4.0;

            correction = ((1.0+iy*iy)*ixx - ix*iy*ixy + (1.0+ix*ix)*iyy)/(1.0+ix*ix+iy*iy);

            tmp.BlindSetPixelIndex(x,y,(BYTE)min(255,max(0,(xy0 + radius * correction + 0.5))));
        }
    }

    ch->Transfer(&tmp,TRUE);
    return TRUE;
}


BOOL CxImage::Contour()
{
    if (!pDib) return FALSE;

    long Ksize = 3;
    long k2 = Ksize/2;
    long kmax= Ksize-k2;
    long i,j,k;
    BYTE maxr,maxg,maxb;
    RGBQUAD pix1,pix2;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
                {
                pix1 = BlindGetPixelColor(x,y,TRUE);
                maxr=maxg=maxb=0;
                for(j=-k2, i=0;j<kmax;j++){
                    for(k=-k2;k<kmax;k++, i++){
                        if (!IsInside(x+j,y+k)) continue;
                        pix2 = BlindGetPixelColor(x+j,y+k,TRUE);
                        if ((pix2.rgbBlue-pix1.rgbBlue)>maxb) maxb = pix2.rgbBlue;
                        if ((pix2.rgbGreen-pix1.rgbGreen)>maxg) maxg = pix2.rgbGreen;
                        if ((pix2.rgbRed-pix1.rgbRed)>maxr) maxr = pix2.rgbRed;
                    }
                }
                pix1.rgbBlue=(BYTE)(255-maxb);
                pix1.rgbGreen=(BYTE)(255-maxg);
                pix1.rgbRed=(BYTE)(255-maxr);
                tmp.BlindSetPixelColor(x,y,pix1,FALSE);
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}


BOOL CxImage::Jitter(long radius)
{
    if (!pDib) return FALSE;

    long nx,ny;

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                nx=x+(long)((rand()/(float)RAND_MAX - 0.5)*(radius*2));
                ny=y+(long)((rand()/(float)RAND_MAX - 0.5)*(radius*2));
                if (!IsInside(nx,ny)) {
                    nx=x;
                    ny=y;
                }
                if (head.biClrUsed==0){
                    tmp.BlindSetPixelColor(x,y,BlindGetPixelColor(nx,ny,TRUE),FALSE);
                } else {
                    tmp.BlindSetPixelIndex(x,y,BlindGetPixelIndex(nx,ny));
                }
#if CXIMAGE_SUPPORT_ALPHA
                tmp.AlphaSet(x,y,AlphaGet(nx,ny));
#endif 
            }
        }
    }
    Transfer(&tmp,TRUE);
    return TRUE;
}


int CxImage::gen_convolve_matrix (float radius, float **cmatrix_p)
{
    int matrix_length;
    int matrix_midpoint;
    float* cmatrix;
    int i,j;
    float std_dev;
    float sum;
        
    radius = (float)fabs(0.5*radius) + 0.25f;
    
    std_dev = radius;
    radius = std_dev * 2;
        
    matrix_length = int (2 * ceil(radius-0.5) + 1);
    if (matrix_length <= 0) matrix_length = 1;
    matrix_midpoint = matrix_length/2 + 1;
    NEW_ARRAY(*cmatrix_p,float,matrix_length);
    cmatrix = *cmatrix_p;
    
    for (i = matrix_length/2 + 1; i < matrix_length; i++)
    {
        float base_x = i - (float)floor((float)(matrix_length/2)) - 0.5f;
        sum = 0;
        for (j = 1; j <= 50; j++)
        {
            if ( base_x+0.02*j <= radius ) 
                sum += (float)exp (-(base_x+0.02*j)*(base_x+0.02*j) / 
                (2*std_dev*std_dev));
        }
        cmatrix[i] = sum/50;
    }
    
    
    for (i=0; i<=matrix_length/2; i++) {
        cmatrix[i] = cmatrix[matrix_length-1-i];
    }
    
    
    sum = 0;
    for (j=0; j<=50; j++)
    {
        sum += (float)exp (-(0.5+0.02*j)*(0.5+0.02*j) /
            (2*std_dev*std_dev));
    }
    cmatrix[matrix_length/2] = sum/51;
    
    
    sum=0;
    for (i=0; i<matrix_length; i++) sum += cmatrix[i];
    for (i=0; i<matrix_length; i++) cmatrix[i] = cmatrix[i] / sum;
    
    return matrix_length;
}


float* CxImage::gen_lookup_table (float *cmatrix, int cmatrix_length)
{
    float* lookup_table;
    
    NEW_ARRAY(lookup_table,float,cmatrix_length * 256);

    float* lookup_table_p = lookup_table;
    float* cmatrix_p      = cmatrix;
    
    
    for (int i=0; i<cmatrix_length; i++)
    {
        for (int j=0; j<256; j++)
        {
            *(lookup_table_p++) = *cmatrix_p * (float)j;
        }
        cmatrix_p++;
    }
    
    return lookup_table;
}


void CxImage::blur_line (float *ctable, float *cmatrix, int cmatrix_length, BYTE* cur_col, BYTE* dest_col, int y, long bytes)
{
    float scale;
    float sum;
    int i=0, j=0;
    int row;
    int cmatrix_middle = cmatrix_length/2;
    
    float *cmatrix_p;
    BYTE  *cur_col_p;
    BYTE  *cur_col_p1;
    BYTE  *dest_col_p;
    float *ctable_p;
    
    
    if (cmatrix_length > y)
    {
        for (row = 0; row < y ; row++)
        {
            scale=0;
            
            for (j = 0; j < y ; j++)
            {
                
                if ((j + cmatrix_middle - row >= 0) &&
                    (j + cmatrix_middle - row < cmatrix_length))
                    scale += cmatrix[j + cmatrix_middle - row];
            }
            for (i = 0; i<bytes; i++)
            {
                sum = 0;
                for (j = 0; j < y; j++)
                {
                    if ((j >= row - cmatrix_middle) &&
                        (j <= row + cmatrix_middle))
                        sum += cur_col[j*bytes + i] * cmatrix[j];
                }
                dest_col[row*bytes + i] = (BYTE)(0.5f + sum / scale);
            }
        }
    }
    else
    {
        
        for (row = 0; row < cmatrix_middle; row++)
        {
            
            scale=0;
            for (j = cmatrix_middle - row; j<cmatrix_length; j++)
                scale += cmatrix[j];
            for (i = 0; i<bytes; i++)
            {
                sum = 0;
                for (j = cmatrix_middle - row; j<cmatrix_length; j++)
                {
                    sum += cur_col[(row + j-cmatrix_middle)*bytes + i] * cmatrix[j];
                }
                dest_col[row*bytes + i] = (BYTE)(0.5f + sum / scale);
            }
        }
        
        dest_col_p = dest_col + row*bytes;
        for (; row < y-cmatrix_middle; row++)
        {
            cur_col_p = (row - cmatrix_middle) * bytes + cur_col;
            for (i = 0; i<bytes; i++)
            {
                sum = 0;
                cmatrix_p = cmatrix;
                cur_col_p1 = cur_col_p;
                ctable_p = ctable;
                for (j = cmatrix_length; j>0; j--)
                {
                    sum += *(ctable_p + *cur_col_p1);
                    cur_col_p1 += bytes;
                    ctable_p += 256;
                }
                cur_col_p++;
                *(dest_col_p++) = (BYTE)(0.5f + sum);
            }
        }
        
        
        for (; row < y; row++)
        {
            
            scale=0;
            for (j = 0; j< y-row + cmatrix_middle; j++)
                scale += cmatrix[j];
            for (i = 0; i<bytes; i++)
            {
                sum = 0;
                for (j = 0; j<y-row + cmatrix_middle; j++)
                {
                    sum += cur_col[(row + j-cmatrix_middle)*bytes + i] * cmatrix[j];
                }
                dest_col[row*bytes + i] = (BYTE) (0.5f + sum / scale);
            }
        }
    }
}


void CxImage::blur_text (BYTE threshold, BYTE decay, BYTE max_depth, CxImage* iSrc, CxImage* iDst, BYTE bytes)
{
    long x,y,z,m;
    BYTE *pSrc, *pSrc2, *pSrc3, *pDst;
    BYTE step,n;
    int pivot;

    if (max_depth<1) max_depth = 1;

    long nmin,nmax,xmin,xmax,ymin,ymax;
    xmin = ymin = 0;
    xmax = iSrc->head.biWidth;
    ymax = iSrc->head.biHeight;

    if (xmin==xmax || ymin==ymax) return;

    nmin = xmin * bytes;
    nmax = xmax * bytes;

    CImageIterator itSrc(iSrc);
    CImageIterator itTmp(iDst);

    double dbScaler = 100.0f/(ymax-ymin)/bytes;

    for (n=0; n<bytes; n++){
        for (y=ymin+1;y<(ymax-1);y++)
        {
            if (info.nEscape) break;
            info.nProgress = (long)((y-ymin)*dbScaler*(1+n));

            pSrc  = itSrc.GetRow(y);
            pSrc2 = itSrc.GetRow(y+1);
            pSrc3 = itSrc.GetRow(y-1);
            pDst  = itTmp.GetRow(y);

            
            for (x=n+nmin ; x<(nmax-1); x+=bytes )
            {
                z=x+bytes;
                pivot = pSrc[z]-threshold;
                
                if (pSrc[x]<pivot && pSrc2[z]<pivot && pSrc3[x]>=pivot){
                    while (z<nmax && pSrc2[z]<pSrc[x+bytes] && pSrc[x+bytes]<=pSrc[z]){
                        z+=bytes;
                    }
                    m = z-x;
                    m = (decay>1) ? ((m/bytes)/decay+1) : m/bytes;
                    if (m>max_depth) m = max_depth;
                    step = (BYTE)((pSrc[x+bytes]-pSrc[x])/(m+1));
                    while (m-->1){
                        pDst[x+m*bytes] = (BYTE)(pDst[x]+(step*(m+1)));
                    }
                }
                
                z=x+bytes;
                if (pSrc[x]<pivot && pSrc3[z]<pivot && pSrc2[x]>=pivot){
                    while (z<nmax && pSrc3[z]<pSrc[x+bytes] && pSrc[x+bytes]<=pSrc[z]){
                        z+=bytes;
                    }
                    m = z-x;
                    m = (decay>1) ? ((m/bytes)/decay+1) : m/bytes;
                    if (m>max_depth) m = max_depth;
                    step = (BYTE)((pSrc[x+bytes]-pSrc[x])/(m+1));
                    while (m-->1){
                        pDst[x+m*bytes] = (BYTE)(pDst[x]+(step*(m+1)));
                    }
                }
            }
            
            for (x=nmax-1-n ; x>0; x-=bytes )
            {
                z=x-bytes;
                pivot = pSrc[z]-threshold;
                
                if (pSrc[x]<pivot && pSrc2[z]<pivot && pSrc3[x]>=pivot){
                    while (z>n && pSrc2[z]<pSrc[x-bytes] && pSrc[x-bytes]<=pSrc[z]){
                        z-=bytes;
                    }
                    m = x-z;
                    m = (decay>1) ? ((m/bytes)/decay+1) : m/bytes;
                    if (m>max_depth) m = max_depth;
                    step = (BYTE)((pSrc[x-bytes]-pSrc[x])/(m+1));
                    while (m-->1){
                        pDst[x-m*bytes] = (BYTE)(pDst[x]+(step*(m+1)));
                    }
                }
                
                z=x-bytes;
                if (pSrc[x]<pivot && pSrc3[z]<pivot && pSrc2[x]>=pivot){
                    while (z>n && pSrc3[z]<pSrc[x-bytes] && pSrc[x-bytes]<=pSrc[z]){
                        z-=bytes;
                    }
                    m = x-z;
                    m = (decay>1) ? ((m/bytes)/decay+1) : m/bytes;
                    if (m>max_depth) m = max_depth;
                    step = (BYTE)((pSrc[x-bytes]-pSrc[x])/(m+1));
                    while (m-->1){
                        pDst[x-m*bytes] = (BYTE)(pDst[x]+(step*(m+1)));
                    }
                }
            }
        }
    }
}


BOOL CxImage::TextBlur(BYTE threshold, BYTE decay, BYTE max_depth, BOOL bBlurHorizontal, BOOL bBlurVertical, CxImage* iDst)
{
    if (!pDib) return FALSE;

    RGBQUAD* pPalette=NULL;
    WORD bpp = GetBpp();

    
    if (!(head.biBitCount == 24 || IsGrayScale())){
        NEW_ARRAY(pPalette,RGBQUAD,head.biClrUsed);
        memcpy(pPalette, GetPalette(),GetPaletteSize());
        if (!IncreaseBpp(24))
            return FALSE;
    }

    CxImage tmp;
    tmp.Copy(this);

    if (!tmp.IsValid()){
        strcpy(info.szLastError,tmp.GetLastError());
        return FALSE;
    }

    if (bBlurHorizontal)
        blur_text(threshold, decay, max_depth, this, &tmp, head.biBitCount>>3);

    if (bBlurVertical){
        CxImage src2;
        src2.Copy(this);

        src2.RotateLeft(NULL);
        tmp.RotateLeft(NULL);
        blur_text(threshold, decay, max_depth, &src2, &tmp, head.biBitCount>>3);
        tmp.RotateRight(NULL);
    }

#if CXIMAGE_SUPPORT_SELECTION
    
    if (pSelection){
        for(long y=0; y<head.biHeight; y++){
            for(long x=0; x<head.biWidth; x++){
                if (!BlindSelectionIsInside(x,y)){
                    tmp.BlindSetPixelColor(x,y,BlindGetPixelColor(x,y,TRUE),FALSE);
                }
            }
        }
    }
#endif 

    
    if (pPalette){
        tmp.DecreaseBpp(bpp, TRUE, pPalette,0);
        DEL_ARRAY(pPalette);
    }

    if (iDst) iDst->Transfer(&tmp,TRUE);
    else Transfer(&tmp,TRUE);

    return TRUE;
}


BOOL CxImage::GaussianBlur(float radius , CxImage* iDst )
{
    if (!pDib) return FALSE;

    RGBQUAD* pPalette=NULL;
    WORD bpp = GetBpp();

    
    if (!(head.biBitCount == 24 || IsGrayScale())){     
        NEW_ARRAY(pPalette,RGBQUAD,head.biClrUsed);
        memcpy(pPalette, GetPalette(),GetPaletteSize());
        if (!IncreaseBpp(24))
            return FALSE;
    }

    CxImage tmp_x(this, FALSE, TRUE, TRUE);
    if (!tmp_x.IsValid()){
        strcpy(info.szLastError,tmp_x.GetLastError());
        return FALSE;
    }

    
    float *cmatrix = NULL;
    int cmatrix_length = gen_convolve_matrix(radius, &cmatrix);
    
    float *ctable = gen_lookup_table(cmatrix, cmatrix_length);

    long x,y;
    int bypp = head.biBitCount>>3;

    CImageIterator itSrc(this);
    CImageIterator itTmp(&tmp_x);

    double dbScaler = 50.0f/head.biHeight;

    
    for (y=0;y<head.biHeight;y++)
    {
        if (info.nEscape) break;
        info.nProgress = (long)(y*dbScaler);

        blur_line(ctable, cmatrix, cmatrix_length, itSrc.GetRow(y), itTmp.GetRow(y), head.biWidth, bypp);
    }

    CxImage tmp_y(&tmp_x, FALSE, TRUE, TRUE);
    if (!tmp_y.IsValid()){
        strcpy(info.szLastError,tmp_y.GetLastError());
        return FALSE;
    }

    CImageIterator itDst(&tmp_y);
    
    BYTE* cur_col;
    MALLOC(cur_col,BYTE,bypp*head.biHeight);

    BYTE* dest_col;
    MALLOC(dest_col,BYTE,bypp*head.biHeight);

    dbScaler = 50.0f/head.biWidth;

    for (x=0;x<head.biWidth;x++)
    {
        if (info.nEscape) break;
        info.nProgress = (long)(50.0f+x*dbScaler);

        itTmp.GetCol(cur_col, x);
        itDst.GetCol(dest_col, x);
        blur_line(ctable, cmatrix, cmatrix_length, cur_col, dest_col, head.biHeight, bypp);
        itDst.SetCol(dest_col, x);
    }

    FREE(cur_col);
    FREE(dest_col);

    DEL_ARRAY(cmatrix);
    DEL_ARRAY(ctable);

#if CXIMAGE_SUPPORT_SELECTION
    
    if (pSelection){
        for(y=0; y<head.biHeight; y++){
            for(x=0; x<head.biWidth; x++){
                if (!BlindSelectionIsInside(x,y)){
                    tmp_y.BlindSetPixelColor(x,y,BlindGetPixelColor(x,y,TRUE),FALSE);
                }
            }
        }
    }
#endif 

    
    if (pPalette){
        tmp_y.DecreaseBpp(bpp, FALSE, pPalette,0);
        if (iDst) DecreaseBpp(bpp, FALSE, pPalette,0);
        DEL_ARRAY(pPalette);
    }

    if (iDst) iDst->Transfer(&tmp_y,TRUE);
    else Transfer(&tmp_y,TRUE);

    return TRUE;
}


BOOL CxImage::SelectiveBlur(float radius, BYTE threshold, CxImage* iDst)
{
    if (!pDib) return FALSE;

    RGBQUAD* pPalette=NULL;
    WORD bpp = GetBpp();

    CxImage Tmp(this, TRUE, TRUE, TRUE);
    if (!Tmp.IsValid()){
        strcpy(info.szLastError,Tmp.GetLastError());
        return FALSE;
    }

    
    if (!(head.biBitCount == 24 || IsGrayScale())){
        NEW_ARRAY(pPalette,RGBQUAD,head.biClrUsed);
        memcpy(pPalette, GetPalette(),GetPaletteSize());
        if (!Tmp.IncreaseBpp(24))
            return FALSE;
    }

    CxImage Dst(&Tmp, TRUE, TRUE, TRUE);
    if (!Dst.IsValid()){
        strcpy(info.szLastError,Dst.GetLastError());
        return FALSE;
    }

    
    BYTE thresh_dw = (BYTE)max( 0 ,(int)(128 - threshold));
    BYTE thresh_up = (BYTE)min(255,(int)(128 + threshold));
    long kernel[]={-100,-100,-100,-100,801,-100,-100,-100,-100};
    if (!Tmp.Filter(kernel,3,800,128)){
        strcpy(info.szLastError,Tmp.GetLastError());
        return FALSE;
    }

    
    if (!Tmp.SelectionIsValid()){
        Tmp.SelectionCreate();
        Tmp.SelectionClear(255);
    }

    long xmin,xmax,ymin,ymax;
    xmin = Tmp.info.rSelectionBox.left;
    xmax = Tmp.info.rSelectionBox.right;
    ymin = Tmp.info.rSelectionBox.bottom;
    ymax = Tmp.info.rSelectionBox.top;

    
    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
            if(Tmp.BlindSelectionIsInside(x,y)){
                RGBQUAD c = Tmp.BlindGetPixelColor(x,y,TRUE);
                if ((c.rgbRed   < thresh_dw || c.rgbRed   > thresh_up) ||
                    (c.rgbGreen < thresh_dw || c.rgbGreen > thresh_up) ||
                    (c.rgbBlue  < thresh_dw || c.rgbBlue  > thresh_up))
                {
                    Tmp.SelectionSet(x,y,0);
                }
            }
        }
    }

    
    Dst.SelectionCopy(&Tmp);
    if (!Dst.GaussianBlur(radius,NULL)){
        strcpy(info.szLastError,Dst.GetLastError());
        return FALSE;
    }

    
    Dst.SelectionCopy(this);

    
    if (pPalette){
        Dst.DecreaseBpp(bpp, FALSE, pPalette,0);
        DEL_ARRAY(pPalette);
    }

    if (iDst) iDst->Transfer(&Dst,TRUE);
    else Transfer(&Dst,TRUE);

    return TRUE;
}


BOOL CxImage::UnsharpMask(float radius , float amount , int threshold )
{
    if (!pDib) return FALSE;

    RGBQUAD* pPalette=NULL;
    WORD bpp = GetBpp();

    
    if (!(head.biBitCount == 24 || IsGrayScale())){
        NEW_ARRAY(pPalette,RGBQUAD,head.biClrUsed);
        memcpy(pPalette, GetPalette(),GetPaletteSize());
        if (!IncreaseBpp(24))
            return FALSE;
    }

    CxImage iDst;
    if (!GaussianBlur(radius,&iDst))
        return FALSE;

    CImageIterator itSrc(this);
    CImageIterator itDst(&iDst);

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    if (xmin==xmax || ymin==ymax)
        return FALSE;

    double dbScaler = 100.0/(ymax-ymin);
    int bypp = head.biBitCount>>3;
    
    
    
    for (long y=ymin; y<ymax; y++)
    {
        if (info.nEscape) break;
        info.nProgress = (long)((y-ymin)*dbScaler);

        
        BYTE* cur_row = itSrc.GetRow(y);
        
        BYTE* dest_row = itDst.GetRow(y);
        
        for (long x=xmin; x<xmax; x++) {
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                for (long b=0, z=x*bypp; b<bypp; b++, z++){
                    int diff = cur_row[z] - dest_row[z];

                    
                    if (abs(diff) < threshold){
                        dest_row[z] = cur_row[z];
                    } else {
                        dest_row[z] = (BYTE)min(255, max(0,(int)(cur_row[z] + amount * diff)));
                    }
                }
            }
        }
    }

    
    if (pPalette){
        iDst.DecreaseBpp(bpp, FALSE, pPalette,0);
        DEL_ARRAY(pPalette);
    }

    Transfer(&iDst,TRUE);

    return TRUE;
}


BOOL CxImage::Lut(BYTE* pLut)
{
    if (!pDib || !pLut) return FALSE;
    RGBQUAD color;

    double dbScaler;
    if (head.biClrUsed==0){

        long xmin,xmax,ymin,ymax;
        if (pSelection){
            xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
            ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
        } else {
            
            BYTE *iSrc=info.pImage;
            for(unsigned long i=0; i < head.biSizeImage ; i++){
                *iSrc = pLut[*iSrc];
                iSrc++;
            }
            return TRUE;
        }

        if (xmin==xmax || ymin==ymax)
            return FALSE;

        dbScaler = 100.0/(ymax-ymin);

        for(long y=ymin; y<ymax; y++){
            info.nProgress = (long)((y-ymin)*dbScaler); 
            for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                if (BlindSelectionIsInside(x,y))
#endif 
                {
                    color = BlindGetPixelColor(x,y,TRUE);
                    color.rgbRed = pLut[color.rgbRed];
                    color.rgbGreen = pLut[color.rgbGreen];
                    color.rgbBlue = pLut[color.rgbBlue];
                    BlindSetPixelColor(x,y,color,FALSE);
                }
            }
        }
#if CXIMAGE_SUPPORT_SELECTION
    } else if (pSelection && (head.biBitCount==8) && IsGrayScale()){
        long xmin,xmax,ymin,ymax;
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;

        if (xmin==xmax || ymin==ymax)
            return FALSE;

        dbScaler = 100.0/(ymax-ymin);
        for(long y=ymin; y<ymax; y++){
            info.nProgress = (long)((y-ymin)*dbScaler);
            for(long x=xmin; x<xmax; x++){
                if (BlindSelectionIsInside(x,y))
                {
                    BlindSetPixelIndex(x,y,pLut[BlindGetPixelIndex(x,y)]);
                }
            }
        }
#endif 
    } else {
        BOOL bIsGrayScale = IsGrayScale();
        for(DWORD j=0; j<head.biClrUsed; j++){
            color = GetPaletteColor((BYTE)j);
            color.rgbRed = pLut[color.rgbRed];
            color.rgbGreen = pLut[color.rgbGreen];
            color.rgbBlue = pLut[color.rgbBlue];
            SetPaletteColor((BYTE)j,color);
        }
        if (bIsGrayScale) GrayScale();
    }
    return TRUE;

}


BOOL CxImage::Lut(BYTE* pLutR, BYTE* pLutG, BYTE* pLutB, BYTE* pLutA)
{
    if (!pDib || !pLutR || !pLutG || !pLutB) return FALSE;
    RGBQUAD color;

    double dbScaler;
    if (head.biClrUsed==0){

        long xmin,xmax,ymin,ymax;
        if (pSelection){
            xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
            ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
        } else {
            xmin = ymin = 0;
            xmax = head.biWidth; ymax=head.biHeight;
        }

        if (xmin==xmax || ymin==ymax)
            return FALSE;

        dbScaler = 100.0/(ymax-ymin);

        for(long y=ymin; y<ymax; y++){
            info.nProgress = (long)((y-ymin)*dbScaler);
            for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                if (BlindSelectionIsInside(x,y))
#endif 
                {
                    color = BlindGetPixelColor(x,y,TRUE);
                    color.rgbRed =   pLutR[color.rgbRed];
                    color.rgbGreen = pLutG[color.rgbGreen];
                    color.rgbBlue =  pLutB[color.rgbBlue];
                    if (pLutA) color.rgbReserved=pLutA[color.rgbReserved];
                    BlindSetPixelColor(x,y,color,FALSE);
                }
            }
        }
    } else {
        BOOL bIsGrayScale = IsGrayScale();
        for(DWORD j=0; j<head.biClrUsed; j++){
            color = GetPaletteColor((BYTE)j);
            color.rgbRed =   pLutR[color.rgbRed];
            color.rgbGreen = pLutG[color.rgbGreen];
            color.rgbBlue =  pLutB[color.rgbBlue];
            SetPaletteColor((BYTE)j,color);
        }
        if (bIsGrayScale) GrayScale();
    }

    return TRUE;

}


BOOL CxImage::RedEyeRemove(float strength)
{
    if (!pDib) return FALSE;
    RGBQUAD color;

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    if (xmin==xmax || ymin==ymax)
        return FALSE;

    if (strength<0.0f) strength = 0.0f;
    if (strength>1.0f) strength = 1.0f;

    for(long y=ymin; y<ymax; y++){
        info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
        if (info.nEscape) break;
        for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
            if (BlindSelectionIsInside(x,y))
#endif 
            {
                float a = 1.0f-5.0f*((float)((x-0.5f*(xmax+xmin))*(x-0.5f*(xmax+xmin))+(y-0.5f*(ymax+ymin))*(y-0.5f*(ymax+ymin))))/((float)((xmax-xmin)*(ymax-ymin)));
                if (a<0) a=0;
                color = BlindGetPixelColor(x,y,TRUE);
                color.rgbRed = (BYTE)(a*min(color.rgbGreen,color.rgbBlue)+(1.0f-a)*color.rgbRed);
                BlindSetPixelColor(x,y,color,FALSE);
            }
        }
    }
    return TRUE;
}


BOOL CxImage::Saturate(const long saturation, const long colorspace)
{
    if (!pDib)
        return FALSE;

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    if (xmin==xmax || ymin==ymax)
        return FALSE;

    BYTE cTable[256];

    switch(colorspace)
    {
    case 1:
        {
            for (int i=0;i<256;i++) {
                cTable[i] = (BYTE)max(0,min(255,(int)(i + saturation)));
            }
            for(long y=ymin; y<ymax; y++){
                info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
                if (info.nEscape) break;
                for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                    if (BlindSelectionIsInside(x,y))
#endif 
                    {
                        RGBQUAD c = RGBtoHSL(BlindGetPixelColor(x,y,TRUE));
                        c.rgbGreen  = cTable[c.rgbGreen];
                        c = HSLtoRGB(c);
                        BlindSetPixelColor(x,y,c,FALSE);
                    }
                }
            }
        }
        break;
    case 2:
        {
            for (int i=0;i<256;i++) {
                cTable[i] = (BYTE)max(0,min(255,(int)((i-128)*(100 + saturation)/100.0f + 128.5f)));
            }
            for(long y=ymin; y<ymax; y++){
                info.nProgress = (long)(100*(y-ymin)/(ymax-ymin));
                if (info.nEscape) break;
                for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                    if (BlindSelectionIsInside(x,y))
#endif 
                    {
                        RGBQUAD c = RGBtoYUV(BlindGetPixelColor(x,y,TRUE));
                        c.rgbGreen  = cTable[c.rgbGreen];
                        c.rgbBlue = cTable[c.rgbBlue];
                        c = YUVtoRGB(c);
                        BlindSetPixelColor(x,y,c,FALSE);
                    }
                }
            }
        }
        break;
    default:
        strcpy(info.szLastError,"Saturate: wrong colorspace");
        return FALSE;
    }
    return TRUE;
}



BOOL CxImage::Solarize(BYTE level, BOOL bLinkedChannels)
{
    if (!pDib) return FALSE;

    long xmin,xmax,ymin,ymax;
    if (pSelection){
        xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
        ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }

    if (head.biBitCount<=8){
        if (IsGrayScale()){ 
            for(long y=ymin; y<ymax; y++){
                for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                    if (BlindSelectionIsInside(x,y))
#endif 
                    {
                        BYTE index = BlindGetPixelIndex(x,y);
                        RGBQUAD color = GetPaletteColor(index);
                        if ((BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue)>level){
                            BlindSetPixelIndex(x,y,255-index);
                        }
                    }
                }
            }
        } else { 
            RGBQUAD* ppal=GetPalette();
            for(DWORD i=0;i<head.biClrUsed;i++){
                RGBQUAD color = GetPaletteColor((BYTE)i);
                if (bLinkedChannels){
                    if ((BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue)>level){
                        ppal[i].rgbBlue =(BYTE)(255-ppal[i].rgbBlue);
                        ppal[i].rgbGreen =(BYTE)(255-ppal[i].rgbGreen);
                        ppal[i].rgbRed =(BYTE)(255-ppal[i].rgbRed);
                    }
                } else {
                    if (color.rgbBlue>level)    ppal[i].rgbBlue =(BYTE)(255-ppal[i].rgbBlue);
                    if (color.rgbGreen>level)   ppal[i].rgbGreen =(BYTE)(255-ppal[i].rgbGreen);
                    if (color.rgbRed>level)     ppal[i].rgbRed =(BYTE)(255-ppal[i].rgbRed);
                }
            }
        }
    } else { 
        for(long y=ymin; y<ymax; y++){
            for(long x=xmin; x<xmax; x++){
#if CXIMAGE_SUPPORT_SELECTION
                if (BlindSelectionIsInside(x,y))
#endif 
                {
                    RGBQUAD color = BlindGetPixelColor(x,y,TRUE);
                    if (bLinkedChannels){
                        if ((BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue)>level){
                            color.rgbRed = (BYTE)(255-color.rgbRed);
                            color.rgbGreen = (BYTE)(255-color.rgbGreen);
                            color.rgbBlue = (BYTE)(255-color.rgbBlue);
                        }
                    } else {
                        if (color.rgbBlue>level)    color.rgbBlue =(BYTE)(255-color.rgbBlue);
                        if (color.rgbGreen>level)   color.rgbGreen =(BYTE)(255-color.rgbGreen);
                        if (color.rgbRed>level)     color.rgbRed =(BYTE)(255-color.rgbRed);
                    }
                    BlindSetPixelColor(x,y,color,FALSE);
                }
            }
        }
    }

    
    if (pSelection==0 || (!IsGrayScale() && IsIndexed())){
        if (bLinkedChannels){
            if ((BYTE)RGB2GRAY(info.nBkgndColor.rgbRed,info.nBkgndColor.rgbGreen,info.nBkgndColor.rgbBlue)>level){
                info.nBkgndColor.rgbBlue = (BYTE)(255-info.nBkgndColor.rgbBlue);
                info.nBkgndColor.rgbGreen = (BYTE)(255-info.nBkgndColor.rgbGreen);
                info.nBkgndColor.rgbRed = (BYTE)(255-info.nBkgndColor.rgbRed);
            } 
        } else {
            if (info.nBkgndColor.rgbBlue>level)  info.nBkgndColor.rgbBlue = (BYTE)(255-info.nBkgndColor.rgbBlue);
            if (info.nBkgndColor.rgbGreen>level) info.nBkgndColor.rgbGreen = (BYTE)(255-info.nBkgndColor.rgbGreen);
            if (info.nBkgndColor.rgbRed>level)   info.nBkgndColor.rgbRed = (BYTE)(255-info.nBkgndColor.rgbRed);
        }
    }

    return TRUE;
}



BOOL CxImage::ConvertColorSpace(const long dstColorSpace, const long srcColorSpace)
{
    if (!pDib)
        return FALSE;

    if (dstColorSpace == srcColorSpace)
        return TRUE;

    long w = GetWidth();
    long h = GetHeight();

    for (long y=0;y<h;y++){
        info.nProgress = (long)(100*y/h);
        if (info.nEscape) break;
        for (long x=0;x<w;x++){
            RGBQUAD c = BlindGetPixelColor(x,y,TRUE);
            switch (srcColorSpace){
            case 0:
                break;
            case 1:
                c = HSLtoRGB(c);
                break;
            case 2:
                c = YUVtoRGB(c);
                break;
            case 3:
                c = YIQtoRGB(c);
                break;
            case 4:
                c = XYZtoRGB(c);
                break;
            default:
                strcpy(info.szLastError,"ConvertColorSpace: unknown source colorspace");
                return FALSE;
            }
            switch (dstColorSpace){
            case 0:
                break;
            case 1:
                c = RGBtoHSL(c);
                break;
            case 2:
                c = RGBtoYUV(c);
                break;
            case 3:
                c = RGBtoYIQ(c);
                break;
            case 4:
                c = RGBtoXYZ(c);
                break;
            default:
                strcpy(info.szLastError,"ConvertColorSpace: unknown destination colorspace");
                return FALSE;
            }
            BlindSetPixelColor(x,y,c,FALSE);
        }
    }
    return TRUE;
}


int  CxImage::OptimalThreshold(long method, RECT * pBox, CxImage* pContrastMask)
{
    if (!pDib)
        return FALSE;

    if (head.biBitCount!=8){
        strcpy(info.szLastError,"OptimalThreshold works only on 8 bit images");
        return -1;
    }

    if (pContrastMask){
        if (!pContrastMask->IsValid() ||
            !pContrastMask->IsGrayScale() ||
            pContrastMask->GetWidth() != GetWidth() ||
            pContrastMask->GetHeight() != GetHeight()){
            strcpy(info.szLastError,"OptimalThreshold invalid ContrastMask");
            return -1;
        }
    }

    long xmin,xmax,ymin,ymax;
    if (pBox){
        xmin = max(pBox->left,0);
        xmax = min(pBox->right,head.biWidth);
        ymin = max(pBox->bottom,0);
        ymax = min(pBox->top,head.biHeight);
    } else {
        xmin = ymin = 0;
        xmax = head.biWidth; ymax=head.biHeight;
    }
    
    if (xmin>=xmax || ymin>=ymax)
        return -1;

    double p[256];
    memset(p,  0, 256*sizeof(double));
    
    for (long y = ymin; y<ymax; y++){
        BYTE* pGray = GetBits(y) + xmin;
        BYTE* pContr = 0;
        if (pContrastMask) pContr = pContrastMask->GetBits(y) + xmin;
        for (long x = xmin; x<xmax; x++){
            BYTE n = *pGray++;
            if (pContr){
                if (*pContr) p[n]++;
                pContr++;
            } else {
                p[n]++;
            }
        }
    }

    
    int gray_min = 0;
    while (gray_min<255 && p[gray_min]==0) gray_min++;
    int gray_max = 255;
    while (gray_max>0 && p[gray_max]==0) gray_max--;
    if (gray_min > gray_max)
        return -1;
    if (gray_min == gray_max){
        if (gray_min == 0)
            return 0;
        else
            return gray_max-1;
    }

    
    int i,k;
    double w_tot = 0;
    double m_tot = 0;
    double q_tot = 0;
    for (i = gray_min; i <= gray_max; i++){
        w_tot += p[i];
        m_tot += i*p[i];
        q_tot += i*i*p[i];
    }

    double L, L1max, L2max, L3max, L4max; 
    int th1,th2,th3,th4; 
    L1max = L2max = L3max = L4max = 0;
    th1 = th2 = th3 = th4 = -1;

    double w1, w2, m1, m2, q1, q2, s1, s2;
    w1 = m1 = q1 = 0;
    for (i = gray_min; i < gray_max; i++){
        w1 += p[i];
        w2 = w_tot - w1;
        m1 += i*p[i];
        m2 = m_tot - m1;
        q1 += i*i*p[i];
        q2 = q_tot - q1;
        s1 = q1/w1-m1*m1/w1/w1; 
        s2 = q2/w2-m2*m2/w2/w2; 

        
        L = -(s1*w1 + s2*w2); 
        
        if (L1max < L || th1<0){
            L1max = L;
            th1 = i;
        }

        
        if (s1>0 && s2>0){
            L = w1*log(w1/sqrt(s1))+w2*log(w2/sqrt(s2));
            
            if (L2max < L || th2<0){
                L2max = L;
                th2 = i;
            }
        }

        
        L = 0;
        for (k=gray_min;k<=i;k++) if (p[k] > 0) L -= p[k]*log(p[k]/w1)/w1;
        for (;k<=gray_max;k++) if (p[k] > 0)    L -= p[k]*log(p[k]/w2)/w2;
        if (L3max < L || th3<0){
            L3max = L;
            th3 = i;
        }

        
        
        
        double vdiff = 0;
        for (k=gray_min;k<=i;k++)
            vdiff += p[k]*(i-k)*(i-k);
        double vsum = vdiff;
        for (;k<=gray_max;k++){
            double dv = p[k]*(k-i)*(k-i);
            vdiff -= dv;
            vsum += dv;
        }
        if (vsum>0) L = -fabs(vdiff/vsum); else L = 0;
        if (L4max < L || th4<0){
            L4max = L;
            th4 = i;
        }
    }

    int threshold;
    switch (method){
    case 1: 
        threshold = th1;
        break;
    case 2: 
        threshold = th2;
        break;
    case 3: 
        threshold = th3;
        break;
    case 4: 
        threshold = th4;
        break;
    default: 
        {
            int nt = 0;
            threshold = 0;
            if (th1>=0) { threshold += th1; nt++;}
            if (th2>=0) { threshold += th2; nt++;}
            if (th3>=0) { threshold += th3; nt++;}
            if (th4>=0) { threshold += th4; nt++;}
            if (nt)
                threshold /= nt;
            else
                threshold = (gray_min+gray_max)/2;

            

        }
    }

    if (threshold <= gray_min || threshold >= gray_max)
        threshold = (gray_min+gray_max)/2;
    
    return threshold;
}


BOOL CxImage::AdaptiveThreshold(long method, long nBoxSize, CxImage* pContrastMask, long nBias, float fGlobalLocalBalance)
{
    if (!pDib)
        return FALSE;

    if (pContrastMask){
        if (!pContrastMask->IsValid() ||
            !pContrastMask->IsGrayScale() ||
            pContrastMask->GetWidth() != GetWidth() ||
            pContrastMask->GetHeight() != GetHeight()){
            strcpy(info.szLastError,"AdaptiveThreshold invalid ContrastMask");
            return FALSE;
        }
    }

    if (nBoxSize<8) nBoxSize = 8;
    if (fGlobalLocalBalance<0.0f) fGlobalLocalBalance = 0.0f;
    if (fGlobalLocalBalance>1.0f) fGlobalLocalBalance = 1.0f;

    long mw = (head.biWidth + nBoxSize - 1)/nBoxSize;
    long mh = (head.biHeight + nBoxSize - 1)/nBoxSize;

    CxImage mask(mw,mh,8,0);
    if(!mask.GrayScale())
        return FALSE;

    if(!GrayScale())
        return FALSE;

    int globalthreshold = OptimalThreshold(method, 0, pContrastMask);
    if (globalthreshold <0)
        return FALSE;

    for (long y=0; y<mh; y++){
        for (long x=0; x<mw; x++){
            info.nProgress = (long)(100*(x+y*mw)/(mw*mh));
            if (info.nEscape) break;
            RECT r;
            r.left = x*nBoxSize;
            r.right = r.left + nBoxSize;
            r.bottom = y*nBoxSize;
            r.top = r.bottom + nBoxSize;
            int threshold = OptimalThreshold(method, &r, pContrastMask);
            if (threshold <0) return FALSE;
            mask.SetPixelIndex(x,y,(BYTE)max(0,min(255,nBias+((1.0f-fGlobalLocalBalance)*threshold + fGlobalLocalBalance*globalthreshold))));
        }
    }

    mask.Resample(mw*nBoxSize,mh*nBoxSize,0,NULL);
    mask.Crop(0,head.biHeight,head.biWidth,0,NULL);

    if(!Threshold(&mask))
        return FALSE;

    return TRUE;
}
#endif //CXIMAGE_SUPPORT_DSP
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_DSP END
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_WINDOWS
//////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_WINDOWS

long CxImage::Blt(HDC pDC, long x, long y)
{
    if((pDib==0)||(pDC==0)||(!info.bEnabled)) return 0;

    HBRUSH brImage = CreateDIBPatternBrushPt(pDib, DIB_RGB_COLORS);
    POINT pt;
    SetBrushOrgEx(pDC,x,y,&pt); //<RT>
    HBRUSH brOld = (HBRUSH) SelectObject(pDC, brImage);
    PatBlt(pDC, x, y, head.biWidth, head.biHeight, PATCOPY);
    SelectObject(pDC, brOld);
    SetBrushOrgEx(pDC,pt.x,pt.y,NULL);
    DeleteObject(brImage);
    return 1;
}

HANDLE CxImage::CopyToHandle()
{
    HANDLE hMem=NULL;
    if (pDib){
        hMem= GlobalAlloc(GHND, GetSize());
        if (hMem){
            BYTE* pDst=(BYTE*)GlobalLock(hMem);
            if (pDst){
                memcpy(pDst,pDib,GetSize());
            }
            GlobalUnlock(hMem);
        }
    }
    return hMem;
}

bool CxImage::CreateFromHANDLE(HANDLE hMem)
{
    if (!Destroy())
        return false;

    DWORD dwSize = GlobalSize(hMem);
    if (!dwSize) return false;

    BYTE *lpVoid;                       //pointer to the bitmap
    lpVoid = (BYTE *)GlobalLock(hMem);
    BITMAPINFOHEADER *pHead;            //pointer to the bitmap header
    pHead = (BITMAPINFOHEADER *)lpVoid;
    if (lpVoid){

        //CxMemFile hFile(lpVoid,dwSize);

        //copy the bitmap header
        memcpy(&head,pHead,sizeof(BITMAPINFOHEADER));
        //check if it's a top-down bitmap
        bool bTopDownDib = head.biHeight<0;
        if (bTopDownDib) head.biHeight=-head.biHeight;
        //create the image
        if(!Create(head.biWidth,head.biHeight,head.biBitCount,CXIMAGE_FORMAT_UNKNOWN)){
            GlobalUnlock(lpVoid);
            return false;
        }
        //preserve DPI
        SetXDPI((long)floor(head.biXPelsPerMeter * 254.0 / 10000.0 + 0.5));
        SetYDPI((long)floor(head.biYPelsPerMeter * 254.0 / 10000.0 + 0.5));

        /*//copy the pixels (old way)
        if((pHead->biCompression != BI_RGB) || (pHead->biBitCount == 32)){ //<J\F6rgen Alfredsson>
            // BITFIELD case
            // set the internal header in the dib
            memcpy(pDib,&head,sizeof(head));
            // get the bitfield masks
            DWORD bf[3];
            memcpy(bf,lpVoid+pHead->biSize,12);
            // transform into RGB
            Bitfield2RGB(lpVoid+pHead->biSize+12,bf[0],bf[1],bf[2],(BYTE)pHead->biBitCount);
        } else { //normal bitmap
            memcpy(pDib,lpVoid,GetSize());
        }*/

        // <Michael Gandyra>
        // fill in color map
        bool bIsOldBmp = (head.biSize == sizeof(BITMAPCOREHEADER));
        RGBQUAD *pRgb = GetPalette();
        if (pRgb) {
            // number of colors to fill in
            int nColors = DibNumColors(pHead);
            if (bIsOldBmp) {
                /* get pointer to BITMAPCOREINFO (old style 1.x) */
                LPBITMAPCOREINFO lpbmc = (LPBITMAPCOREINFO)lpVoid;
                for (int i = nColors - 1; i >= 0; i--) {
                    pRgb[i].rgbRed      = lpbmc->bmciColors[i].rgbtRed;
                    pRgb[i].rgbGreen    = lpbmc->bmciColors[i].rgbtGreen;
                    pRgb[i].rgbBlue     = lpbmc->bmciColors[i].rgbtBlue;
                    pRgb[i].rgbReserved = (BYTE)0;
                }
            } else {
                /* get pointer to BITMAPINFO (new style 3.x) */
                LPBITMAPINFO lpbmi = (LPBITMAPINFO)lpVoid;
                for (int i = nColors - 1; i >= 0; i--) {
                    pRgb[i].rgbRed      = lpbmi->bmiColors[i].rgbRed;
                    pRgb[i].rgbGreen    = lpbmi->bmiColors[i].rgbGreen;
                    pRgb[i].rgbBlue     = lpbmi->bmiColors[i].rgbBlue;
                    pRgb[i].rgbReserved = (BYTE)0;
                }
            }
        }

        // <Michael Gandyra>
        DWORD dwCompression = pHead->biCompression;
        // compressed bitmap ?
        if(dwCompression!=BI_RGB || pHead->biBitCount==32 || pHead->biBitCount ==16) {
            // get the bitmap bits
            LPSTR lpDIBBits = (LPSTR)((BYTE*)pHead + *(DWORD*)pHead + (WORD)(GetNumColors() * sizeof(RGBQUAD)));
            // decode and copy them to our image
            switch (pHead->biBitCount) {
            case 32 :
                {
                    // BITFIELD case
                    if (dwCompression == BI_BITFIELDS || dwCompression == BI_RGB) {
                        // get the bitfield masks
                        DWORD bf[3];
                        memcpy(bf,lpVoid+pHead->biSize,12);
                        // transform into RGB
                        Bitfield2RGB(lpVoid+pHead->biSize+12,bf[0],bf[1],bf[2],(BYTE)pHead->biBitCount);
                    } else {
                        // "unknown compression";
                        GlobalUnlock(lpVoid);
                        return false;
                    }
                }
                break;
            case 16 :
                {
                    // get the bitfield masks
                    long offset=0;
                    DWORD bf[3];
                    if (dwCompression == BI_BITFIELDS) {
                        memcpy(bf,lpVoid+pHead->biSize,12);
                        offset= 12;
                    } else {
                        bf[0] = 0x7C00;
                        bf[1] = 0x3E0;
                        bf[2] = 0x1F; // RGB555
                    }
                    // copy the pixels
                    memcpy(info.pImage, lpDIBBits + offset, head.biHeight*((head.biWidth+1)/2)*4);
                    // transform into RGB
                    Bitfield2RGB(info.pImage, bf[0], bf[1], bf[2], 16);
                }
                break;
            case 8 :
            case 4 :
            case 1 :
                {
                    switch (dwCompression) {
                    case BI_RLE4:
                        {
                            BYTE status_byte = 0;
                            BYTE second_byte = 0;
                            int scanline = 0;
                            int bits = 0;
                            BOOL low_nibble = FALSE;
                            CImageIterator iter(this);

                            for (BOOL bContinue = TRUE; bContinue; ) {
                                status_byte = *(lpDIBBits++);
                                switch (status_byte) {
                                case RLE_COMMAND :
                                    status_byte = *(lpDIBBits++);
                                    switch (status_byte) {
                                    case RLE_ENDOFLINE :
                                        bits = 0;
                                        scanline++;
                                        low_nibble = FALSE;
                                        break;
                                    case RLE_ENDOFBITMAP :
                                        bContinue = FALSE;
                                        break;
                                    case RLE_DELTA :
                                        {
                                            // read the delta values
                                            BYTE delta_x;
                                            BYTE delta_y;
                                            delta_x = *(lpDIBBits++);
                                            delta_y = *(lpDIBBits++);
                                            // apply them
                                            bits       += delta_x / 2;
                                            scanline   += delta_y;
                                            break;
                                        }
                                    default :
                                        second_byte = *(lpDIBBits++);
                                        BYTE* sline = iter.GetRow(scanline);
                                        for (int i = 0; i < status_byte; i++) {
                                            if ((BYTE*)(sline+bits) < (BYTE*)(info.pImage+head.biSizeImage)){
                                                if (low_nibble) {
                                                    if (i&1)
                                                        *(sline + bits) |= (second_byte & 0x0f);
                                                    else
                                                        *(sline + bits) |= (second_byte & 0xf0)>>4;
                                                    bits++;
                                                } else {
                                                    if (i&1)
                                                        *(sline + bits) = (BYTE)(second_byte & 0x0f)<<4;
                                                    else
                                                        *(sline + bits) = (BYTE)(second_byte & 0xf0);
                                                }
                                            }

                                            if ((i & 1) && (i != (status_byte - 1)))
                                                second_byte = *(lpDIBBits++);

                                            low_nibble = !low_nibble;
                                        }
                                        if ((((status_byte+1) >> 1) & 1 ) == 1)
                                            second_byte = *(lpDIBBits++);                                               
                                        break;
                                    };
                                    break;
                                    default :
                                    {
                                        BYTE* sline = iter.GetRow(scanline);
                                        second_byte = *(lpDIBBits++);
                                        for (unsigned i = 0; i < status_byte; i++) {
                                            if ((BYTE*)(sline+bits) < (BYTE*)(info.pImage+head.biSizeImage)){
                                                if (low_nibble) {
                                                    if (i&1)
                                                        *(sline + bits) |= (second_byte & 0x0f);
                                                    else
                                                        *(sline + bits) |= (second_byte & 0xf0)>>4;
                                                    bits++;
                                                } else {
                                                    if (i&1)
                                                        *(sline + bits) = (BYTE)(second_byte & 0x0f)<<4;
                                                    else
                                                        *(sline + bits) = (BYTE)(second_byte & 0xf0);
                                                }
                                            }
                                            low_nibble = !low_nibble;
                                        }
                                    }
                                    break;
                                };
                            }
                        }
                        break;
                    case BI_RLE8 :
                        {
                            BYTE status_byte = 0;
                            BYTE second_byte = 0;
                            int scanline = 0;
                            int bits = 0;
                            CImageIterator iter(this);

                            for (BOOL bContinue = TRUE; bContinue; ) {
                                status_byte = *(lpDIBBits++);
                                if (status_byte==RLE_COMMAND) {
                                    status_byte = *(lpDIBBits++);
                                    switch (status_byte) {
                                    case RLE_ENDOFLINE :
                                        bits = 0;
                                        scanline++;
                                        break;
                                    case RLE_ENDOFBITMAP :
                                        bContinue = FALSE;
                                        break;
                                    case RLE_DELTA :
                                        {
                                            // read the delta values
                                            BYTE delta_x;
                                            BYTE delta_y;
                                            delta_x = *(lpDIBBits++);
                                            delta_y = *(lpDIBBits++);
                                            // apply them
                                            bits     += delta_x;
                                            scanline += delta_y;
                                        }
                                        break;
                                    default :
                                        int nNumBytes = sizeof(BYTE) * status_byte;
                                        memcpy((void *)(iter.GetRow(scanline) + bits), lpDIBBits, nNumBytes);
                                        lpDIBBits += nNumBytes;
                                        // align run length to even number of bytes 
                                        if ((status_byte & 1) == 1)
                                            second_byte = *(lpDIBBits++);
                                        bits += status_byte;
                                        break;
                                    };
                                } else {
                                    BYTE *sline = iter.GetRow(scanline);
                                    second_byte = *(lpDIBBits++);
                                    for (unsigned i = 0; i < status_byte; i++) {
                                        if ((DWORD)bits<info.dwEffWidth){
                                            *(sline + bits) = second_byte;
                                            bits++;
                                        } else {
                                            bContinue = FALSE; //don't delete: we are in memory, it is not as with files
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    default :
                        {
                            // "compression type not supported";
                            GlobalUnlock(lpVoid);
                            return false;
                        }
                    }
                }
            }
        } else {
            //normal bitmap (not compressed)
            memcpy(pDib,lpVoid,GetSize());
        }

        GlobalUnlock(lpVoid);

        if (bTopDownDib) Flip(TRUE,TRUE);

        return true;
    }
    return false;
}

HBITMAP CxImage::MakeBitmap(HDC hdc)
{
    if (!pDib)
        return NULL;

    if (!hdc){
        // this call to CreateBitmap doesn't create a DIB <jaslet>
        // // Create a device-independent bitmap <CSC>
        //  return CreateBitmap(head.biWidth,head.biHeight, 1, head.biBitCount, GetBits());
        // use instead this code
        HDC hMemDC = CreateCompatibleDC(NULL);
        LPVOID pBit32;
        HBITMAP bmp = CreateDIBSection(hMemDC,(LPBITMAPINFO)pDib,DIB_RGB_COLORS, &pBit32, NULL, 0);
        if (pBit32) memcpy(pBit32, GetBits(0), head.biSizeImage);
        DeleteDC(hMemDC);
        return bmp;
    }

    // this single line seems to work very well
    //HBITMAP bmp = CreateDIBitmap(hdc, (LPBITMAPINFOHEADER)pDib, CBM_INIT,
    //  GetBits(), (LPBITMAPINFO)pDib, DIB_RGB_COLORS);
    // this alternative works also with _WIN32_WCE
    LPVOID pBit32;
    HBITMAP bmp = CreateDIBSection(hdc, (LPBITMAPINFO)pDib, DIB_RGB_COLORS, &pBit32, NULL, 0);
    if (pBit32) memcpy(pBit32, GetBits(0), head.biSizeImage);

    return bmp;
}
bool CxImage::CreateFromHBITMAP(HBITMAP hbmp, HPALETTE hpal)
{
    if (!Destroy())
        return false;

    if (hbmp) { 
        BITMAP bm;
        // get informations about the bitmap
        GetObject(hbmp, sizeof(BITMAP), (LPSTR) &bm);
        // create the image
        if (!Create(bm.bmWidth, bm.bmHeight, bm.bmBitsPixel, 0))
            return false;
        // create a device context for the bitmap
        HDC dc = ::GetDC(NULL);
        if (!dc)
            return false;

        if (hpal){
            SelectObject(dc,hpal); //the palette you should get from the user or have a stock one
            RealizePalette(dc);
        }

        // copy the pixels
        if (GetDIBits(dc, hbmp, 0, head.biHeight, info.pImage,
            (LPBITMAPINFO)pDib, DIB_RGB_COLORS) == 0){ //replace &head with pDib <Wil Stark>
            strcpy(info.szLastError,"GetDIBits failed");
            ::ReleaseDC(NULL, dc);
            return false;
        }
        ::ReleaseDC(NULL, dc);
        return true;
    }
    return false;
}

BOOL CxImage::CreateFromHICON(HICON hico)
{
    if (!Destroy() || !hico)
        return false;

    BOOL l_bResult = true;

    ICONINFO iinfo;
    GetIconInfo(hico,&iinfo);

    BITMAP l_Bitmap;
    GetObject(iinfo.hbmColor, sizeof(BITMAP), &l_Bitmap);

    if(l_Bitmap.bmBitsPixel == 32)
    {
        BITMAPINFO l_BitmapInfo;
        l_BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        l_BitmapInfo.bmiHeader.biWidth = l_Bitmap.bmWidth;
        l_BitmapInfo.bmiHeader.biHeight = l_Bitmap.bmHeight;
        l_BitmapInfo.bmiHeader.biPlanes = l_Bitmap.bmPlanes;
        l_BitmapInfo.bmiHeader.biBitCount = l_Bitmap.bmBitsPixel;
        l_BitmapInfo.bmiHeader.biCompression = BI_RGB;

        RGBQUAD *l_pRawBytes = new RGBQUAD[l_Bitmap.bmWidth * l_Bitmap.bmHeight];

        HDC dc = ::GetDC(NULL);

        if(dc)
        {
            if(GetDIBits(dc, iinfo.hbmColor, 0, l_Bitmap.bmHeight, l_pRawBytes, &l_BitmapInfo, DIB_RGB_COLORS))
                l_bResult = CreateFromArray((BYTE*)l_pRawBytes, l_Bitmap.bmWidth, l_Bitmap.bmHeight, l_Bitmap.bmBitsPixel, l_Bitmap.bmWidthBytes, false);
            else
                l_bResult = false;

            ::ReleaseDC(NULL, dc);
        }
        else
            l_bResult = false;

        delete [] l_pRawBytes;
    }
    else
    {
        l_bResult = CreateFromHBITMAP(iinfo.hbmColor,0);
#if CXIMAGE_SUPPORT_ALPHA
        if(l_bResult)
        {
            CxImage mask;
            mask.CreateFromHBITMAP(iinfo.hbmMask,0);
            mask.GrayScale();
            mask.Negative();
            AlphaSet(&mask);
        }
#endif
    }

    DeleteObject(iinfo.hbmColor); //<Sims>
    DeleteObject(iinfo.hbmMask);  //<Sims>
    
    return l_bResult;
}

long CxImage::Draw(HDC hdc, const RECT& rect, RECT* pClipRect, bool bSmooth)
{
    return Draw(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pClipRect,bSmooth);
}


long CxImage::Draw(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth)
{
    if((pDib==0)||(hdc==0)||(cx==0)||(cy==0)||(!info.bEnabled)) return 0;

    if (cx < 0) cx = head.biWidth;
    if (cy < 0) cy = head.biHeight;
    bool bTransparent = info.nBkgndIndex >= 0;
    bool bAlpha = pAlpha != 0;

    
    int hdc_Restore = ::SaveDC(hdc);
    if (!hdc_Restore) 
        return 0;

#if !defined (_WIN32_WCE)
    RECT mainbox; 
    if (pClipRect){
        GetClipBox(hdc,&mainbox);
        HRGN rgn = CreateRectRgnIndirect(pClipRect);
        ExtSelectClipRgn(hdc,rgn,RGN_AND);
        DeleteObject(rgn);
    }
#endif
    
    RECT clipbox,paintbox;
    GetClipBox(hdc,&clipbox);

    paintbox.top = min(clipbox.bottom,max(clipbox.top,y));
    paintbox.left = min(clipbox.right,max(clipbox.left,x));
    paintbox.right = max(clipbox.left,min(clipbox.right,x+cx));
    paintbox.bottom = max(clipbox.top,min(clipbox.bottom,y+cy));

    long destw = paintbox.right - paintbox.left;
    long desth = paintbox.bottom - paintbox.top;

    if (!(bTransparent || bAlpha || info.bAlphaPaletteEnabled)){
        if (cx==head.biWidth && cy==head.biHeight){ 
#if !defined (_WIN32_WCE)
            SetStretchBltMode(hdc,COLORONCOLOR);
#endif
            SetDIBitsToDevice(hdc, x, y, cx, cy, 0, 0, 0, cy,
                        info.pImage,(BITMAPINFO*)pDib,DIB_RGB_COLORS);
        } else { 
            
            RGBQUAD c={0,0,0,0};
            
            BITMAPINFO bmInfo;
            memset(&bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
            bmInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
            bmInfo.bmiHeader.biWidth=destw;
            bmInfo.bmiHeader.biHeight=desth;
            bmInfo.bmiHeader.biPlanes=1;
            bmInfo.bmiHeader.biBitCount=24;
            BYTE *pbase;    
            BYTE *pdst;     
            BYTE *ppix;     
            
            HDC TmpDC=CreateCompatibleDC(hdc);
            HBITMAP TmpBmp=CreateDIBSection(hdc,&bmInfo,DIB_RGB_COLORS,(void**)&pbase,0,0);
            HGDIOBJ TmpObj=SelectObject(TmpDC,TmpBmp);

            if (pbase){
                long xx,yy;
                long sx,sy;
                float dx,dy;
                BYTE *psrc;

                long ew = ((((24 * destw) + 31) / 32) * 4);
                long ymax = paintbox.bottom;
                long xmin = paintbox.left;
                float fx=(float)head.biWidth/(float)cx;
                float fy=(float)head.biHeight/(float)cy;

                for(yy=0;yy<desth;yy++){
                    dy = head.biHeight-(ymax-yy-y)*fy;
                    sy = max(0L,(long)floor(dy));
                    psrc = info.pImage+sy*info.dwEffWidth;
                    pdst = pbase+yy*ew;
                    for(xx=0;xx<destw;xx++){
                        dx = (xx+xmin-x)*fx;
                        sx = max(0L,(long)floor(dx));
#if CXIMAGE_SUPPORT_INTERPOLATION
                        if (bSmooth){
                            if (fx > 1 && fy > 1) { 
                                c = GetAreaColorInterpolated(dx - 0.5f, dy - 0.5f, fx, fy, IM_BILINEAR, OM_REPEAT,0); 
                            } else { 
                                c = GetPixelColorInterpolated(dx - 0.5f, dy - 0.5f, IM_BILINEAR, OM_REPEAT,0); 
                            } 
                        } else
#endif 
                        {
                            if (head.biClrUsed){
                                c=GetPaletteColor(GetPixelIndex(sx,sy));
                            } else {
                                ppix = psrc + sx*3;
                                c.rgbBlue = *ppix++;
                                c.rgbGreen= *ppix++;
                                c.rgbRed  = *ppix;
                            }
                        }
                        *pdst++=c.rgbBlue;
                        *pdst++=c.rgbGreen;
                        *pdst++=c.rgbRed;
                    }
                }
            }
            
            SetDIBitsToDevice(hdc,paintbox.left,paintbox.top,destw,desth,0,0,0,desth,pbase,&bmInfo,0);
            DeleteObject(SelectObject(TmpDC,TmpObj));
            DeleteDC(TmpDC);
        }
    } else {    
        RGBQUAD c={0,0,0,0};
        RGBQUAD ct = GetTransColor();
        long* pc = (long*)&c;
        long* pct= (long*)&ct;
        long cit = GetTransIndex();
        long ci = 0;

        
        BITMAPINFO bmInfo;
        memset(&bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
        bmInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bmInfo.bmiHeader.biWidth=destw;
        bmInfo.bmiHeader.biHeight=desth;
        bmInfo.bmiHeader.biPlanes=1;
        bmInfo.bmiHeader.biBitCount=24;

        BYTE *pbase;    
        BYTE *pdst;     
        BYTE *ppix;     

        
        HDC TmpDC=CreateCompatibleDC(hdc);
        HBITMAP TmpBmp=CreateDIBSection(hdc,&bmInfo,DIB_RGB_COLORS,(void**)&pbase,0,0);
        HGDIOBJ TmpObj=SelectObject(TmpDC,TmpBmp);
        BitBlt(TmpDC,0,0,destw,desth,hdc,paintbox.left,paintbox.top,SRCCOPY);

        if (pbase){
            long xx,yy,alphaoffset,ix,iy;
            BYTE a,a1,*psrc;
            long ew = ((((24 * destw) + 31) / 32) * 4);
            long ymax = paintbox.bottom;
            long xmin = paintbox.left;

            if (cx!=head.biWidth || cy!=head.biHeight){
                
                float fx=(float)head.biWidth/(float)cx;
                float fy=(float)head.biHeight/(float)cy;
                float dx,dy;
                long sx,sy;
                
                for(yy=0;yy<desth;yy++){
                    dy = head.biHeight-(ymax-yy-y)*fy;
                    sy = max(0L,(long)floor(dy));

                    alphaoffset = sy*head.biWidth;
                    pdst = pbase + yy*ew;
                    psrc = info.pImage + sy*info.dwEffWidth;

                    for(xx=0;xx<destw;xx++){
                        dx = (xx+xmin-x)*fx;
                        sx = max(0L,(long)floor(dx));

                        if (bAlpha) a=pAlpha[alphaoffset+sx]; else a=255;
                        a =(BYTE)((a*(1+info.nAlphaMax))>>8);

                        if (head.biClrUsed){
                            ci = GetPixelIndex(sx,sy);
#if CXIMAGE_SUPPORT_INTERPOLATION
                            if (bSmooth){
                                if (fx > 1 && fy > 1) { 
                                    c = GetAreaColorInterpolated(dx - 0.5f, dy - 0.5f, fx, fy, IM_BILINEAR, OM_REPEAT,0); 
                                } else { 
                                    c = GetPixelColorInterpolated(dx - 0.5f, dy - 0.5f, IM_BILINEAR, OM_REPEAT,0); 
                                } 
                            } else
#endif 
                            {
                                c = GetPaletteColor(GetPixelIndex(sx,sy));
                            }
                            if (info.bAlphaPaletteEnabled){
                                a = (BYTE)((a*(1+c.rgbReserved))>>8);
                            }
                        } else {
#if CXIMAGE_SUPPORT_INTERPOLATION
                            if (bSmooth){
                                if (fx > 1 && fy > 1) { 
                                    c = GetAreaColorInterpolated(dx - 0.5f, dy - 0.5f, fx, fy, IM_BILINEAR, OM_REPEAT,0); 
                                } else { 
                                    c = GetPixelColorInterpolated(dx - 0.5f, dy - 0.5f, IM_BILINEAR, OM_REPEAT,0); 
                                } 
                            } else
#endif 
                            {
                                ppix = psrc + sx*3;
                                c.rgbBlue = *ppix++;
                                c.rgbGreen= *ppix++;
                                c.rgbRed  = *ppix;
                            }
                        }                       
                        
                        if ((head.biClrUsed && ci!=cit) || (!head.biClrUsed && *pc!=*pct) || !bTransparent){
                            
                            if (a == 0) {           
                                pdst+=3; 
                            } else if (a == 255) {  
                                *pdst++= c.rgbBlue; 
                                *pdst++= c.rgbGreen; 
                                *pdst++= c.rgbRed; 
                            } else {                
                                a1=(BYTE)~a;
                                *pdst=(BYTE)((*pdst * a1 + a * c.rgbBlue)>>8); 
                                pdst++;
                                *pdst=(BYTE)((*pdst * a1 + a * c.rgbGreen)>>8); 
                                pdst++;
                                *pdst=(BYTE)((*pdst * a1 + a * c.rgbRed)>>8); 
                                pdst++;
                            } 
                        } else {
                            pdst+=3;
                        }
                    }
                }
            } else {
                
                iy=head.biHeight-ymax+y;
                for(yy=0;yy<desth;yy++,iy++){
                    alphaoffset=iy*head.biWidth;
                    ix=xmin-x;
                    pdst=pbase+yy*ew;
                    ppix=info.pImage+iy*info.dwEffWidth+ix*3;
                    for(xx=0;xx<destw;xx++,ix++){

                        if (bAlpha) a=pAlpha[alphaoffset+ix]; else a=255;
                        a = (BYTE)((a*(1+info.nAlphaMax))>>8);

                        if (head.biClrUsed){
                            ci = GetPixelIndex(ix,iy);
                            c = GetPaletteColor((BYTE)ci);
                            if (info.bAlphaPaletteEnabled){
                                a = (BYTE)((a*(1+c.rgbReserved))>>8);
                            }
                        } else {
                            c.rgbBlue = *ppix++;
                            c.rgbGreen= *ppix++;
                            c.rgbRed  = *ppix++;
                        }

                        
                        if ((head.biClrUsed && ci!=cit) || (!head.biClrUsed && *pc!=*pct) || !bTransparent){
                            
                            if (a == 0) {           
                                pdst+=3; 
                            } else if (a == 255) {  
                                *pdst++= c.rgbBlue; 
                                *pdst++= c.rgbGreen; 
                                *pdst++= c.rgbRed; 
                            } else {                
                                a1=(BYTE)~a;
                                *pdst=(BYTE)((*pdst * a1 + a * c.rgbBlue)>>8); 
                                pdst++;
                                *pdst=(BYTE)((*pdst * a1 + a * c.rgbGreen)>>8); 
                                pdst++;
                                *pdst=(BYTE)((*pdst * a1 + a * c.rgbRed)>>8); 
                                pdst++;
                            } 
                        } else {
                            pdst+=3;
                        }
                    }
                }
            }
        }
        
        SetDIBitsToDevice(hdc,paintbox.left,paintbox.top,destw,desth,0,0,0,desth,pbase,&bmInfo,0);
        DeleteObject(SelectObject(TmpDC,TmpObj));
        DeleteDC(TmpDC);
    }

#if !defined (_WIN32_WCE)
    if (pClipRect){  
        HRGN rgn = CreateRectRgnIndirect(&mainbox);
        ExtSelectClipRgn(hdc,rgn,RGN_OR);
        DeleteObject(rgn);
    }
#endif

    ::RestoreDC(hdc,hdc_Restore);
    return 1;
}

long CxImage::Draw2(HDC hdc, const RECT& rect)
{
    return Draw2(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}


long CxImage::Draw2(HDC hdc, long x, long y, long cx, long cy)
{
    if((pDib==0)||(hdc==0)||(cx==0)||(cy==0)||(!info.bEnabled)) return 0;
    if (cx < 0) cx = head.biWidth;
    if (cy < 0) cy = head.biHeight;
    bool bTransparent = (info.nBkgndIndex >= 0);

    
    int hdc_Restore = ::SaveDC(hdc);
    if (!hdc_Restore) 
        return 0;

    if (!bTransparent){
#if !defined (_WIN32_WCE)
        SetStretchBltMode(hdc,COLORONCOLOR);    
#endif
        StretchDIBits(hdc, x, y, cx, cy, 0, 0, head.biWidth, head.biHeight,
                        info.pImage,(BITMAPINFO*)pDib, DIB_RGB_COLORS,SRCCOPY);
    } else {
        
        const int safe = 0; 
        RECT rcDst = {x+safe, y+safe, x+cx, y+cy};
        if (RectVisible(hdc, &rcDst)){
        
            
            int nWidth = head.biWidth;
            int nHeight = head.biHeight;
            
            HDC dcImage=CreateCompatibleDC(hdc);
            HDC dcTrans=CreateCompatibleDC(hdc);
            
            HBITMAP bm = CreateCompatibleBitmap(hdc, nWidth, nHeight);
            HBITMAP pOldBitmapImage = (HBITMAP)SelectObject(dcImage,bm);
#if !defined (_WIN32_WCE)
            SetStretchBltMode(dcImage,COLORONCOLOR);
#endif
            StretchDIBits(dcImage, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
                            info.pImage,(BITMAPINFO*)pDib,DIB_RGB_COLORS,SRCCOPY);

            
            HBITMAP bitmapTrans = CreateBitmap(nWidth, nHeight, 1, 1, NULL);
            
            HBITMAP pOldBitmapTrans = (HBITMAP)SelectObject(dcTrans, bitmapTrans);
            
            RGBQUAD rgbBG;
            if (head.biBitCount<24) rgbBG = GetPaletteColor((BYTE)info.nBkgndIndex);
            else rgbBG = info.nBkgndColor;
            COLORREF crColour = RGB(rgbBG.rgbRed, rgbBG.rgbGreen, rgbBG.rgbBlue);
            COLORREF crOldBack = SetBkColor(dcImage,crColour);
            BitBlt(dcTrans,0, 0, nWidth, nHeight, dcImage, 0, 0, SRCCOPY);

            
            StretchBlt(hdc,x, y,cx,cy, dcImage, 0, 0, nWidth, nHeight, SRCINVERT);
            StretchBlt(hdc,x, y,cx,cy, dcTrans, 0, 0, nWidth, nHeight, SRCAND);
            StretchBlt(hdc,x, y,cx,cy, dcImage, 0, 0, nWidth, nHeight, SRCINVERT);

            
            SelectObject(dcImage,pOldBitmapImage);
            SelectObject(dcTrans,pOldBitmapTrans);
            SetBkColor(hdc,crOldBack);
            DeleteObject( bitmapTrans );  
            DeleteDC(dcImage);
            DeleteDC(dcTrans);
            DeleteObject(bm);
        }
    }
    ::RestoreDC(hdc,hdc_Restore);
    return 1;
}

long CxImage::Stretch(HDC hdc, const RECT& rect, DWORD dwRop)
{
    return Stretch(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, dwRop);
}


long CxImage::Stretch(HDC hdc, long xoffset, long yoffset, long xsize, long ysize, DWORD dwRop)
{
    if((pDib)&&(hdc)) {
        
#if !defined (_WIN32_WCE)
        SetStretchBltMode(hdc,COLORONCOLOR);    
#endif
        StretchDIBits(hdc, xoffset, yoffset,
                    xsize, ysize, 0, 0, head.biWidth, head.biHeight,
                    info.pImage,(BITMAPINFO*)pDib,DIB_RGB_COLORS,dwRop);
        return 1;
    }
    return 0;
}


long CxImage::Tile(HDC hdc, RECT *rc)
{
    if((pDib)&&(hdc)&&(rc)) {
        int w = rc->right - rc->left;
        int h = rc->bottom - rc->top;
        int x,y,z;
        int bx=head.biWidth;
        int by=head.biHeight;
        for (y = 0 ; y < h ; y += by){
            if ((y+by)>h) by=h-y;
            z=bx;
            for (x = 0 ; x < w ; x += z){
                if ((x+z)>w) z=w-x;
                RECT r = {rc->left + x,rc->top + y,rc->left + x + z,rc->top + y + by};
                Draw(hdc,rc->left + x, rc->top + y,-1,-1,&r,false);
            }
        }
        return 1;
    }
    return 0;
}


long CxImage::DrawString(HDC hdc, long x, long y, const char* text, RGBQUAD color, const char* font, long lSize, long lWeight, BYTE bItalic, BYTE bUnderline, BOOL bSetAlpha)

{
    if (IsValid()){
        
        HDC pDC;
        if (hdc) pDC=hdc; else pDC = ::GetDC(0);
        if (pDC==NULL) return 0;
        HDC TmpDC=CreateCompatibleDC(pDC);
        if (hdc==NULL) ::ReleaseDC(0, pDC);
        if (TmpDC==NULL) return 0;
        
        HFONT m_Font;
        LOGFONTA* m_pLF;
        m_pLF=(LOGFONTA*)calloc(1,sizeof(LOGFONTA));
        strncpy(m_pLF->lfFaceName,font,31); 
        
        m_pLF->lfHeight=lSize;
        m_pLF->lfWeight=lWeight;
        m_pLF->lfItalic=bItalic;
        m_pLF->lfUnderline=bUnderline;
        m_pLF->lfCharSet = 134;
        m_Font=CreateFontIndirectA(m_pLF);
        
        HFONT pOldFont=NULL;
        if (m_Font)
            pOldFont = (HFONT)SelectObject(TmpDC,m_Font);
        else
            pOldFont = (HFONT)SelectObject(TmpDC,GetStockObject(DEFAULT_GUI_FONT));

        
        SetTextColor(TmpDC,RGB(255,255,255));
        SetBkColor(TmpDC,RGB(0,0,0));
        
        SetBkMode(TmpDC,OPAQUE);
        
        RECT pos = {0,0,0,0};
        
        long len = (long)strlen(text);  
        ::DrawTextA(TmpDC,text,len,&pos,DT_CALCRECT);
        pos.right+=pos.bottom; 

        
        long width=pos.right;
        long height=pos.bottom;
        BITMAPINFO bmInfo;
        memset(&bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
        bmInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        bmInfo.bmiHeader.biWidth=width;
        bmInfo.bmiHeader.biHeight=height;
        bmInfo.bmiHeader.biPlanes=1;
        bmInfo.bmiHeader.biBitCount=24;
        BYTE *pbase; 

        HBITMAP TmpBmp=CreateDIBSection(TmpDC,&bmInfo,DIB_RGB_COLORS,(void**)&pbase,0,0);
        HGDIOBJ TmpObj=SelectObject(TmpDC,TmpBmp);
        memset(pbase,0,height*((((24 * width) + 31) / 32) * 4));

        ::DrawTextA(TmpDC,text,len,&pos,0);

        CxImage itext;
        itext.CreateFromHBITMAP(TmpBmp,0);

        y=head.biHeight-y-1;
        for (long ix=0;ix<width;ix++){
            for (long iy=0;iy<height;iy++){
                if (itext.GetPixelColor(ix,iy,false).rgbBlue) SetPixelColor(x+ix,y+iy,color,bSetAlpha);
            }
        }

        
        if (pOldFont) SelectObject(TmpDC,pOldFont);
        DeleteObject(m_Font);
        free(m_pLF);
        DeleteObject(SelectObject(TmpDC,TmpObj));
        DeleteDC(TmpDC);
    }

    return 1;
}


long CxImage::DrawStringEx(HDC hdc, long x, long y, CXTEXTINFO *pTextType, bool bSetAlpha )
{
    if (!IsValid())
        return -1;
    
    
    HDC pDC;
    if (hdc) pDC=hdc; else pDC = ::GetDC(0);
    if (pDC==NULL) return 0;
    HDC TmpDC=CreateCompatibleDC(pDC);
    if (hdc==NULL) ::ReleaseDC(0, pDC);
    if (TmpDC==NULL) return 0;

    
    HFONT m_Font;
    m_Font=CreateFontIndirectA( &pTextType->lfont );
    
    
    RGBQUAD p_forecolor = RGBtoRGBQUAD(pTextType->fcolor);
    RGBQUAD p_backcolor = RGBtoRGBQUAD(pTextType->bcolor);

    
    if ( pTextType->align != DT_CENTER &&
         pTextType->align != DT_LEFT &&
         pTextType->align != DT_RIGHT )
        pTextType->align = DT_CENTER;

    
    if ( pTextType->b_round > 50 )
        pTextType->b_round = 10;

    
    if ( pTextType->b_opacity > 1. || pTextType->b_opacity < .0 )
        pTextType->b_opacity = 0.;

    
    HFONT pOldFont=NULL;
    if (m_Font)
        pOldFont = (HFONT)SelectObject(TmpDC,m_Font);
    else
        pOldFont = (HFONT)SelectObject(TmpDC,GetStockObject(DEFAULT_GUI_FONT));

    
    SetTextColor(TmpDC,RGB(255,255,255));
    SetBkColor(TmpDC,RGB(0,0,0));
    SetBkMode(TmpDC,OPAQUE);
    
    RECT pos = {0,0,0,0};
    
    
    long i=0, numlines=1, len=(long)strlen(pTextType->text);
    while (i<len)
    {
        if ( pTextType->text[i++]==13 )
            numlines++;
    }

    ::DrawTextA(TmpDC, pTextType->text, len, &pos, DT_NOPREFIX | DT_CALCRECT );

    
    if ( pTextType->lfont.lfItalic ) 
        pos.right += pos.bottom/2/numlines; 

    
    int frame = 0, roundR = 0;
    if ( pTextType->opaque )
    {
        roundR= (int)(pos.bottom/numlines * pTextType->b_round / 100 ) ;
        frame = (int)(0.29289*roundR ) ;
        pos.right += pos.bottom/numlines/3 ; 
    }

    
    long width=pos.right +frame*2;
    long height=pos.bottom +frame*2;
    BITMAPINFO bmInfo;
    memset(&bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
    bmInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    bmInfo.bmiHeader.biWidth=width;
    bmInfo.bmiHeader.biHeight=height;
    bmInfo.bmiHeader.biPlanes=1;
    bmInfo.bmiHeader.biBitCount=24;
    BYTE *pbase; 

    HBITMAP TmpBmp=CreateDIBSection(TmpDC,&bmInfo,DIB_RGB_COLORS,(void**)&pbase,0,0);
    HGDIOBJ TmpObj=SelectObject(TmpDC,TmpBmp);
    memset(pbase,0,height*((((24 * width) + 31) / 32) * 4));

    ::DrawTextA(TmpDC,pTextType->text,len, &pos, DT_NOPREFIX| pTextType->align );
    
    CxImage itext;
    itext.CreateFromHBITMAP(TmpBmp,0);
    y=head.biHeight-y-1;

    itext.Negative();

    if (pTextType->smooth==FALSE){
        itext.Threshold(128);
    } else {
        
    }
    
    if ( pTextType->align == DT_CENTER )
        x -= width/2;
    else if ( pTextType->align == DT_RIGHT )
        x -= width;
    if (x<0) x=0;
    
    
    long ix,iy;
    if ( pTextType->opaque )
    {
        int ixf=0; 
        for (ix=0;ix<width;ix++)
        {
            if ( ix<=roundR )
                ixf = (int)(.5+roundR-sqrt((float)(roundR*roundR-(ix-roundR)*(ix-roundR))));
            else if ( ix>=width-roundR-1 )
                ixf = (int)(.5+roundR-sqrt((float)(roundR*roundR-(width-1-ix-roundR)*(width-1-ix-roundR))));
            else
                ixf=0;

            for (iy=0;iy<height;iy++)
            {
                if ( (ix<=roundR && ( iy > height-ixf-1 || iy < ixf )) ||
                     (ix>=width-roundR-1 && ( iy > height-ixf-1 || iy < ixf )) )
                    continue;
                else
                    if ( pTextType->b_opacity > 0.0 && pTextType->b_opacity < 1.0 )
                    {
                        RGBQUAD bcolor, pcolor;
                        
                        pcolor = GetPixelColor(x+ix,y+iy,0);
                        bcolor.rgbBlue = (unsigned char)(pTextType->b_opacity * pcolor.rgbBlue + (1.0-pTextType->b_opacity) * p_backcolor.rgbBlue );
                        bcolor.rgbRed = (unsigned char)(pTextType->b_opacity * pcolor.rgbRed + (1.0-pTextType->b_opacity) * p_backcolor.rgbRed ) ;
                        bcolor.rgbGreen = (unsigned char)(pTextType->b_opacity * pcolor.rgbGreen + (1.0-pTextType->b_opacity) * p_backcolor.rgbGreen ) ;
                        bcolor.rgbReserved = 0;
                        SetPixelColor(x+ix,y+iy,bcolor,bSetAlpha);
                    }
                    else
                        SetPixelColor(x+ix,y+iy,p_backcolor,bSetAlpha);
            }
        }
    }

    
    for (ix=0;ix<width;ix++)
    {
        for (iy=0;iy<height;iy++)
        {
            RGBQUAD pcolor = GetPixelColor(x+ix,y+iy,0);
            RGBQUAD tcolor = itext.GetPixelColor(ix,iy,0);
            if (tcolor.rgbBlue!=255){
                float a = tcolor.rgbBlue/255.0f;
                pcolor.rgbBlue  = (unsigned char)(a * (pcolor.rgbBlue  - p_forecolor.rgbBlue)  + p_forecolor.rgbBlue );
                pcolor.rgbRed   = (unsigned char)(a * (pcolor.rgbRed   - p_forecolor.rgbRed)   + p_forecolor.rgbRed ) ;
                pcolor.rgbGreen = (unsigned char)(a * (pcolor.rgbGreen - p_forecolor.rgbGreen) + p_forecolor.rgbGreen );
                pcolor.rgbReserved = 0;
                SetPixelColor(x+ix+frame,y+iy-frame,pcolor,bSetAlpha);
              
            }
        }
    }

    
    if (pOldFont) SelectObject(TmpDC,pOldFont);
    DeleteObject(m_Font);
    DeleteObject(SelectObject(TmpDC,TmpObj));
    DeleteDC(TmpDC);
    return 1;
}


void CxImage::InitTextInfo( CXTEXTINFO *txt )
{

    memset( txt, 0, sizeof(CXTEXTINFO));
    
    
    txt->lfont.lfHeight        = -36; 
    txt->lfont.lfCharSet       = EASTEUROPE_CHARSET; 
    txt->lfont.lfWeight        = FW_NORMAL;
    txt->lfont.lfWidth         = 0; 
    txt->lfont.lfEscapement    = 0; 
    txt->lfont.lfOrientation   = 0; 
    txt->lfont.lfItalic        = FALSE; 
    txt->lfont.lfUnderline     = FALSE; 
    txt->lfont.lfStrikeOut     = FALSE; 
    txt->lfont.lfOutPrecision  = OUT_DEFAULT_PRECIS; 
    txt->lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS; 
    txt->lfont.lfQuality       = PROOF_QUALITY; 
    txt->lfont.lfPitchAndFamily= DEFAULT_PITCH | FF_DONTCARE ; 
    sprintf( txt->lfont.lfFaceName, "Arial"); 

    
    txt->fcolor = RGB( 255,255,160 );  
    txt->bcolor = RGB(   0, 80,160 );  

    
    txt->opaque    = TRUE;  
    txt->smooth    = TRUE;
    txt->b_opacity = 0.0;   
    txt->b_outline = 0;     
    txt->b_round   = 20;    
    
    sprintf( txt->text, "Hello CxImage"); 
    txt->align = DT_CENTER;
    return;
}

bool CxImage::CreateFromHdc(HDC hdc)
{
    HBITMAP hbmp = MakeBitmap(hdc);
    ASSERT(hbmp);
    CreateFromHBITMAP(hbmp,0);
    ::DeleteObject(hbmp);
    return OK;
}

#if CXIMAGE_SUPPORT_LAYERS

long CxImage::LayerDrawAll(HDC hdc, const RECT& rect, RECT* pClipRect, bool bSmooth)
{
    return LayerDrawAll(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pClipRect,bSmooth);
}

long CxImage::LayerDrawAll(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth)
{
    long n=0;
    CxImage* pLayer;
    while((pLayer=GetLayer(n++))!=NULL){
        if (pLayer->Draw(hdc,x+pLayer->info.xOffset,y+pLayer->info.yOffset,cx,cy,pClipRect,bSmooth)==0)
            return 0;
        if (pLayer->LayerDrawAll(hdc,x+pLayer->info.xOffset,y+pLayer->info.yOffset,cx,cy,pClipRect,bSmooth)==0)
            return 0;
    }
    return 1;
}
#endif 

#endif
//////////////////////////////////////////////////////////////
////  CXIMAGE_SUPPORT_WINDOWS END
//////////////////////////////////////////////////////////////
CxImage::CxImage()
{
    WEAK_REF_CLEAR();
    this->InitBasic();
    
}
CxImage::~CxImage()
{
    this->DestroyAll();
    WEAK_REF_DESTROY();
}

CxImage::CxImage(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype)
{
    WEAK_REF_CLEAR();    
    this->StartUp(imagetype);
    this->Create(dwWidth,dwHeight,wBpp,imagetype);
    
}
CxImage::CxImage(CxImage *src, BOOL copypixels, BOOL copyselection, BOOL copyalpha)
{   
    WEAK_REF_CLEAR();   
    StartUp(src->GetType());
    Copy(src,copypixels,copyselection,copyalpha);
    
}

CxImage::CxImage(const CxImage &img)
{
    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,"construe CxImage Like CxImage tmp(*this), is not allowed\n");
}

CxImage& CxImage::operator=(const CxImage &img)
{
    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,"construe CxImage Like CxImage tmp = *this, is not allowed\n");
    return *this;
}

int CxImage::InitBasic()
{
    WEAK_REF_CLEAR();
    this->pDib = 0; 
    this->pSelection = 0;
    this->pAlpha = 0;
    this->ppLayers = 0;
    this->ppFrames = 0;
    this->info.InitBasic(); 
    return OK;
}
int CxImage::Init()
{
    this->InitBasic();
    this->info.Init();
    this->StartUp(CXIMAGE_FORMAT_UNKNOWN);
    
    return OK;
}
int CxImage::Destroy()
{
    long n;     
    if (info.pGhost==NULL)
    {
        if (ppLayers) 
        { 
            for(n=0; n<info.nNumLayers;n++)
            {
                DEL(ppLayers[n]); 
            }

            DEL_ARRAY( ppLayers );
            info.nNumLayers = 0;
        }
    
        FREE(pSelection);
        FREE(pAlpha);
        FREE(pDib);
    }

    return OK;
}
int CxImage::Copy(CxImage *p)
{
    if(this == p) return OK;
    return this->Copy(p,TRUE,TRUE,TRUE);
}

int CxImage::Comp(CxImage *p)
{
    return 0;
}
int CxImage::Print()
{
    PD(head.biSize);
    PD(head.biWidth);
    PD(head.biHeight);
    PD(head.biPlanes);
    PD(head.biBitCount);
    PD(head.biCompression);
    PD(head.biSizeImage);
    PD(head.biXPelsPerMeter);
    PD(head.biYPelsPerMeter);
    PD(head.biClrUsed);
    PD(head.biClrImportant);
    this->info.Print();
    return TRUE;
}

int CxImage::StartUp(DWORD imagetype)
{
    //init pointers
    pDib = pSelection = pAlpha = NULL;
    ppLayers = ppFrames = NULL;

    //init structures
    memset(&head,0,sizeof(BITMAPINFOHEADER));
    
    this->info.Clear();

    //init default attributes
    info.dwType = imagetype;
    info.fQuality = 90.0f;
    info.nAlphaMax = 255;
    info.nBkgndIndex = -1;
    info.bEnabled = TRUE;
    SetXDPI(CXIMAGE_DEFAULT_DPI);
    SetYDPI(CXIMAGE_DEFAULT_DPI);

    short test = 1;
    info.bLittleEndianHost = (*((char *) &test) == 1);

    return OK;
}

int CxImage::SetXDPI(long dpi)
{
    if (dpi<=0) dpi = CXIMAGE_DEFAULT_DPI;
    info.xDPI = dpi;
    head.biXPelsPerMeter = (long) floor(dpi * 10000.0 / 254.0 + 0.5);
    if (pDib) ((BITMAPINFOHEADER*)pDib)->biXPelsPerMeter = head.biXPelsPerMeter;

    return OK;
}

int CxImage::SetYDPI(long dpi)
{
    if (dpi<=0) dpi = CXIMAGE_DEFAULT_DPI;
    info.yDPI = dpi;
    head.biYPelsPerMeter = (long) floor(dpi * 10000.0 / 254.0 + 0.5);
    if (pDib) ((BITMAPINFOHEADER*)pDib)->biYPelsPerMeter = head.biYPelsPerMeter;

    return OK;
}

int CxImage::CopyInfo(CxImage *src)
{
    ASSERT(src);
    
    if (pDib==NULL)
        this->info.Copy(&src->info);

    return OK;
}

int CxImage::Copy_No_Rebuild(CxImage *src)
{
    ASSERT(src);

    ASSERT(this->GetWidth() == src->GetWidth());
    ASSERT(this->GetHeight() == src->GetHeight());
    ASSERT(this->GetBpp() == src->GetBpp());

    // if the source is a ghost, the copy is still a ghost
    if (src->info.pGhost)
    {
        this->Ghost(src);
        return OK;
    }

    //copy the attributes
    this->info.Copy(&src->info);
    info.pImage=GetBits(0);
    memcpy(&head,&src->head,sizeof(BITMAPINFOHEADER)); // [andy] - fix for bitmap header DPI

    //copy the pixels and the palette, or at least copy the palette only.
    if (pDib && src->pDib)
        memcpy(pDib,src->pDib,GetSize());
    else 
        SetPalette(src->GetPalette(),256);

    long nSize = head.biWidth * head.biHeight;

    //copy the selection
    if (src->pSelection)
    {
        ASSERT(this->pSelection);
        memcpy(pSelection,src->pSelection,nSize);
    }
    //copy the alpha channel
    if (src->pAlpha)
    {
        ASSERT(this->pAlpha);
        memcpy(pAlpha,src->pAlpha,nSize);
    }
    return OK;
}

int CxImage::Copy(CxImage *src, BOOL copypixels, BOOL copyselection, BOOL copyalpha)
{
    ASSERT(src);

    // if the source is a ghost, the copy is still a ghost
    if (src->info.pGhost)
    {
        Ghost(src);
        return OK;
    }

    //copy the attributes
    this->info.Copy(&src->info);

    memcpy(&head,&src->head,sizeof(BITMAPINFOHEADER)); // [andy] - fix for bitmap header DPI

    //rebuild the image
    Create(src->GetWidth(),src->GetHeight(),src->GetBpp(),src->GetType());

    //copy the pixels and the palette, or at least copy the palette only.
    if (copypixels && pDib && src->pDib)
        memcpy(pDib,src->pDib,GetSize());
    else 
        SetPalette(src->GetPalette(),256);

    long nSize = head.biWidth * head.biHeight;

    //copy the selection
    if (copyselection && src->pSelection)
    {
        FREE(pSelection);       
        MALLOC(pSelection,BYTE,nSize);
        memcpy(pSelection,src->pSelection,nSize);
    }
    //copy the alpha channel
    if (copyalpha && src->pAlpha)
    {
        FREE(pAlpha);
        MALLOC(pAlpha,BYTE,nSize);
        memcpy(pAlpha,src->pAlpha,nSize);
    }

    return OK;
}

int CxImage::Ghost(CxImage *from)
{
    ASSERT(from);

    memcpy(&head,&from->head,sizeof(BITMAPINFOHEADER));

    this->info.Copy(&from->info);
    pDib = from->pDib;
    pSelection = from->pSelection;
    pAlpha = from->pAlpha;
    ppLayers = from->ppLayers;
    ppFrames = from->ppFrames;
    info.pGhost=(CxImage *)from;

    return OK;
}

void *CxImage::Create(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype)
{   
    this->Destroy(); // destroy the existing image (if any)
    
    // prevent further actions if width or height are not vaild <Balabasnia>
    if ((dwWidth == 0) || (dwHeight == 0)){
        strcpy(info.szLastError,"CxImage::Create : width and height must be greater than zero");
        return NULL;
    }

    // Make sure bits per pixel is valid
    if      (wBpp <= 1) wBpp = 1;
    else if (wBpp <= 4) wBpp = 4;
    else if (wBpp <= 8) wBpp = 8;
    else                wBpp = 24;

    // limit memory requirements (and also a check for bad parameters)
    if (((dwWidth*dwHeight*wBpp)>>3) > CXIMAGE_MAX_MEMORY ||
        ((dwWidth*dwHeight*wBpp)/wBpp) != (dwWidth*dwHeight))
    {
        strcpy(info.szLastError,"CXIMAGE_MAX_MEMORY exceeded");
        return NULL;
    }

    // set the correct bpp value
    switch (wBpp){
        case 1:
            head.biClrUsed = 2; break;
        case 4:
            head.biClrUsed = 16; break;
        case 8:
            head.biClrUsed = 256; break;
        default:
            head.biClrUsed = 0;
    }

    //set the common image informations
    info.dwEffWidth = ((((wBpp * dwWidth) + 31) / 32) * 4);
    info.dwType = imagetype;

    // initialize BITMAPINFOHEADER
    head.biSize = sizeof(BITMAPINFOHEADER); //<ralphw>
    head.biWidth = dwWidth;     // fill in width from parameter
    head.biHeight = dwHeight;   // fill in height from parameter
    head.biPlanes = 1;          // must be 1
    head.biBitCount = (WORD)wBpp;       // from parameter
    head.biCompression = BI_RGB;    
    head.biSizeImage = info.dwEffWidth * dwHeight;
//    head.biXPelsPerMeter = 0; See SetXDPI
//    head.biYPelsPerMeter = 0; See SetYDPI
//    head.biClrImportant = 0;  See SetClrImportant

    BYTE *__p;
    MALLOC(__p,BYTE,GetSize());
    this->pDib = (void*)__p;

    //clear the palette
    RGBQUAD* pal=GetPalette();
    if (pal) memset(pal,0,GetPaletteSize());
    //Destroy the existing selection
#if CXIMAGE_SUPPORT_SELECTION
    if (pSelection) SelectionDelete();
#endif //CXIMAGE_SUPPORT_SELECTION
    //Destroy the existing alpha channel
#if CXIMAGE_SUPPORT_ALPHA
    if (pAlpha) AlphaDelete();
#endif //CXIMAGE_SUPPORT_ALPHA

    // use our bitmap info structure to fill in first part of
    // our DIB with the BITMAPINFOHEADER
    BITMAPINFOHEADER*  lpbi;
    lpbi = (BITMAPINFOHEADER*)(pDib);
    *lpbi = head;

    info.pImage=GetBits(0);

    return pDib; //return handle to the DIB

}

long CxImage::GetWidth()
{
    return head.biWidth;
}

long CxImage::GetHeight()
{
    return head.biHeight;
}

WORD CxImage::GetBpp()
{
    return head.biBitCount;
}

DWORD CxImage::GetType()
{
    return info.dwType;
}

long CxImage::GetSize()
{
    return head.biSize + head.biSizeImage + GetPaletteSize();
}

int CxImage::SetPalette(RGBQUAD *pPal, DWORD nColors)
{
    if ((pPal==NULL)||(pDib==NULL)||(head.biClrUsed==0)) 
        return ERROR;
    memcpy(GetPalette(),pPal,min(GetPaletteSize(),nColors*sizeof(RGBQUAD)));
    info.last_c_isvalid = FALSE;

    return OK;
}

RGBQUAD* CxImage::GetPalette()
{
    if ((pDib)&&(head.biClrUsed))
        return (RGBQUAD*)((BYTE*)pDib + sizeof(BITMAPINFOHEADER));
    return NULL;
}

DWORD CxImage::GetPaletteSize()
{
    return (head.biClrUsed * sizeof(RGBQUAD));
}

BYTE * CxImage::GetBits(DWORD row)
{
    if (pDib){
        if (row) {
            if (row<(DWORD)head.biHeight){
                return ((BYTE*)pDib + *(DWORD*)pDib + GetPaletteSize() + (info.dwEffWidth * row));
            } else {
                return NULL;
            }
        } else {
            return ((BYTE*)pDib + *(DWORD*)pDib + GetPaletteSize());
        }
    }
    return NULL;
}



BOOL CxImage::IsInside(long x, long y)
{
    return (0<=y && y<head.biHeight && 0<=x && x<head.biWidth);
}

int CxImage::Clear(BYTE bval)
{
    if (pDib == 0) return ERROR;

    if (GetBpp() == 1){
        if (bval > 0) bval = 255;
    }
    if (GetBpp() == 4){
        bval = (BYTE)(17*(0x0F & bval));
    }

    memset(info.pImage,bval,head.biSizeImage);

    return OK;
}

int CxImage::Transfer(CxImage *from, BOOL bTransferFrames)
{
    ASSERT(from);
	
	SAVE_WEAK_REF_ID(*this,w);
    if (!Destroy())
        return ERROR;
	RESTORE_WEAK_REF_ID(*this,w);

    memcpy(&head,&from->head,sizeof(BITMAPINFOHEADER));
    this->info.Copy(&from->info);

    pDib = from->pDib;
    pSelection = from->pSelection;
    pAlpha = from->pAlpha;
    ppLayers = from->ppLayers;

    memset(&from->head,0,sizeof(BITMAPINFOHEADER));
    from->info.Clear();

    from->pDib = from->pSelection = from->pAlpha = NULL;
    from->ppLayers = NULL;

    if (bTransferFrames){
        DestroyFrames();
        ppFrames = from->ppFrames;
        from->ppFrames = NULL;
    }

    return TRUE;
}

int CxImage::DestroyFrames()
{
    if (info.pGhost==NULL) 
    {
        if (ppFrames) 
        {
            for (long n=0; n<info.nNumFrames; n++) 
            {
                DEL(ppFrames[n]);
            }
            DEL_ARRAY(ppFrames);
            info.nNumFrames = 0;
        }       
    }

    return OK;
}

int CxImage::CompareColors(const void *elem1, const void *elem2)
{
    RGBQUAD* c1 = (RGBQUAD*)elem1;
    RGBQUAD* c2 = (RGBQUAD*)elem2;

    int g1 = (int)RGB2GRAY(c1->rgbRed,c1->rgbGreen,c1->rgbBlue);
    int g2 = (int)RGB2GRAY(c2->rgbRed,c2->rgbGreen,c2->rgbBlue);
    
    return (g1-g2);
}

int CxImage::Bitfield2RGB(BYTE *src, DWORD redmask, DWORD greenmask, DWORD bluemask, BYTE bpp)
{
    switch (bpp){
    case 16:
        {
            DWORD ns[3]={0,0,0};
            // compute the number of shift for each mask
            for (int i=0;i<16;i++){
                if ((redmask>>i)&0x01) ns[0]++;
                if ((greenmask>>i)&0x01) ns[1]++;
                if ((bluemask>>i)&0x01) ns[2]++;
            }
            ns[1]+=ns[0]; ns[2]+=ns[1]; ns[0]=8-ns[0]; ns[1]-=8; ns[2]-=8;
            // dword aligned width for 16 bit image
            long effwidth2=(((head.biWidth + 1) / 2) * 4);
            WORD w;
            long y2,y3,x2,x3;
            BYTE *p=info.pImage;
            // scan the buffer in reverse direction to avoid reallocations
            for (long y=head.biHeight-1; y>=0; y--){
                y2=effwidth2*y;
                y3=info.dwEffWidth*y;
                for (long x=head.biWidth-1; x>=0; x--){
                    x2 = 2*x+y2;
                    x3 = 3*x+y3;
                    w = (WORD)(src[x2]+256*src[1+x2]);
                    p[  x3]=(BYTE)((w & bluemask)<<ns[0]);
                    p[1+x3]=(BYTE)((w & greenmask)>>ns[1]);
                    p[2+x3]=(BYTE)((w & redmask)>>ns[2]);
                }
            }
            break;
        }
    case 32:
        {
            DWORD ns[3]={0,0,0};
            // compute the number of shift for each mask
            for (int i=8;i<32;i+=8){
                if (redmask>>i) ns[0]++;
                if (greenmask>>i) ns[1]++;
                if (bluemask>>i) ns[2]++;
            }
            // dword aligned width for 32 bit image
            long effwidth4 = head.biWidth * 4;
            long y4,y3,x4,x3;
            BYTE *p=info.pImage;
            // scan the buffer in reverse direction to avoid reallocations
            for (long y=head.biHeight-1; y>=0; y--){
                y4=effwidth4*y;
                y3=info.dwEffWidth*y;
                for (long x=head.biWidth-1; x>=0; x--){
                    x4 = 4*x+y4;
                    x3 = 3*x+y3;
                    p[  x3]=src[ns[2]+x4];
                    p[1+x3]=src[ns[1]+x4];
                    p[2+x3]=src[ns[0]+x4];
                }
            }
        }
        
    }
    return OK;
}

int CxImage::CreateFromArray(BYTE *pArray, DWORD dwWidth, DWORD dwHeight, DWORD dwBitsperpixel, DWORD dwBytesperline, BOOL bFlipImage)
{
    if (pArray==NULL) return FALSE;
    if (!((dwBitsperpixel==1)||(dwBitsperpixel==4)||(dwBitsperpixel==8)||
        (dwBitsperpixel==24)||(dwBitsperpixel==32))) return FALSE;

    if (!Create(dwWidth,dwHeight,dwBitsperpixel,0)) return FALSE;

    if (dwBitsperpixel<24) SetGrayPalette();

#if CXIMAGE_SUPPORT_ALPHA
    if (dwBitsperpixel==32) AlphaCreate();
#endif //CXIMAGE_SUPPORT_ALPHA

    BYTE *dst,*src;

    for (DWORD y = 0; y<dwHeight; y++) {
        dst = info.pImage + (bFlipImage?(dwHeight-1-y):y) * info.dwEffWidth;
        src = pArray + y * dwBytesperline;
        if (dwBitsperpixel==32){
            for(DWORD x=0;x<dwWidth;x++){
                *dst++=src[0];
                *dst++=src[1];
                *dst++=src[2];
#if CXIMAGE_SUPPORT_ALPHA
                AlphaSet(x,(bFlipImage?(dwHeight-1-y):y),src[3]);
#endif //CXIMAGE_SUPPORT_ALPHA
                src+=4;
            }
        } else {
            memcpy(dst,src,min(info.dwEffWidth,dwBytesperline));
        }
    }
    return TRUE;
}

RGBQUAD CxImage::GetPaletteColor(BYTE idx)
{
    RGBQUAD rgb = {0,0,0,0};
    if ((pDib)&&(head.biClrUsed)){
        BYTE* iDst = (BYTE*)(pDib) + sizeof(BITMAPINFOHEADER);
        if (idx<head.biClrUsed){
            long ldx=idx*sizeof(RGBQUAD);
            rgb.rgbBlue = iDst[ldx++];
            rgb.rgbGreen=iDst[ldx++];
            rgb.rgbRed =iDst[ldx++];
            rgb.rgbReserved = iDst[ldx];
        }
    }
    return rgb;
}

int CxImage::SetPaletteColor(BYTE idx, COLORREF cr)
{
    if ((pDib)&&(head.biClrUsed)){
        BYTE* iDst = (BYTE*)(pDib) + sizeof(BITMAPINFOHEADER);
        if (idx<head.biClrUsed){
            long ldx=idx*sizeof(RGBQUAD);
            iDst[ldx++] = (BYTE) GetBValue(cr);
            iDst[ldx++] = (BYTE) GetGValue(cr);
            iDst[ldx++] = (BYTE) GetRValue(cr);
            iDst[ldx] = (BYTE) 0;
            info.last_c_isvalid = FALSE;
        }
    }

    return OK;
}

int CxImage::SetPaletteColor(BYTE idx, RGBQUAD c)
{
    if ((pDib)&&(head.biClrUsed)){
        BYTE* iDst = (BYTE*)(pDib) + sizeof(BITMAPINFOHEADER);
        if (idx<head.biClrUsed){
            long ldx=idx*sizeof(RGBQUAD);
            iDst[ldx++] = (BYTE) c.rgbBlue;
            iDst[ldx++] = (BYTE) c.rgbGreen;
            iDst[ldx++] = (BYTE) c.rgbRed;
            iDst[ldx] = (BYTE) c.rgbReserved;
            info.last_c_isvalid = FALSE;
        }
    }
    return OK;
}
BOOL CxImage::IsGrayScale()
{
    RGBQUAD* ppal=GetPalette();
    if(!(pDib && ppal && head.biClrUsed)) return FALSE;
    for(DWORD i=0;i<head.biClrUsed;i++){
        if (ppal[i].rgbBlue!=i || ppal[i].rgbGreen!=i || ppal[i].rgbRed!=i) return FALSE;
    }
    return TRUE;
}

void CxImage::SwapIndex(BYTE idx1, BYTE idx2)
{
    RGBQUAD* ppal=GetPalette();
    if(!(pDib && ppal)) return;
    //swap the colors
    RGBQUAD tempRGB=GetPaletteColor(idx1);
    SetPaletteColor(idx1,GetPaletteColor(idx2));
    SetPaletteColor(idx2,tempRGB);
    //swap the pixels
    BYTE idx;
    for(long y=0; y < head.biHeight; y++){
        for(long x=0; x < head.biWidth; x++){
            idx=BlindGetPixelIndex(x,y);
            if (idx==idx1) BlindSetPixelIndex(x,y,idx2);
            if (idx==idx2) BlindSetPixelIndex(x,y,idx1);
        }
    }

}

void CxImage::BlindSetPixelIndex(long x, long y, BYTE i)
{

    if ((pDib==NULL)||(head.biClrUsed==0)|| (x<0)||(y<0)||(x>=head.biWidth)||(y>=head.biHeight))
        return;

    if (head.biBitCount==8){
        info.pImage[y*info.dwEffWidth + x]=i;
        return;
    } else {
        BYTE pos;
        BYTE* iDst= info.pImage + y*info.dwEffWidth + (x*head.biBitCount >> 3);
        if (head.biBitCount==4){
            pos = (BYTE)(4*(1-x%2));
            *iDst &= ~(0x0F<<pos);
            *iDst |= ((i & 0x0F)<<pos);
            return;
        } else if (head.biBitCount==1){
            pos = (BYTE)(7-x%8);
            *iDst &= ~(0x01<<pos);
            *iDst |= ((i & 0x01)<<pos);
            return;
        }
    }
}

BYTE CxImage::BlindGetPixelIndex(const long x, const long y)
{
    if ((pDib==NULL) || (head.biClrUsed==0) || !IsInside(x,y))
        return 0;

    if (head.biBitCount==8){
        return info.pImage[y*info.dwEffWidth + x];
    } else {
        BYTE pos;
        BYTE iDst= info.pImage[y*info.dwEffWidth + (x*head.biBitCount >> 3)];
        if (head.biBitCount==4){
            pos = (BYTE)(4*(1-x%2));
            iDst &= (0x0F<<pos);
            return (BYTE)(iDst >> pos);
        } else if (head.biBitCount==1){
            pos = (BYTE)(7-x%8);
            iDst &= (0x01<<pos);
            return (BYTE)(iDst >> pos);
        }
    }
    return 0;

}
RGBQUAD CxImage::BlindGetPixelColor(const long x, const long y, BOOL bGetAlpha)
{
    RGBQUAD rgb;

    if ((pDib==NULL) || !IsInside(x,y))
        {rgb.rgbReserved = 0; return rgb;}

    if (head.biClrUsed){
        rgb = GetPaletteColor(BlindGetPixelIndex(x,y));
    } else {
        BYTE* iDst  = info.pImage + y*info.dwEffWidth + x*3;
        rgb.rgbBlue = *iDst++;
        rgb.rgbGreen= *iDst++;
        rgb.rgbRed  = *iDst;
        rgb.rgbReserved = 0; //needed for images without alpha layer
    }

#if CXIMAGE_SUPPORT_ALPHA
    if (pAlpha && bGetAlpha) rgb.rgbReserved = BlindAlphaGet(x,y);
#else
    rgb.rgbReserved = 0;
#endif //CXIMAGE_SUPPORT_ALPHA

    return rgb;
}

void CxImage::BlindSetPixelColor(long x, long y, RGBQUAD c, BOOL bSetAlpha)
{
    if ((pDib==NULL)||(x<0)||(y<0)||
        (x>=head.biWidth)||(y>=head.biHeight))
        return;
    if (head.biClrUsed)
        BlindSetPixelIndex(x,y,GetNearestIndex(c));
    else {
        BYTE* iDst = info.pImage + y*info.dwEffWidth + x*3;
        *iDst++ = c.rgbBlue;
        *iDst++ = c.rgbGreen;
        *iDst   = c.rgbRed;
    }

#if CXIMAGE_SUPPORT_ALPHA
    if (bSetAlpha) AlphaSet(x,y,c.rgbReserved);
#endif //CXIMAGE_SUPPORT_ALPHA

}

BYTE CxImage::GetNearestIndex(RGBQUAD c)
{
    if ((pDib==NULL)||(head.biClrUsed==0)) return 0;

    // <RJ> check matching with the previous result
    if (info.last_c_isvalid && (*(long*)&info.last_c == *(long*)&c)) return info.last_c_index;
    info.last_c = c;
    info.last_c_isvalid = TRUE;

    BYTE* iDst = (BYTE*)(pDib) + sizeof(BITMAPINFOHEADER);
    long distance=200000;
    int i,j = 0;
    long k,l;
    int m = (int)(head.biClrImportant==0 ? head.biClrUsed : head.biClrImportant);
    for(i=0,l=0;i<m;i++,l+=sizeof(RGBQUAD)){
        k = (iDst[l]-c.rgbBlue)*(iDst[l]-c.rgbBlue)+
            (iDst[l+1]-c.rgbGreen)*(iDst[l+1]-c.rgbGreen)+
            (iDst[l+2]-c.rgbRed)*(iDst[l+2]-c.rgbRed);
//      k = abs(iDst[l]-c.rgbBlue)+abs(iDst[l+1]-c.rgbGreen)+abs(iDst[l+2]-c.rgbRed);
        if (k==0){
            j=i;
            break;
        }
        if (k<distance){
            distance=k;
            j=i;
        }
    }
    info.last_c_index = (BYTE)j;
    return (BYTE)j;
}
BOOL CxImage::IsTransparent()
{
    return info.nBkgndIndex>=0; // <vho>
}

BOOL CxImage::IsTransparent(long x, long y)
{
    if (!pDib) return FALSE;

    if (info.nBkgndIndex>=0){
        if (head.biClrUsed){
            if (GetPixelIndex(x,y) == info.nBkgndIndex) return TRUE;
        } else {
            RGBQUAD ct = info.nBkgndColor;
            RGBQUAD c = GetPixelColor(x,y,FALSE);
            if (*(long*)&c==*(long*)&ct) return TRUE;
        }
    }

#if CXIMAGE_SUPPORT_ALPHA
    if (pAlpha) return AlphaGet(x,y)==0;
#endif

    return FALSE;
}

RGBQUAD CxImage::GetPixelColor(long x, long y, BOOL bGetAlpha)
{
    RGBQUAD rgb=info.nBkgndColor; //<mpwolski>
    if ((pDib==NULL)||(x<0)||(y<0)||
        (x>=head.biWidth)||(y>=head.biHeight)){
        if (info.nBkgndIndex >= 0){
            if (head.biBitCount<24) return GetPaletteColor((BYTE)info.nBkgndIndex);
            else return info.nBkgndColor;
        } else if (pDib) return GetPixelColor(0,0,TRUE);
        return rgb;
    }

    if (head.biClrUsed){
        rgb = GetPaletteColor(BlindGetPixelIndex(x,y));
    } else {
        BYTE* iDst  = info.pImage + y*info.dwEffWidth + x*3;
        rgb.rgbBlue = *iDst++;
        rgb.rgbGreen= *iDst++;
        rgb.rgbRed  = *iDst;
    }
#if CXIMAGE_SUPPORT_ALPHA
    if (pAlpha && bGetAlpha) rgb.rgbReserved = BlindAlphaGet(x,y);
#else
    rgb.rgbReserved = 0;
#endif //CXIMAGE_SUPPORT_ALPHA
    return rgb;
}

BYTE CxImage::GetPixelIndex(long x, long y)
{
    if ((pDib==NULL)||(head.biClrUsed==0)) return 0;

    if ((x<0)||(y<0)||(x>=head.biWidth)||(y>=head.biHeight)) {
        if (info.nBkgndIndex >= 0)  return (BYTE)info.nBkgndIndex;
        else return *info.pImage;
    }
    if (head.biBitCount==8){
        return info.pImage[y*info.dwEffWidth + x];
    } else {
        BYTE pos;
        BYTE iDst= info.pImage[y*info.dwEffWidth + (x*head.biBitCount >> 3)];
        if (head.biBitCount==4){
            pos = (BYTE)(4*(1-x%2));
            iDst &= (0x0F<<pos);
            return (BYTE)(iDst >> pos);
        } else if (head.biBitCount==1){
            pos = (BYTE)(7-x%8);
            iDst &= (0x01<<pos);
            return (BYTE)(iDst >> pos);
        }
    }
    return 0;
}
int CxImage::SetGrayPalette()
{
    if ((pDib==NULL)||(head.biClrUsed==0)) return ERROR;

    RGBQUAD* pal=GetPalette();
    ASSERT(pal);

    for (DWORD ni=0;ni<head.biClrUsed;ni++)
        pal[ni].rgbBlue=pal[ni].rgbGreen = pal[ni].rgbRed = (BYTE)(ni*(255/(head.biClrUsed-1)));

    return OK;
}

void CxImage::SetPaletteColor(BYTE idx, BYTE r, BYTE g, BYTE b, BYTE alpha)
{
    if ((pDib)&&(head.biClrUsed)){
        BYTE* iDst = (BYTE*)(pDib) + sizeof(BITMAPINFOHEADER);
        if (idx<head.biClrUsed){
            long ldx=idx*sizeof(RGBQUAD);
            iDst[ldx++] = (BYTE) b;
            iDst[ldx++] = (BYTE) g;
            iDst[ldx++] = (BYTE) r;
            iDst[ldx] = (BYTE) alpha;
            info.last_c_isvalid = FALSE;
        }
    }
}

BYTE CxImage::GetPixelGray(long x, long y)
{
    RGBQUAD color = GetPixelColor(x,y,TRUE);
    return (BYTE)RGB2GRAY(color.rgbRed,color.rgbGreen,color.rgbBlue);
}


void CxImage::SetPixelIndex(long x, long y, BYTE i)
{
    if ((pDib==NULL)||(head.biClrUsed==0)||
        (x<0)||(y<0)||(x>=head.biWidth)||(y>=head.biHeight)) return ;

    if (head.biBitCount==8){
        info.pImage[y*info.dwEffWidth + x]=i;
        return;
    } else {
        BYTE pos;
        BYTE* iDst= info.pImage + y*info.dwEffWidth + (x*head.biBitCount >> 3);
        if (head.biBitCount==4){
            pos = (BYTE)(4*(1-x%2));
            *iDst &= ~(0x0F<<pos);
            *iDst |= ((i & 0x0F)<<pos);
            return;
        } else if (head.biBitCount==1){
            pos = (BYTE)(7-x%8);
            *iDst &= ~(0x01<<pos);
            *iDst |= ((i & 0x01)<<pos);
            return;
        }
    }
}

void CxImage::SetPixelColor(long x, long y, COLORREF cr)
{
    SetPixelColor(x,y,RGBtoRGBQUAD(cr),FALSE);
}

RGBQUAD CxImage::RGBtoRGBQUAD(COLORREF cr)
{
    RGBQUAD c;
    c.rgbRed = GetRValue(cr);   /* get R, G, and B out of DWORD */
    c.rgbGreen = GetGValue(cr);
    c.rgbBlue = GetBValue(cr);
    c.rgbReserved=0;
    return c;
}


void CxImage::SetPixelColor(long x, long y, RGBQUAD c, BOOL bSetAlpha)
{
    if ((pDib==NULL)||(x<0)||(y<0)||
        (x>=head.biWidth)||(y>=head.biHeight)) return;
    if (head.biClrUsed)
        BlindSetPixelIndex(x,y,GetNearestIndex(c));
    else {
        BYTE* iDst = info.pImage + y*info.dwEffWidth + x*3;
        *iDst++ = c.rgbBlue;
        *iDst++ = c.rgbGreen;
        *iDst   = c.rgbRed;
    }
#if CXIMAGE_SUPPORT_ALPHA
    if (bSetAlpha) AlphaSet(x,y,c.rgbReserved);
#endif //CXIMAGE_SUPPORT_ALPHA
}

void CxImage::BlendPixelColor(long x, long y, RGBQUAD c, float blend, BOOL bSetAlpha)
{
    if ((pDib==NULL)||(x<0)||(y<0)||
        (x>=head.biWidth)||(y>=head.biHeight)) return;

    int a0 = (int)(256*blend);
    int a1 = 256 - a0;

    RGBQUAD c0 = BlindGetPixelColor(x,y,TRUE);
    c.rgbRed  = (BYTE)((c.rgbRed * a0 + c0.rgbRed * a1)>>8);
    c.rgbBlue  = (BYTE)((c.rgbBlue * a0 + c0.rgbBlue * a1)>>8);
    c.rgbGreen  = (BYTE)((c.rgbGreen * a0 + c0.rgbGreen * a1)>>8);

    if (head.biClrUsed)
        BlindSetPixelIndex(x,y,GetNearestIndex(c));
    else {
        BYTE* iDst = info.pImage + y*info.dwEffWidth + x*3;
        *iDst++ = c.rgbBlue;
        *iDst++ = c.rgbGreen;
        *iDst   = c.rgbRed;
#if CXIMAGE_SUPPORT_ALPHA
        if (bSetAlpha) AlphaSet(x,y,c.rgbReserved);
#endif //CXIMAGE_SUPPORT_ALPHA
    }
}

void CxImage::RGBtoBGR(BYTE *buffer, int length)
{
    if (buffer && (head.biClrUsed==0)){
        BYTE temp;
        length = min(length,(int)info.dwEffWidth);
        length = min(length,(int)(3*head.biWidth));
        for (int i=0;i<length;i+=3){
            temp = buffer[i]; buffer[i] = buffer[i+2]; buffer[i+2] = temp;
        }
    }
}

COLORREF CxImage::RGBQUADtoRGB(RGBQUAD c)
{
    return RGB(c.rgbRed,c.rgbGreen,c.rgbBlue);
}


BOOL CxImage::GetPaletteColor(BYTE i, BYTE *r, BYTE *g, BYTE *b)
{
    RGBQUAD* ppal=GetPalette();
    if (ppal) {
        *r = ppal[i].rgbRed;
        *g = ppal[i].rgbGreen;
        *b = ppal[i].rgbBlue; 
        return TRUE;
    }
    return FALSE;
}

void CxImage::SetPalette(DWORD n, BYTE *r, BYTE *g, BYTE *b)
{
    if ((!r)||(pDib==NULL)||(head.biClrUsed==0)) return;
    if (!g) g = r;
    if (!b) b = g;
    RGBQUAD* ppal=GetPalette();
    DWORD m=min(n,head.biClrUsed);
    for (DWORD i=0; i<m;i++){
        ppal[i].rgbRed=r[i];
        ppal[i].rgbGreen=g[i];
        ppal[i].rgbBlue=b[i];
    }
    info.last_c_isvalid = FALSE;

}

void CxImage::SetPalette(rgb_color *rgb, DWORD nColors)
{
    if ((!rgb)||(pDib==NULL)||(head.biClrUsed==0)) return;
    RGBQUAD* ppal=GetPalette();
    DWORD m=min(nColors,head.biClrUsed);
    for (DWORD i=0; i<m;i++){
        ppal[i].rgbRed=rgb[i].r;
        ppal[i].rgbGreen=rgb[i].g;
        ppal[i].rgbBlue=rgb[i].b;
    }
    info.last_c_isvalid = FALSE;
}

void CxImage::BlendPalette(COLORREF cr, long perc)
{
    if ((pDib==NULL)||(head.biClrUsed==0)) return;
    BYTE* iDst = (BYTE*)(pDib) + sizeof(BITMAPINFOHEADER);
    DWORD i,r,g,b;
    RGBQUAD* pPal=(RGBQUAD*)iDst;
    r = GetRValue(cr);
    g = GetGValue(cr);
    b = GetBValue(cr);
    if (perc>100) perc=100;
    for(i=0;i<head.biClrUsed;i++){
        pPal[i].rgbBlue=(BYTE)((pPal[i].rgbBlue*(100-perc)+b*perc)/100);
        pPal[i].rgbGreen =(BYTE)((pPal[i].rgbGreen*(100-perc)+g*perc)/100);
        pPal[i].rgbRed =(BYTE)((pPal[i].rgbRed*(100-perc)+r*perc)/100);
    }
}

void CxImage::SwapRGB2BGR()
{
    if (!pDib) return;

    if (head.biClrUsed){
        RGBQUAD* ppal=GetPalette();
        BYTE b;
        if(!ppal) return;
        for(WORD a=0;a<head.biClrUsed;a++){
            b=ppal[a].rgbBlue; ppal[a].rgbBlue=ppal[a].rgbRed; ppal[a].rgbRed=b;
        }
    } else {
        for(long y=0;y<head.biHeight;y++){
            RGBtoBGR(GetBits(y),3*head.biWidth);
        }
    }
}

BOOL CxImage::GetTransparentMask(CxImage *iDst)
{
    if (!pDib) return FALSE;

    CxImage tmp;

    tmp.Init();
    tmp.Create(head.biWidth, head.biHeight, 1, GetType());
    tmp.SetStdPalette();
    tmp.Clear(0);

    for(long y=0; y<head.biHeight; y++){
        for(long x=0; x<head.biWidth; x++){
            if (IsTransparent(x,y)){
                tmp.BlindSetPixelIndex(x,y,1);
            }
        }
    }

    if (iDst) iDst->Transfer(&tmp,TRUE);
    else Transfer(&tmp,TRUE);

    return TRUE;
}

void CxImage::SetStdPalette()
{
    if (!pDib) return;
    switch (head.biBitCount){
    case 8:
        {
            memcpy(GetPalette(),__pal256,1024);
            break;
        }
    case 4:
        {
            memcpy(GetPalette(),__pal16,64);
            break;
        }
    case 1:
        {
            memcpy(GetPalette(),__pal2,8);
            break;
        }
    }
    info.last_c_isvalid = FALSE;
}

BOOL CxImage::IsSamePalette(CxImage &img, BOOL bCheckAlpha)
{
    if (head.biClrUsed != img.head.biClrUsed)
        return FALSE;
    if (head.biClrUsed == 0)
        return FALSE;

    RGBQUAD c1,c2;
    for (DWORD n=0; n<head.biClrUsed; n++){
        c1 = GetPaletteColor((BYTE)n);
        c2 = img.GetPaletteColor((BYTE)n);
        if (c1.rgbRed != c2.rgbRed) return FALSE;
        if (c1.rgbBlue != c2.rgbBlue) return FALSE;
        if (c1.rgbGreen != c2.rgbGreen) return FALSE;
        if (bCheckAlpha && (c1.rgbReserved != c2.rgbReserved)) return FALSE;
    }
    return TRUE;
}

DWORD CxImage::GetClrImportant()
{
    return head.biClrImportant;
}

void CxImage::SetClrImportant(DWORD ncolors)
{
    if (ncolors==0 || ncolors>256) {
        head.biClrImportant = 0;
        return;
    }

    switch(head.biBitCount){
    case 1:
        head.biClrImportant = min(ncolors,2);
        break;
    case 4:
        head.biClrImportant = min(ncolors,16);
        break;
    case 8:
        head.biClrImportant = ncolors;
        break;
    }
    return;
}

RGBQUAD CxImage::GetTransColor()
{
    if (head.biBitCount<24 && info.nBkgndIndex>=0) return GetPaletteColor((BYTE)info.nBkgndIndex);
    return info.nBkgndColor;
}

long CxImage::GetTransIndex()
{
    return info.nBkgndIndex;
}
void CxImage::SetTransIndex(long idx)
{
    if (idx<(long)head.biClrUsed)
        info.nBkgndIndex = idx;
    else 
        info.nBkgndIndex = 0;
}
void * CxImage::BlindGetPixelPointer(const long x, const long y)
{
    if ((pDib==NULL) || !IsInside(x,y))
        return 0;
    if (!IsIndexed())
        return info.pImage + y*info.dwEffWidth + x*3;
    else
        return 0;
}

BOOL CxImage::IsIndexed()
{
    return head.biClrUsed!=0;
}

void CxImage::DrawLine(int StartX, int EndX, int StartY, int EndY, COLORREF cr)
{
    RGBQUAD c = RGBtoRGBQUAD(cr);
    c.rgbReserved = 255;
    this->DrawLine(StartX, EndX, StartY, EndY,c ,TRUE);
}

BOOL CxImage::DrawLine(int StartX, int EndX, int StartY, int EndY, RGBQUAD color, BOOL bSetAlpha)
{
    ASSERT(pDib);
    //////////////////////////////////////////////////////
    // Draws a line using the Bresenham line algorithm
    // Thanks to Jordan DeLozier <JDL>
    //////////////////////////////////////////////////////
    int x1 = StartX;
    int y1 = StartY;
    int x = x1;                       // Start x off at the first pixel
    int y = y1;                       // Start y off at the first pixel
    int x2 = EndX;
    int y2 = EndY;

    int xinc1,xinc2,yinc1,yinc2;      // Increasing values
    int den, num, numadd,numpixels;   
    int deltax = abs(x2 - x1);        // The difference between the x's
    int deltay = abs(y2 - y1);        // The difference between the y's

    // Get Increasing Values
    if (x2 >= x1) {                // The x-values are increasing
        xinc1 = 1;
        xinc2 = 1;
    } else {                         // The x-values are decreasing
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {                // The y-values are increasing
        yinc1 = 1;
        yinc2 = 1;
    } else {                         // The y-values are decreasing
        yinc1 = -1;
        yinc2 = -1;
    }

    // Actually draw the line
    if (deltax >= deltay)         // There is at least one x-value for every y-value
    {
        xinc1 = 0;                  // Don't change the x when numerator >= denominator
        yinc2 = 0;                  // Don't change the y for every iteration
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;         // There are more x-values than y-values
    }
    else                          // There is at least one y-value for every x-value
    {
        xinc2 = 0;                  // Don't change the x for every iteration
        yinc1 = 0;                  // Don't change the y when numerator >= denominator
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;         // There are more y-values than x-values
    }
    
    for (int curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        // Draw the current pixel
        SetPixelColor(x,y,color,bSetAlpha);
        
        num += numadd;              // Increase the numerator by the top of the fraction
        if (num >= den)             // Check if numerator >= denominator
        {
            num -= den;               // Calculate the new numerator value
            x += xinc1;               // Change the x as appropriate
            y += yinc1;               // Change the y as appropriate
        }
        x += xinc2;                 // Change the x as appropriate
        y += yinc2;                 // Change the y as appropriate
    }

    return OK;
}

DWORD CxImage::GetEffWidth()
{
    return info.dwEffWidth;
}


DWORD CxImage::GetNumColors()
{
    return head.biClrUsed;
}

BOOL CxImage::SetType(DWORD type)
{
    switch (type){
#if CXIMAGE_SUPPORT_BMP
    case CXIMAGE_FORMAT_BMP:
#endif
#if CXIMAGE_SUPPORT_GIF
    case CXIMAGE_FORMAT_GIF:
#endif
#if CXIMAGE_SUPPORT_JPG
    case CXIMAGE_FORMAT_JPG:
#endif
#if CXIMAGE_SUPPORT_PNG
    case CXIMAGE_FORMAT_PNG:
#endif
#if CXIMAGE_SUPPORT_MNG
    case CXIMAGE_FORMAT_MNG:
#endif
#if CXIMAGE_SUPPORT_ICO
    case CXIMAGE_FORMAT_ICO:
#endif
#if CXIMAGE_SUPPORT_TIF
    case CXIMAGE_FORMAT_TIF:
#endif
#if CXIMAGE_SUPPORT_TGA
    case CXIMAGE_FORMAT_TGA:
#endif
#if CXIMAGE_SUPPORT_PCX
    case CXIMAGE_FORMAT_PCX:
#endif
#if CXIMAGE_SUPPORT_WBMP
    case CXIMAGE_FORMAT_WBMP:
#endif
#if CXIMAGE_SUPPORT_WMF
    case CXIMAGE_FORMAT_WMF:
#endif
#if CXIMAGE_SUPPORT_JBG
    case CXIMAGE_FORMAT_JBG:
#endif
#if CXIMAGE_SUPPORT_JP2
    case CXIMAGE_FORMAT_JP2:
#endif
#if CXIMAGE_SUPPORT_JPC
    case CXIMAGE_FORMAT_JPC:
#endif
#if CXIMAGE_SUPPORT_PGX
    case CXIMAGE_FORMAT_PGX:
#endif
#if CXIMAGE_SUPPORT_PNM
    case CXIMAGE_FORMAT_PNM:
#endif
#if CXIMAGE_SUPPORT_RAS
    case CXIMAGE_FORMAT_RAS:
#endif
#if CXIMAGE_SUPPORT_SKA
    case CXIMAGE_FORMAT_SKA:
#endif
#if CXIMAGE_SUPPORT_RAW
    case CXIMAGE_FORMAT_RAW:
#endif
        info.dwType = type;
        return TRUE;
    }
    info.dwType = CXIMAGE_FORMAT_UNKNOWN;
    return FALSE;
}

DWORD CxImage::GetNumTypes()
{
    return CMAX_IMAGE_FORMATS-1;
}

DWORD CxImage::GetTypeIdFromName(const char *ext)
{
#if CXIMAGE_SUPPORT_BMP
    if (crt_stricmp(ext,("bmp"))==0 )       return CXIMAGE_FORMAT_BMP;
#endif
#if CXIMAGE_SUPPORT_JPG
    if (crt_stricmp(ext,("jpg"))==0 ||
        crt_stricmp(ext,("jpe"))==0 ||
        crt_stricmp(ext,("jfi"))==0 )       return CXIMAGE_FORMAT_JPG;
#endif
#if CXIMAGE_SUPPORT_GIF
    if (crt_stricmp(ext,("gif"))==0 )       return CXIMAGE_FORMAT_GIF;
#endif
#if CXIMAGE_SUPPORT_PNG
    if (crt_stricmp(ext,("png"))==0 )       return CXIMAGE_FORMAT_PNG;
#endif
#if CXIMAGE_SUPPORT_ICO
    if (crt_stricmp(ext,("ico"))==0 ||
        crt_stricmp(ext,("cur"))==0 )       return CXIMAGE_FORMAT_ICO;
#endif
#if CXIMAGE_SUPPORTIF
    if (crt_stricmp(ext,("tif"))==0 )       return CXIMAGE_FORMATIF;
#endif
#if CXIMAGE_SUPPORTGA
    if (crt_stricmp(ext,("tga"))==0 )       return CXIMAGE_FORMATGA;
#endif
#if CXIMAGE_SUPPORT_PCX
    if (crt_stricmp(ext,("pcx"))==0 )       return CXIMAGE_FORMAT_PCX;
#endif
#if CXIMAGE_SUPPORT_WBMP
    if (crt_stricmp(ext,("wbm"))==0 )       return CXIMAGE_FORMAT_WBMP;
#endif
#if CXIMAGE_SUPPORT_WMF
    if (crt_stricmp(ext,("wmf"))==0 ||
        crt_stricmp(ext,("emf"))==0 )       return CXIMAGE_FORMAT_WMF;
#endif
#if CXIMAGE_SUPPORT_JP2
    if (crt_stricmp(ext,("jp2"))==0 ||
        crt_stricmp(ext,("j2k"))==0 )       return CXIMAGE_FORMAT_JP2;
#endif
#if CXIMAGE_SUPPORT_JPC
    if (crt_stricmp(ext,("jpc"))==0 ||
        crt_stricmp(ext,("j2c"))==0 )       return CXIMAGE_FORMAT_JPC;
#endif
#if CXIMAGE_SUPPORT_PGX
    if (crt_stricmp(ext,("pgx"))==0 )       return CXIMAGE_FORMAT_PGX;
#endif
#if CXIMAGE_SUPPORT_RAS
    if (crt_stricmp(ext,("ras"))==0 )       return CXIMAGE_FORMAT_RAS;
#endif
#if CXIMAGE_SUPPORT_PNM
    if (crt_stricmp(ext,("pnm"))==0 ||
        crt_stricmp(ext,("pgm"))==0 ||
        crt_stricmp(ext,("ppm"))==0 )       return CXIMAGE_FORMAT_PNM;
#endif
#if CXIMAGE_SUPPORT_JBG
    if (crt_stricmp(ext,("jbg"))==0 )       return CXIMAGE_FORMAT_JBG;
#endif
#if CXIMAGE_SUPPORT_MNG
    if (crt_stricmp(ext,("mng"))==0 ||
        crt_stricmp(ext,("jng"))==0 )       return CXIMAGE_FORMAT_MNG;
#endif
#if CXIMAGE_SUPPORT_SKA
    if (crt_stricmp(ext,("ska"))==0 )       return CXIMAGE_FORMAT_SKA;
#endif
#if CXIMAGE_SUPPORT_RAW
    if (crt_stricmp(ext,("nef"))==0 ||
        crt_stricmp(ext,("crw"))==0 ||
        crt_stricmp(ext,("cr2"))==0 ||
        crt_stricmp(ext,("dng"))==0 ||
        crt_stricmp(ext,("arw"))==0 ||
        crt_stricmp(ext,("erf"))==0 ||
        crt_stricmp(ext,("3fr"))==0 ||
        crt_stricmp(ext,("dcr"))==0 ||
        crt_stricmp(ext,("raw"))==0 ||
        crt_stricmp(ext,("x3f"))==0 ||
        crt_stricmp(ext,("mef"))==0 ||
        crt_stricmp(ext,("raf"))==0 ||
        crt_stricmp(ext,("mrw"))==0 ||
        crt_stricmp(ext,("pef"))==0 ||
        crt_stricmp(ext,("sr2"))==0 ||
        crt_stricmp(ext,("orf"))==0 )       return CXIMAGE_FORMAT_RAW;
#endif

    return CXIMAGE_FORMAT_UNKNOWN;
}

DWORD CxImage::GetTypeIdFromIndex(const DWORD index)
{
    DWORD n;

    n=0; if (index == n) return CXIMAGE_FORMAT_UNKNOWN;
#if CXIMAGE_SUPPORT_BMP
    n++; if (index == n) return CXIMAGE_FORMAT_BMP;
#endif
#if CXIMAGE_SUPPORT_GIF
    n++; if (index == n) return CXIMAGE_FORMAT_GIF;
#endif
#if CXIMAGE_SUPPORT_JPG
    n++; if (index == n) return CXIMAGE_FORMAT_JPG;
#endif
#if CXIMAGE_SUPPORT_PNG
    n++; if (index == n) return CXIMAGE_FORMAT_PNG;
#endif
#if CXIMAGE_SUPPORT_ICO
    n++; if (index == n) return CXIMAGE_FORMAT_ICO;
#endif
#if CXIMAGE_SUPPORT_TIF
    n++; if (index == n) return CXIMAGE_FORMAT_TIF;
#endif
#if CXIMAGE_SUPPORT_TGA
    n++; if (index == n) return CXIMAGE_FORMAT_TGA;
#endif
#if CXIMAGE_SUPPORT_PCX
    n++; if (index == n) return CXIMAGE_FORMAT_PCX;
#endif
#if CXIMAGE_SUPPORT_WBMP
    n++; if (index == n) return CXIMAGE_FORMAT_WBMP;
#endif
#if CXIMAGE_SUPPORT_WMF
    n++; if (index == n) return CXIMAGE_FORMAT_WMF;
#endif
#if CXIMAGE_SUPPORT_JP2
    n++; if (index == n) return CXIMAGE_FORMAT_JP2;
#endif
#if CXIMAGE_SUPPORT_JPC
    n++; if (index == n) return CXIMAGE_FORMAT_JPC;
#endif
#if CXIMAGE_SUPPORT_PGX
    n++; if (index == n) return CXIMAGE_FORMAT_PGX;
#endif
#if CXIMAGE_SUPPORT_PNM
    n++; if (index == n) return CXIMAGE_FORMAT_PNM;
#endif
#if CXIMAGE_SUPPORT_RAS
    n++; if (index == n) return CXIMAGE_FORMAT_RAS;
#endif
#if CXIMAGE_SUPPORT_JBG
    n++; if (index == n) return CXIMAGE_FORMAT_JBG;
#endif
#if CXIMAGE_SUPPORT_MNG
    n++; if (index == n) return CXIMAGE_FORMAT_MNG;
#endif
#if CXIMAGE_SUPPORT_SKA
    n++; if (index == n) return CXIMAGE_FORMAT_SKA;
#endif
#if CXIMAGE_SUPPORT_RAW
    n++; if (index == n) return CXIMAGE_FORMAT_RAW;
#endif

    return CXIMAGE_FORMAT_UNKNOWN;
}

DWORD CxImage::GetTypeIndexFromId(const DWORD id)
{
    DWORD n;

    n=0; if (id == CXIMAGE_FORMAT_UNKNOWN) return n;
#if CXIMAGE_SUPPORT_BMP
    n++; if (id == CXIMAGE_FORMAT_BMP) return n;
#endif
#if CXIMAGE_SUPPORT_GIF
    n++; if (id == CXIMAGE_FORMAT_GIF) return n;
#endif
#if CXIMAGE_SUPPORT_JPG
    n++; if (id == CXIMAGE_FORMAT_JPG) return n;
#endif
#if CXIMAGE_SUPPORT_PNG
    n++; if (id == CXIMAGE_FORMAT_PNG) return n;
#endif
#if CXIMAGE_SUPPORT_ICO
    n++; if (id == CXIMAGE_FORMAT_ICO) return n;
#endif
#if CXIMAGE_SUPPORT_TIF
    n++; if (id == CXIMAGE_FORMAT_TIF) return n;
#endif
#if CXIMAGE_SUPPORT_TGA
    n++; if (id == CXIMAGE_FORMAT_TGA) return n;
#endif
#if CXIMAGE_SUPPORT_PCX
    n++; if (id == CXIMAGE_FORMAT_PCX) return n;
#endif
#if CXIMAGE_SUPPORT_WBMP
    n++; if (id == CXIMAGE_FORMAT_WBMP) return n;
#endif
#if CXIMAGE_SUPPORT_WMF
    n++; if (id == CXIMAGE_FORMAT_WMF) return n;
#endif
#if CXIMAGE_SUPPORT_JP2
    n++; if (id == CXIMAGE_FORMAT_JP2) return n;
#endif
#if CXIMAGE_SUPPORT_JPC
    n++; if (id == CXIMAGE_FORMAT_JPC) return n;
#endif
#if CXIMAGE_SUPPORT_PGX
    n++; if (id == CXIMAGE_FORMAT_PGX) return n;
#endif
#if CXIMAGE_SUPPORT_PNM
    n++; if (id == CXIMAGE_FORMAT_PNM) return n;
#endif
#if CXIMAGE_SUPPORT_RAS
    n++; if (id == CXIMAGE_FORMAT_RAS) return n;
#endif
#if CXIMAGE_SUPPORT_JBG
    n++; if (id == CXIMAGE_FORMAT_JBG) return n;
#endif
#if CXIMAGE_SUPPORT_MNG
    n++; if (id == CXIMAGE_FORMAT_MNG) return n;
#endif
#if CXIMAGE_SUPPORT_SKA
    n++; if (id == CXIMAGE_FORMAT_SKA) return n;
#endif
#if CXIMAGE_SUPPORT_RAW
    n++; if (id == CXIMAGE_FORMAT_RAW) return n;
#endif

    return 0;
}

DWORD CxImage::GetFrameDelay()
{
    return info.dwFrameDelay;
}

void CxImage::SetFrameDelay(DWORD d)
{
    info.dwFrameDelay=d;
}


void CxImage::GetOffset(long *x, long *y)
{
    *x=info.xOffset;
    *y=info.yOffset;
}

void CxImage::SetOffset(long x, long y)
{
    info.xOffset=x;
    info.yOffset=y;
}

BYTE CxImage::GetJpegQuality()
{
    return (BYTE)(info.fQuality + 0.5f);
}

float CxImage::GetJpegQualityF()
{
    return info.fQuality;
}


void CxImage::SetJpegQualityF(float q)
{
    if (q>0) info.fQuality = q;
    else  info.fQuality = 0.0f;
}

BYTE CxImage::GetJpegScale()
{
    return info.nJpegScale;
}

void CxImage::SetJpegScale(BYTE q)
{
    info.nJpegScale = q;
}

long CxImage::GetProgress()
{
    return info.nProgress;
}

long CxImage::GetEscape()
{
    return info.nEscape;
}

void CxImage::SetProgress(long p)
{
    info.nProgress = p;
}

void CxImage::SetEscape(long i)
{
    info.nEscape = i;
}
BOOL CxImage::IsValid()
{
    return pDib!=0;
}

BOOL CxImage::IsEnabled()
{
    return info.bEnabled;
}

void CxImage::Enable(BOOL enable)
{
    info.bEnabled=enable;
}

const char * CxImage::GetLastError()
{
    return info.szLastError;
}

long CxImage::GetNumFrames()
{
    return info.nNumFrames;
}

long CxImage::GetFrame()
{
    return info.nFrame;
}

void CxImage::SetFrame(long nFrame)
{
    info.nFrame=nFrame;
}

void CxImage::SetDisposalMethod(BYTE dm)
{
    info.dispmeth=dm; 
}

BYTE CxImage::GetDisposalMethod()
{
    return info.dispmeth;
}

BOOL CxImage::GetRetreiveAllFrames()
{
    return info.bGetAllFrames;
}

void CxImage::SetRetreiveAllFrames(BOOL flag)
{
    info.bGetAllFrames = flag;
}

CxImage * CxImage::GetFrame(long nFrame)
{
    if ( ppFrames == NULL) return NULL;
    if ( info.nNumFrames == 0) return NULL;
    if ( nFrame >= info.nNumFrames ) return NULL;
    if ( nFrame < 0) nFrame = info.nNumFrames - 1;
    return ppFrames[nFrame];
}

short CxImage::ntohs(const short word)
{
    if (info.bLittleEndianHost) return word;
    return ( (word & 0xff) << 8 ) | ( (word >> 8) & 0xff );
}

long CxImage::ntohl(const long dword)
{
    if (info.bLittleEndianHost) return dword;
    return  ((dword & 0xff) << 24 ) | ((dword & 0xff00) << 8 ) |
            ((dword >> 8) & 0xff00) | ((dword >> 24) & 0xff);
}

void CxImage::bihtoh(BITMAPINFOHEADER *bih)
{
    bih->biSize = ntohl(bih->biSize);
    bih->biWidth = ntohl(bih->biWidth);
    bih->biHeight = ntohl(bih->biHeight);
    bih->biPlanes = ntohs(bih->biPlanes);
    bih->biBitCount = ntohs(bih->biBitCount);
    bih->biCompression = ntohl(bih->biCompression);
    bih->biSizeImage = ntohl(bih->biSizeImage);
    bih->biXPelsPerMeter = ntohl(bih->biXPelsPerMeter);
    bih->biYPelsPerMeter = ntohl(bih->biYPelsPerMeter);
    bih->biClrUsed = ntohl(bih->biClrUsed);
    bih->biClrImportant = ntohl(bih->biClrImportant);
}

DWORD CxImage::DumpSize()
{
    DWORD n;
    n = sizeof(BITMAPINFOHEADER) + sizeof(CxImageInfo) + GetSize();
    
    if (pAlpha){
        n += 1 + head.biWidth * head.biHeight;
    } else n++;
    
    if (pSelection){
        n += 1 + head.biWidth * head.biHeight;
    } else n++;
    
    if (ppLayers){
        for (long m=0; m<GetNumLayers(); m++){
            if (GetLayer(m)){
                n += 1 + GetLayer(m)->DumpSize();
            }
        }
    } else n++;
    
    if (ppFrames){
        for (long m=0; m<GetNumFrames(); m++){
            if (GetFrame(m)){
                n += 1 + GetFrame(m)->DumpSize();
            }
        }
    } else n++;
    
    return n;
}

DWORD CxImage::Dump(BYTE *dst)
{
    if (!dst) return 0;

    memcpy(dst,&head,sizeof(BITMAPINFOHEADER));
    dst += sizeof(BITMAPINFOHEADER);

    memcpy(dst,&info,sizeof(CxImageInfo));
    dst += sizeof(CxImageInfo);

    memcpy(dst,pDib,GetSize());
    dst += GetSize();

    if (pAlpha){
        memset(dst++, 1, 1);
        memcpy(dst,pAlpha,head.biWidth * head.biHeight);
        dst += head.biWidth * head.biHeight;
    } else {
        memset(dst++, 0, 1);
    }

    if (pSelection){
        memset(dst++, 1, 1);
        memcpy(dst,pSelection,head.biWidth * head.biHeight);
        dst += head.biWidth * head.biHeight;
    } else {
        memset(dst++, 0, 1);
    }

    if (ppLayers){
        memset(dst++, 1, 1);
        for (long m=0; m<GetNumLayers(); m++){
            if (GetLayer(m)){
                dst += GetLayer(m)->Dump(dst);
            }
        }
    } else {
        memset(dst++, 0, 1);
    }

    if (ppFrames){
        memset(dst++, 1, 1);
        for (long m=0; m<GetNumFrames(); m++){
            if (GetFrame(m)){
                dst += GetFrame(m)->Dump(dst);
            }
        }
    } else {
        memset(dst++, 0, 1);
    }

    return DumpSize();
}

DWORD CxImage::UnDump(const BYTE *src)
{
    if (!src)
        return 0;
    if (!Destroy())
        return 0;
    if (!DestroyFrames())
        return 0;

    DWORD n = 0;

    memcpy(&head,src,sizeof(BITMAPINFOHEADER));
    n += sizeof(BITMAPINFOHEADER);

    memcpy(&info,&src[n],sizeof(CxImageInfo));
    n += sizeof(CxImageInfo);

    if (!Create(head.biWidth, head.biHeight, head.biBitCount, info.dwType))
        return 0;

    memcpy(pDib,&src[n],GetSize());
    n += GetSize();

    if (src[n++]){
        if (AlphaCreate()){
            memcpy(pAlpha, &src[n], head.biWidth * head.biHeight);
        }
        n += head.biWidth * head.biHeight;
    }

    if (src[n++]){
        RECT box = info.rSelectionBox;
        if (SelectionCreate()){
            info.rSelectionBox = box;
            memcpy(pSelection, &src[n], head.biWidth * head.biHeight);
        }
        n += head.biWidth * head.biHeight;
    }

    if (src[n++]){
        NEW_ARRAY(ppLayers,CxImage*,info.nNumLayers);
        for (long m=0; m<GetNumLayers(); m++){
            NEW(ppLayers[m],CxImage);
            n += ppLayers[m]->UnDump(&src[n]);
        }
    }

    if (src[n++]){
        NEW_ARRAY(ppFrames,CxImage*,info.nNumFrames);
        for (long m=0; m<GetNumFrames(); m++){
            NEW(ppFrames[m],CxImage);
            n += ppFrames[m]->UnDump(&src[n]);
        }
    }

    return n;
}

BOOL CxImage::EncodeSafeCheck(CFileBase *hFile)
{
    if (hFile==NULL) {
        strcpy(info.szLastError,CXIMAGE_ERR_NOFILE);
        return TRUE;
    }

    if (pDib==NULL){
        strcpy(info.szLastError,CXIMAGE_ERR_NOIMAGE);
        return TRUE;
    }
    return FALSE;
}

long CxImage::GetXDPI()
{
    return info.xDPI;
}

long CxImage::GetYDPI()
{
    return info.yDPI;
}

DWORD CxImage::GetCodecOption(DWORD imagetype)
{
    imagetype = GetTypeIndexFromId(imagetype);
    if (imagetype==0){
        imagetype = GetTypeIndexFromId(GetType());
    }
    return info.dwCodecOpt[imagetype];
}

void * CxImage::GetDIB()
{
    return pDib;
}

void CxImage::SetTransColor(RGBQUAD rgb)
{
    rgb.rgbReserved=0;
    info.nBkgndColor = rgb;
}

int CxImage::DestroyAll()
{
    WEAK_REF_DESTROY();
    this->DestroyFrames();
    this->Destroy();
    return OK;
}

BOOL CxImage::SetCodecOption(DWORD opt, DWORD imagetype)
{
    imagetype = GetTypeIndexFromId(imagetype);
    if (imagetype==0){
        imagetype = GetTypeIndexFromId(GetType());
    }
    info.dwCodecOpt[imagetype] = opt;
    return TRUE;
}

int CxImage::GetTypeByFileHeader(CFileBase *file)
{
    ASSERT(file);
    
    BYTE p[128],temp[128];
    WORD *wp = (WORD*)p;
    
    file->Seek(0);
    file->Read(p,100);

    if(p[0] == 0x42 && p[1] == 0x4D)
        return CXIMAGE_FORMAT_BMP;
    if(p[0] == (BYTE)0x89 && p[1] == (BYTE)0x50 && p[2] == (BYTE)0x4E && p[3] == (BYTE)0x47)
        return CXIMAGE_FORMAT_PNG;
    
    memcpy(temp,p+6,4);
    temp[4] = 0;

    if(crt_stricmp((char*)temp,"jfif") == 0 || crt_stricmp((char*)temp,"exif") == 0)
        return CXIMAGE_FORMAT_JPG;

    if(p[0] == 0xff && p[1] == 0xd8 && p[2] == 0xff)
        return CXIMAGE_FORMAT_JPG;

    if(p[0] == 0x47 && p[1] == 0x49 && p[2] == 0x46 && p[3] == 0x38 && p[4] == 0x39)
        return CXIMAGE_FORMAT_GIF;
    if(wp[0] == 0 &&(wp[1] ==1 || wp[1] == 2))
        return CXIMAGE_FORMAT_ICO;

    return CXIMAGE_FORMAT_UNKNOWN;
}

int CxImage::FillRect(RECT r,RGBQUAD color,BOOL setAlpha)
{
    int i;
    for(i = r.top; i <= r.bottom; i++)
    {
        this->DrawLine(r.left,r.right,i,i,color,setAlpha);
    }
    return OK;
}

int CxImage::DrawRect(RECT r,RGBQUAD color,BOOL setAlpha)
{
    this->DrawLine(r.left,r.right,r.top,r.top,color,setAlpha);
    this->DrawLine(r.left,r.left,r.top,r.bottom,color,setAlpha);
    this->DrawLine(r.right,r.right,r.top,r.bottom,color,setAlpha);
    this->DrawLine(r.left,r.right,r.bottom,r.bottom,color,setAlpha);
    return OK;
}

