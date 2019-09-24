#ifndef __MEM_TOOL_2007_6_27_16_17_29_H
#define __MEM_TOOL_2007_6_27_16_17_29_H

#include "cruntime.h"

#ifdef __cplusplus
extern "C" {
#endif

status_t Mem_Tool_Init(const char *fn);
status_t Mem_Tool_Destroy(void);
status_t _leak_printf(const char *szFormat, ...);

#ifdef __cplusplus
} //extern "C"
#endif

#define LEAK_DETECT_PRINT _leak_printf

#define NEW(p,obj) \
do{\
p = new obj;\
__ASSERT(p);\
LEAK_DETECT_PRINT("alloc: addr=0x%x file=%s line=%d obj = %s",p,__FILE__,__LINE__,#obj);\
}while(0)

#define NEW_ARRAY(p,obj,items) \
do{\
p = new obj[items];\
__ASSERT(p);\
LEAK_DETECT_PRINT("alloc: addr=0x%x file=%s line=%d obj = %s",p,__FILE__,__LINE__,#obj);\
}while(0)

#define DEL(p) if(p)\
do{\
delete p;\
LEAK_DETECT_PRINT("free: addr=0x%x file=%s line=%d",p,__FILE__,__LINE__);\
p = NULL;\
}while(0)

#define DEL_ARRAY(p) if(p)\
do{\
delete [] p;\
LEAK_DETECT_PRINT("free: addr=0x%x file=%s line=%d",p,__FILE__,__LINE__);\
p = NULL;\
}while(0)

#define MALLOC(p,obj,items) \
do{\
p = (obj*)malloc(sizeof(obj)*(items));\
__ASSERT(p);\
LEAK_DETECT_PRINT("alloc: addr=0x%x file=%s line=%d obj = %s",p,__FILE__,__LINE__,#obj);\
}while(0)

#define ZALLOC(p,obj,items) \
do{\
MALLOC(p,obj,items);\
memset(p,0,sizeof(obj)*(items));\
}while(0)

#define FREE(p) if(p)\
do{\
free(p);\
LEAK_DETECT_PRINT("free: addr=0x%x file=%s line=%d",p,__FILE__,__LINE__);\
p = NULL;\
}while(0)

#define REALLOC(p,obj,old_items,items) if(p)\
do{\
obj *__p__;\
MALLOC(__p__,obj,items);\
memcpy(__p__,(p),sizeof(obj)*(old_items));\
FREE(p);\
(p)=__p__;\
}while(0)

#define X_MALLOC MALLOC
#define X_FREE FREE
#define X_NEW NEW
#define X_DEL DEL
#define X_NEW_ARRAY NEW_ARRAY
#define X_DEL_ARRAY DEL_ARRAY
#define X_REALLOC REALLOC

#endif
