// xImagePNG.h: interface for the CxImagePNG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIMAGEPNG_H__2F65578D_BE86_4ABC_85DD_03F3618628BE__INCLUDED_)
#define AFX_XIMAGEPNG_H__2F65578D_BE86_4ABC_85DD_03F3618628BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ximage.h"

extern "C" 
{
  #include "png_lib.h"
}

class CxImagePNG : public CxImage
{
public:
    BOOL Encode(CFileBase *hFile);
    static void PNGAPI user_error_fn(png_structp png_ptr,png_const_charp error_msg);
    static void PNGAPI user_flush_data(png_structp png_ptr);
    static void PNGAPI user_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
    static void PNGAPI user_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
    void expand2to4bpp(BYTE* prow);
    BOOL DecodeSizeOnly(CFileBase * hFile);
    BOOL Decode(CFileBase * hFile);
    CxImagePNG();
};

#endif // !defined(AFX_XIMAGEPNG_H__2F65578D_BE86_4ABC_85DD_03F3618628BE__INCLUDED_)
