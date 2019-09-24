#ifndef __S2_BIGENDIAN_H
#define __S2_BIGENDIAN_H

#include "cruntime.h"
#include "filebase.h"

class CBigEndian{
public:
    CFileBase *i_File;
public:
	int_ptr_t GetLeftSize();
    int_ptr_t ReadU8(uint8_t *out);
    int_ptr_t ReadU16(uint16_t *out);
    int_ptr_t ReadU32(uint32_t *out);
    int_ptr_t ReadU64(uint64_t *out);
    status_t Skip(int_ptr_t size);
    status_t Seek(fsize_t offset);
    int_ptr_t WriteU64(uint64_t value);
    int_ptr_t WriteU32(uint32_t value);
    int_ptr_t WriteU16(uint16_t value);
    int_ptr_t WriteU8(uint8_t value);
    int_ptr_t WriteBytes(const void* buf, size_t len);
	int_ptr_t ReadBytes(void *buf, size_t len);
    status_t AttachFile(CFileBase *file);
    CBigEndian();
    virtual ~CBigEndian();
    status_t InitBasic();
    status_t Init(CFileBase *file);
    status_t Destroy();
};

#endif
