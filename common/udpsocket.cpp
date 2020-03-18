#include "udpsocket.h"
#include "syslog.h"
#include "mem_tool.h"

CUdpSocket::CUdpSocket()
{
	this->InitBasic();
}
CUdpSocket::~CUdpSocket()
{
	this->Destroy();
}
status_t CUdpSocket::InitBasic()
{
    WEAK_REF_CLEAR();
	m_sock_fd = 0;
	memset(&m_addr,0,sizeof(m_addr));
	memset(&m_destaddr,0,sizeof(m_destaddr));
	memset(&m_srcaddr,0,sizeof(m_srcaddr));
	return OK;
}
status_t CUdpSocket::Init()
{
	this->InitBasic();
    
	return OK;
}
status_t CUdpSocket::Destroy()
{
	WEAK_REF_DESTROY();
	if(m_sock_fd)
	{
		close(m_sock_fd);
		m_sock_fd = 0;
	}
	this->InitBasic();
	return OK;
}

status_t CUdpSocket::Create()
{
	ASSERT(m_sock_fd == 0);
	m_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	ASSERT(m_sock_fd > 0);
	return OK;
}

status_t CUdpSocket::Bind(int port)
{
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	m_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int res = bind(m_sock_fd, (struct sockaddr *)&m_addr, sizeof(m_addr));
	if(res)
	{
		close(m_sock_fd);
		m_sock_fd = 0;
	}
	
	return res == 0;
}

status_t CUdpSocket::SetDestIpAndPort(const char *ip, int port)
{
	ASSERT(ip);
	m_destaddr.sin_family = AF_INET;
	m_destaddr.sin_port = htons(port);
	m_destaddr.sin_addr.s_addr = inet_addr(ip);
	return OK;
}
status_t CUdpSocket::SendMsg(const void *data, int data_len)
{
	ASSERT(m_sock_fd > 0);
	ASSERT(data);
	
	int n = sendto(m_sock_fd, 
		(const char*)data, data_len, 0, 
		(struct sockaddr *)&m_destaddr, 
		sizeof(m_destaddr)
	);
	return n >= 0;
}

status_t CUdpSocket::SendMsg(CMem *data)
{
	ASSERT(data);
	return this->SendMsg(data->GetRawBuf(),(int)data->GetSize());
}

status_t CUdpSocket::RecvMsg(CMem *data)
{
	ASSERT(data);
	ASSERT(m_sock_fd > 0);
	
	struct sockaddr_in srcaddr;
	socklen_t src_addr_len = sizeof(srcaddr);

	data->SetSize(0);
	int n = recvfrom(m_sock_fd, 
		data->GetRawBuf(),(int)data->GetMaxSize(), 0, 
		(struct sockaddr *)&srcaddr, 
		&src_addr_len
	);

	if(n <= 0)return ERROR;
	memcpy(&m_srcaddr,&srcaddr,sizeof(srcaddr));
	data->SetSize(n);
	return OK;
}


status_t CUdpSocket::SetBlocking(status_t enable)
{
    return crt_set_blocking_mode(this->m_sock_fd,enable);
}

struct sockaddr_in *CUdpSocket::GetSrcAddr()
{
	return &m_srcaddr;
}

status_t CUdpSocket::GetSrcAddr(CMem *ip, int *port)
{
	ASSERT(ip && port);
	ip->SetSize(0);
	ip->Puts(inet_ntoa(m_srcaddr.sin_addr));
	*port = m_srcaddr.sin_port;
	return OK;
}