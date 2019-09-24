#ifndef __WINTYPES_H
#define __WINTYPES_H
#include "config.h"

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L
#define ANSI_CHARSET            0
#define DEFAULT_CHARSET         1
#define SYMBOL_CHARSET          2
#define SHIFTJIS_CHARSET        128
#define HANGEUL_CHARSET         129
#define HANGUL_CHARSET          129
#define GB2312_CHARSET          134
#define CHINESEBIG5_CHARSET     136
#define OEM_CHARSET             255
#define OUT_DEFAULT_PRECIS          0
#define OUT_STRING_PRECIS           1
#define OUT_CHARACTER_PRECIS        2
#define OUT_STROKE_PRECIS           3
#define OUT_TT_PRECIS               4
#define OUT_DEVICE_PRECIS           5
#define OUT_RASTER_PRECIS           6
#define OUT_TT_ONLY_PRECIS          7
#define OUT_OUTLINE_PRECIS          8
#define OUT_SCREEN_OUTLINE_PRECIS   9
#define OUT_PS_ONLY_PRECIS          10

#define CLIP_DEFAULT_PRECIS     0
#define CLIP_CHARACTER_PRECIS   1
#define CLIP_STROKE_PRECIS      2
#define CLIP_MASK               0xf
#define CLIP_LH_ANGLES          (1<<4)
#define CLIP_TT_ALWAYS          (2<<4)
#define DEFAULT_QUALITY         0
#define DRAFT_QUALITY           1
#define PROOF_QUALITY           2
#define DEFAULT_PITCH           0
#define FIXED_PITCH             1
#define VARIABLE_PITCH          2
#define FF_DONTCARE         (0<<4)  /* Don't care or don't know. */
#define FF_ROMAN            (1<<4)  /* Variable stroke width, serifed. */
                                    /* Times Roman, Century Schoolbook, etc. */
#define FF_SWISS            (2<<4)  /* Variable stroke width, sans-serifed. */
                                    /* Helvetica, Swiss, etc. */
#define FF_MODERN           (3<<4)  /* Constant stroke width, serifed or sans-serifed. */
                                    /* Pica, Elite, Courier, etc. */
#define FF_SCRIPT           (4<<4)  /* Cursive, etc. */
#define FF_DECORATIVE       (5<<4)  /* Old English, etc. */
#define TA_NOUPDATECP                0
#define TA_UPDATECP                  1

#define TA_LEFT                      0
#define TA_RIGHT                     2
#define TA_CENTER                    6

#define TA_TOP                       0
#define TA_BOTTOM                    8
#define TA_BASELINE                  24
#define DT_CENTER       TA_CENTER
#define DT_LEFT         TA_LEFT
#define DT_RIGHT        TA_RIGHT
typedef void*   HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name;
typedef uint32_t DWORD_PTR;
typedef uint32_t ULONG;
typedef ULONG *PULONG;
typedef uint16_t USHORT;
typedef USHORT *PUSHORT;
typedef uint8_t UCHAR;
typedef UCHAR *PUCHAR;
typedef int8_t *PSZ;
typedef uint32_t            DWORD;
typedef int                 BOOL;
typedef uint8_t             BYTE;
typedef uint16_t            WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL                *PBOOL;
typedef BOOL                *LPBOOL;
typedef BYTE                *PBYTE;
typedef BYTE                *LPBYTE;
typedef int                 *PINT;
typedef int                 *LPINT;
typedef WORD                *PWORD;
typedef WORD                *LPWORD;
typedef int32_t             *LPLONG;
typedef DWORD               *PDWORD;
typedef DWORD               *LPDWORD;
typedef void                *LPVOID;
typedef const void          *LPCVOID;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
typedef int32_t             LONG;
typedef char                CHAR;
typedef struct
{
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
typedef struct
{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
}__attribute__((packed)) BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct
{
        DWORD   bcSize;                 /* used to get to color table */
        WORD    bcWidth;
        WORD    bcHeight;
        WORD    bcPlanes;
        WORD    bcBitCount;
}__attribute__((packed)) BITMAPCOREHEADER, *LPBITMAPCOREHEADER, *PBITMAPCOREHEADER;
typedef struct
{
        BYTE    rgbtBlue;
        BYTE    rgbtGreen;
        BYTE    rgbtRed;
}__attribute__((packed)) RGBTRIPLE, *PRGBTRIPLE, *NPRGBTRIPLE, *LPRGBTRIPLE;
typedef struct
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
}__attribute__((packed)) RECT, *PRECT, *NPRECT, *LPRECT;

typedef struct _RGBQUAD
{
    BYTE rgbRed, rgbGreen, rgbBlue, rgbReserved;
}__attribute__((packed)) RGBQUAD;

typedef DWORD COLORREF;

typedef struct
{
    int x, y;
}__attribute__((packed)) POINT;
#define LF_FACESIZE         (32)
typedef struct tagLOGFONTA
{
    LONG      lfHeight;
    LONG      lfWidth;
    LONG      lfEscapement;
    LONG      lfOrientation;
    LONG      lfWeight;
    BYTE      lfItalic;
    BYTE      lfUnderline;
    BYTE      lfStrikeOut;
    BYTE      lfCharSet;
    BYTE      lfOutPrecision;
    BYTE      lfClipPrecision;
    BYTE      lfQuality;
    BYTE      lfPitchAndFamily;
    CHAR      lfFaceName[LF_FACESIZE];
}__attribute__((packed)) LOGFONTA, *PLOGFONTA, *NPLOGFONTA, *LPLOGFONTA;
DECLARE_HANDLE(HRGN);
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
}__attribute__((packed)) CXTEXTINFO;
typedef struct
{
    double x, y;
}__attribute__((packed)) _complex;
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#endif
