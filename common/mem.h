#ifndef __CMEM_H
#define __CMEM_H

#include "filebase.h"

#define LOCAL_MEM_WITH_SIZE(name,size) char __##name[size];\
CMem name;\
name.Init();\
name.SetRawBuf(__##name,size,false);\
name.SetSize(0)\

#define AUTO_LOCAL_MEM(mem,size) \
LOCAL_MEM_WITH_SIZE(mem,LBUF_SIZE);\
do{if((fsize_t)(size)> mem.GetMaxSize())\
{\
    mem.Free();\
    mem.Malloc((int_ptr_t)(size));\
}}while(0)

#define LOCAL_MEM(name) LOCAL_MEM_WITH_SIZE(name,LBUF_SIZE)

class CMem:public CFileBase{
public:
    char *mBuf;
    int_ptr_t mSize;
    int_ptr_t mOffset;
    bool mSelfAlloc;
    int_ptr_t mMaxSize; 
    bool mIsConst;
public: 
    status_t Slice(int_ptr_t start, int_ptr_t size,CMem *out);
    status_t SetIsReadOnly(bool read_only);
	bool IsReadOnly();
    bool IsMalloc();
    status_t Transfer(CMem *from);
    status_t Realloc(int_ptr_t newSize);
    status_t AutoRealloc(int_ptr_t newSize);
    bool StrEqu(CMem *str, bool case_sensitive=true);
    char C(int index);
    char* GetRawBuf();
    status_t Malloc(int_ptr_t asize);
    status_t Free();
    int_ptr_t Read(void *buf,int_ptr_t n);
    int_ptr_t Write(const void *buf,int_ptr_t n);
    status_t Zero();
    status_t InitBasic();
    status_t Init();
    status_t Print(CFileBase *_buf);
    status_t SetRawBuf(void *p,int_ptr_t s,bool is_const);
    fsize_t Seek(fsize_t off);
    fsize_t GetOffset();
    fsize_t GetSize();
    status_t SetSize(fsize_t ssize);
    status_t AddBlock(fsize_t bsize);
    fsize_t GetMaxSize();
    status_t Copy(CFileBase *file);
    status_t SetStr(const char *p);
    status_t SetStr(const char *p,int_ptr_t len);
    const char* CStr();
    virtual char CharAt(fsize_t index);
    virtual status_t SetChar(fsize_t index,char ch);
    virtual char Getc();
    virtual char UnGetc();
    virtual fsize_t Putc(char ch);
   
    int StrICmp(const char *str);
    int StrICmp(CMem *file);
    int StrCmp(CMem *file); 
    int StrCmp(const char *str);

#if _UNICODE_
    CMem(const wchar_t *str);
    wchar_t CW(int index);
    const wchar_t* CStrW();
    status_t SetStrW(const wchar_t *p);
#endif

    CMem();
    ~CMem();
    CMem(const char *str);
    CMem(const char *str, int_ptr_t len);
    status_t Destroy();

};

#endif
