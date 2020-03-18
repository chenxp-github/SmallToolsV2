#include "tcp.h"
#include "syslog.h"
#include "mem_tool.h"
#include "misc.h"

#define READ_STATUS     1
#define WRITE_STATUS    2
#define EXCPT_STATUS    3

CSocket::CSocket()
{
    this->InitBasic();
}
CSocket::~CSocket()
{
    this->Destroy();
}
status_t CSocket::InitBasic()
{
    CFileBase::InitBasic();
    this->socket_num = -1;
    return OK;
}
status_t CSocket::Init()
{
    this->InitBasic();
    CFileBase::Init();    
    return OK;
}
status_t CSocket::Destroy()
{
    CFileBase::Destroy();
    this->CloseConnect();
    this->InitBasic();
    return OK;
}
status_t CSocket::CloseConnect()
{
    if(this->socket_num >= 0)
    {
        crt_closesocket(this->socket_num);
        this->socket_num = -1;
    }
    return OK;
}
int_ptr_t CSocket::Write(const void *buf, int_ptr_t n)
{
    int_ptr_t ret;
    ASSERT(this->socket_num > 0);
    if(n <= 0)return 0;
    ret = crt_send(this->socket_num,(const char*)buf,n,0);
    if(ret ==SOCKET_ERROR)
    {
        if(crt_is_socket_broken())
            this->CloseConnect();
        return 0;
    }
    else if(ret == 0)
    {
        this->CloseConnect();
    }
    return ret;
}

int_ptr_t CSocket::Read(void *buf, int_ptr_t n)
{
    int_ptr_t ret;
    ASSERT(this->socket_num > 0);
    if(n <= 0) return 0;

    ret = crt_recv(this->socket_num,(char*)buf,n,0);
    if(ret==SOCKET_ERROR)
    {
        if(crt_is_socket_broken())
            this->CloseConnect();
        return 0;
    }
    else if(ret == 0)
    {
        this->CloseConnect();
    }
    return ret;
}
void CSocket::StartNet(void)
{
    crt_socket_start();
}
void CSocket::EndNet(void)
{
    crt_socket_end();
}
fsize_t CSocket::GetOffset()
{
    return 0;
}
fsize_t CSocket::GetSize()
{
    return MAX_FSIZE;
}
status_t CSocket::SetSize(fsize_t s)
{
    return OK;
}
fsize_t CSocket::Seek(fsize_t o)
{
    return 0;
}
status_t CSocket::AddBlock(fsize_t bsize)
{
    return 0;
}
fsize_t CSocket::GetMaxSize()
{
    return MAX_FSIZE;
}

status_t CSocket::SetBlocking(status_t enable)
{
    return crt_set_blocking_mode(this->socket_num,enable);
}

bool CSocket::IsConnected()
{
    return this->socket_num > 0;
}

status_t CSocket::GetAllIP(CFileBase *file)
{
    char buf[1024];
    crt_get_all_ip(buf);
    file->SetSize(0);
    file->Puts(buf);
    return OK;
}

status_t CSocket::AttachSocketFd(int32_t fd)
{
    this->socket_num = fd;
    return OK;
}

status_t CSocket::TransferSocketFd(CSocket *from)
{
    ASSERT(from);
    this->socket_num = from->socket_num;
    from->socket_num = -1;
    return OK;
}

bool CSocket::CanReadOrWrite(bool check_read)
{
    if(!IsConnected())
        return false;

    fd_set l_fdset;

    FD_ZERO(&l_fdset);
    FD_SET(socket_num,&l_fdset);

    timeval mWaitTime;
    mWaitTime.tv_sec = 0;
    mWaitTime.tv_usec = 0;

    if(check_read)
        ::select(0, &l_fdset, NULL ,  NULL,  &mWaitTime);
    else
        ::select(0, NULL,  &l_fdset,  NULL,  &mWaitTime);

    if (FD_ISSET(socket_num,&l_fdset))
        return true;
    return false;
}

bool CSocket::CanWrite()
{
    return this->CanReadOrWrite(false);
}
bool CSocket::CanRead()
{
    return this->CanReadOrWrite(true);
}

bool CSocket::IsIpAddress(CMem *str)
{
    ASSERT(str);

    LOCAL_MEM(mem);

    str->SetSplitChars(".");
    str->Seek(0);

    int c = 0;
    while(str->ReadString(&mem))
    {
        if(!is_dec(mem.CStr()))
            break;
        c++;
    }
    
    return c == 4;
}

int32_t CSocket::GetSocketFd()
{
    return socket_num;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CTcpServer::CTcpServer()
{
}
CTcpServer::~CTcpServer()
{
    this->Destroy();
}
status_t CTcpServer::Init()
{
    CSocket::Init();
    crt_memset((void*)&this->sad,0,sizeof(struct  sockaddr_in));
    crt_memset((void*)&this->cad,0,sizeof(struct  sockaddr_in));
    this->ptrh = NULL;
    this->socket_num = -1;
    this->max_connect = 10;
    return OK;
}
status_t  CTcpServer::Destroy()
{
    CSocket::Destroy();
    return OK;
}
status_t CTcpServer::InitServer()
{
    crt_memset((char *)&sad,0,sizeof(sad));  /* clear sockaddr structure   */
    sad.sin_family = AF_INET;            /* set family to Internet     */
    sad.sin_addr.s_addr = INADDR_ANY;    /* set the local IP address   */
    this->socket_num = crt_socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket_num < 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "crt_socket creation failed!\r\n"
        );
        return ERROR;
    }
    return OK;
}
status_t CTcpServer::SetPort(int32_t port)
{
    sad.sin_port = crt_htons((u_short)port);
    if (crt_bind(this->socket_num, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "bind failed on port %d!\r\n",port
        );
        return ERROR;
    }
    if (crt_listen(this->socket_num, this->max_connect) < 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "listen failed!\r\n"
        );
        return ERROR;
    }
    return OK;
}
int32_t CTcpServer::Accept()
{
    int32_t alen = sizeof(cad);
    int32_t snum;
    if ( (snum = crt_accept(this->socket_num, (struct sockaddr *)&cad, &alen)) < 0)
    {
        snum = -1;
    }
    return snum;
}
status_t CTcpServer::SetMaxConnect(int32_t m)
{
    this->max_connect = m;
    return OK;
}

status_t CTcpServer::GetClientIpAndPort(CFileBase *ip, int32_t *port)
{
    ASSERT(ip && port);
    char *pszAddr=crt_inet_ntoa(cad.sin_addr);
    ASSERT(pszAddr);
    ip->SetSize(0);
    ip->Puts(pszAddr);
    *port = cad.sin_port;
    return OK;
}
/////////////////////////////////////////////
CTcpClient::CTcpClient()
{
}
CTcpClient::~CTcpClient()
{
    this->Destroy();
}
status_t CTcpClient::Init()
{
    CSocket::Init();
    crt_memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;           /* set family to Internet*/
    this->socket_num = -1;
    return OK;
}
status_t CTcpClient::Destroy()
{
    CSocket::Destroy();
    return OK;
}
status_t CTcpClient::SetServerIP(const char *name)
{
    struct in_addr addr;
    addr.s_addr = inet_addr(name);
    if(addr.s_addr == INADDR_NONE)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "invalidate ip %s\n",name
        );
        return ERROR;
    }
    else
    {
        sad.sin_addr.s_addr = addr.s_addr;
    }
    return OK;
}
status_t CTcpClient::SetPort(int32_t port)
{
    if (port > 0)
    {
        sad.sin_port = crt_htons((u_short)port);
    }
    else
    {
        return ERROR;
    }

    if(this->socket_num >= 0)
    {
        return ERROR;
    }

    this->socket_num = crt_socket(AF_INET, SOCK_STREAM, 0);

    if (this->socket_num < 0)
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "crt_socket creation failed!\r\n"
        );
        return ERROR;
    }
    return OK;
}
status_t CTcpClient::Connect()
{
    ASSERT(this->socket_num >0);
    int32_t ret = crt_connect(this->socket_num, (struct sockaddr *)&sad, sizeof(sad));
    if(ret < 0)
    {
        return ERROR;
    }
    return OK;
}
status_t CTcpClient::IsConnectComplete()
{
    ASSERT(this->socket_num > 0);
    return crt_is_connect_complete(this->socket_num);
}
