#ifndef __S2_UDPSOCKET_H
#define __S2_UDPSOCKET_H

#include "cruntime.h"
#include "mem.h"

class CUdpSocket{
public: 
    WEAK_REF_ID_DEFINE();
public:
	int m_sock_fd;
	struct sockaddr_in m_addr;
	struct sockaddr_in m_destaddr;
	struct sockaddr_in m_srcaddr;
public:
	CUdpSocket();
	virtual ~CUdpSocket();
	status_t InitBasic();
	status_t Init();
	status_t Destroy();

	status_t UseNonBlockingMode(bool non_blocking);
	status_t Create();	
	status_t Bind(int port);
	status_t SetDestIpAndPort(const char *ip, int port);
	status_t SendMsg(const void *data, int data_len);
	status_t SendMsg(CMem *data);
	status_t RecvMsg(CMem *data);
	struct sockaddr_in *GetSrcAddr();
	status_t GetSrcAddr(CMem *ip, int *port);
};

#endif
