#include "closure.h"
#include "syslog.h"
#include "mem_tool.h"
#include "minibson.h"

#define CHECK_INDEX_RANGE(index) ASSERT(index >= 0 && index < MAX_PARAMS)

#define SET_PARAM(type,var) \
CHECK_INDEX_RANGE(index); \
this->FreeParam(index);\
m_Types[index] = type;\
m_Params[index] = (int64_t)(var)

#define CHECK_TYPE(index,type) \
CHECK_INDEX_RANGE(index); \
ASSERT(m_Types[index] == type)

#define TO_POINTER(index) ((void*)m_Params[index])

#define INDEX_HOW_TO_DEL   0
#define INDEX_HOW_TO_CLONE 1
#define INDEX_HOW_TO_PRINT 2
#define OP_INDEX_MAX       3

#define OP_INDEX_START     1

typedef status_t (*DELETE_OBJ_FUNC)(void *p);
typedef status_t (*PRINT_OBJ_FUNC)(void *p,CFileBase *_buf);
typedef void*    (*CLONE_OBJ_FUNC)(void *p);

static status_t how_to_delete_bson(void *_p)
{
    CMiniBson *bson = (CMiniBson*)_p;
    DEL(bson);
    return OK;
}

static status_t how_to_print_bson(void *_p,CFileBase *buf)
{
    CMiniBson *bson = (CMiniBson*)_p;
    ASSERT(bson);
    return bson->Print(buf);
}

static void* how_to_clone_bson(void *_p)
{
    CMiniBson *bson = (CMiniBson*)_p;
    ASSERT(bson);

    CMiniBson *tmp;
    NEW(tmp,CMiniBson);
    tmp->Init();
    tmp->Copy(bson);

    return tmp;
}
//////////////////////////////////////////
CClosure::CClosure()
{
    this->InitBasic();
}
CClosure::CClosure(CLOSURE_FUNC func)
{
    this->Init();
    this->SetFunc(func);
}
CClosure::~CClosure()
{
    this->Destroy();
}
status_t CClosure::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    m_Func = NULL;
    m_Flags = 0;
    memset(m_Params,0,sizeof(m_Params));
    memset(m_Types,0,sizeof(m_Types));
    return OK;
}
status_t CClosure::Init()
{
    this->InitBasic();
    WEAK_REF_ID_INIT();
    return OK;
}
status_t CClosure::Destroy()
{
    for(int i = 0; i < MAX_PARAMS; i++)
    {
        FreeParam(i);
    }

    this->InitBasic();
    return OK;
}
status_t CClosure::Print(CFileBase *_buf)
{
    for(int i = 0; i < MAX_PARAMS; i++)
    {
        int type = this->GetParamType(i);

        if(type != PARAM_TYPE_UNKNOWN)
        {
            _buf->Log(NULL);
            _buf->Printf("[%d]=",i);
        }

        if(type == PARAM_TYPE_MALLOC)
        {
            int size = GetMallocSize(i);
            _buf->Printf("<malloc %d bytes>\r\n",size);
        }
        else if(type == PARAM_TYPE_STRING)
        {
            const char *str = GetParamString(i);
            _buf->Printf("\"%s\"\r\n",str);
        }
        else if(type == PARAM_TYPE_WEAKPTR)
        {
            int weak_id;
            void *p = this->GetParamWeakPointer(i,&weak_id);
            _buf->Printf("<%p>\r\n",p);
        }
        else if(type == PARAM_TYPE_POINTER)
        {
            void *p = this->GetParamPointer(i);
            _buf->Printf("<%p>\r\n",p);
        }
        else if(type == PARAM_TYPE_INT)
        {
            int v = this->GetParamInt(i);
            _buf->Printf("%d\r\n",v);
        }
        else if(type == PARAM_TYPE_INT64)
        {
            int64_t v = this->GetParamInt64(i);
            _buf->Printf("%ld\r\n",v);
        }
        else if(type == PARAM_TYPE_FLOAT)
        {
            float f = this->GetParamFloat(i);
            _buf->Printf("%f\r\n",f);
        }
        else if(type == PARAM_TYPE_DOUBLE)
        {
            double v = this->GetParamDouble(i);
            _buf->Printf("%lf\r\n",v);
        }
        else if(type == PARAM_TYPE_OBJECT)
        {
            _buf->Printf("{\r\n");
            this->PrintObject(i,_buf);
            _buf->Printf("}\r\n");
        }
    }
    return OK;
}

status_t CClosure::CopyParams(CClosure *closure)
{
    return this->CopyParams(closure,0,0,MAX_PARAMS);
}

status_t CClosure::CopyParams(CClosure *closure, int from_start,int to_start, int copy_size)
{
    for(int i = 0; i < copy_size; i++)
    {
        int from = from_start + i;
        int to = to_start + i;

        if(from >= MAX_PARAMS)
            break;
        if(to >= MAX_PARAMS)
            break;

        int type = closure->GetParamType(from);
        if(type == PARAM_TYPE_MALLOC)
        {
            int size = closure->GetMallocSize(from);
            this->Malloc(to,size);
            memcpy(this->GetParamPointer(to),closure->GetParamPointer(from),size);
        }
        else if(type == PARAM_TYPE_STRING)
        {
            const char *str = closure->GetParamString(from);
            this->SetParamString(to,str);
        }
        else if(type == PARAM_TYPE_WEAKPTR)
        {
            int weak_ref_id;
            void *ptr = closure->GetParamWeakPointer(from,&weak_ref_id);
            this->SetParamWeakPointer(to,ptr,weak_ref_id);
        }
        else if(type == PARAM_TYPE_OBJECT)
        {
            this->CopyObject(to,closure,from);
        }
        else if(type != PARAM_TYPE_UNKNOWN)
        {
            m_Types[to] = closure->m_Types[from];
            m_Params[to] = closure->m_Params[from];
        }
    }

    return OK;
}

status_t CClosure::Copy(CClosure *closure)
{
    ASSERT(closure);
    if(this == closure)
        return OK;
    this->Destroy();
    this->m_Func = closure->m_Func;
    this->CopyParams(closure);
    return OK;
}

status_t CClosure::SetFunc(CLOSURE_FUNC func)
{
    m_Func = func;
    return OK;
}

CLOSURE_FUNC CClosure::GetFunc()
{
    return m_Func;
}

status_t CClosure::FreeParam(int index)
{
    CHECK_INDEX_RANGE(index);
    if(    m_Types[index] == PARAM_TYPE_STRING
        || m_Types[index] == PARAM_TYPE_MALLOC
        || m_Types[index] == PARAM_TYPE_WEAKPTR)
    {
        void *p = TO_POINTER(index);
        FREE(p);
        m_Params[index] = 0;
    }

    if(m_Types[index] == PARAM_TYPE_OBJECT)
    {
        this->DeleteObject(index);
    }

    m_Types[index] = PARAM_TYPE_UNKNOWN;
    m_Params[index] = 0;
    return OK;
}

status_t CClosure::SetParamPointer(int index, void *p)
{
    SET_PARAM(PARAM_TYPE_POINTER,p);
    return OK;
}

status_t CClosure::SetParamObject(int index, void *obj, void** obj_op, int obj_op_num)
{
    ASSERT(obj);
    CHECK_INDEX_RANGE(index);
    ASSERT(obj_op_num >= OP_INDEX_MAX);

    this->FreeParam(index);
    m_Types[index] = PARAM_TYPE_OBJECT;

    int_ptr_t *p;
    ZALLOC(p,int_ptr_t,OP_INDEX_MAX+1); //first is object
    m_Params[index] = (int64_t)p;

    p[0] = (int_ptr_t)obj;
    for(int i = 0; i < OP_INDEX_MAX; i++)
    {
        p[i+1] = (int_ptr_t)obj_op[i];
    }

    return OK;
}

status_t CClosure::SetParamBson(int index, CMiniBson *bson)
{
    ASSERT(bson);
    void *bson_op[]={
        (void*)how_to_delete_bson,
        (void*)how_to_clone_bson,
        (void*)how_to_print_bson
    };
    return this->SetParamObject(index,bson,bson_op,3);
}

status_t CClosure::SetParamInt(int index, int i)
{
    SET_PARAM(PARAM_TYPE_INT,i);
    return OK;
}
status_t CClosure::SetParamInt64(int index, int64_t i)
{
    SET_PARAM(PARAM_TYPE_INT64,i);
    return OK;
}

status_t CClosure::SetParamFloat(int index, float f)
{
    ASSERT(sizeof(f) == sizeof(int_ptr_t));
    int_ptr_t *t = (int_ptr_t*)&f;
    SET_PARAM(PARAM_TYPE_FLOAT,*t);
    return OK;
}
status_t CClosure::SetParamDouble(int index, double f)
{
    ASSERT(sizeof(f) == sizeof(int64_t));
    int64_t *t = (int64_t*)&f;
    SET_PARAM(PARAM_TYPE_DOUBLE,*t);
    return OK;
}

status_t CClosure::SetParamString(int index, const char *str,int len)
{
    ASSERT(str);

    char *tmp;
    CHECK_INDEX_RANGE(index);
    MALLOC(tmp,char,len+1);
    crt_strcpy(tmp,str);
    SET_PARAM(PARAM_TYPE_STRING,tmp);
    return OK;
}
status_t CClosure::SetParamString(int index, const char *str)
{
    ASSERT(str);
    return this->SetParamString(index,str,crt_strlen(str));
}
status_t CClosure::SetParamWeakPointer(int index,void *ptr, int weak_ref_id)
{
    int_ptr_t *tmp;
    CHECK_INDEX_RANGE(index);
    MALLOC(tmp,int_ptr_t,2);
    tmp[0] = (int_ptr_t)ptr;
    tmp[1] = (int_ptr_t)weak_ref_id;
    SET_PARAM(PARAM_TYPE_WEAKPTR,tmp);
    return OK;
}

status_t CClosure::Run()
{
    if(m_Func==NULL)
        return ERROR;
    status_t ret = m_Func(this);
    this->SetIsCalled(true);
    return ret;
}

int CClosure::GetParamInt(int index)
{
    CHECK_TYPE(index,PARAM_TYPE_INT);
    return (int)((int_ptr_t)m_Params[index]);
}

int64_t CClosure::GetParamInt64(int index)
{
    CHECK_TYPE(index,PARAM_TYPE_INT64);
    return m_Params[index];
}

float CClosure::GetParamFloat(int index)
{
    CHECK_TYPE(index,PARAM_TYPE_FLOAT);
    float *t = (float*)&m_Params[index];
    return *t;
}

double CClosure::GetParamDouble(int index)
{
    CHECK_TYPE(index,PARAM_TYPE_DOUBLE);
    double *t = (double*)&m_Params[index];
    return *t;
}

const char* CClosure::GetParamString(int index)
{
    CHECK_INDEX_RANGE(index);
    ASSERT(m_Types[index] == PARAM_TYPE_STRING);
    return (const char*)m_Params[index];
}

void* CClosure::GetParamPointer(int index)
{
    CHECK_INDEX_RANGE(index);

    if(m_Types[index] == PARAM_TYPE_POINTER)
        return TO_POINTER(index);

    if(m_Types[index] == PARAM_TYPE_MALLOC)
    {
        int *p = (int*)TO_POINTER(index);
        return (void*)(p+1); //skip size
    }

    ASSERT(0); //type error
    return NULL;
}

void* CClosure::GetParamObject(int index)
{
    CHECK_INDEX_RANGE(index);
    ASSERT(m_Types[index] == PARAM_TYPE_OBJECT);
    int_ptr_t *p = (int_ptr_t*)m_Params[index];
    ASSERT(p);
    return (void*)p[0];
}

CMiniBson* CClosure::GetParamBson(int index)
{
    CMiniBson *_bson = (CMiniBson*)this->GetParamObject(index);
    ASSERT(_bson);
    void *del_func = this->GetDeleteObjectFunc(index);
    ASSERT(del_func == how_to_delete_bson);
    return _bson;
}

void* CClosure::GetParamWeakPointer(int index,int *weak_ref_id)
{
    CHECK_TYPE(index,PARAM_TYPE_WEAKPTR);
    int_ptr_t *p = (int_ptr_t*)TO_POINTER(index);
    *weak_ref_id = (int)p[1];
    return (void*)p[0];
}

void* CClosure::Malloc(int index, int size)
{
    if(size <= 0)
        return NULL;
    void *p;
    MALLOC(p,char,size+sizeof(int));
    SET_PARAM(PARAM_TYPE_MALLOC,p);

    int *ip = (int*)p;
    *ip = size;

    return (void*)(ip+1);
}

void* CClosure::Malloc(int index, const void *buf,int size)
{
    if(size <= 0)return NULL;
    void *p = this->Malloc(index,size);
    ASSERT(p);
    memcpy(p,buf,size);
    return p;
}

status_t CClosure::Run(int event)
{
    SetParamInt(0,event);
    return Run();
}

int CClosure::GetMallocSize(int index)
{
    ASSERT(m_Types[index] == PARAM_TYPE_MALLOC);
    int *p = (int*)TO_POINTER(index);
    ASSERT(p);
    return p[0];
}

int CClosure::GetParamType(int index)
{
    CHECK_INDEX_RANGE(index);
    return m_Types[index];
}

void* CClosure::GetObjectFunc(int index, int op_index)
{
    CHECK_INDEX_RANGE(index);
    ASSERT(m_Types[index] == PARAM_TYPE_OBJECT);
    int_ptr_t *p = (int_ptr_t*)m_Params[index];
    ASSERT(p);
    return (void*)p[op_index+OP_INDEX_START];
}

status_t CClosure::DeleteObject(int index)
{
    CHECK_INDEX_RANGE(index);
    ASSERT(m_Types[index] == PARAM_TYPE_OBJECT);

    DELETE_OBJ_FUNC del_func = NULL;
    void *obj = this->GetParamObject(index);

    if(obj == NULL)goto end;
    del_func = (DELETE_OBJ_FUNC)GetDeleteObjectFunc(index);
    if(del_func == NULL) goto end;
    del_func(obj);

end:
    int_ptr_t *p = (int_ptr_t*)m_Params[index];
    FREE(p);
    m_Params[index] = 0;
    m_Types[index] = PARAM_TYPE_UNKNOWN;
    return OK;
}

status_t CClosure::PrintObject(int index, CFileBase *buf)
{
    void *obj = this->GetParamObject(index);
    if(obj == NULL)return ERROR;
    PRINT_OBJ_FUNC print_func = (PRINT_OBJ_FUNC)GetPrintObjectFunc(index);
    if(print_func == NULL)
        return ERROR;
    return print_func(obj,buf);
}

status_t CClosure::CopyObject(int index, CClosure *closure2, int index2)
{
    ASSERT(closure2);
    CHECK_INDEX_RANGE(index2);
    void **p2 = (void**)closure2->m_Params[index2];
    if(p2 == NULL)return ERROR;
    CLONE_OBJ_FUNC clone_func = (CLONE_OBJ_FUNC)closure2->GetCloneObjectFunc(index2);
    ASSERT(clone_func);
    void *p1 = clone_func((void*)p2[0]);
    return this->SetParamObject(index,p1,p2+OP_INDEX_START,OP_INDEX_MAX);
}

void* CClosure::GetPrintObjectFunc(int index)
{
    CHECK_INDEX_RANGE(index);
    PRINT_OBJ_FUNC print_func = (PRINT_OBJ_FUNC)this->GetObjectFunc(index, INDEX_HOW_TO_PRINT);
    return (void*)print_func;
}

void* CClosure::GetDeleteObjectFunc(int index)
{
    CHECK_INDEX_RANGE(index);
    DELETE_OBJ_FUNC del_func = (DELETE_OBJ_FUNC)this->GetObjectFunc(index, INDEX_HOW_TO_DEL);
    return (void*)del_func;
}

void* CClosure::GetCloneObjectFunc(int index)
{
    CHECK_INDEX_RANGE(index);
    CLONE_OBJ_FUNC clone_func = (CLONE_OBJ_FUNC)this->GetObjectFunc(index, INDEX_HOW_TO_CLONE);
    return (void*)clone_func;
}

status_t CClosure::BsonToParam(CMiniBson *bson)
{
    ASSERT(bson);

    int type;
    LOCAL_MEM(name);

    for(int k = 0; k < MAX_PARAMS; k++)
    {
        if(bson->IsEnd())break;
        bson->PeekNext(&type,&name);
        int i = atoi(name.CStr());
        ASSERT(i >= 0 && i < MAX_PARAMS);

        if(type == CMiniBson::BSON_TYPE_INT32)
        {
            int32_t v;
            bson->GetInt32(NULL,&v);
            this->SetParamInt(i,v);
        }
        else if(type == CMiniBson::BSON_TYPE_INT64)
        {
            int64_t v;
            bson->GetInt64(NULL,&v);
            this->SetParamInt64(i,v);
        }
        else if(type == CMiniBson::BSON_TYPE_DOUBLE)
        {
            double v;
            bson->GetDouble(NULL,&v);
            this->SetParamDouble(i,v);
        }
        else if(type == CMiniBson::BSON_TYPE_BOOLEAN)
        {
            bool v;
            bson->GetBoolean(NULL,&v);
            this->SetParamInt(i,v);
        }
        else if(type == CMiniBson::BSON_TYPE_STRING)
        {
            CMem v;
            bson->GetString(NULL,&v);
            this->SetParamString(i,v.CStr(),v.StrLen());
        }
        else if(type == CMiniBson::BSON_TYPE_DOCUMENT)
        {
            CMiniBson *document;
            NEW(document,CMiniBson);
            document->Init();
            bson->GetDocument(NULL,document);
            this->SetParamBson(i,document);
        }
        else if(type == CMiniBson::BSON_TYPE_BINARY)
        {
            CMem bin;
            bson->GetBinary(NULL,&bin);
            ASSERT(bin.GetSize() > 0);
            void *p = this->Malloc(i,(int_ptr_t)bin.GetSize());
            memcpy(p,bin.GetRawBuf(),(int_ptr_t)bin.GetSize());
        }
        else
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "BsonToParam:unsupported type %d",type
            );
            break;
        }
    }
    return OK;
}

status_t CClosure::ParamToBson(CMiniBson *bson)
{
    ASSERT(bson);

    char name[64];

    for(int i = 0; i < MAX_PARAMS; i++)
    {
        sprintf(name,"%d",i);
        int type = this->GetParamType(i);

        if(type == PARAM_TYPE_UNKNOWN)
        {
            continue;
        }

        if(type == PARAM_TYPE_MALLOC)
        {
            int size = GetMallocSize(i);
            void *p = this->GetParamPointer(i);
            ASSERT(p);
            bson->PutBinary(name,p,size);
        }
        else if(type == PARAM_TYPE_STRING)
        {
            const char *str = GetParamString(i);
            ASSERT(str);
            bson->PutString(name,str);
        }
        else if(type == PARAM_TYPE_INT)
        {
            int v = this->GetParamInt(i);
            bson->PutInt32(name,v);
        }
        else if(type == PARAM_TYPE_INT64)
        {
            int64_t v = this->GetParamInt64(i);
            bson->PutInt64(name,v);
        }
        else if(type == PARAM_TYPE_FLOAT)
        {
            float v = this->GetParamFloat(i);
            bson->PutDouble(name,v);
        }
        else if(type == PARAM_TYPE_DOUBLE)
        {
            double v = this->GetParamDouble(i);
            bson->PutDouble(name,v);
        }
        else if(type == PARAM_TYPE_OBJECT)
        {
            CMiniBson *_bson = this->GetParamBson(i);
            ASSERT(_bson);
            bson->PutDocument(name,_bson);
        }
        else
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "ParamToBson:unsupported type %d",type
            );
        }
    }
    return OK;
}

int CClosure::GetParamsCount()
{
    int count = 0;
    for(int i = 0; i < MAX_PARAMS; i++)
    {
        int type = this->GetParamType(i);
        if(type != PARAM_TYPE_UNKNOWN)
            count++;
    }
    return count;
}

