#ifndef __SERIAL_H
#define __SERIAL_H

#include "filebase.h"

#if !_IS_WINDOWS_
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>    
#include <errno.h>
#endif

class CSerial:public CFileBase{
public:
#if _IS_WINDOWS_
    HANDLE handle;
#else
    int fd;
#endif

public:
    status_t EnableDtrHandshake(bool enable);
    int_ptr_t Read(void *buf,int_ptr_t n);
    int_ptr_t Write(const void *buf,int_ptr_t n);
    fsize_t GetOffset();
    fsize_t GetSize();
    status_t SetSize(fsize_t s);
    fsize_t  Seek(fsize_t o);
    status_t AddBlock(fsize_t bsize);
    fsize_t GetMaxSize();
    status_t Configure(int baudrate = 115200,
        int databits = 8, int stopbits = 1,int parity='n');
    status_t Open(const char *dev_name);
    CSerial();
    virtual ~CSerial();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();

    status_t Copy(CSerial *p){return OK;};
    status_t Print(CFileBase *buf){return OK;}
};

#endif
