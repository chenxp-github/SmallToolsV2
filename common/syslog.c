#include "syslog.h"

#define CHECK_MODULE_RANGE(module)\
if(module < 0 || module > MAX_LOG_ITEMS)\
{\
    syslog_printf("%s:%d::error module range %d\n",__FILE__,__LINE__,module);\
    return ERROR;\
}\

#define CHECK_LOG_CONFIG() \
if(g_log_config == NULL)\
{\
    syslog_printf("%s:%d::g_log_config is NULL\n",__FILE__,__LINE__);\
    return ERROR;\
}\

static LogConfig *g_log_config = NULL;
static SYSLOG_PUTS_HANDLER g_puts_handler = NULL;
static const char *g_cur_file_name = "";
static int g_cur_line = 0;

status_t syslog_attach(LogConfig *log_config)
{
    g_log_config = log_config;
    CHECK_LOG_CONFIG();
    return OK;
}

status_t syslog_init(LogConfig *log_config)
{
    int i;

    if(log_config == NULL)
        return ERROR;

    for(i = 0; i < MAX_LOG_ITEMS; i++)
    {
        log_config->log_items[i].parent_index = LOG_MODULE_ROOT;
        log_config->log_items[i].log_level = LOG_LEVEL_DEBUG;
    }
    return syslog_attach(log_config);
}

status_t syslog_destroy()
{
    g_log_config = NULL;
    return OK;
}

status_t syslog_set_log_level(int module, int level)
{
    CHECK_LOG_CONFIG();
    CHECK_MODULE_RANGE(module);
    g_log_config->log_items[module].log_level = level;
    return OK;
}

static int syslog_get_log_level(int module)
{
    int cur_level,parent,parent_level,nest = 0;

    if(g_log_config == NULL)
        return LOG_LEVEL_DEBUG;

    cur_level = g_log_config->log_items[module].log_level;
    parent = g_log_config->log_items[module].parent_index;

    CHECK_MODULE_RANGE(module);

    while(nest < 32 && module != parent)
    {
        parent_level = g_log_config->log_items[parent].log_level;
        if(parent_level < cur_level)
            cur_level = parent_level;
        parent = g_log_config->log_items[parent].parent_index;
        if(parent == LOG_MODULE_ROOT || parent >= MAX_LOG_ITEMS)
            break;
        nest ++;
    }

    if(nest>=32)
    {
        syslog_printf("two many log module nest:%d\n",nest);
        return LOG_LEVEL_QUIET;
    }

    return cur_level;
}

status_t syslog_set_puts_handler(SYSLOG_PUTS_HANDLER handler)
{
    g_puts_handler = handler;
    return OK;
}
SYSLOG_PUTS_HANDLER syslog_get_puts_handler()
{
    return g_puts_handler;
}
status_t syslog_set_parent_module(int module, int parent)
{
    CHECK_LOG_CONFIG();
    CHECK_MODULE_RANGE(module);
    CHECK_MODULE_RANGE(parent);
    if(module == parent)
    {
        syslog_printf("parent module can not be itself");
        return ERROR;
    }
    g_log_config->log_items[module].parent_index = parent;
    return OK;
}

status_t syslog_puts(int module,int log_level,const char *str)
{
    if(g_puts_handler)
        return g_puts_handler(module,log_level,str);
    else
        printf("%s",str);
    return OK;
}

static status_t syslog_va_printf(int module,int log_level,const char *format, va_list ap)
{
    char buffer[LBUF_SIZE];
    crt_vsnprintf (buffer,sizeof(buffer),format, ap);
    syslog_puts(module,log_level,buffer);
    return OK;
}

status_t syslog_printf(const char* format, ...)
{
    crt_va_list ap;
    crt_va_start(ap, format);
    syslog_va_printf(0,LOG_LEVEL_ERROR,format, ap);
    crt_va_end(ap);
    return OK;
}

status_t syslog_log(int module,int log_level,const char* format, ...)
{
    crt_va_list ap;
    int level;
    level = syslog_get_log_level(module);
    if(level < log_level)
        return ERROR;
    crt_va_start(ap, format);
    syslog_va_printf(module, log_level, format, ap);
    syslog_puts(LOG_MODULE_EOL,log_level,"\r\n");
    crt_va_end(ap);
    return OK;
}

status_t syslog_set_file_and_line(const char *file_name, int line)
{
    g_cur_file_name = file_name;
    g_cur_line = line;
    return OK;
}

const char *syslog_get_cur_file_name()
{
    return g_cur_file_name;
}

int syslog_get_cur_line()
{
    return g_cur_line;
}
