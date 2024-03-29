#include "serial.h"
#include "mem.h"
#include "sys_log.h"

CSerial::CSerial()
{
    this->InitBasic();
}
CSerial::~CSerial()
{
    this->Destroy();
}

status_t CSerial::Init()
{
    this->InitBasic();
    CFileBase::Init();
    //add your code
    return OK;
}

fsize_t CSerial::GetOffset()
{
    return 0;
}

fsize_t CSerial::GetSize()
{
    return MAX_FSIZE;
}

status_t CSerial::SetSize(fsize_t s)
{
    return OK;
}

fsize_t CSerial::Seek(fsize_t o)
{
    return 0;
}

status_t CSerial::AddBlock(fsize_t bsize)
{
    return 0;
}

fsize_t CSerial::GetMaxSize()
{
    return 0;
}

//////////////////////////////////////////////////////////////////
#if !_IS_WINDOWS_ && !_IS_ESP32_
//////////////////////////////////////////////////////////////////
status_t CSerial::InitBasic()
{
    CFileBase::InitBasic();
    this->fd = ERROR_FILE_HANDLE;
    return OK;
}

status_t CSerial::Destroy()
{
    if(this->fd > 0)
    {
        close(this->fd);
        this->fd = ERROR_FILE_HANDLE;
    }

    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CSerial::Open(const char *dev_name)
{
    ASSERT(this->fd < 0);

    this->fd = open(dev_name,O_RDWR);
    if(this->fd <= 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "open %s error\n",dev_name);
        return ERROR;
    }
    return OK;
}

static const int speed_arr[] = 
{ 
    B4000000,B3500000,B3000000,B2500000,B2000000,B1500000,B1152000,
    B1000000,B921600,B576000,B500000,B460800,B230400,B115200, B57600, 
    B38400, B19200, B9600, B4800, B2400, B1200, B300
};

static const int name_arr[] = 
{
    4000000,3500000,3000000,2500000,2000000,1500000,1152000,
    1000000, 921600,576000,500000,460800,230400,115200, 57600, 
    38400,19200,9600,4800,2400,1200,300
};

static status_t set_baud_rate(int fd,int speed,struct termios *options)
{
    int i,find = 0;   
    int array_size = sizeof(speed_arr)/sizeof(speed_arr[0]);
    
    for ( i= 0;  i < array_size;  i++) 
    { 
        if  (speed == name_arr[i]) 
        {     
            find = 1;
            tcflush(fd, TCIOFLUSH);     
            cfsetispeed(options, speed_arr[i]);  
            cfsetospeed(options, speed_arr[i]);   
        }  
    }

    return find;
}

status_t CSerial::Configure(int baudrate,int databits,int stopbits,int parity)
{
    ASSERT(this->fd > 0);
    
    struct termios options; 
    memset(&options,0,sizeof(options));

    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;   

    switch (databits) 
    {   
        case 7:     
            options.c_cflag |= CS7; 
            break;
        case 8:     
            options.c_cflag |= CS8;
            break;   
        default:    
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "Unsupported data size\n"); 
            return ERROR;
    }

    switch (parity) 
    {   
        case 'n':
        case 'N':    
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
            break;  
        case 'o':   
        case 'O':     
            options.c_cflag |= (PARODD | PARENB); 
            options.c_iflag |= INPCK; 
            break;  
        case 'e':  
        case 'E':   
            options.c_cflag |= PARENB; 
            options.c_cflag &= ~PARODD; 
            options.c_iflag |= INPCK; 
            break;
        case 'S': 
        case 's':  /*as no parity*/   
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;  
        default:   
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "Unsupported parity\n");    
            return ERROR;
    }  
    
    switch (stopbits)
    {   
        case 1:    
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2:    
            options.c_cflag |= CSTOPB;  
            break;
        default:    
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "Unsupported stop bits\n");
            return ERROR;
    } 

    if (parity != 'n')   
        options.c_iflag |= INPCK; 
    
    ASSERT(set_baud_rate(this->fd,baudrate,&options));

    options.c_cc[VTIME] = 0; /* time out 0 seconds*/   
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */

    if (tcsetattr(fd,TCSANOW,&options) != 0)   
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "SetupSerial Error 2\n");
        return ERROR;
    }

    tcflush(fd,TCIFLUSH);
    return OK;  
}

int_ptr_t CSerial::Read(void *buf,int_ptr_t n)
{
    ASSERT(this->fd > 0);
    return read(this->fd,buf,n);
}
int_ptr_t CSerial::Write(const void *buf,int_ptr_t n)
{
    ASSERT(this->fd > 0);
    return write(this->fd,buf,n);
}
status_t CSerial::EnableDtrHandshake(bool enable)
{
    ASSERT(0);
    return OK;
}
bool CSerial::IsOpened()
{
    return fd > 0;
}
//////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
#if _IS_WINDOWS_
//////////////////////////////////////////////////////////////////
status_t CSerial::InitBasic()
{
    CFileBase::InitBasic();
    this->handle = NULL;    
    return OK;
}

status_t CSerial::Destroy()
{
    if(this->handle)
    {
        CloseHandle(this->handle);
        this->handle = NULL;
    }

    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CSerial::Open(const char *dev_name)
{
    this->handle = ::CreateFileA(dev_name,
        GENERIC_READ|GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        0,
        0);
    
    if(this->handle == INVALID_HANDLE_VALUE)
    {
        this->handle = NULL;
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "open file %s error\n",dev_name);
        return ERROR;
    }

    COMMTIMEOUTS to;

    //set non block mode
    GetCommTimeouts(handle,&to);
    to.ReadIntervalTimeout = MAXDWORD;
    to.ReadTotalTimeoutMultiplier = 0;
    to.ReadTotalTimeoutConstant = 0;
    to.WriteTotalTimeoutMultiplier = 0;
    to.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(handle,&to);

    return OK;
}
status_t CSerial::Configure(int baudrate,int databits,int stopbits,int parity)
{
    ASSERT(this->handle);

    DCB dcb;
    ::GetCommState(this->handle,&dcb);
    dcb.BaudRate = baudrate;

    switch (databits) 
    {   
        case 7:     
            dcb.ByteSize = 7;
            break;
        case 8:     
            dcb.ByteSize  = 8;  
            break;   
        default:    
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "Unsupported data size\n"); 
            return ERROR;
    }

    switch (parity) 
    {   
        case 'n':
        case 'N':
            dcb.Parity = NOPARITY;
            break;  
        case 'o':   
        case 'O':     
            dcb.Parity = ODDPARITY;
            break;  
        case 'e':  
        case 'E':   
            dcb.Parity = EVENPARITY;
            break;
        case 'S': 
        case 's':  /*as no parity*/   
        default:   
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,"Unsupported parity\n");    
            return ERROR;
    }  
    
    switch (stopbits)
    {   
        case 1:    
            dcb.StopBits = ONESTOPBIT;
            break;  
        case 2:    
            dcb.StopBits = TWOSTOPBITS;
            break;
        default:    
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,"Unsupported stop bits\n");
            return ERROR;
    } 

    return ::SetCommState(this->handle,&dcb);
}

int_ptr_t CSerial::Read(void *buf,int_ptr_t n)
{
    ASSERT(this->handle);
    DWORD rsize = 0;
    if(::ReadFile(this->handle, buf, (DWORD)n, &rsize, NULL))
    {
        return rsize;
    }
    else
    {
        return -1;
    }
}
int_ptr_t CSerial::Write(const void *buf,int_ptr_t n)
{
    ASSERT(this->handle);
    DWORD wsize = 0;
    if(::WriteFile(this->handle,buf,(DWORD)n,&wsize,NULL))
    {
        return wsize;
    }
    else
    {
        return -1;
    }
}
status_t CSerial::EnableDtrHandshake(bool enable)
{
    DCB dcb;
    ::GetCommState(this->handle,&dcb);
    
    if(enable)
    {
        dcb.fOutxDsrFlow = true;
        dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
    }
    else
    {
        dcb.fOutxDsrFlow = false;
        dcb.fDtrControl = DTR_CONTROL_DISABLE;      
    }

    return ::SetCommState(this->handle,&dcb);
}
bool CSerial::IsOpened()
{
    return handle != NULL;
}
//////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
#if _IS_ESP32_
//////////////////////////////////////////////////////////////////

#define BUF_SIZE 1024

status_t CSerial::InitBasic()
{
    CFileBase::InitBasic();
    this->fd = ERROR_FILE_HANDLE;
    return OK;
}

status_t CSerial::Destroy()
{
    if(this->fd >= 0)
    {
        uart_driver_delete(this->fd);
        this->fd = ERROR_FILE_HANDLE;
    }

    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CSerial::Open(int port_num)
{
    ASSERT(this->fd < 0);
    this->fd = port_num;
    return OK;
}

status_t CSerial::Configure(int baudrate,int databits,int stopbits,int parity)
{
    ASSERT(this->fd >= 0);

    uart_config_t uart_config;
    memset(&uart_config,0,sizeof(uart_config));
    
    uart_config.baud_rate = baudrate;
    uart_config.data_bits = (uart_word_length_t)databits;
    uart_config.parity    = (uart_parity_t)parity;
    uart_config.stop_bits = (uart_stop_bits_t)stopbits;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;
    uart_config.rx_flow_ctrl_thresh = UART_HW_FLOWCTRL_DISABLE;

    ESP_ERROR_CHECK(uart_driver_install(
        this->fd, BUF_SIZE , BUF_SIZE, 0, NULL, 0
    ));

    ESP_ERROR_CHECK(uart_param_config(this->fd, &uart_config));

    return OK;  
}

int_ptr_t CSerial::Read(void *buf,int_ptr_t n)
{
    ASSERT(this->fd >= 0);
    return uart_read_bytes(this->fd,buf,n,0);
}

int_ptr_t CSerial::Write(const void *buf,int_ptr_t n)
{
    ASSERT(this->fd >= 0);
    return uart_write_bytes(this->fd,buf,n);
}

status_t CSerial::EnableDtrHandshake(bool enable)
{
    ASSERT(0);
    return OK;
}

bool CSerial::IsOpened()
{
    return fd >= 0;
}

//////////////////////////////////////////////////////////////////
#endif  //ESP32
//////////////////////////////////////////////////////////////////
