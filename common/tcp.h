#ifndef __WIN_TCP_H
#define __WIN_TCP_H

#include "filebase.h"
#include "mem.h"

class CSocket : public CFileBase
{
public:
    int32_t socket_num;
public:
    int32_t GetSocketFd();
    static bool IsIpAddress(CMem *str);
    bool CanReadOrWrite(bool check_read);
    bool CanRead();
    bool CanWrite();
    status_t TransferSocketFd(CSocket *from);
    status_t AttachSocketFd(int32_t fd);
    static status_t GetAllIP(CFileBase *file);
    bool IsConnected();
    status_t SetBlocking(status_t enable);
    virtual fsize_t GetMaxSize();
    virtual status_t AddBlock(fsize_t bsize);
    virtual fsize_t Seek(fsize_t o);
    virtual status_t SetSize(fsize_t s);
    virtual fsize_t GetSize();
    virtual fsize_t GetOffset();
    virtual status_t CloseConnect();
    virtual int_ptr_t Write(const void *buf, int_ptr_t n);
    virtual int_ptr_t Read(void *buf, int_ptr_t n);
    static void StartNet(void);
    static void EndNet(void);
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
    CSocket();
    virtual ~CSocket();
};
///////////////////////////////////////////////////////////////////////////
class CTcpServer:public CSocket{
public:
    struct  hostent  *ptrh;       /* pointer to a host table entry          */     
    struct  sockaddr_in sad;      /* structure to hold server's address     */   
    struct  sockaddr_in cad;      /* structure to hold client's address     */    
    int32_t max_connect;          /* max connection */
public:
    status_t GetClientIpAndPort(CFileBase *ip, int32_t *port);
    status_t SetMaxConnect(int32_t m);
    int32_t Accept();
    status_t SetPort(int32_t port);
    status_t InitServer(void);
    CTcpServer();
    ~CTcpServer();   
    status_t Init();
    status_t Destroy();
};
///////////////////////////////////////////////////////////////////////////
class CTcpClient:public CSocket{
public:
    struct  hostent  *ptrh;   /* pointer to a host table entry       */ 
    struct  protoent *ptrp;   /* pointer to a protocol table entry   */ 
    struct  sockaddr_in sad;  /* structure to hold an IP address     */
    int32_t port;             /* protocol port number       */         
public: 
    status_t IsConnectComplete();
    status_t Destroy();
    CTcpClient();
    ~CTcpClient();   
    status_t Init();
    status_t SetServerIP(const char *name);
    status_t SetPort(int32_t port);
    status_t Connect();
};

#endif
