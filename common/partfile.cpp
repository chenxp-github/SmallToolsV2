#include "partfile.h"
#include "mem_tool.h"
#include "syslog.h"

CPartFile::CPartFile()
{
    this->InitBasic();
}
CPartFile::~CPartFile()
{
    this->Destroy();
}
int CPartFile::InitBasic()
{
    this->i_host_file = NULL;
    this->offset = 0;
    this->size = 0;
    this->host_off = 0;
    this->max_size = 0;
    return OK;
}
int CPartFile::Init()
{
    this->InitBasic();
    CFileBase::Init();
    //add your code
    return OK;
}
int CPartFile::Destroy()
{
    //add your code
    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CPartFile::Copy(CPartFile *from)
{
    ASSERT(from);
    this->i_host_file = from->i_host_file;
    this->offset = from->offset;
    this->max_size = from->max_size;
    this->size = from->size;
    this->host_off = from->host_off;
    return OK;
}

fsize_t CPartFile::GetOffset()
{
    return this->offset;
}

fsize_t CPartFile::GetSize()
{
    return this->size;
}

int_ptr_t CPartFile::Read(void *buf, int_ptr_t n)
{
    ASSERT(this->i_host_file);

    if(n <= 0)return 0;
    fsize_t old_host_offset = i_host_file->GetOffset();

    this->i_host_file->Seek(this->host_off + this->offset);
    fsize_t max_rs = this->GetSize() - this->offset;

    if(n > max_rs)n = (int_ptr_t)max_rs;
    this->offset += n;

    int_ptr_t read_size = this->i_host_file->Read(buf,n);
    i_host_file->Seek(old_host_offset);
	return read_size;
}

int_ptr_t CPartFile::Write(const void *buf, int_ptr_t n)
{
    ASSERT(this->i_host_file);

    if(n <= 0)return 0;

    if(n + this->offset > this->size)
    {
        this->AddBlock(n + this->offset - this->size);
    }

    fsize_t old_host_offset = i_host_file->GetOffset();
    this->i_host_file->Seek(this->host_off + this->offset);
    fsize_t max_ws = this->GetSize() - this->offset;

    if(n > max_ws) n = (int_ptr_t)max_ws;
    this->offset += n;

    int_ptr_t write_size = this->i_host_file->Write(buf,n);
    i_host_file->Seek(old_host_offset);
	return write_size;
}

int CPartFile::SetSize(fsize_t s)
{
    if(s > this->GetMaxSize())
        s = this->GetMaxSize();
    if(s < 0) s = 0;
    this->size = s;
    this->offset = 0;
    return OK;
}

fsize_t CPartFile::GetMaxSize()
{
    return this->max_size;
}

int CPartFile::AddBlock(fsize_t bsize)
{
    fsize_t max = this->GetMaxSize();
    this->size += bsize;
    if(this->size > max)
        this->size = max;
    return OK;
}

fsize_t CPartFile::Seek(fsize_t off)
{
    if(this->offset > this->size)
        this->offset = this->size;
    if(off < 0 )off = 0;
    this->offset = off;
    return this->offset;
}

int CPartFile::SetHostFile(CFileBase *host, fsize_t off, fsize_t size)
{
    ASSERT(host);
    this->i_host_file = host;
    this->host_off = off;
    this->max_size = size;
    if(size > host->GetMaxSize() - this->host_off)
        this->max_size = host->GetSize() - this->host_off;
    this->SetSize(this->max_size);
    return OK;
}

fsize_t CPartFile::GetHostOffset()
{
    return this->host_off;
}

