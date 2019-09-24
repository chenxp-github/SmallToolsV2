// xImageICO.h: interface for the CxImageICO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIMAGEICO_H__3C7E0DE3_03ED_48A4_809F_AD7DCCE28E8B__INCLUDED_)
#define AFX_XIMAGEICO_H__3C7E0DE3_03ED_48A4_809F_AD7DCCE28E8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"
#include "ximage.h"


class CxImageICO : public CxImage
{
public:
    DWORD m_dwImageOffset;
public:
    BOOL Encode(CFileBase * hFile, CxImage ** pImages, int nPageCount);
    BOOL Encode(CFileBase * hFile, BOOL bAppend, int nPageCount);
    BOOL Decode(CFileBase *hFile);
    CxImageICO();
};

#endif // !defined(AFX_XIMAGEICO_H__3C7E0DE3_03ED_48A4_809F_AD7DCCE28E8B__INCLUDED_)
