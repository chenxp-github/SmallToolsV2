#include "stream.h"
#include "syslog.h"
#include "mem_tool.h"

CStream::CStream()
{
    this->InitBasic();
}
CStream::~CStream()
{
    this->Destroy();
}
status_t CStream::InitBasic()
{
	CMem::InitBasic();
	return OK;
}
status_t CStream::Init()
{
	this->InitBasic();
	CMem::Init();
	return OK;
}
status_t CStream::Destroy()
{
    CMem::Destroy();
    this->InitBasic();
    return OK;
}
int CStream::Comp(CStream *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CStream::Copy(CStream *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    CMem::Copy(_p);
    return OK;
}
status_t CStream::Print(CFileBase *_buf)
{
    CMem::Print(_buf);
    return OK;
}

status_t CStream::PutInt8(int8_t i)
{
    return Write(&i,sizeof(i));
}

int8_t CStream::GetInt8()
{
    int8_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutInt16(int16_t i)
{
    return Write(&i,sizeof(i));
}

int16_t CStream::GetInt16()
{
    int16_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutInt32(int32_t i)
{
    return Write(&i,sizeof(i));
}

int32_t CStream::GetInt32()
{
    int32_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutInt64(int64_t i)
{
    return Write(&i,sizeof(i));
}

int64_t CStream::GetInt64()
{
    int64_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutUInt8(uint8_t i)
{
    return Write(&i,sizeof(i));
}

uint8_t CStream::GetUInt8()
{
    uint8_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutUInt16(uint16_t i)
{
    return Write(&i,sizeof(i));
}

uint16_t CStream::GetUInt16()
{
    uint16_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutUInt32(uint32_t i)
{
    return Write(&i,sizeof(i));
}

uint32_t CStream::GetUInt32()
{
    uint32_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutUInt64(uint64_t i)
{
    return Write(&i,sizeof(i));
}

uint64_t CStream::GetUInt64()
{
    uint64_t i = 0;
    Read(&i,sizeof(i));
    return i;
}

status_t CStream::PutBool(bool b)
{
    int8_t t = b;
    return Write(&t,sizeof(t));
}

bool CStream::GetBool()
{
    int8_t t = 0;
    Read(&t,sizeof(t));
    return t != 0;
}
status_t CStream::PutDouble(double d)
{
    return Write(&d,sizeof(d));
}

double CStream::GetDouble()
{
    double t = 0;
    Read(&t,sizeof(t));
    return t;
}

status_t CStream::PutZeroEndString(const char *str)
{
    ASSERT(str);
    this->Puts(str);
    this->Putc(0);
    return OK;
}

status_t CStream::GetZeroEndString(CMem *str)
{
    ASSERT(str);
    char ch;
    int_ptr_t size = 0;
    str->Destroy();
    str->Init();

    const char *p = GetRawBuf() + GetOffset();
    while(true)
    {
        ch = p[size++];
        if(ch == 0)break;
    }
    this->Seek(GetOffset()+size);
    str->SetRawBuf((void*)p,size,true);
    return size;
}

status_t CStream::PutBinary(CMem *bin)
{
    ASSERT(bin);
    this->PutInt32((int32_t)bin->GetSize());
    this->Puts(bin);
    return OK;
}

status_t CStream::GetBinary(CMem *bin)
{
    ASSERT(bin);

    bin->Destroy();
    bin->Init();

    int32_t size = 0;
    size = this->GetInt32();

    if(size > 0)
    {
        bin->SetRawBuf(this->GetRawBuf()+this->GetOffset(),size,true);
        this->Seek(this->GetOffset()+size);
    }
    return OK;
}

status_t CStream::PutString(CMem *str)
{
    ASSERT(str);
    this->PutBinary(str);
    this->Putc(0); //zero end
    return OK;
}

status_t CStream::GetString(CMem *str)
{
    ASSERT(str);
    this->GetBinary(str);
    this->GetInt8(); //zero end
    return OK;
}

status_t CStream::PutString(const char *str)
{
    ASSERT(str);
    CMem mem(str);
    return this->PutString(&mem);
}

status_t CStream::GetAsString(CMem *str, int len)
{
    ASSERT(str);
    const char *p = GetRawBuf() + GetOffset();
    for(int i = 0; i < len; i++)
    {
        str->Putc(p[i]);
    }
    str->Putc(0);
    str->SetSize(len);
    return OK;
}
