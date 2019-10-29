#ifndef __CRUNTIME_H
#define __CRUNTIME_H

#include "config.h"

/////////////////////////////////////////////////////////////
#if _WIN32_
#include "crt_win32.h"
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#if _MAC64_
#include "crt_mac64.h"
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#if _IOS32_
#include "crt_ios32.h"
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#if _LINUX_
#include "crt_linux.h"
#endif
/////////////////////////////////////////////////////////////
#if _LINUX64_
#include "crt_linux64.h"
#endif
/////////////////////////////////////////////////////////////
#if _GBAVM_
#include "crt_gba.h"
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#if _WINCE_
#include "crt_ce.h"
#undef ASSERT
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#if _WM6X_
#include "crt_wm.h"
#undef ASSERT
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#if _ANDROID_
#include "crt_android.h"
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
#define WEAK_REF_ID_DEFINE() int __weak_ref_id
#define WEAK_REF_ID_INIT()  __weak_ref_id = crt_get_unique_id()
#define WEAK_REF_ID_CLEAR() __weak_ref_id = 0
////////////////////////////////////////////////////////////
#define C_WEAK_REF_ID_DEFINE() int __weak_ref_id
#define C_WEAK_REF_ID_CLEAR(self) self->__weak_ref_id = 0
#define C_WEAK_REF_ID_INIT(self)  self->__weak_ref_id = crt_get_unique_id()
////////////////////////////////////////////////////////////

#define CLEAR_BITS(v,bits) v &= ~(bits)
#define SET_BITS(v,bits) v |= (bits)
#define TEST_BITS(v,bits) ((v&(bits)) == (bits))

///used in C++///
#define FLAG_FUNC(var,func, bit) \
status_t Set##func(bool have) \
{ \
    if(have) \
    SET_BITS(var,bit); \
    else \
    CLEAR_BITS(var,bit); \
    return OK; \
} \
bool func() \
{ \
    return TEST_BITS(var,bit); \
} \

///used in pure C///
#define C_FLAG_FUNC_H(_class,func) \
status_t _class##_##set##_##func(struct _class *self,bool_t have); \
bool_t _class##_##func(struct _class *self); \

#define C_FLAG_FUNC_C(_class,var,func, bit) \
status_t _class##_##set##_##func(struct _class *self,bool_t have) \
{ \
    if(have) \
    SET_BITS(self->var,bit); \
    else \
    CLEAR_BITS(self->var,bit); \
    return OK; \
} \
bool_t _class##_##func(struct _class *self) \
{ \
    return TEST_BITS(self->var,bit); \
} \
/////////////////////////////////////////////////////////////
#define MAKE_VARGS_BUFFER(format) \
    char szBuffer [FILEBASE_LBUF_SIZE]; \
    crt_va_list pArgList;\
    crt_va_start(pArgList, format);\
    crt_vsprintf(szBuffer, format, pArgList);\
    crt_va_end (pArgList)\
	
/////////////////////////////////////////////////////////////
#define UNUSED(x) (void)(x)
////////////////////////////////////////////////////////////
#define DEFAULT_STACK_SIZE 1024
////////////////////////////////////////////////////////////
#define SAVE_WEAK_REF_ID(obj,tmp)\
	int tmp = (obj).__weak_ref_id\
	
#define RESTORE_WEAK_REF_ID(obj,tmp)\
	(obj).__weak_ref_id = tmp\

//////////////////////////////////////////////////////////////
#define OFFSET_OF(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)

#define CONTAINER_OF(type, ptr,base_ptr,member)\
    type *ptr = (type *)((char *)base_ptr - OFFSET_OF(type,member))\
    
#endif
