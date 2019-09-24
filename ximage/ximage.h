// XImage.h: interface for the CxImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIMAGE_H__3AD1B86C_9635_407D_B57D_639B2D7EC4F3__INCLUDED_)
#define AFX_XIMAGE_H__3AD1B86C_9635_407D_B57D_639B2D7EC4F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "filebase.h"
#include "mem_tool.h"
#include <math.h>
#include "cruntime.h"
#define CXIMAGE_SUPPORT_ALPHA          1
#define CXIMAGE_SUPPORT_SELECTION      1
#define CXIMAGE_SUPPORT_TRANSFORMATION 1
#define CXIMAGE_SUPPORT_DSP            1
#define CXIMAGE_SUPPORT_LAYERS         1
#define CXIMAGE_SUPPORT_INTERPOLATION  1

#define CXIMAGE_SUPPORT_DECODE  1
#define CXIMAGE_SUPPORT_ENCODE  1

#ifdef  HAVE_WINDOWS_H
#define CXIMAGE_SUPPORT_WINDOWS 1
#else
#include "wintypes.h"
#endif

#define CXIMAGE_SUPPORT_BMP 1
#define CXIMAGE_SUPPORT_GIF 1
#define CXIMAGE_SUPPORT_JPG 1
#define CXIMAGE_SUPPORT_PNG 1
#define CXIMAGE_SUPPORT_ICO 1
#define CXIMAGE_SUPPORT_TIF 1
#define CXIMAGE_SUPPORT_TGA 1
#define CXIMAGE_SUPPORT_PCX 1
#define CXIMAGE_SUPPORT_WBMP 1
#define CXIMAGE_SUPPORT_WMF 1

#define CXIMAGE_SUPPORT_JP2 0
#define CXIMAGE_SUPPORT_JPC 0
#define CXIMAGE_SUPPORT_PGX 0
#define CXIMAGE_SUPPORT_PNM 0
#define CXIMAGE_SUPPORT_RAS 0

#define CXIMAGE_SUPPORT_JBG 0

#define CXIMAGE_SUPPORT_MNG 0
#define CXIMAGE_SUPPORT_SKA 0
#define CXIMAGE_SUPPORT_RAW 0

#if CXIMAGE_SUPPORT_JP2 || CXIMAGE_SUPPORT_JPC || CXIMAGE_SUPPORT_PGX || CXIMAGE_SUPPORT_PNM || CXIMAGE_SUPPORT_RAS
#define CXIMAGE_SUPPORT_JASPER 1
#else
#define CXIMAGE_SUPPORT_JASPER 0
#endif

#if CXIMAGE_SUPPORT_DSP
#define CXIMAGE_SUPPORT_TRANSFORMATION 1
#endif

#if CXIMAGE_SUPPORT_TRANSFORMATION || CXIMAGE_SUPPORT_TIF || CXIMAGE_SUPPORT_TGA || CXIMAGE_SUPPORT_BMP || CXIMAGE_SUPPORT_WINDOWS
#define CXIMAGE_SUPPORT_BASICTRANSFORMATIONS 1
#endif

#if CXIMAGE_SUPPORT_DSP || CXIMAGE_SUPPORT_TRANSFORMATION
#define CXIMAGE_SUPPORT_INTERPOLATION 1
#endif

#define CXIMAGE_FORMAT_UNKNOWN 0

#if CXIMAGE_SUPPORT_BMP
#define CXIMAGE_FORMAT_BMP  1
#endif
#if CXIMAGE_SUPPORT_GIF
#define CXIMAGE_FORMAT_GIF  2
#endif
#if CXIMAGE_SUPPORT_JPG
#define CXIMAGE_FORMAT_JPG  3
#endif
#if CXIMAGE_SUPPORT_PNG
#define CXIMAGE_FORMAT_PNG  4
#endif
#if CXIMAGE_SUPPORT_ICO
#define CXIMAGE_FORMAT_ICO  5
#endif
#if CXIMAGE_SUPPORT_TIF
#define CXIMAGE_FORMAT_TIF  6
#endif
#if CXIMAGE_SUPPORT_TGA
#define CXIMAGE_FORMAT_TGA  7
#endif
#if CXIMAGE_SUPPORT_PCX
#define CXIMAGE_FORMAT_PCX  8
#endif
#if CXIMAGE_SUPPORT_WBMP
#define CXIMAGE_FORMAT_WBMP  9
#endif
#if CXIMAGE_SUPPORT_WMF
#define CXIMAGE_FORMAT_WMF  10
#endif
#if CXIMAGE_SUPPORT_JP2
#define CXIMAGE_FORMAT_JP2  11
#endif
#if CXIMAGE_SUPPORT_JPC
#define CXIMAGE_FORMAT_JPC  12
#endif
#if CXIMAGE_SUPPORT_PGX
#define CXIMAGE_FORMAT_PGX  13
#endif
#if CXIMAGE_SUPPORT_PNM
#define CXIMAGE_FORMAT_PNM  14
#endif
#if CXIMAGE_SUPPORT_RAS
#define CXIMAGE_FORMAT_RAS  15
#endif
#if CXIMAGE_SUPPORT_JBG
#define CXIMAGE_FORMAT_JBG  16
#endif
#if CXIMAGE_SUPPORT_MNG
#define CXIMAGE_FORMAT_MNG  17
#endif
#if CXIMAGE_SUPPORT_SKA
#define CXIMAGE_FORMAT_SKA  18
#endif
#if CXIMAGE_SUPPORT_RAW
#define CXIMAGE_FORMAT_RAW  19
#endif

#define CMAX_IMAGE_FORMATS  \
                    (CXIMAGE_SUPPORT_BMP + CXIMAGE_SUPPORT_GIF + CXIMAGE_SUPPORT_JPG + \
                     CXIMAGE_SUPPORT_PNG + CXIMAGE_SUPPORT_MNG + CXIMAGE_SUPPORT_ICO + \
                     CXIMAGE_SUPPORT_TIF + CXIMAGE_SUPPORT_TGA + CXIMAGE_SUPPORT_PCX + \
                     CXIMAGE_SUPPORT_WBMP+ CXIMAGE_SUPPORT_WMF + \
                     CXIMAGE_SUPPORT_JBG + CXIMAGE_SUPPORT_JP2 + CXIMAGE_SUPPORT_JPC + \
                     CXIMAGE_SUPPORT_PGX + CXIMAGE_SUPPORT_PNM + CXIMAGE_SUPPORT_RAS + \
                     CXIMAGE_SUPPORT_SKA + CXIMAGE_SUPPORT_RAW + 1 )

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

#undef ntohl
#undef ntohs

#define CXIMAGE_MAX_MEMORY (512*1024*1024)
#define CXIMAGE_DEFAULT_DPI 96
#define CXIMAGE_ERR_NOFILE "null file handler"
#define CXIMAGE_ERR_NOIMAGE "null image!!!"
#define CXIMAGE_SUPPORT_EXCEPTION_HANDLING 1
#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

#define IM_NEAREST_NEIGHBOUR    1
#define IM_BILINEAR     2
#define IM_BSPLINE  3
#define IM_BICUBIC  4
#define IM_BICUBIC2     5
#define IM_LANCZOS  6
#define IM_BOX  7
#define IM_HERMITE  8
#define IM_HAMMING  9
#define IM_SINC     10
#define IM_BLACKMAN     11
#define IM_BESSEL   12
#define IM_GAUSSIAN     13
#define IM_QUADRATIC    14
#define IM_MITCHELL     15
#define IM_CATROM   16
#define IM_HANNING  17
#define IM_POWER    18

#define OM_COLOR    1
#define OM_BACKGROUND   2
#define OM_TRANSPARENT  3
#define OM_WRAP     4
#define OM_REPEAT   5
#define OM_MIRROR   6

#define RLE_COMMAND     0
#define RLE_ENDOFLINE   0
#define RLE_ENDOFBITMAP 1
#define RLE_DELTA       2


#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

#ifndef WIDTHBYTES
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */
#endif

#define DibWidthBytesN(lpbi, n) (UINT)WIDTHBYTES((UINT)(lpbi)->biWidth * (UINT)(n))
#define DibWidthBytes(lpbi)     DibWidthBytesN(lpbi, (lpbi)->biBitCount)

#define DibSizeImage(lpbi)      ((lpbi)->biSizeImage == 0 \
                                    ? ((DWORD)(UINT)DibWidthBytes(lpbi) * (DWORD)(UINT)(lpbi)->biHeight) \
                                    : (lpbi)->biSizeImage)

#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)

#define FixBitmapInfo(lpbi)     if ((lpbi)->biSizeImage == 0)                 \
                                                (lpbi)->biSizeImage = DibSizeImage(lpbi); \
                                if ((lpbi)->biClrUsed == 0)                   \
                                    (lpbi)->biClrUsed = DibNumColors(lpbi);   \

#define cx_try BOOL cx_error=FALSE;
#define cx_throw(message) {cx_error=TRUE; if(strcmp(message,"")) strcpy(info.szLastError,message); goto cx_error_catch;}
#define cx_catch cx_error_catch: char message[]=""; if(cx_error)

enum ImageOpType { OpAdd, OpAnd, OpXor, OpOr, OpMask, OpSrcCopy, OpDstCopy, OpSub, OpSrcBlend, OpScreen, OpAvg };

struct rgb_color { BYTE r,g,b; };

#if CXIMAGE_SUPPORT_WINDOWS
// <VATI> text placement data
// members must be initialized with the InitTextInfo(&this) function.
typedef struct tagCxTextInfo
{
#if defined (_WIN32_WCE)
    char    text[256];  ///< text for windows CE
#else
    char    text[4096]; ///< text (char -> TCHAR for UNICODE [Cesar M])
#endif
    LOGFONTA  lfont;      ///< font and codepage data
    COLORREF fcolor;     ///< foreground color
    long     align;      ///< DT_CENTER, DT_RIGHT, DT_LEFT aligment for multiline text
    BYTE     smooth;     ///< text smoothing option. Default is false.
    BYTE     opaque;     ///< text has background or hasn't. Default is true.
                         ///< data for background (ignored if .opaque==FALSE) 
    COLORREF bcolor;     ///< background color
    float    b_opacity;  ///< opacity value for background between 0.0-1.0 Default is 0. (opaque)
    BYTE     b_outline;  ///< outline width for background (zero: no outline)
    BYTE     b_round;    ///< rounding radius for background rectangle. % of the height, between 0-50. Default is 10.
                         ///< (backgr. always has a frame: width = 3 pixel + 10% of height by default.)
} CXTEXTINFO;
#endif

class CxPoint2
{
public:
  CxPoint2();
  CxPoint2(float const x_, float const y_);
  CxPoint2(CxPoint2 const &p);

  float Distance(CxPoint2 const p2);
  float Distance(float const x_, float const y_);

  float x,y;
};

//and simple rectangle
class CxRect2
{
public:
  CxRect2();
  CxRect2(float const x1_, float const y1_, float const x2_, float const y2_);
  CxRect2(CxPoint2 const &bl, CxPoint2 const &tr);
  CxRect2(CxRect2 const &p);

  float Surface();
  CxRect2 CrossSection(CxRect2 const &r2);
  CxPoint2 Center();
  float Width();
  float Height();

  CxPoint2 botLeft;
  CxPoint2 topRight;
};

class CxImage;
class CxImageInfo{
public:
    DWORD   dwEffWidth;         ///< DWORD aligned scan line width
    BYTE*   pImage;             ///< THE IMAGE BITS
    CxImage* pGhost;            ///< if this is a ghost, pGhost points to the body
    CxImage* pParent;           ///< if this is a layer, pParent points to the body
    DWORD   dwType;             ///< original image format
    char    szLastError[256];   ///< debugging
    long    nProgress;          ///< monitor
    long    nEscape;            ///< escape
    long    nBkgndIndex;        ///< used for GIF, PNG, MNG
    RGBQUAD nBkgndColor;        ///< used for RGB transparency
    float   fQuality;           ///< used for JPEG, JPEG2000 (0.0f ... 100.0f)
    BYTE    nJpegScale;         ///< used for JPEG [ignacio]
    long    nFrame;             ///< used for TIF, GIF, MNG : actual frame
    long    nNumFrames;         ///< used for TIF, GIF, MNG : total number of frames
    DWORD   dwFrameDelay;       ///< used for GIF, MNG
    long    xDPI;               ///< horizontal resolution
    long    yDPI;               ///< vertical resolution
    RECT    rSelectionBox;      ///< bounding rectangle
    BYTE    nAlphaMax;          ///< max opacity (fade)
    BOOL    bAlphaPaletteEnabled; ///< true if alpha values in the palette are enabled.
    BOOL    bEnabled;           ///< enables the painting functions
    long    xOffset;
    long    yOffset;
    DWORD   dwCodecOpt[CMAX_IMAGE_FORMATS];     ///< for GIF, TIF : 0=def.1=unc,2=fax3,3=fax4,4=pack,5=jpg
    RGBQUAD last_c;             ///< for GetNearestIndex optimization
    BYTE    last_c_index;
    BOOL    last_c_isvalid;
    long    nNumLayers;
    DWORD   dwFlags;            ///< 0x??00000 = reserved, 0x00??0000 = blend mode, 0x0000???? = layer id - user flags
    BYTE    dispmeth;
    BOOL    bGetAllFrames;
    BOOL    bLittleEndianHost;
public:
    int Clear();
    CxImageInfo();
    virtual ~CxImageInfo();
    int Init();
    int Destroy();
    int Copy(CxImageInfo *p);
    int Comp(CxImageInfo *p);
    int Print();
    int InitBasic();
};

class CImageIterator
{
public:
    int Itx, Ity;       // Counters
    int Stepx, Stepy;
    BYTE* IterImage;    //  Image pointer
    CxImage *ima;
public:
    // Constructors
    CImageIterator ( void );
    CImageIterator ( CxImage *image );

    // Iterators
    BOOL ItOK ();
    void Reset ();
    void Upset ();
    void SetRow(BYTE *buf, int n);
    void GetRow(BYTE *buf, int n);
    BYTE GetByte( ) { return IterImage[Itx]; }
    void SetByte(BYTE b) { IterImage[Itx] = b; }
    BYTE* GetRow(void);
    BYTE* GetRow(int n);
    BOOL NextRow();
    BOOL PrevRow();
    BOOL NextByte();
    BOOL PrevByte();

    void SetSteps(int x, int y=0) {  Stepx = x; Stepy = y; }
    void GetSteps(int *x, int *y) {  *x = Stepx; *y = Stepy; }
    BOOL NextStep();
    BOOL PrevStep();

    void SetY(int y);   /* AD - for interlace */
    int  GetY() {return Ity;}
    BOOL GetCol(BYTE* pCol, DWORD x);
    BOOL SetCol(BYTE* pCol, DWORD x);
};

class CxImage{
public:
    WEAK_REF_ID_DEFINE();
public:
    void*               pDib; //contains the header, the palette, the pixels
    BITMAPINFOHEADER    head; //standard header
    CxImageInfo         info; //extended information
    BYTE*               pSelection; //selected region
    BYTE*               pAlpha; //alpha channel
    CxImage**           ppLayers; //generic layers
    CxImage**           ppFrames;
public:

#if CXIMAGE_SUPPORT_WINDOWS
    long Blt(HDC pDC, long x, long y);
    HANDLE CopyToHandle();
    bool CreateFromHANDLE(HANDLE hMem);
    HBITMAP MakeBitmap(HDC hdc);
    bool CreateFromHdc(HDC hdc);
    bool CreateFromHBITMAP(HBITMAP hbmp, HPALETTE hpal);
    BOOL CreateFromHICON(HICON hico);
    long Draw(HDC hdc, const RECT& rect, RECT* pClipRect, bool bSmooth);
    long Draw(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth);
    long Draw2(HDC hdc, const RECT& rect);
    long Draw2(HDC hdc, long x, long y, long cx, long cy);
    long Stretch(HDC hdc, const RECT& rect, DWORD dwRop);
    long Stretch(HDC hdc, long xoffset, long yoffset, long xsize, long ysize, DWORD dwRop);
    long Tile(HDC hdc, RECT *rc);
    long DrawString(HDC hdc, long x, long y, const char* text, RGBQUAD color, const char* font, long lSize, long lWeight, BYTE bItalic, BYTE bUnderline, BOOL bSetAlpha);
    long DrawStringEx(HDC hdc, long x, long y, CXTEXTINFO *pTextType, bool bSetAlpha );
    void InitTextInfo( CXTEXTINFO *txt );
    long LayerDrawAll(HDC hdc, const RECT& rect, RECT* pClipRect, bool bSmooth);
    long LayerDrawAll(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth);
#endif
    int FillRect(RECT r,RGBQUAD color,BOOL setAlpha);
    int DrawRect(RECT r,RGBQUAD color,BOOL setAlpha);
    static int GetTypeByFileHeader(CFileBase *file);
    BOOL SetCodecOption(DWORD opt, DWORD imagetype);
    int DestroyAll();
    void SetTransColor(RGBQUAD rgb);
    void * GetDIB();
    DWORD GetCodecOption(DWORD imagetype);
    long GetYDPI();
    long GetXDPI();
    BOOL EncodeSafeCheck(CFileBase *hFile);
    BOOL Threshold(BYTE level);
    BOOL Threshold(CxImage* pThresholdMask);
    BOOL Threshold2(BYTE level, BOOL bDirection, RGBQUAD nBkgndColor, BOOL bSetAlpha);
    BOOL SplitRGB(CxImage* r,CxImage* g,CxImage* b);
    BOOL SplitCMYK(CxImage* c,CxImage* m,CxImage* y,CxImage* k);
    BOOL SplitYUV(CxImage* y,CxImage* u,CxImage* v);
    BOOL SplitYIQ(CxImage* y,CxImage* i,CxImage* q);
    BOOL SplitXYZ(CxImage* x,CxImage* y,CxImage* z);
    BOOL SplitHSL(CxImage* h,CxImage* s,CxImage* l);
    static RGBQUAD RGBtoHSL(RGBQUAD lRGBColor);
    static float HueToRGB(float n1,float n2, float hue);
    static RGBQUAD HSLtoRGB(COLORREF cHSLColor);
    static RGBQUAD HSLtoRGB(RGBQUAD lHSLColor);
    static RGBQUAD YUVtoRGB(RGBQUAD lYUVColor);
    static RGBQUAD RGBtoYUV(RGBQUAD lRGBColor);
    static RGBQUAD YIQtoRGB(RGBQUAD lYIQColor);
    static RGBQUAD RGBtoYIQ(RGBQUAD lRGBColor);
    static RGBQUAD XYZtoRGB(RGBQUAD lXYZColor);
    static RGBQUAD RGBtoXYZ(RGBQUAD lRGBColor);
    void HuePalette(float correction);
    BOOL Colorize(BYTE hue, BYTE sat, float blend);
    BOOL Light(long brightness, long contrast);
    float Mean();
    BOOL Filter(long* kernel, long Ksize, long Kfactor, long Koffset);
    BOOL Erode(long Ksize);
    BOOL Dilate(long Ksize);
    BOOL Edge(long Ksize);
    void Mix(CxImage & imgsrc2, ImageOpType op, long lXOffset, long lYOffset, BOOL bMixAlpha);
    void MixFrom(CxImage & imagesrc2, long lXOffset, long lYOffset);
    BOOL ShiftRGB(long r, long g, long b);
    BOOL Gamma(float gamma);
    BOOL GammaRGB(float gammaR, float gammaG, float gammaB);
    BOOL Median(long Ksize);
    BOOL Noise(long level);
    BOOL FFT2(CxImage* srcReal, CxImage* srcImag, CxImage* dstReal, CxImage* dstImag,
        long direction, BOOL bForceFFT, BOOL bMagnitude);
    BOOL IsPowerof2(long x);
    BOOL FFT(int dir,int m,double *x,double *y);
    BOOL DFT(int dir,long m,double *x1,double *y1,double *x2,double *y2);
    BOOL Combine(CxImage* r,CxImage* g,CxImage* b,CxImage* a, long colorspace);
    BOOL Repair(float radius, long niterations, long colorspace);
    BOOL RepairChannel(CxImage *ch, float radius);
    BOOL Contour();
    BOOL Jitter(long radius);
    int gen_convolve_matrix (float radius, float **cmatrix_p);
    float* gen_lookup_table (float *cmatrix, int cmatrix_length);
    void blur_line (float *ctable, float *cmatrix, int cmatrix_length, BYTE* cur_col, BYTE* dest_col, int y, long bytes);
    void blur_text (BYTE threshold, BYTE decay, BYTE max_depth, CxImage* iSrc, CxImage* iDst, BYTE bytes);
    BOOL TextBlur(BYTE threshold, BYTE decay, BYTE max_depth, BOOL bBlurHorizontal, BOOL bBlurVertical, CxImage* iDst);
    BOOL GaussianBlur(float radius , CxImage* iDst );
    BOOL SelectiveBlur(float radius, BYTE threshold, CxImage* iDst);
    BOOL UnsharpMask(float radius , float amount , int threshold );
    BOOL Lut(BYTE* pLut);
    BOOL Lut(BYTE* pLutR, BYTE* pLutG, BYTE* pLutB, BYTE* pLutA);
    BOOL RedEyeRemove(float strength);
    BOOL Saturate(const long saturation, const long colorspace);
    BOOL Solarize(BYTE level, BOOL bLinkedChannels);
    BOOL ConvertColorSpace(const long dstColorSpace, const long srcColorSpace);
    int  OptimalThreshold(long method, RECT * pBox, CxImage* pContrastMask);
    BOOL AdaptiveThreshold(long method, long nBoxSize, CxImage* pContrastMask, long nBias, float fGlobalLocalBalance);

    BOOL QIShrink(long newx, long newy, CxImage* const iDst, BOOL bChangeBpp);
    BOOL CircleTransform(int type,long rmax,float Koeff);
    BOOL Thumbnail(long newx, long newy, RGBQUAD canvascolor, CxImage* iDst);
    BOOL Expand(long newx, long newy, RGBQUAD canvascolor, CxImage* iDst);
    BOOL Expand(long left, long top, long right, long bottom, RGBQUAD canvascolor, CxImage* iDst);
    BOOL Skew(float xgain, float ygain, long xpivot, long ypivot, BOOL bEnableInterpolation);
    BOOL Crop(long left, long top, long right, long bottom, CxImage* iDst);
    BOOL Crop(const RECT& rect, CxImage* iDst);
    BOOL CropRotatedRectangle( long topx, long topy, long width, long height, float angle, CxImage* iDst);
    BOOL Dither(long method);
    BOOL IncreaseBpp(DWORD nbit);
    BOOL DecreaseBpp(DWORD nbit, BOOL errordiffusion, RGBQUAD* ppal, DWORD clrimportant);
    BOOL Resample2(long newx, long newy, int inMethod, int ofMethod, CxImage* iDst, BOOL disableAveraging);
    BOOL Resample(long newx, long newy, int mode, CxImage* iDst);
    BOOL Rotate180(CxImage* iDst);

    RGBQUAD GetPixelColorInterpolated(
        float x,float y, 
        int const inMethod, 
        int const ofMethod, 
        RGBQUAD* const rplColor);
    
    RGBQUAD GetAreaColorInterpolated(
        float const xc, float const yc, float const w, float const h, 
        int const inMethod, 
        int const ofMethod, 
        RGBQUAD* const rplColor);

    void AddAveragingCont(RGBQUAD const &color, float const surf, float &rr, float &gg, float &bb, float &aa);
    RGBQUAD GetPixelColorWithOverflow(long x, long y, int const ofMethod, RGBQUAD* const rplColor);
    void OverflowCoordinates(long &x, long &y, int const ofMethod);
    void OverflowCoordinates(float &x, float &y, int const ofMethod);
    float KernelBSpline(const float x);
    float KernelLinear(const float t);
    float KernelCubic(const float t);
    float KernelGeneralizedCubic(const float t, const float a);
    float KernelLanczosSinc(const float t, const float r);
    float KernelBox(const float x);
    float KernelHermite(const float x);
    float KernelHanning(const float x);
    float KernelHamming(const float x);
    float KernelSinc(const float x);
    float KernelBlackman(const float x);
    float KernelBessel_J1(const float x);
    float KernelBessel_P1(const float x);
    float KernelBessel_Q1(const float x);
    float KernelBessel_Order1(float x);
    float KernelBessel(const float x);
    float KernelGaussian(const float x);
    float KernelQuadratic(const float x);
    float KernelMitchell(const float x);
    float KernelCatrom(const float x);
    float KernelPower(const float x, const float a);

    BOOL Rotate2(float angle, CxImage *iDst, int inMethod, int ofMethod, RGBQUAD *replColor, BOOL const optimizeRightAngles,BOOL const bKeepOriginalSize);
    BOOL Rotate(float angle, CxImage* iDst);
    BOOL Negative();
    BOOL RotateRight(CxImage* iDst);
    BOOL RotateLeft(CxImage* iDst);
    BOOL Mirror(BOOL bMirrorSelection, BOOL bMirrorAlpha);
    BOOL Flip(BOOL bFlipSelection, BOOL bFlipAlpha);
    DWORD UnDump(const BYTE * src);
    DWORD Dump(BYTE * dst);
    void LayerDeleteAll();
    BOOL LayerDelete(long position);
    BOOL LayerCreate(long position);
    CxImage * GetParent();
    CxImage * GetLayer(long position);
    long GetNumLayers();
    DWORD DumpSize();
    void bihtoh(BITMAPINFOHEADER* bih);
    long ntohl(const long dword);
    short ntohs(const short word);
    CxImage * GetFrame(long nFrame);
    void SetRetreiveAllFrames(BOOL flag);
    BOOL GetRetreiveAllFrames();
    BYTE GetDisposalMethod();
    void SetDisposalMethod(BYTE dm);
    void SetFrame(long nFrame);
    long GetFrame();
    long GetNumFrames();
    void SetEscape(long i);
    void SetProgress(long p);
    long GetEscape();
    long GetProgress();
    void SetJpegScale(BYTE q);
    BYTE GetJpegScale();
    void SetJpegQualityF(float q);
    float GetJpegQualityF();
    BYTE GetJpegQuality();
    void SetOffset(long x,long y);
    void GetOffset(long *x,long *y);
    void SetFrameDelay(DWORD d);
    RGBQUAD GetTransColor();
    long GetTransIndex();
    void SetTransIndex(long idx);
    DWORD GetFrameDelay();
    DWORD GetTypeIndexFromId(const DWORD id);
    DWORD GetTypeIdFromIndex(const DWORD index);
    DWORD GetTypeIdFromName(const char* ext);
    DWORD GetNumTypes();
    BOOL SetType(DWORD type);
    DWORD GetNumColors();
    DWORD GetEffWidth();
    BOOL GrayScale();
    BOOL SelectionToHRGN(HRGN& region);
    BOOL SelectionMirror();
    BOOL SelectionFlip();
    BYTE * SelectionGetPointer(const long x,const long y);
    BYTE BlindSelectionGet(const long x,const long y);
    BYTE SelectionGet(const long x,const long y);
    void SelectionSet(const long x,const long y,const BYTE level);
    BOOL SelectionSet(CxImage *from);
    BOOL SelectionSplit(CxImage *dest);
    BOOL SelectionAddPixel(long x, long y, BYTE level);
    BOOL SelectionAddColor(RGBQUAD c, BYTE level);
    BOOL SelectionAddPolygon(POINT *points, long npoints, BYTE level);
    BOOL SelectionCopy(CxImage *from);
    void SelectionRebuildBox();
    BOOL SelectionInvert();
    BOOL SelectionAddEllipse(RECT r, BYTE level);
    BOOL SelectionAddRect(RECT r, BYTE level);
    BOOL BlindSelectionIsInside(long x, long y);
    BOOL SelectionIsInside(long x, long y);
    BOOL SelectionCreate();
    BOOL SelectionClear(BYTE level);
    BOOL SelectionGetBox(RECT *r);
    BOOL SelectionIsValid();
    BOOL DrawLine(int StartX, int EndX, int StartY, int EndY, RGBQUAD color, BOOL bSetAlpha);
    void DrawLine(int StartX, int EndX, int StartY, int EndY, COLORREF cr);
    BOOL IsIndexed();
    void * BlindGetPixelPointer(const long x, const long y);
    void SetClrImportant(DWORD ncolors);
    DWORD GetClrImportant();
    BOOL IsSamePalette(CxImage &img, BOOL bCheckAlpha);
    void SetStdPalette();
    BOOL GetTransparentMask(CxImage* iDst);
    void SwapRGB2BGR();
    void BlendPalette(COLORREF cr,long perc);
    void SetPalette(rgb_color *rgb,DWORD nColors);
    void SetPalette(DWORD n, BYTE *r, BYTE *g, BYTE *b);
    BOOL GetPaletteColor(BYTE i, BYTE* r, BYTE* g, BYTE* b);
    static COLORREF RGBQUADtoRGB (RGBQUAD c);
    void RGBtoBGR(BYTE *buffer, int length);
    void BlendPixelColor(long x,long y,RGBQUAD c, float blend, BOOL bSetAlpha);
    void SetPixelColor(long x,long y,RGBQUAD c, BOOL bSetAlpha);
    static RGBQUAD RGBtoRGBQUAD(COLORREF cr);
    void SetPixelColor(long x,long y,COLORREF cr);
    void SetPixelIndex(long x,long y,BYTE i);
    BYTE GetPixelGray(long x, long y);
    void SetPaletteColor(BYTE idx, BYTE r, BYTE g, BYTE b, BYTE alpha);
    BYTE GetPixelIndex(long x,long y);
    RGBQUAD GetPixelColor(long x,long y, BOOL bGetAlpha);
    BOOL IsTransparent(long x, long y);
    BOOL IsTransparent();
    BOOL AlphaFromTransparency();
    BOOL AlphaPaletteSplit(CxImage *dest);
    BOOL AlphaSplit(CxImage *dest);
    BOOL AlphaMirror();
    BOOL AlphaFlip();
    BYTE GetNearestIndex(RGBQUAD c);
    const char * GetLastError();
    void Enable(BOOL enable);
    BOOL IsEnabled();
    BOOL IsValid();
    void BlindSetPixelColor(long x,long y,RGBQUAD c, BOOL bSetAlpha);
    RGBQUAD BlindGetPixelColor(const long x,const long y, BOOL bGetAlpha);
    void AlphaStrip();
    BOOL AlphaPaletteIsValid();
    BYTE BlindGetPixelIndex(const long x,const long y);
    void BlindSetPixelIndex(long x,long y,BYTE i);
    void SwapIndex(BYTE idx1, BYTE idx2);
    BOOL IsGrayScale();
    BOOL AlphaSet(CxImage *from);
    BOOL AlphaCopy(CxImage *from);
    void AlphaInvert();
    void AlphaClear();
    BOOL AlphaPaletteIsEnabled();
    void AlphaPaletteEnable(BOOL enable);
    BOOL AlphaIsValid();
    int AlphaSetMax(BYTE nAlphaMax);
    BYTE AlphaGetMax();
    int SetPaletteColor(BYTE idx, RGBQUAD c);
    int SetPaletteColor(BYTE idx, COLORREF cr);
    RGBQUAD GetPaletteColor(BYTE idx);
    int AlphaPaletteClear();
    BYTE BlindAlphaGet(const long x,const long y);
    BYTE * AlphaGetPointer(const long x,const long y);
    BYTE AlphaGet(const long x,const long y);
    int AlphaSet(const long x,const long y,const BYTE level);
    int AlphaSet(BYTE level);
    int AlphaCreate();
    int SetGrayPalette();
    int CreateFromArray(BYTE* pArray,DWORD dwWidth,DWORD dwHeight,DWORD dwBitsperpixel, DWORD dwBytesperline, BOOL bFlipImage);
    int Bitfield2RGB(BYTE *src, DWORD redmask, DWORD greenmask, DWORD bluemask, BYTE bpp);
    static int CompareColors(const void *elem1,  const void *elem2);
    int DestroyFrames();
    int Transfer(CxImage *from, BOOL bTransferFrames);
    int Clear(BYTE bval);
    BOOL IsInside(long x, long y);
    int AlphaDelete();
    int SelectionDelete();
    BYTE * GetBits(DWORD row);
    DWORD GetPaletteSize();
    RGBQUAD* GetPalette();
    int SetPalette(RGBQUAD* pPal,DWORD nColors);
    long GetSize();
    DWORD GetType();
    WORD GetBpp();
    long GetHeight();
    long GetWidth();
    void* Create(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype);
    int Ghost(CxImage *from);
    int Copy_No_Rebuild(CxImage *src);
    int Copy(CxImage *src,BOOL copypixels, BOOL copyselection, BOOL copyalpha);
    int CopyInfo(CxImage *src);
    int SetYDPI(long dpi);
    int SetXDPI(long dpi);
    int StartUp(DWORD imagetype);
    CxImage(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype);
    CxImage(CxImage *src, BOOL copypixels, BOOL copyselection, BOOL copyalpha);
    CxImage();
    virtual ~CxImage();
    int Init();
    int Destroy();
    int Copy(CxImage *p);
    int Comp(CxImage *p);
    int Print();
    int InitBasic();
private:
    CxImage(const CxImage&);
    CxImage& operator=(const CxImage&);
};

#endif // !defined(AFX_XIMAGE_H__3AD1B86C_9635_407D_B57D_639B2D7EC4F3__INCLUDED_)
