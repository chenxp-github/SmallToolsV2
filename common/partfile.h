// PartFile.h: interface for the CPartFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTFILE_H__4C18A0DD_E8B2_4491_962E_17E001A18EBC__INCLUDED_)
#define AFX_PARTFILE_H__4C18A0DD_E8B2_4491_962E_17E001A18EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"

class CPartFile:public CFileBase{
public:
    CFileBase *i_host_file;
    fsize_t offset,size;
    fsize_t host_off;
    fsize_t max_size;
public:
    status_t Copy(CPartFile *from);
    fsize_t GetHostOffset();
    int SetHostFile(CFileBase *host,fsize_t off,fsize_t size);
    fsize_t Seek(fsize_t off);
    status_t AddBlock(fsize_t bsize);
    fsize_t GetMaxSize();
    status_t SetSize(fsize_t s);
    int_ptr_t Write(const void *buf,int_ptr_t n);
    int_ptr_t Read(void *buf,int_ptr_t n);
    fsize_t GetSize();
    fsize_t GetOffset();
    CPartFile();
    virtual ~CPartFile();
    int Init();
    int Destroy();
    int InitBasic();
};

#endif // !defined(AFX_PARTFILE_H__4C18A0DD_E8B2_4491_962E_17E001A18EBC__INCLUDED_)
