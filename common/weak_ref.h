#ifndef __WEAK_REF_H
#define __WEAK_REF_H

typedef struct{
    int ref_count;
    int weak_ref_id;
}WeakRefContext_t;

#define WEAK_REF_CONTEXT_DEFINE(context)\
    WeakRefContext_t *context\

#define  WEAK_REF_CONTEXT_INIT(context) do{\
    context = NULL;\
}while(0)\

#define WEAK_REF_CONTEXT_ADD_REF(context) do{\
    if(context)\
    {\
        (context)->ref_count++;\
    }\
}while(0)\
    
#define WEAK_REF_CONTEXT_RELEASE(context) do{\
    if(context)\
    {\
        (context)->ref_count--;\
        if((context)->ref_count == 0)\
        {\
            FREE(context);\
            (context) = NULL;\
        }\
    }\
    context = NULL;\
}while(0)\
    
#define WEAK_REF_CONTEXT_CREATE(context) do{\
    if(!(context))\
    {\
        MALLOC(context,WeakRefContext_t,1);\
        (context)->weak_ref_id = crt_get_unique_id();\
        (context)->ref_count = 1;\
    }\
}while(0)\

#define WEAK_REF_CONTEXT_DESTROY(context) do{\
    if(context)\
    {\
        (context)->weak_ref_id = 0;\
		WEAK_REF_CONTEXT_RELEASE(context);\
    }\
}while(0)\

#define SAVE_WEAK_REF_ID(obj, c)\
WeakRefContext_t *c = (obj).__weak_ref_context;\
(obj).__weak_ref_context = NULL\

#define RESTORE_WEAK_REF_ID(obj,c)\
(obj).__weak_ref_context = c\

#define WEAK_REF_DEFINE()   WEAK_REF_CONTEXT_DEFINE(__weak_ref_context)
#define WEAK_REF_CLEAR()    WEAK_REF_CONTEXT_INIT(__weak_ref_context)
#define WEAK_REF_DESTROY()  WEAK_REF_CONTEXT_DESTROY(__weak_ref_context)
#define WEAK_REF_RELEASE()  WEAK_REF_CONTEXT_RELEASE(__weak_ref_context)
#define WEAK_REF_ADD_REF()  WEAK_REF_CONTEXT_ADD_REF(__weak_ref_context)

#endif
