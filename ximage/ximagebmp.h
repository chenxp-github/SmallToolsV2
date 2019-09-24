// xImageBMP.h: interface for the CxImageBMP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIMAGEBMP_H__9BEC9053_604F_42B9_908E_F33F3F3BE243__INCLUDED_)
#define AFX_XIMAGEBMP_H__9BEC9053_604F_42B9_908E_F33F3F3BE243__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ximage.h"
#include "filebase.h"

#if CXIMAGE_SUPPORT_BMP

class CxImageBMP: public CxImage
{
public:
    CxImageBMP();
    ~CxImageBMP();
    BOOL Decode(CFileBase * hFile);
    BOOL Encode(CFileBase * hFile);
    BOOL DibReadBitmapInfo(CFileBase* fh, BITMAPINFOHEADER *pdib);
};

#endif //CXIMAGE_SUPPORT_BMP
#endif // !defined(AFX_XIMAGEBMP_H__9BEC9053_604F_42B9_908E_F33F3F3BE243__INCLUDED_)
