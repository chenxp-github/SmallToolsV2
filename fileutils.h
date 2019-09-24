#ifndef __FILEUTILS_H
#define __FILEUTILS_H

#include "mem.h"
#include "memfile.h"
#include "memstk.h"
#include "file.h"
#include "misc.h"
#include "encoder.h"
#include "dirmgr.h"
#include "misc.h"
#include "closure.h"

#define UNDEFINED 0x01
#define DEFINED 0x02

#define MODE_OVERWRITE  0x00000001
#define MODE_CHECKSAME  0x00000002
#define MODE_USE_LOG    0x00000004
#define MODE_COPY_LINK  0x00000010
#define MODE_PART_COPY  0x00000020


#define LOCAL_DISK_FILE(file,fn,mode,need_buf,use_log) \
    CFile file; \
    file.Init(); \
    if(need_buf)file.SetBufSize(1024*1024);\
    if(!file.OpenFile(fn,mode))\
{\
    if(use_log)XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,\
        "open file %s error",fn\
    );\
    return ERROR;\
}do{}while(0) \


status_t CopySingleFile_Interruptable(CMem *_from,CMem *_to, fsize_t start, fsize_t size,fsize_t to_start,uint32_t mode, int *running);
status_t CopySingleFile_Interruptable(CMem *_from,CMem *_to, fsize_t start, fsize_t size, uint32_t mode, int *running);
status_t CopySingleFile_Interruptable(CMem *from,CMem *to, uint32_t mode, int *running);
status_t CopySingleFile_Interruptable(const char *from,const char *to,uint32_t mode, int *running);
status_t CopySingleFile(CMem *_from,CMem *_to, fsize_t start, fsize_t size,fsize_t to_start,uint32_t mode);
status_t CopySingleFile(CMem *_from,CMem *_to, fsize_t start, fsize_t size, uint32_t mode);
status_t CopySingleFile(CMem *from,CMem *to, uint32_t mode);
status_t CopySingleFile(const char *from,const char *to,uint32_t mode);
bool IsTextFile(CFileBase *file);
bool IsTextFile(const char *fn);
status_t ConvertEol(CFileBase *in, CFileBase *out, int mode);
status_t ConvertEol(const char *in, const char *out, int mode);
bool IsFileSame(CFileBase *src1, CFileBase *src2);
bool IsFileSame(const char *fn1, const char *fn2);

#endif
