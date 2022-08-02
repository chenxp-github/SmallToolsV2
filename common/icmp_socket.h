#ifndef __ICMP_SOCKET_H
#define __ICMP_SOCKET_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CIcmpSocket
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

/*##End Members##*/
    int m_sock;
    struct sockaddr_in m_addr;
public:
    CIcmpSocket();
    virtual ~CIcmpSocket();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CIcmpSocket *_p);
    int Comp(CIcmpSocket *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/

    status_t SetDestIp(const char *ip);
    status_t Create();
    static uint16_t CalculateChecksum(const uint8_t *buffer, int bytes);
    status_t SendMsg(uint8_t type, uint8_t code,const void *data, int data_size);
    status_t RecvMsg(CMem *data);

    status_t SendEchoMsg(uint16_t id, uint16_t seq, const uint8_t *data, int bytes);
    status_t RecvIcmpMsg(CMem *data, uint8_t *type=NULL, uint8_t *code=NULL);

    static int IpHeaderSize();    
    static uint16_t CalculateIcmpChecksum(uint8_t *buffer, int bytes);
};

#endif

