#ifndef __S2_SHA1CALCULATOR_H
#define __S2_SHA1CALCULATOR_H

#include "cruntime.h"
#include "mem.h"
#include "sha1.h"

class CSha1Calculator{
public:
    SHA1Context m_Context;
    uint8_t m_Result[SHA1HashSize];
public:
    status_t GetStringResult(CMem *out);
    status_t CalcuHash(CMem *file);
    status_t CalcuHash(CFileBase *file);
    uint8_t* GetResult();
    status_t Input(const void *buf, int_ptr_t size);
    status_t Reset();
    CSha1Calculator();
    virtual ~CSha1Calculator();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
};

#endif
