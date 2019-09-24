#ifndef __CMEMFILE_H
#define __CMEMFILE_H

#include "cruntime.h"
#include "filebase.h"

class CMemFile:public CFileBase{    
public:
    int_ptr_t mSize;
    int_ptr_t mPages;
    int_ptr_t mOffset;
    int_ptr_t mMaxPages;  
    char **mBase;
    int_ptr_t mPageSize;
    uint32_t mod_n;
    uint32_t shift_n;
public:
    status_t Transfer(CMemFile *from);
    CMemFile();
    ~CMemFile();
    status_t InitBasic();
    status_t Init();
    status_t Init(int_ptr_t page_size,int_ptr_t max_pages);
    status_t InitShift();
    status_t Destroy();
    status_t AddPage();
    status_t AddBlock(fsize_t block_size);
    int_ptr_t Read(void *buf,int_ptr_t rsize);
    int_ptr_t Write(const void *buf,int_ptr_t wsize);
    status_t Print();
    fsize_t Seek(fsize_t off);
    status_t SetSize(fsize_t s);
    fsize_t GetOffset();
    fsize_t GetSize();
    fsize_t GetMaxSize();
    virtual char CharAt(fsize_t index);
    virtual status_t SetChar(fsize_t index,char ch);
    virtual char Getc();
    virtual char UnGetc();
    virtual fsize_t Putc(char ch);

};

#endif
