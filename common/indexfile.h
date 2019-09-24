// IndexFile.h: interface for the CIndexFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INDEXFILE_H__4B690A25_0206_46A0_B56D_0F08313E2822__INCLUDED_)
#define AFX_INDEXFILE_H__4B690A25_0206_46A0_B56D_0F08313E2822__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"
#include "memfile.h"
#include "partfile.h"

class CIndexFile{
public:
    int64_t mHeaderSize;
    int64_t mCurOffset;
    CMemFile *mIndexFile;
    CFileBase *iDstFile;
    int64_t mMaxBlocks;
public:
    static bool IsIndexFile(CFileBase *file);
    status_t GhostBlock(int64_t index, CPartFile* file);
    int64_t GetBlockCount();
    status_t GetBlock(int64_t index,CFileBase *file_dst);
    int64_t GetBlockSize(int64_t index);
    int64_t GetDataSize();
    int64_t GetBlockOffset(int64_t index);
    status_t LoadIndexFile(CFileBase *i_file);
    status_t Save();
    status_t AddBlock(CFileBase *file);
    status_t SetDstFile(CFileBase *i_file);
    CIndexFile();
    virtual ~CIndexFile();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();   
    int64_t GetCurOffset(void);
};

#endif // !defined(AFX_INDEXFILE_H__4B690A25_0206_46A0_B56D_0F08313E2822__INCLUDED_)
