#include "bigendian.h"
#include "syslog.h"
#include "mem_tool.h"

template<typename T>
inline int_ptr_t WriteBigEndian(CFileBase *file, T val) 
{
    char buf[sizeof(T)];
    for (size_t i = 0; i < sizeof(T); i++) 
    {
        buf[sizeof(T)-i-1] = static_cast<char>(val & 0xFF);
        val >>= 8;
    }
    return file->Write(buf,sizeof(T));
}

template<typename T>
inline int_ptr_t ReadBigEndian(CFileBase *file, T* out) 
{
    uint8_t tmp[sizeof(T)];
    int_ptr_t ret = file->Read(tmp,sizeof(T));
    *out = tmp[0];
    for (size_t i = 1; i < sizeof(T); i++) 
    {
        *out <<= 8;
        *out |= tmp[i];
    }
    return ret;
}
/////////////////////////////////////////
CBigEndian::CBigEndian()
{
    this->InitBasic();
}
CBigEndian::~CBigEndian()
{
    this->Destroy();
}
status_t CBigEndian::InitBasic()
{
    i_File = NULL;
    return OK;
}
status_t CBigEndian::Init(CFileBase *file)
{
    this->InitBasic();
    this->AttachFile(file);
    return OK;
}
status_t CBigEndian::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CBigEndian::AttachFile(CFileBase *file)
{
    ASSERT(file);
    i_File = file;
    return OK;
}

int_ptr_t CBigEndian::WriteBytes(const void *buf, size_t len)
{
    ASSERT(buf);
    return i_File->Write(buf,len);
}

int_ptr_t CBigEndian::ReadBytes(void *buf, size_t len)
{
	ASSERT(buf);
	return i_File->Read(buf,len);
}
int_ptr_t CBigEndian::WriteU8(uint8_t value)
{
    return WriteBigEndian(i_File,value);
}

int_ptr_t CBigEndian::WriteU16(uint16_t value)
{
    return WriteBigEndian(i_File,value);
}

int_ptr_t CBigEndian::WriteU32(uint32_t value)
{
    return WriteBigEndian(i_File,value);
}

int_ptr_t CBigEndian::WriteU64(uint64_t value)
{
    return WriteBigEndian(i_File,value);
}

status_t CBigEndian::Seek(fsize_t offset)
{
    ASSERT(i_File);
    i_File->Seek(offset);
    return OK;
}

status_t CBigEndian::Skip(int_ptr_t size)
{
    ASSERT(i_File);
    fsize_t off = i_File->GetOffset();
    i_File->Seek(size+off);
    return OK;
}

int_ptr_t CBigEndian::ReadU8(uint8_t *out)
{
    return ReadBigEndian(i_File,out);
}

int_ptr_t CBigEndian::ReadU16(uint16_t *out)
{
    return ReadBigEndian(i_File,out);
}

int_ptr_t CBigEndian::ReadU32(uint32_t *out)
{
    return ReadBigEndian(i_File,out);
}

int_ptr_t CBigEndian::ReadU64(uint64_t *out)
{
    return ReadBigEndian(i_File,out);
}

int_ptr_t CBigEndian::GetLeftSize()
{
	ASSERT(i_File);
	return (int_ptr_t)(i_File->GetSize() - i_File->GetOffset());
}
