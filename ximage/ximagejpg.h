// xImageJPG.h: interface for the CxImageJPG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIMAGEJPG_H__AA53B403_2DE7_445C_828C_549591F70FF2__INCLUDED_)
#define AFX_XIMAGEJPG_H__AA53B403_2DE7_445C_828C_549591F70FF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"
#include "ximage.h"

#define CXIMAGEJPG_SUPPORT_EXIF 1

extern "C" 
{
 #include "jpeg_lib.h"
 #include "jpeg_error.h"
}

#define MAX_COMMENT 1000
#define MAX_SECTIONS 20
#define M_SOF0  0xC0            // Start Of Frame N
#define M_SOF1  0xC1            // N indicates which compression process
#define M_SOF2  0xC2            // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5            // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            // Start Of Image (beginning of datastream)
#define M_EOI   0xD9            // End Of Image (end of datastream)
#define M_SOS   0xDA            // Start Of Scan (begins compressed data)
#define M_JFIF  0xE0            // Jfif marker
#define M_EXIF  0xE1            // Exif marker
#define M_COM   0xFE            // COMment 

#define PSEUDO_IMAGE_MARKER 0x123; // Extra value.

#define EXIF_READ_EXIF  0x01
#define EXIF_READ_IMAGE 0x02
#define EXIF_READ_ALL   0x03

enum CODEC_OPTION
{
    ENCODE_BASELINE = 0x1,
    ENCODE_ARITHMETIC = 0x2,
    ENCODE_GRAYSCALE = 0x4,
    ENCODE_OPTIMIZE = 0x8,
    ENCODE_PROGRESSIVE = 0x10,
    ENCODE_LOSSLESS = 0x20,
    ENCODE_SMOOTHING = 0x40,
    DECODE_GRAYSCALE = 0x80,
    DECODE_QUANTIZE = 0x100,
    DECODE_DITHER = 0x200,
    DECODE_ONEPASS = 0x400,
    DECODE_NOSMOOTH = 0x800,
    ENCODE_SUBSAMPLE_422 = 0x1000,
    ENCODE_SUBSAMPLE_444 = 0x2000
}; 

typedef struct tag_Section_t{
    BYTE*    Data;
    int      Type;
    unsigned Size;
} Section_t;

typedef struct tag_ExifInfo {
    char  Version      [5];
    char  CameraMake   [32];
    char  CameraModel  [40];
    char  DateTime     [20];
    int   Height, Width;
    int   Orientation;
    int   IsColor;
    int   Process;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    float CCDWidth;
    float ExposureBias;
    int   Whitebalance;
    int   MeteringMode;
    int   ExposureProgram;
    int   ISOequivalent;
    int   CompressionLevel;
    float FocalplaneXRes;
    float FocalplaneYRes;
    float FocalplaneUnits;
    float Xresolution;
    float Yresolution;
    float ResolutionUnit;
    float Brightness;
    char  Comments[MAX_COMMENT];

    unsigned char * ThumbnailPointer;  /* Pointer at the thumbnail */
    unsigned ThumbnailSize;     /* Size of thumbnail. */

    BOOL  IsExif;
} EXIFINFO;

class CxExifInfo
{
public:
    EXIFINFO* m_exifinfo;
    char m_szLastError[256];
public:
    int Destroy();
    int Init(EXIFINFO* info);
    CxExifInfo(EXIFINFO* info = NULL);
    ~CxExifInfo();
    BOOL DecodeExif(CFileBase * hFile, int nReadMode = EXIF_READ_EXIF);
    BOOL EncodeExif(CFileBase * hFile);
    void DiscardAllButExif();

    BOOL process_EXIF(unsigned char * CharBuf, unsigned int length);
    void process_COM (const BYTE * Data, int length);
    void process_SOFn (const BYTE * Data, int marker);
    int Get16u(void * Short);
    int Get16m(void * Short);
    long Get32s(void * Long);
    unsigned long Get32u(void * Long);
    double ConvertAnyFormat(void * ValuePtr, int Format);
    void* FindSection(int SectionType);
    BOOL ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength,
                           EXIFINFO * const pInfo, unsigned char ** const LastExifRefdP, int NestingLevel=0);
    int ExifImageWidth;
    int MotorolaOrder;
    Section_t Sections[MAX_SECTIONS];
    int SectionsRead;
    BOOL freeinfo;
};

class CxFileJpg : public jpeg_destination_mgr, public jpeg_source_mgr
{
public:
    CFileBase  *m_pFile;
    unsigned char *m_pBuffer;
    BOOL m_bStartOfFile;
public:
    CxFileJpg(CFileBase* pFile);
    ~CxFileJpg();
    int Init();
    int Destroy();
    static void InitDestination(j_compress_ptr cinfo);
    static boolean EmptyOutputBuffer(j_compress_ptr cinfo);
    static void TermDestination(j_compress_ptr cinfo);
    static void InitSource(j_decompress_ptr cinfo);
    static boolean FillInputBuffer(j_decompress_ptr cinfo);
    static void SkipInputData(j_decompress_ptr cinfo, long num_bytes);
    static void TermSource(j_decompress_ptr /*cinfo*/);

};

class CxImageJPG: public CxImage
{
public:
    int m_nPredictor;
    int m_nPointTransform;
    int m_nSmoothing;
    int m_nQuantize;
    J_DITHER_MODE m_nDither;
    CxExifInfo* m_exif;
    EXIFINFO m_exifinfo;
public:
    int Destroy();
    CxImageJPG();
    ~CxImageJPG();
    BOOL Decode(CFileBase * hFile);
    BOOL Encode(CFileBase * hFile);
    BOOL DecodeExif(CFileBase * hFile);
};

#endif // !defined(AFX_XIMAGEJPG_H__AA53B403_2DE7_445C_828C_549591F70FF2__INCLUDED_)
