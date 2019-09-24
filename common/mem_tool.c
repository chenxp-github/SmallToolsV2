#include "mem_tool.h"

#if _DEBUG_
static FILE *_fp_leak_detect=NULL;
static MUTEX_TYPE _mutex;
#endif

status_t Mem_Tool_Init(const char *fn)
{
#if _DEBUG_
    _fp_leak_detect = fopen(fn,"wb+");
    if(_fp_leak_detect == NULL)
        return ERROR;
    crt_init_mutex(&_mutex);
#endif
    return OK;
}

status_t Mem_Tool_Destroy(void)
{
#if _DEBUG_
    if(_fp_leak_detect)
    {
        fclose(_fp_leak_detect);
        _fp_leak_detect = NULL;
    }
    crt_destroy_mutex(&_mutex);
#endif  
    return OK;
}

status_t _leak_printf(const char *szFormat, ...)
{
#if _DEBUG_
    char szBuffer [4096] ;
    va_list pArgList ;

    if(_fp_leak_detect == NULL)
        return ERROR;
    
    va_start (pArgList, szFormat) ;
    vsprintf (szBuffer, szFormat, pArgList) ;
    va_end (pArgList) ;
    
    crt_lock_mutex(&_mutex);
    fputs(szBuffer,_fp_leak_detect);
    fputs("\r\n",_fp_leak_detect);
    crt_unlock_mutex(&_mutex);
#endif
    return OK;  
}
