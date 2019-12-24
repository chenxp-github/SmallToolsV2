#ifndef __QUEUE_FILE_H
#define __QUEUE_FILE_H

#include "mem.h"
#include "closure.h"

#define QUEUE_FILE_CONTEXT(context) int_ptr_t context[16]

class CQueueFile:public CFileBase{
private:
    int_ptr_t head,tail;
    int_ptr_t max_size,size_shift,size_mod;
    char *pdata;
    bool is_full;
public:
    int_ptr_t FindTag(CClosure *match);
    int_ptr_t FindTag(const void *tag, int size);
    status_t RestoreContext(void *buf);
    status_t SaveContext(void *buf);
    int_ptr_t PeekData(void *data, int_ptr_t size);
    int Clear();
    CQueueFile();
    ~CQueueFile();
    fsize_t GetMaxSize();
    fsize_t GetOffset();
    fsize_t GetSize();
    int_ptr_t Read(void *buf,int_ptr_t n);
    int_ptr_t Write(const void *buf,int_ptr_t n);
    status_t AddBlock(fsize_t bsize);
    status_t Destroy();
    status_t Dump();
    fsize_t GetFreeSize();
    bool HasEmptyLine();
    bool HasWholeLine();
    status_t Init(int_ptr_t bytes);
    status_t InitBasic();
    fsize_t Seek(fsize_t off);
    status_t SetSize(fsize_t ssize);
};

#endif
