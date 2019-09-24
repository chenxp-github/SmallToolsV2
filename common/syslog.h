#ifndef __SYSLOG_H
#define __SYSLOG_H

#include "cruntime.h"

#define LOG_LEVEL_QUIET     0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     4

#define MAX_LOG_ITEMS       512

#define LOG_MODULE_ROOT         0
#define LOG_MODULE_COMMON       1
#define LOG_MODULE_MESSAGEPEER  2
#define LOG_MODULE_USER         100

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    uint16_t parent_index;
    uint16_t log_level;
}LogItem;

typedef struct{
    LogItem log_items[MAX_LOG_ITEMS];
}LogConfig;

typedef status_t (*SYSLOG_PUTS_HANDLER)(int module,int log_level,const char *str);

status_t syslog_attach(LogConfig *log_config);
status_t syslog_init(LogConfig *log_config);
status_t syslog_destroy();
status_t syslog_set_log_level(int module, int level);
status_t syslog_set_puts_handler(SYSLOG_PUTS_HANDLER handler);
SYSLOG_PUTS_HANDLER syslog_get_puts_handler();
status_t syslog_puts(int module,int log_level,const char *str);
status_t syslog_set_parent_module(int module, int parent);
status_t syslog_printf(const char* format, ...);
status_t syslog_tab_printf(int tab,const char* format, ...);
status_t syslog_log(int module,int log_level,const char* format, ...);
status_t syslog_set_file_and_line(const char *file_name, int line);
const char *syslog_get_cur_file_name();
int syslog_get_cur_line();

#ifdef __cplusplus
} //extern "C"
#endif

//////////////////////////////////////////////////////////////////////////////
#define XLOG syslog_set_file_and_line(__FILE__,__LINE__),syslog_log

#define __LOG XLOG(LOG_MODULE_ROOT,LOG_LEVEL_ERROR,

#define _ASSERT_LOG(f) \
__LOG("assertion fail \"%s\" at file=%s line=%d"),\
#f,__FILE__,__LINE__)\

//////////////////////////////////////////////////////////////////////////////
#define __ASSERT(f)do{if(!(f)){_ASSERT_LOG(f);}}while(0)
#define ASSERT(f) do{if(!(f)){_ASSERT_LOG(f);return 0;}}while(0)
#define VASSERT(f) do{if(!(f)){_ASSERT_LOG(f);return;}}while(0)
#define RASSERT(f,r) do{if(!(f)){_ASSERT_LOG(f);return r;}}while(0)

#define PD(p) __LOG("%s = %d"),#p,(int)(p))
#define PS(p) __LOG("%s = %s"),#p,p)
#define PF(p) __LOG("%s = %f"),#p,p)
#define PP(p) __LOG("%s = %p"),#p,(void*)((int_ptr_t)p))

#define PRINT_OBJ(obj) do{\
    CMemFile _mf;\
    _mf.Init();\
    _mf.Log("%s = {",#obj);\
    _mf.IncLogLevel(1);\
    (obj).Print(&_mf);\
    _mf.IncLogLevel(-1);\
    _mf.Log("}");\
    _mf.Dump();\
}while(0)\

#define C_PRINT_OBJ(obj,print) do{\
    struct log_buffer lb;\
    log_buffer_init(&lb);\
    log_buffer_log(&lb,"%s = {",#obj);\
    log_buffer_inc_level(&lb,1);\
    print(&(obj),&lb);\
    log_buffer_inc_level(&lb,-1);\
    log_buffer_log(&lb,"}");\
    syslog_puts(LOG_MODULE_ROOT,LOG_LEVEL_ERROR,\
        log_buffer_get_text(&lb)\
    );\
    log_buffer_destroy(&lb);\
}while(0)\

#endif
