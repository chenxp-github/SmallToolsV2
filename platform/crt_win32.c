#include "crt_win32.h"

#pragma comment(lib, "winmm.lib")

char* crt_strncpy(char *dest,const char *src,int n)
{
    return strncpy(dest,src,n);
}

void *crt_memset(void *buf, int_ptr_t n, int_ptr_t size)
{
    return memset(buf,n,size);
}

void *crt_memcpy(void *dest, const void *src, int_ptr_t n)
{
    return memcpy(dest, src, n);
}

int crt_memcmp(const void *buf1, const void *buf2, int_ptr_t count)
{
    return memcmp(buf1,buf2,count);
}

int64_t crt_atoi64(const char *str)
{
    return _atoi64(str);
}

status_t crt_int64_to_str(int64_t size, char *str)
{
    sprintf(str,"%I64d",size);
    return OK;
}

status_t crt_fsize_to_str(fsize_t size, char *str)
{
    return crt_int64_to_str(size,str);
}

fsize_t crt_str_to_fsize(const char *str)
{
    return _atoi64(str);
}

static uint32_t string_to_mode(const char *mode)
{
    uint32_t i;
    int read = 0;
    int write = 0;
    uint32_t type = 0;
    for(i = 0; i < strlen(mode); i++)
    {
        if(mode[i] == 'r' || mode[i] == 'R')
            read ++;
        if(mode[i] == 'w' || mode[i] == 'W')
            write++;
        if(mode[i] == '+')
            write++;
    }
    if(read > 0 && write == 0) //rb
        type |= _O_RDONLY;
    if(read > 0 && write > 0) //rw
        type |= _O_RDWR;
    if(read == 0 && write >0) //w
        type |= (_O_CREAT|_O_RDWR|_O_TRUNC);
    return type|_O_BINARY|_O_RANDOM|_O_NOINHERIT;
}

int crt_is_file_handle(FILE_HANDLE fd)
{
    return fd >= 0;
}
FILE_HANDLE crt_fopen(const char *fn, const char *mode)
{
    return _open(fn,string_to_mode(mode),0755);
}
void crt_fclose(FILE_HANDLE fd)
{
    _close(fd);
}

fsize_t crt_fseek(FILE_HANDLE fd, fsize_t offset, int where)
{
    return _lseeki64(fd,offset,where);
}

fsize_t crt_ftell(FILE_HANDLE fd)
{
    return _telli64(fd);
}

int_ptr_t crt_fread(FILE_HANDLE fd,void *buffer, int_ptr_t size)
{
    return read(fd,buffer,size);
}
int_ptr_t crt_fwrite(FILE_HANDLE fd,const void*buffer,int_ptr_t size)
{
    return write(fd,buffer,size);
}

status_t crt_vsprintf(char *string, const char *format, crt_va_list param)
{
    return vsprintf(string, format, param);
}

status_t crt_vsnprintf(char *string, int size, const char *format, crt_va_list param)
{
    return vsprintf(string, format, param);
}

int_ptr_t crt_strlen(const char *s)
{
    return strlen(s);
}

char *crt_strcpy(char *dst,const char *src)
{
    return strcpy(dst,src);
}

int crt_strcmp(const char *s1,const char * s2)
{
    return strcmp(s1,s2);
}

int crt_stricmp(const char *s1,const char * s2)
{
    return stricmp(s1,s2);
}

char *crt_strcat(char *dest,const char *src)
{
    return strcat(dest,src);
}

char *crt_strchr(const char *src,int ch)
{
    return strchr(src,ch);
}

void crt_strcpy_w(wchar_t *dst, const wchar_t *src)
{
    while(*src) *dst++ = *src++;
    *dst++ = 0;
}

wchar_t *crt_strcat_w(wchar_t *dest,const wchar_t *src)
{
    wchar_t *cp = dest;
    while (*cp) cp++;
    while (((*cp++) = (*src++)));
    return dest;
}

int_ptr_t crt_strlen_w(const wchar_t *str)
{
    int_ptr_t s = 0;
    while(*str++)s++; 
    return s;
}

wchar_t crt_tolwr_w(wchar_t uc)
{
    if(uc >= L'A' && uc <= L'Z')
    {
        uc += L'a' - L'A';
    }
    return uc;
}

wchar_t crt_toupr_w(wchar_t uc)
{
    if(uc >= L'a' && uc <= L'z')
    {
        uc -= L'a' - L'A';
    }
    return uc;
}

status_t crt_strcmp_w(const wchar_t *s1, const wchar_t *s2)
{
    status_t ret = 0;
    while (!(ret = *s1 - *s2) && *s2) ++s1, ++s2;
    return ret;
}

status_t crt_stricmp_w(const wchar_t *s1, const wchar_t *s2)
{
    wchar_t f, l;
    do 
    {
        f = crt_tolwr_w(*s1);
        l = crt_tolwr_w(*s2);
        s1++;
        s2++;
    } while ((f) && (f == l));  
    return f - l;
}

int crt_tolower(int c)
{
    return tolower(c);
}

int crt_toupper(int c)
{
    return toupper(c);
}

status_t crt_wvsprintf(wchar_t *buf, const wchar_t *format, crt_va_list param)
{
    return wvsprintfW(buf,format, param);
}

FILE *crt_fopen_w(const wchar_t *fn, const wchar_t *mode)
{
    return _wfopen(fn,mode);
}

static uint32_t old_timer = 0;
uint32_t crt_get_sys_timer()
{
    uint32_t now_timer;
    now_timer = timeGetTime();
    if(old_timer == 0) 
        old_timer = now_timer;
    if(now_timer < old_timer)
        old_timer = now_timer;
    return now_timer - old_timer;
}

status_t crt_need_chdir_before_open()
{
    return 0;
}

status_t crt_open_dir(const char *dir, void **p)
{
    WIN32_FIND_DATAA fd;
    HANDLE handle;
    char buf[LBUF_SIZE*2];

    crt_strcpy(buf,dir);
    crt_strcat(buf,"\\*");

    handle = FindFirstFileA(buf,&fd);
    if(handle == INVALID_HANDLE_VALUE)
        return ERROR;
    
    p[0] = (void*)handle;
    crt_strcpy((char*)p[1],fd.cFileName);
    p[2] = (void*)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    p[3] = (void*)(fd.nFileSizeLow);
    p[4] = (void*)(fd.nFileSizeHigh);
    p[5] = (void*)(fd.ftLastWriteTime.dwLowDateTime);
    p[6] = (void*)(fd.ftLastWriteTime.dwHighDateTime);
    p[10] = (void*)1;
    return OK;
}

status_t crt_close_dir(void **p)
{
    HANDLE handle = (HANDLE)p[0];
    return FindClose(handle);
}

status_t crt_read_dir(void **p)
{
    WIN32_FIND_DATAA fd;
    HANDLE handle;

    if(p[10] == (void*)1)
    {
        p[10] = 0;
        return OK;
    }
    
    handle = (HANDLE)p[0];
    if(FindNextFileA(handle,&fd) == 0)
        return ERROR;

    crt_strcpy((char*)p[1],fd.cFileName);
    p[2] = (void*)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    p[3] = (void*)(fd.nFileSizeLow);
    p[4] = (void*)(fd.nFileSizeHigh);
    p[5] = (void*)(fd.ftLastWriteTime.dwLowDateTime);
    p[6] = (void*)(fd.ftLastWriteTime.dwHighDateTime);

    p[10] = (void*)0;

    return OK;
}

char crt_get_path_splitor()
{
    return '\\';
}

int crt_get_cur_dir(char *path, int max)
{
    return GetCurrentDirectoryA(max,path);
}

status_t crt_mkdir(const char *dir)
{
    return CreateDirectoryA(dir,NULL);
}

status_t crt_rmdir(const char *dir)
{
    return RemoveDirectoryA(dir);
}

status_t crt_unlink(const char *filename)
{
    return unlink(filename) == 0;
}

status_t crt_chdir(const char *dir)
{
    return SetCurrentDirectoryA(dir);
}

status_t crt_chdir_w(const wchar_t *dir)
{
    return SetCurrentDirectoryW(dir);
}

status_t crt_open_dir_w(const wchar_t *dir, void **p)
{
    WIN32_FIND_DATAW fd;
    HANDLE handle;
    wchar_t buf[MAX_PATH];

    crt_strcpy_w(buf,dir);
    crt_strcat_w(buf,L"\\*");

    handle = FindFirstFileW(buf,&fd);
    if(handle == INVALID_HANDLE_VALUE)
        return ERROR;
    
    p[0] = (void*)handle;
    crt_strcpy_w((wchar_t*)p[1],fd.cFileName);
    p[2] = (void*)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    p[3] = (void*)(fd.nFileSizeLow);
    p[4] = (void*)(fd.nFileSizeHigh);
    p[5] = (void*)(fd.ftLastWriteTime.dwLowDateTime);
    p[6] = (void*)(fd.ftLastWriteTime.dwHighDateTime);

    p[10] = (void*)1;
    return OK;
}

status_t crt_read_dir_w(void **p)
{
    WIN32_FIND_DATAW fd;
    HANDLE handle;

    if(p[10] == (void*)1)
    {
        p[10] = 0;
        return OK;
    }
    
    handle = (HANDLE)p[0];
    if(FindNextFileW(handle,&fd) == 0)
        return ERROR;

    crt_strcpy_w((wchar_t*)p[1],fd.cFileName);
    p[2] = (void*)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    p[3] = (void*)(fd.nFileSizeLow);
    p[4] = (void*)(fd.nFileSizeHigh);
    p[5] = (void*)(fd.ftLastWriteTime.dwLowDateTime);
    p[6] = (void*)(fd.ftLastWriteTime.dwHighDateTime);

    p[10] = (void*)0;

    return OK;
}

wchar_t crt_get_path_splitor_w()
{
    return L'\\';
}

status_t crt_get_cur_dir_w(wchar_t *path, int max)
{
    return GetCurrentDirectoryW(max,path);
}

status_t crt_mkdir_w(const wchar_t *dir)
{
    return CreateDirectoryW(dir,NULL);
}

status_t crt_rmdir_w(const wchar_t *dir)
{
    return RemoveDirectoryW(dir);
}

status_t crt_putchar(int ch)
{
    return putchar(ch);
}

void crt_msleep(int ms)
{
    Sleep(ms);
}

wchar_t crt_gbk_char_to_unicode(const char *gbk)
{
    wchar_t unicode[16];
    UINT nCodePage = CP_ACP;
    MultiByteToWideChar(nCodePage,0, gbk, -1, (wchar_t*)unicode, 1);
    return unicode[0];
}

status_t crt_unicode_char_to_gbk(wchar_t uc,char *gb)
{
    wchar_t ustr[2];
    UINT nCodePage;

    ustr[0] = uc;ustr[1] = 0;
    gb[0] = 0;gb[1] = 0;gb[2] = 0;gb[3] = 0;
    nCodePage = CP_ACP;
    WideCharToMultiByte(nCodePage,0,(wchar_t*)ustr,-1,gb,4,NULL,NULL);       
    return OK;
}

int_ptr_t crt_get_unique_id()
{
    static int_ptr_t i = 200000;
    if(i == 0)i++;
    return i++;
}
///////////////////////////////////////////////////////////////////
#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////
#pragma comment(lib, "ws2_32.lib")

int32_t crt_socket( int32_t af, int32_t type, int32_t protocol )
{
    return socket(af,type,protocol);
}

int32_t crt_closesocket(int32_t s) 
{
    shutdown(s,SD_BOTH);
    return closesocket(s);
}

int32_t crt_send(int32_t s, const char *buf, int32_t len, int32_t flag)
{
    return send(s,buf,len,flag);
}

int32_t crt_recv(int32_t s, char *buf, int32_t len, int32_t flag)
{
    return recv(s,buf,len,flag);
}

void crt_socket_start()
{
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
}

void crt_socket_end()
{
    WSACleanup();
}

int32_t crt_set_blocking_mode(int32_t s,status_t enable)
{
    u_long block = !enable;
    return ioctlsocket(s,FIONBIO,&block) == 0;
}

int32_t crt_gethostname(char *name, int32_t namelen) 
{
    return gethostname(name,namelen); 
}

static CRITICAL_SECTION mutex;
static int _init_mutex = 0;
status_t crt_gethostbyname(const char * name, char *out)
{
    struct hostent *pHost;
    char *pszAddr;
    int i;

    int ret = ERROR;

    if(_init_mutex == 0)
    {
        InitializeCriticalSection(&mutex);
        _init_mutex = 1;
    }

    EnterCriticalSection(&mutex);
    out[0] = 0;
    pHost = gethostbyname(name);
    if(pHost != NULL)
    {
        for(i = 0; pHost->h_addr_list[i]!=NULL; i++)     
        {       
            pszAddr=crt_inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i]);   
            crt_strcat(out,pszAddr);
            crt_strcat(out,"\n");
        }
        ret = OK;
    }
    LeaveCriticalSection(&mutex);
    return ret;
}

char *crt_inet_ntoa (struct in_addr in)
{
    return inet_ntoa(in); 
}

uint16_t crt_htons( uint16_t hostshort)
{
    return htons(hostshort);
}

int32_t crt_bind( int32_t s,const struct sockaddr * name, int32_t namelen)
{
    return bind(s,name,namelen);
}   

int32_t crt_listen(int32_t s, int32_t backlog)
{
    return listen(s,backlog);
}

int32_t crt_accept( int32_t s, struct sockaddr *addr,int32_t *addrlen)
{
    return accept(s,addr,addrlen);
}

int32_t crt_connect( int32_t s, const struct sockaddr* name,int32_t namelen)
{
    connect(s,name,namelen);
    if(crt_is_connect_complete(s))
        return 0;
    return -1;
}

status_t crt_is_connect_complete(int32_t s)
{
    struct timeval to;  
    fd_set fs;  
    to.tv_sec = 0;  
    to.tv_usec = 10;
    
    FD_ZERO(&fs);   
    FD_SET((SOCKET)s,&fs);

    return select((int32_t)(s+1), 0, &fs, 0, &to) > 0;
}
status_t crt_get_all_ip(char *out)
{
    char szHostName[128];   
    if(crt_gethostname(szHostName,128) == 0)   
    {   
        return crt_gethostbyname(szHostName,out);
    }   
    return ERROR;
}

status_t crt_is_socket_broken()
{
    int err = WSAGetLastError();
    return (err!=WSAEWOULDBLOCK && err!= WSAEINTR);
}
///////////////////////////////////////////////////////////////////
#endif //#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#if USE_THREAD_MODULE
///////////////////////////////////////////////////////////////////
THREAD_HANDLE crt_create_thread(void(*func)(void*), int32_t stack_size, void *param)
{
    return _beginthread(func, stack_size,param);
}

int_ptr_t crt_get_current_thread_id()
{
    return GetCurrentThreadId();
}

void crt_init_mutex(MUTEX_TYPE *mutex)
{
    InitializeCriticalSection (mutex);
}

void crt_destroy_mutex(MUTEX_TYPE *mutex)
{
    DeleteCriticalSection(mutex);
}

void crt_lock_mutex(MUTEX_TYPE *mutex)
{
    EnterCriticalSection(mutex);
}

void crt_unlock_mutex(MUTEX_TYPE *mutex)
{
    LeaveCriticalSection(mutex);
}

status_t crt_cancel_thread(THREAD_HANDLE handle)
{
    return ERROR;
}
#if USE_SOCKET_MODULE
static void _thread_gethostbyname(void *context)
{
    void **p = (void**)context;
    const char *host;
    char *out;

    if(p == NULL)goto fail;
    out = (char*)&p[5];

    host = (const char*)p[2];
    if(host == NULL)goto fail;
    if(!crt_gethostbyname(host,out))
        goto fail;

    p[3] = (void*)1; //can be free
    p[1] = (void*)1; //success
    p[0] = (void*)1; //complete     
    return;

fail:
    p[3] = (void*)1; //can be free
    p[1] = (void*)0; //fail
    p[0] = (void*)1; //complete     
    return;
}
status_t crt_host_to_ip_async(const char *host, int_ptr_t **context)
{
    int_ptr_t *p;
    char *_host;
    if(host == NULL || context == NULL)
        return ERROR;
    crt_free_host_to_ip_context(context);
    *context = (int_ptr_t*)malloc(1024);
    _host = (char*)malloc(crt_strlen(host)+1);
    crt_strcpy(_host,host);
    p = *context;
    p[0] = 0; //is complete
    p[1] = 0; //fail
    p[2] = (int_ptr_t)_host;
    p[3] = 0; //can not be free
    crt_create_thread(_thread_gethostbyname,32*1024,*context);
    return OK;
}
status_t crt_free_host_to_ip_context(int_ptr_t **context)
{   
    int_ptr_t *p = *context;
    char *host;
    if(*context == NULL)
        return ERROR;
    while(p[3] == 0)
    {
        crt_msleep(1);
    }
    host = (char*)p[2];
    if(host)free(host);
    free(p);
    *context = NULL;
    return OK;
}
#endif
///////////////////////////////////////////////////////////////////
#endif //#if USE_THREAD_MODULE
///////////////////////////////////////////////////////////////////
