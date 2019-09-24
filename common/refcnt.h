#ifndef __REFCNT_H
#define __REFCNT_H

#include "mem_tool.h"
#include "syslog.h"

#define REF_COUNT_DEFINE()\
int _ref_count;\
void AddRef()\
{\
    this->_ref_count++;\
}\
void Release()\
{\
    this->_ref_count--;\
    if(this->_ref_count < 0)\
    {\
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,\
            "double or more release detected"\
        );\
        return;\
    }\
    if(this->_ref_count == 0)\
    {\
        LEAK_DETECT_PRINT("free: addr=0x%x file=%s line=%d",this,__FILE__,__LINE__);\
        delete this;\
    }\
}\
int GetRefCnt()\
{\
    return this->_ref_count;\
}\

#define REF_COUNT_INIT() \
this->_ref_count=0 \

#endif
