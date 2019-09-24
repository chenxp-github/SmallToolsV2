#include <fcntl.h>
#include "crt_linux.h"
#include "syslog.h"

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
    return atoll(str);
}

status_t crt_int64_to_str(int64_t size, char *str)
{
    sprintf(str,"%ld",size);
    return OK;
}

status_t crt_fsize_to_str(fsize_t size, char *str)
{
#if USE_FILE_32
    sprintf(str,"%d",size);
#else
    sprintf(str,"%ld",size);
#endif
    return OK;
}

fsize_t crt_str_to_fsize(const char *str)
{
#if USE_FILE_32
    return atoi(str);
#else
    return atoll(str);
#endif
}

int crt_is_file_handle(FILE_HANDLE fd)
{
    return fd >= 0;
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
        type |= O_RDONLY;
    if(read > 0 && write > 0) //rw
        type |= O_RDWR;
    if(read == 0 && write >0) //w
        type |= (O_CREAT|O_RDWR|O_TRUNC);
    return type;
}

FILE_HANDLE crt_fopen(const char *fn, const char *mode)
{
#if USE_FILE_32
    return open(fn,string_to_mode(mode),0666);
#else
    int open64(const char *pathname, int flags,...);
    return open64(fn,string_to_mode(mode),0666);
#endif
}

void crt_fclose(FILE_HANDLE fd)
{
    close(fd);
}

fsize_t crt_fseek(FILE_HANDLE fd, fsize_t offset, int where)
{
#if USE_FILE_32
    return lseek(fd,offset,where);
#else
    fsize_t lseek64(int fd, fsize_t offset, int whence);
    return lseek64(fd,offset,where);
#endif
}

fsize_t crt_ftell(FILE_HANDLE fd)
{
    return crt_fseek(fd,0,SEEK_CUR);
}

int_ptr_t crt_fread(FILE_HANDLE fd,void *buffer, int_ptr_t size)
{
    return read(fd,buffer, size);
}

int_ptr_t crt_fwrite(FILE_HANDLE fd,const void*buffer,int_ptr_t size)
{
    return write(fd,buffer, size);
}

status_t crt_vsprintf(char *string, const char *format, crt_va_list param)
{
    return vsprintf(string, format, param);
}

status_t crt_vsnprintf(char *string, size_t size,const char *format,crt_va_list param)
{
    return vsnprintf(string,size,format, param);
}

int_ptr_t crt_strlen(const char *s)
{
    return strlen(s);
}

char *crt_strcpy(char *dst,const char *src)
{
    return strcpy(dst,src);
}

char* crt_strncpy(char *dest,const char *src,int n)
{
    return strncpy(dest,src,n);
}

int crt_strcmp(const char *s1,const char * s2)
{
    return strcmp(s1,s2);
}

int crt_stricmp(const char *s1,const char * s2)
{
    return strcasecmp(s1,s2);
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
    ASSERT(0);
    return OK;
}

FILE *crt_fopen_w(const wchar_t *fn, const wchar_t *mode)
{
    ASSERT(0);
    return NULL;
}

uint32_t crt_get_sys_timer(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t);
    return t.tv_sec*1000 + t.tv_nsec/1000000;
}


status_t crt_need_chdir_before_open(void)
{
    return 1;
}

status_t crt_open_dir(const char *dir, void **p)
{
    ASSERT(dir);
    if(dir[0] == 0)dir="/";
    DIR * handle = opendir(dir);
    if(handle == NULL)
    {
        return ERROR;
    }
    p[0] = handle;
    return OK;
}

status_t crt_close_dir(void **p)
{
    DIR *handle = (DIR*)p[0];
    ASSERT(handle);
    return closedir(handle);
}

status_t crt_read_dir(void **p)
{
    DIR *handle = (DIR*)p[0];
    struct stat statbuf;
    struct dirent *entry = readdir(handle);
    int is_dir;

    if(entry == NULL)
        return ERROR;

	memset(&statbuf,0,sizeof(statbuf));
    lstat(entry->d_name,&statbuf);
	
    crt_strcpy(p[1],entry->d_name);

    is_dir = S_ISDIR(statbuf.st_mode);
    if(S_ISLNK(statbuf.st_mode))
    {
        handle = opendir(entry->d_name);
        if(handle)
        {
            is_dir = 1;
            closedir(handle);
        }
    }

    p[2] = (void*)((int_ptr_t)is_dir);
    p[3] = (void*)((int_ptr_t)statbuf.st_size);

    int64_t tm = statbuf.st_mtim.tv_sec*1000LL+statbuf.st_mtim.tv_nsec/1000000LL;
    memcpy(&p[5],&tm,sizeof(tm));

    return OK;
}

char crt_get_path_splitor(void)
{
    return '/';
}

int crt_get_cur_dir(char *path, int max)
{
    char *p = getcwd(path,max);
    ASSERT(p);
    return crt_strlen(path);
}

status_t crt_mkdir(const char *dir)
{
    return mkdir(dir,S_IRWXU|S_IRWXG|S_IRWXO) == 0;
}

status_t crt_rmdir(const char *dir)
{
    return rmdir(dir) == 0;
}

status_t crt_unlink(const char *filename)
{
    return unlink(filename) == 0;
}

status_t crt_chdir(const char *dir)
{
    ASSERT(dir);
    if(dir[0] == 0)dir="/";
    return chdir(dir) == 0;
}

status_t crt_chdir_w(const wchar_t *dir)
{
    ASSERT(0);
    return OK;
}


status_t crt_open_dir_w(const wchar_t *dir, void **p)
{
    ASSERT(0);
    return OK;
}

status_t crt_read_dir_w(void **p)
{
    ASSERT(0);
    return OK;
}

wchar_t crt_get_path_splitor_w(void)
{
    return L'/';
}

int crt_get_cur_dir_w(wchar_t *path, int max)
{
    ASSERT(0);
    return OK;
}

status_t crt_mkdir_w(const wchar_t *dir)
{
    ASSERT(0);
    return OK;
}

status_t crt_rmdir_w(const wchar_t *dir)
{
    ASSERT(0);
    return OK;
}

status_t crt_putchar(int ch)
{
    return putchar(ch);
}

void crt_msleep(int ms)
{
    usleep(ms*1000);
}

uint16_t ff_convert (uint16_t   src,uint32_t dir);

uint32_t crt_gbk_char_to_unicode(const char *gbk)
{
    uint16_t t = 0;
    char *p = (char*)&t;
    if(gbk[1] == 0)
    {
        t = gbk[0];
    }
    else
    {
        p[1] = gbk[0];
        p[0] = gbk[1];
    }
    return ff_convert(t,1);
}

status_t crt_unicode_char_to_gbk(wchar_t uc,char *gb)
{
    uint16_t t = ff_convert(uc, 0);
    char *p = (char*)&t;
    if(t < 0x80)
    {
        gb[0] = t;
        gb[1] = 0;
    }
    else
    {
        gb[0] = p[1];
        gb[1] = p[0];
    }
    gb[2] = 0;
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

int32_t crt_socket( int32_t af, int32_t type, int32_t protocol )
{
    return socket(af,type,protocol);
}

int32_t crt_closesocket(int32_t s)
{
    return close(s);
}

int32_t crt_send(int32_t s, const char *buf, int32_t len, int32_t flag)
{
    return (int32_t)send(s,buf,len,flag);
}

int32_t crt_recv(int32_t s, char *buf, int32_t len, int32_t flag)
{
    return (int32_t)recv(s,buf,len,flag);
}

void crt_socket_start(void)
{
    signal(SIGPIPE,SIG_IGN);
}

void crt_socket_end(void)
{
}

status_t crt_set_blocking_mode(int32_t s,status_t enable)
{
    uint32_t flags = fcntl(s,F_GETFL,0);
    if(enable)
        fcntl(s,F_SETFL,flags & (~O_NONBLOCK));
    else
        fcntl(s,F_SETFL,flags|O_NONBLOCK);
    return OK;
}

int32_t crt_gethostname(char *name, int32_t namelen)
{
    return gethostname(name,namelen);
}

static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
status_t crt_gethostbyname(const char * name, char *out)
{
    struct hostent *pHost;
    char *pszAddr;
    int i;
    int ret = ERROR;
    pthread_mutex_lock(&mutex);
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
    pthread_mutex_unlock(&mutex);
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

int32_t crt_bind ( int32_t s,const struct sockaddr * name, int32_t namelen)
{
    int32_t on = 1;
    setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    return bind(s,name,namelen);
}

int32_t crt_listen(int32_t s, int32_t backlog)
{
    return listen(s,backlog);
}

int32_t crt_accept( int32_t s, struct sockaddr *addr,int32_t *addrlen)
{
    return accept(s,addr,(uint32_t*)addrlen);
}

int32_t crt_connect( int32_t s, const struct sockaddr* name,int32_t namelen)
{
    return connect(s,name,namelen);
}

status_t crt_is_connect_complete(int32_t s)
{
    struct timeval to;

    fd_set fs;
    to.tv_sec = 0;
    to.tv_usec = 1000;
    FD_ZERO(&fs);
    FD_SET(s,&fs);

    if(select((int32_t)(s+1), 0, &fs, 0, &to) > 0)
    {
        int32_t err;
        uint32_t len = sizeof(err);
        if(getsockopt(s, SOL_SOCKET, SO_ERROR, &err, &len) != 0)
            return FALSE;

        if(err == 0)
            return TRUE; //connect ok
    }

    return FALSE;
}

status_t crt_get_all_ip(char *out)
{
    int32_t sock_fd;
    struct ifreq buf[32];
    struct ifconf ifc;
    int32_t interface_num;
    char *addr;

    out[0] = 0;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(sock_fd >= 0);

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_req = buf;

    ASSERT(ioctl(sock_fd, SIOCGIFCONF, (char *)&ifc) >= 0);
    interface_num = ifc.ifc_len / sizeof(struct ifreq);

    while(interface_num--)
    {
        if(ioctl(sock_fd, SIOCGIFFLAGS, (char *)&buf[interface_num]) < 0)
        {
            continue;
        }

        if(ioctl(sock_fd, SIOCGIFADDR, (char *)&buf[interface_num]) < 0)
        {
            continue;
        }

        addr = inet_ntoa(((struct sockaddr_in*)(&buf[interface_num].ifr_addr))->sin_addr);
        crt_strcat(out,addr);
        crt_strcat(out,"\n");
    }

    close(sock_fd);

    return OK;
}

status_t crt_is_socket_broken()
{
    int err = errno;
    return (err!=EWOULDBLOCK && err!= EINTR);
}
///////////////////////////////////////////////////////////////////
#endif //#if USE_SOCKET_MODULE
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
#if USE_THREAD_MODULE
///////////////////////////////////////////////////////////////////
THREAD_HANDLE crt_create_thread(void(*func)(void*), int32_t stack_size, void *param)
{
    pthread_t thread = 0;
    pthread_create(&thread, NULL, (void*(*)(void*))func, param);
    if(thread)pthread_detach(thread);
    return thread;
}

status_t crt_cancel_thread(THREAD_HANDLE thread_id)
{
    if(thread_id)
    {
        return pthread_cancel(thread_id) == 0;
    }
    return ERROR;
}

int_ptr_t crt_get_current_thread_id()
{
    return pthread_self();
}

void crt_init_mutex(MUTEX_TYPE *mutex)
{
    pthread_mutex_init(mutex,NULL);
}

void crt_destroy_mutex(MUTEX_TYPE *mutex)
{
    pthread_mutex_destroy(mutex);
}
void crt_lock_mutex(MUTEX_TYPE *mutex)
{
    pthread_mutex_lock(mutex);
}
void crt_unlock_mutex(MUTEX_TYPE *mutex)
{
    pthread_mutex_unlock(mutex);
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
