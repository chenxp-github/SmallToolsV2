#include "icmp_socket.h"
#include "sys_log.h"
#include "mem_tool.h"

CIcmpSocket::CIcmpSocket()
{
    this->InitBasic();
}

CIcmpSocket::~CIcmpSocket()
{
    this->Destroy();
}

status_t CIcmpSocket::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
/*##End InitBasic##*/
    m_sock = -1;
    memset(&m_addr,0,sizeof(m_addr));
    return OK;
}

status_t CIcmpSocket::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CIcmpSocket::Destroy()
{
    if(m_sock >= 0)
    {
        crt_closesocket(m_sock);
        m_sock = -1;
    }
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CIcmpSocket::Copy(CIcmpSocket *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
/*##End Copy##*/
    return OK;
}

int CIcmpSocket::Comp(CIcmpSocket *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CIcmpSocket::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
/*##End Print##*/
    return OK;
}

/*@@ Insert Function Here @@*/
status_t CIcmpSocket::SetDestIp(const char *ip)
{
    ASSERT(ip);

    m_addr.sin_family = AF_INET;

	struct in_addr in_addr;
    in_addr.s_addr = inet_addr(ip);

    if(in_addr.s_addr == INADDR_NONE)
    {
        return ERROR;
    }
    else
    {
        m_addr.sin_addr.s_addr = in_addr.s_addr;
    }

    return OK;
}

status_t CIcmpSocket::Create()
{
    ASSERT(m_sock < 0);
    m_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(m_sock < 0) 
    {
        return ERROR;
    }
    crt_set_blocking_mode(m_sock,0);
    return OK;
}

uint16_t CIcmpSocket::CalculateChecksum(const uint8_t* buffer, int bytes)
{
    uint32_t checksum = 0;
    const uint8_t* end = buffer + bytes;
	
    // odd bytes add last byte and reset end
    if (bytes % 2 == 1) 
    {
        end = buffer + bytes - 1;
        checksum += (*end) << 8;
    }
	
    // add words of two bytes, one by one
    while (buffer < end) 
    {
        checksum += buffer[0] << 8;
        checksum += buffer[1];
        buffer += 2;
    }
	
    // add carry if any
    uint32_t carray = checksum >> 16;
    while (carray) 
    {
        checksum = (checksum & 0xffff) + carray;
        carray = checksum >> 16;
    }
	
    // negate it
    checksum = ~checksum;
	
    return checksum & 0xffff;
}

//will skip checksum bytes
uint16_t CIcmpSocket::CalculateIcmpChecksum(uint8_t *buffer, int bytes)
{
    ASSERT(buffer);
    uint16_t t;
    memcpy(&t,buffer+2,sizeof(t));
    buffer[2] = 0;
    buffer[3] = 0;
    uint16_t checksum = CalculateChecksum(buffer,bytes);
    memcpy(buffer+2,&t,sizeof(t));
    return checksum;
}

status_t CIcmpSocket::SendMsg(uint8_t type, uint8_t code,const void *data, int data_size)
{
    ASSERT(data);
    ASSERT(m_sock > 0);

    LOCAL_MEM(tmp);
    tmp.Putc(type);
    tmp.Putc(code);
    tmp.Putc(0); //checksum
    tmp.Putc(0); //checksum
    tmp.Write(data,data_size);

    uint16_t checksum =  CalculateChecksum(
        (const uint8_t*)tmp.GetRawBuf(),
        (int)tmp.GetSize()
    );

    checksum = htons(checksum);
    tmp.Seek(2);
    tmp.Write(&checksum,sizeof(checksum));

    int s = sendto(
		m_sock,tmp.GetRawBuf(),
		(int)tmp.GetSize(),0,
        (const struct sockaddr *)&m_addr,
        sizeof(m_addr)
    );

    return s > 0;
}

status_t CIcmpSocket::RecvMsg(CMem *data)
{
	ASSERT(data);
	ASSERT(m_sock > 0);
	
	struct sockaddr_in srcaddr;
	socklen_t src_addr_len = sizeof(srcaddr);

	data->SetSize(0);
	int n = recvfrom(m_sock, 
		data->GetRawBuf(),(int)data->GetMaxSize(), 0, 
		(struct sockaddr *)&srcaddr, 
		&src_addr_len
	);

	if(n <= 0)return ERROR;
	data->SetSize(n);
	return OK;
}

status_t CIcmpSocket::SendEchoMsg(uint16_t id, uint16_t seq, const uint8_t *data, int bytes)
{
    LOCAL_MEM(tmp);
    uint16_t t = htons(id);
    tmp.Write(&t,sizeof(t));
    
    t = htons(seq);
    tmp.Write(&t,sizeof(t));

    if(data && bytes>0)
    {
        tmp.Write(data,bytes);
    }
    
    return SendMsg(8,0,tmp.GetRawBuf(),(int)tmp.GetSize());
}

int CIcmpSocket::IpHeaderSize(CMem *data)
{
    ASSERT(data);
	fsize_t off = data->GetOffset();
	data->Seek(0);
	uint8_t v = data->Getc();
	data->Seek(off);
	return v&0x0f;
}

status_t CIcmpSocket::RecvIcmpMsg(CMem *data,uint8_t *type, uint8_t *code)
{
    ASSERT(data);

    LOCAL_MEM(tmp);

    if(!this->RecvMsg(&tmp))
    {
        return ERROR;
    }
    
	int header_size = IpHeaderSize(&tmp)*4;
    tmp.Seek(header_size);
    uint8_t v8 = tmp.Getc();
    if(type)(*type) = v8;
    v8 = tmp.Getc();
    if(code)(*code)=v8;
    
    uint16_t checksum = 0;
    tmp.Read(&checksum,sizeof(checksum));
    checksum = ntohs(checksum);

	uint16_t self_checksum = CalculateIcmpChecksum(
        (uint8_t*)(tmp.GetRawBuf() + header_size),
        (int)(tmp.GetSize() - header_size)
    );

    if(checksum != self_checksum)
    {
        return ERROR;
    }

    data->SetSize(0);    
    data->WriteFile(&tmp,
        4+header_size,
        tmp.GetSize()-4-header_size
    );

    return OK;
}


