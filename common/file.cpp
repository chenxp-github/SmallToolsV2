#include "syslog.h"
#include "file.h"
#include "mem_tool.h"

CFile::CFile()
{
    this->InitBasic();
}
CFile::~CFile()
{
    this->Destroy();
}

status_t CFile::InitBasic()
{
    CFileBase::InitBasic();
    this->fp = ERROR_FILE_HANDLE;
    this->buf_size = 0;
    this->buf_start = -1;
    this->file_buf = NULL;
    this->init_buf_size = 0;
    this->offset = 0;
    this->size = 0;
    return OK;
}

status_t CFile::Init()
{
    this->InitBasic();
    CFileBase::Init();
    return OK;
}
status_t CFile::Destroy()
{
    this->CloseFile();
    CFileBase::Destroy();
    return OK;
}
status_t CFile::OpenFile(const char *fn , const char *mode)
{
    ASSERT(!crt_is_file_handle(this->fp));
    fp = crt_fopen(fn,mode);
    if(!crt_is_file_handle(this->fp)) 
        return ERROR;
    
    this->GetSizeReal();
    this->BufferBlock(0);
    return OK;
}
status_t CFile::CloseFile()
{   
    if(crt_is_file_handle(this->fp))
    {
        crt_fclose(this->fp);
        this->fp = ERROR_FILE_HANDLE;
    }
    FREE(this->file_buf);   
    return OK;
}
fsize_t CFile::GetSize()
{
    return this->size;
}
fsize_t CFile::GetSizeReal()
{
    fsize_t old_off,size;
    
    old_off = crt_ftell(this->fp);
    crt_fseek(this->fp,0,SEEK_END);
    size = crt_ftell(this->fp);
    crt_fseek(this->fp,old_off,SEEK_SET);
    this->size = size;
    return size;    
}

fsize_t CFile::Seek(fsize_t nOffset)
{
    if(nOffset > this->GetSize())
        nOffset = this->GetSize();

    if(nOffset < 0 )
        nOffset = 0;

    this->offset = nOffset;
    return this->offset;
}

int_ptr_t CFile::Read(void *buf,int_ptr_t nByteToRead)
{
    fsize_t old_off,rs;

    if(nByteToRead <= 0)return 0;
    old_off = this->offset; 
    
    if(this->HasBuffer())
    {
        if(old_off >= this->buf_start && (old_off + nByteToRead <= this->buf_start + this->buf_size))
        {
            crt_memcpy(buf,this->file_buf+(old_off-this->buf_start),nByteToRead);
            this->Seek(old_off + nByteToRead);          
        }
        else
        {           
            crt_fseek(this->fp,this->offset,SEEK_SET);
            rs = crt_fread(this->fp,(void*)buf,nByteToRead);
            this->offset += rs;

            if(old_off < this->buf_start)
                this->BufferBlock(old_off - this->init_buf_size + nByteToRead);
            else
                this->BufferBlock(old_off);
        }    
    }
    else
    {
        crt_fseek(this->fp,this->offset,SEEK_SET);
        rs = crt_fread(this->fp,(void*)buf,nByteToRead);
        this->offset += rs;
    }

    return (int_ptr_t)(this->offset - old_off);
}

int_ptr_t CFile::Write(const void *buf,int_ptr_t nByteToWrite)
{
    fsize_t old_off,ws;
    
    if(nByteToWrite <= 0)return 0;

    old_off = this->offset;
    crt_fseek(this->fp,this->offset,SEEK_SET);
    ws = crt_fwrite( this->fp,(void*)buf, nByteToWrite);
    this->offset += ws;

    if(this->offset >= this->size)
        this->size = this->offset;
    return (int_ptr_t)(this->offset - old_off);
}

status_t CFile::SetSize(fsize_t nSize)
{
    this->size = nSize; 
    this->offset = 0;
    return OK;
}

status_t CFile::FindFile(const char *fn)
{
    FILE_HANDLE fp;
    fp=crt_fopen(fn,"rb");
    if(!crt_is_file_handle(fp))
        return FALSE;
    crt_fclose(fp);
    return TRUE;
}

fsize_t CFile::GetOffset()
{
    return this->offset;
}
status_t CFile::AddBlock(fsize_t bsize)
{
    return OK;
}

fsize_t CFile::GetMaxSize()
{
    return this->GetSize();
}

status_t CFile::HasBuffer()
{
    return this->file_buf != NULL;
}

status_t CFile::SetBufSize(int_ptr_t size)
{
    ASSERT(!IsOpened());
    FREE(this->file_buf);
    MALLOC(this->file_buf,char,size);
    this->buf_size = size;
    this->buf_start = -1;
    this->init_buf_size = size;

    return OK;
}

status_t CFile::BufferBlock(fsize_t start_pos)
{   
    if(!this->HasBuffer())
        return ERROR;
    if(start_pos < 0)start_pos = 0;
    if(this->buf_start == start_pos)
        return OK;
    crt_fseek(fp,start_pos,SEEK_SET);
    int_ptr_t rs  = crt_fread(fp,this->file_buf,init_buf_size);
    this->buf_start = start_pos;
    this->buf_size = (int_ptr_t)rs;
    return OK;
}

bool CFile::IsOpened()
{
    return crt_is_file_handle(this->fp) != 0;
}
