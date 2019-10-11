#ifndef __CRT_LINUX_H
#define __CRT_LINUX_H

#include "config.h"

#define USE_SOCKET_MODULE   1
#define USE_THREAD_MODULE   1
#define LOCAL_ENCODING      ENCODING_UTF8

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>

#undef  ERROR
#undef  OK
#undef  TRUE
#undef  FALSE

#define TRUE        1
#define FALSE       0
#define OK          1
#define ERROR       0

#if !defined _STDINT_H
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;
#endif

typedef int32_t             status_t;
typedef int32_t             int_ptr_t;
typedef uint8_t             bool_t;

#define LBUF_SIZE           4096
#define FILEBASE_LBUF_SIZE  (32*1024)
#define ERROR_FILE_HANDLE   -1
#define MAX_PATH            4096

typedef int  FILE_HANDLE;

#if USE_FILE_32
    #define MAX_FSIZE       0x7fffffff
    typedef int32_t         fsize_t;
    typedef uint32_t        ufsize_t;
#else
    #define MAX_FSIZE       0x7fffffffffffffffLL
    typedef int64_t         fsize_t;
    typedef uint64_t        ufsize_t;
#endif

#define DLL_EXPORT __attribute__((visibility("default")))
#define DLL_EXPORT_C extern "C" DLL_EXPORT

#define crt_va_list     va_list
#define crt_va_end      va_end
#define crt_va_list     va_list
#define crt_va_start    va_start

#ifdef __cplusplus
extern "C" {
#endif

void *crt_memset(void *buf, int_ptr_t n, int_ptr_t size);
void *crt_memcpy(void *dest, const void *src, int_ptr_t n);
int crt_memcmp(const void *buf1, const void *buf2, int_ptr_t count);
int64_t crt_atoi64(const char *str);
status_t crt_int64_to_str(int64_t size, char *str);
fsize_t crt_str_to_fsize(const char *str);
status_t crt_fsize_to_str(fsize_t size, char *str);
int crt_is_file_handle(FILE_HANDLE fd);
FILE_HANDLE crt_fopen(const char *fn, const char *mode);
void crt_fclose(FILE_HANDLE fd);
fsize_t crt_fseek(FILE_HANDLE fd, fsize_t offset, int where);
fsize_t crt_ftell(FILE_HANDLE fd);
int_ptr_t crt_fread(FILE_HANDLE fd,void *buffer, int_ptr_t size);
int_ptr_t crt_fwrite(FILE_HANDLE fd,const void*buffer,int_ptr_t size);
int crt_vsprintf(char *string, const char *format, crt_va_list param);
status_t crt_vsnprintf(char *string, size_t size,const char *format, crt_va_list param);
int_ptr_t crt_strlen(const char *s);
char *crt_strcpy(char *dst,const char *src);
char* crt_strncpy(char *dest,const char *src,int n);
int crt_strcmp(const char *s1,const char * s2);
int crt_stricmp(const char *s1,const char * s2);
char *crt_strcat(char *dest,const char *src);
char *crt_strchr(const char *src,int ch);
void crt_strcpy_w(wchar_t *dst, const wchar_t *src);
wchar_t *crt_strcat_w(wchar_t *dest,const wchar_t *src);
int_ptr_t crt_strlen_w(const wchar_t *str);
wchar_t crt_tolwr_w(wchar_t uc);
wchar_t crt_toupr_w(wchar_t uc);
status_t crt_strcmp_w(const wchar_t *s1, const wchar_t *s2);
status_t crt_stricmp_w(const wchar_t *s1, const wchar_t *s2);
int crt_tolower(int c);
int crt_toupper(int c);
status_t crt_wvsprintf(wchar_t *buf, const wchar_t *format, crt_va_list param);
FILE *crt_fopen_w(const wchar_t *fn, const wchar_t *mode);
uint32_t crt_get_sys_timer(void);
status_t crt_need_chdir_before_open(void);
status_t crt_open_dir(const char *dir, void **p);
status_t crt_close_dir(void **p);
status_t crt_read_dir(void **p);
char crt_get_path_splitor(void);
int crt_get_cur_dir(char *path, int max);
status_t crt_mkdir(const char *dir);
status_t crt_rmdir(const char *dir);
status_t crt_unlink(const char *filename);
status_t crt_chdir(const char *dir);
status_t crt_chdir_w(const wchar_t *dir);
status_t crt_open_dir_w(const wchar_t *dir, void **p);
status_t crt_read_dir_w(void **p);
wchar_t crt_get_path_splitor_w(void);
int crt_get_cur_dir_w(wchar_t *path, int max);
status_t crt_mkdir_w(const wchar_t *dir);
status_t crt_rmdir_w(const wchar_t *dir);
status_t crt_putchar(int ch);
void crt_msleep(int ms);
uint32_t crt_gbk_char_to_unicode(const char *gbk);
status_t crt_unicode_char_to_gbk(wchar_t uc,char *gb);
int_ptr_t crt_get_unique_id(void);

///////////////////////////////////////////////////////////////////
#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////
#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_ERROR    -1

int32_t crt_socket( int32_t af, int32_t type, int32_t protocol );
int32_t crt_closesocket(int32_t s);
int32_t crt_send(int32_t s, const char *buf, int32_t len, int32_t flag);
int32_t crt_recv(int32_t s, char *buf, int32_t len, int32_t flag);
void crt_socket_start(void);
void crt_socket_end(void);
status_t crt_set_blocking_mode(int32_t s,status_t enable);
int32_t crt_gethostname(char *name, int32_t namelen);
status_t crt_gethostbyname(const char * name, char *out);
char *crt_inet_ntoa (struct in_addr in);
uint16_t crt_htons( uint16_t hostshort);
int32_t crt_bind ( int32_t s,const struct sockaddr * name, int32_t namelen);
int32_t crt_listen(int32_t s, int32_t backlog);
int32_t crt_accept( int32_t s, struct sockaddr *addr,int32_t *addrlen);
int32_t crt_connect( int32_t s, const struct sockaddr* name,int32_t namelen);
status_t crt_is_connect_complete(int32_t s);
status_t crt_get_all_ip(char *out);
status_t crt_is_socket_broken();
///////////////////////////////////////////////////////////////////
#endif //#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
#if USE_THREAD_MODULE
///////////////////////////////////////////////////////////////////
#include <pthread.h>
typedef pthread_t THREAD_HANDLE;
typedef pthread_mutex_t MUTEX_TYPE;
THREAD_HANDLE crt_create_thread(void(*func)(void*), int32_t stack_size, void *param);
status_t crt_cancel_thread(THREAD_HANDLE thread_id);
int_ptr_t crt_get_current_thread_id();
void crt_init_mutex(MUTEX_TYPE *mutex);
void crt_destroy_mutex(MUTEX_TYPE *mutex);
void crt_lock_mutex(MUTEX_TYPE *mutex);
void crt_unlock_mutex(MUTEX_TYPE *mutex);

#if USE_SOCKET_MODULE
status_t crt_host_to_ip_async(const char *host, int_ptr_t **context);
status_t crt_free_host_to_ip_context(int_ptr_t **context);
#endif
///////////////////////////////////////////////////////////////////
#endif //#if USE_THREAD_MODULE
///////////////////////////////////////////////////////////////////

FILE *fopen64(const char *filename, const char *type);
int fseeko64(FILE *stream, int64_t offset, int whence);
int64_t ftello64(FILE *stream);

#ifdef __cplusplus
} //extern "C"
#endif

#endif
