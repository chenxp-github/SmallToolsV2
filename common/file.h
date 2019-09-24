#ifndef __CFILE_H
#define __CFILE_H

#include "cruntime.h"
#include "filebase.h"

//don't use buffer when open a file for write
class CFile:public CFileBase{
public:
    FILE_HANDLE fp;
    char *file_buf;
    int_ptr_t buf_size,init_buf_size;
    fsize_t offset,size,buf_start;
public:
    bool IsOpened();
    CFile();
    ~CFile();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t OpenFile(const char *fn , const char *mode);
    status_t CloseFile();
    fsize_t GetSize();
    fsize_t GetSizeReal();
    fsize_t Seek(fsize_t nOffset);
    int_ptr_t Read(void *buf,int_ptr_t nByteToRead);
    int_ptr_t Write(const void *buf,int_ptr_t nByteToWrite);
    status_t SetSize(fsize_t nSize);
    static status_t FindFile(const char *fn);
    fsize_t GetOffset();
    status_t AddBlock(fsize_t bsize);
    fsize_t GetMaxSize();
    status_t HasBuffer();
    status_t SetBufSize(int_ptr_t size);
    status_t BufferBlock(fsize_t start_pos);
};

#endif
