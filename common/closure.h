#ifndef __S2_CLOSURE_H
#define __S2_CLOSURE_H

#include "cruntime.h"
#include "filebase.h"

#define MAX_PARAMS  16

enum{
    PARAM_TYPE_UNKNOWN = 0,
    PARAM_TYPE_POINTER,
    PARAM_TYPE_INT,
    PARAM_TYPE_INT64,
    PARAM_TYPE_DOUBLE,
    PARAM_TYPE_FLOAT,
    PARAM_TYPE_STRING,
    PARAM_TYPE_MALLOC,
    PARAM_TYPE_WEAKPTR,
    PARAM_TYPE_OBJECT,
};

class CClosure;
typedef int (*CLOSURE_FUNC)(CClosure *);

#define BEGIN_CLOSURE_FUNC(name) \
class T##name{public: \
static status_t F##name(CClosure *closure){ \

#define END_CLOSURE_FUNC(name) \
}};CLOSURE_FUNC name=T##name::F##name \

#define BEGIN_CLOSURE(name) \
BEGIN_CLOSURE_FUNC(_func_##name) \

#define END_CLOSURE(name) \
END_CLOSURE_FUNC(_func_##name); \
CClosure name(_func_##name); \
name.SetIsOnHeap(false) \

#define BEGIN_NEW_CLOSURE(name) \
BEGIN_CLOSURE_FUNC(_func_##name) \

#define END_NEW_CLOSURE(name) \
END_CLOSURE_FUNC(_func_##name); \
CClosure *name; \
NEW(name,CClosure(_func_##name)); \
name->SetIsOnHeap(true) \

#define __CLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n) \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_POINTER \
||(closure)->GetParamType(n) == PARAM_TYPE_MALLOC);\
type var = (type)(closure)->GetParamPointer(n)

#define __CLOSURE_PARAM_PTR(closure,type,var,n) \
__CLOSURE_PARAM_PTR_CAN_BE_NULL((closure),type,var,n);\
ASSERT(var) \

#define __CLOSURE_PARAM_INT(closure,var,n) \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_INT);\
int var = (closure)->GetParamInt(n)\

#define __CLOSURE_PARAM_INT64(closure,var,n) \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_INT64);\
int64_t var = (closure)->GetParamInt64(n) \

#define __CLOSURE_PARAM_FLOAT(closure,var,n) \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_FLOAT);\
float var = (closure)->GetParamFloat(n)

#define __CLOSURE_PARAM_DOUBLE(closure,var,n) \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_DOUBLE);\
double var = (closure)->GetParamDouble(n) \

#define __CLOSURE_PARAM_STRING(closure,var,n) \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_STRING);\
const char* var = (closure)->GetParamString(n) \

#define __CLOSURE_PARAM_WEAKPTR(closure,type,var,n) \
int __weak_ref_id_##var; \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_WEAKPTR); \
type var = (type)(closure)->GetParamWeakPointer(n,&__weak_ref_id_##var);\
if(!(var && var->__weak_ref_id != 0 && var->__weak_ref_id == __weak_ref_id_##var))\
var=NULL\

#define __CLOSURE_PARAM_OBJECT_CAN_BE_NULL(closure,type,var,n) \
ASSERT((closure)->GetParamType(n) == PARAM_TYPE_OBJECT);\
type var = (type)(closure)->GetParamObject(n)\

#define __CLOSURE_PARAM_OBJECT(closure,type,var,n)\
__CLOSURE_PARAM_OBJECT_CAN_BE_NULL((closure),type,var,n);\
ASSERT(var)\

#define CLOSURE_PARAM_PTR_CAN_BE_NULL(type,var,n)    __CLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define CLOSURE_PARAM_PTR(type,var,n)                __CLOSURE_PARAM_PTR(closure,type,var,n)
#define CLOSURE_PARAM_OBJECT_CAN_BE_NULL(type,var,n) __CLOSURE_PARAM_PTR_CAN_BE_NULL(closure,type,var,n)
#define CLOSURE_PARAM_OBJECT(type,var,n)             __CLOSURE_PARAM_OBJECT(closure,type,var,n)
#define CLOSURE_PARAM_INT(var,n)                     __CLOSURE_PARAM_INT(closure,var,n)
#define CLOSURE_PARAM_INT64(var,n)                   __CLOSURE_PARAM_INT64(closure,var,n)
#define CLOSURE_PARAM_FLOAT(var,n)                   __CLOSURE_PARAM_FLOAT(closure,var,n)
#define CLOSURE_PARAM_DOUBLE(var,n)                  __CLOSURE_PARAM_DOUBLE(closure,var,n)
#define CLOSURE_PARAM_STRING(var,n)                  __CLOSURE_PARAM_STRING(closure,var,n)
#define CLOSURE_PARAM_WEAKPTR(type,var,n)            __CLOSURE_PARAM_WEAKPTR(closure,type,var,n)

#define CLOSURE_FLAG_FUNC(func,bit) FLAG_FUNC(m_Flags,func,bit)

class CMiniBson;
class CClosure{
public:
    WEAK_REF_ID_DEFINE();
public:
    int64_t m_Params[MAX_PARAMS];
    uint8_t m_Types[MAX_PARAMS];
    CLOSURE_FUNC m_Func;
    uint32_t m_Flags;
public:
    int GetParamsCount();
    CMiniBson* GetParamBson(int index);
    status_t SetParamBson(int index, CMiniBson *bson);
    status_t ParamToBson(CMiniBson *bson);
    status_t BsonToParam(CMiniBson *bson);
    void* GetCloneObjectFunc(int index);
    void* GetPrintObjectFunc(int index);
    void* GetDeleteObjectFunc(int index);
    status_t CopyObject(int index,CClosure *closure2, int index2);
    status_t PrintObject(int index, CFileBase *buf);
    status_t DeleteObject(int index);
    void* GetObjectFunc(int index,int op_index);
    status_t CopyParams(CClosure *closure);
    status_t CopyParams(CClosure *closure, int from_start,int to_start, int copy_size);
    int GetParamType(int index);
    int GetMallocSize(int index);
    status_t Run(int event);
    void* Malloc(int index, int size);
    void* Malloc(int index, const void *buf,int size);
    void* GetParamPointer(int index);
    void* GetParamObject(int index);
    const char* GetParamString(int index);
    float GetParamFloat(int index);
    double GetParamDouble(int index);
    int GetParamInt(int index);
    int64_t GetParamInt64(int index);
    void* GetParamWeakPointer(int index,int *weak_ref_id);
    status_t Run();
    status_t SetParamString(int index, const char *str);
    status_t SetParamString(int index, const char *str, int len);
    status_t SetParamFloat(int index, float f);
    status_t SetParamDouble(int index, double f);
    status_t SetParamInt(int index, int i);
    status_t SetParamInt64(int index, int64_t i);
    status_t SetParamPointer(int index, void *p);
    status_t SetParamObject(int index, void *obj,void **obj_op,int obj_op_num);
    status_t SetParamWeakPointer(int index,void *ptr, int weak_ref_id);
    status_t FreeParam(int index);
    CLOSURE_FUNC GetFunc();
    status_t SetFunc(CLOSURE_FUNC func);
    CClosure(CLOSURE_FUNC func);
    CClosure();
    virtual ~CClosure();
    status_t InitBasic();
    status_t Init();
    status_t Copy(CClosure *closure);
    status_t Print(CFileBase *_buf);
    status_t Destroy();

    CLOSURE_FLAG_FUNC(IsCalled,0x00000001);
    CLOSURE_FLAG_FUNC(IsTimeout,0x00000002);
    CLOSURE_FLAG_FUNC(IsOnHeap,0x00000004);
};

#endif
