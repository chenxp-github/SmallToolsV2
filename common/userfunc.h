#ifndef __S2_USERFUNC_H
#define __S2_USERFUNC_H

#include "cruntime.h"
#include "filebase.h"
#include "closure.h"

#define USER_FUNC_FLAG(func,bit) FLAG_FUNC(_Flags_,func,bit)

class CUserFunc{
public:
    enum{
        HOW_TO_COMP = 0x10000000,
        HOW_TO_DEL =  0x20000000,
        HOW_TO_COPY = 0x40000000,
        HOW_TO_PRINT = 0x80000000,
    };
    uint32_t _Flags_;
    CClosure _UserFunc_;
public:
    uint32_t GetUseUserFunc();
    status_t SetUseUserFunc(uint32_t flags);
    bool IsUserFunc(uint32_t flags);
    CClosure* Callback();
    CUserFunc();
    virtual ~CUserFunc();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
};

#endif
