#include "memfile.h"
#include "syslog.h"
#include "mem_tool.h"

#define BLOCK_AND_OFFSET(raw_offset,block,off) \
int_ptr_t block = (int_ptr_t)(raw_offset >> this->shift_n);\
int_ptr_t off = (int_ptr_t)(raw_offset & this->mod_n)\

CMemFile::CMemFile()
{

}
CMemFile::~CMemFile()
{
    this->Destroy();
}
status_t CMemFile::InitBasic()
{
    this->mSize = 0;
    this->mBase = NULL;
    this->mMaxPages = 0;
    this->mOffset = 0;
    this->mPages = 0;
    this->mPageSize = 0;
    this->mod_n = ~0x0;
    this->shift_n = 0;
    return OK;
}
status_t CMemFile::Init()
{
    return this->Init(64*1024,4*1024);
}
status_t CMemFile::Init(int_ptr_t page_size,int_ptr_t max_pages)
{
    this->InitBasic();
    CFileBase::Init();
    ASSERT(max_pages > 0);
    this->mMaxPages = max_pages;
    this->mOffset = 0;
    this->mPages = 0;
    this->mPageSize = page_size;
    this->InitShift();
    return OK;
}
status_t CMemFile::InitShift()
{
    int32_t i;

    for(i = 5; i < (int32_t)sizeof(fsize_t)*8; i++)
    {
        if( (fsize_t)(1 << i) == this->mPageSize)
        {
            this->shift_n = i;
            this->mod_n = this->mPageSize - 1;
            return OK;
        }
    }
    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
        "error page size, must be pow of 2."
    );
    return ERROR;
}

status_t CMemFile::Destroy()
{
    if(this->mBase)
    {
        for(int_ptr_t i=0;i<this->mPages;i++)
        {
            FREE(this->mBase[i]);
        }
        FREE(this->mBase);
    }
    this->InitBasic();
    return OK;
}
status_t CMemFile::AddPage()
{
    if(this->mBase == NULL)
    {
        MALLOC(this->mBase,char *,this->mMaxPages);
        for(int_ptr_t i=0;i<this->mMaxPages;i++)
            this->mBase[i] = NULL;
    }

    if(this->mPages < this->mMaxPages - 1)
    {
        MALLOC(this->mBase[this->mPages] ,char, this->mPageSize);
        this->mPages ++;
    }
    else
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "CMemFile: Too many pages!"
        );
        return ERROR;
    }

    return OK;
}
status_t CMemFile::AddBlock(fsize_t block_size)
{
    int_ptr_t i,pn,new_page;
    new_page = ((this->mSize+(int_ptr_t)block_size) >> this->shift_n) + 1;
    pn=new_page - this->mPages;
    if(new_page > this->mPages)
    {
        for(i = 0; i < pn; i++)
            this->AddPage();
    }
    this->mSize += (int_ptr_t)block_size;
    return OK;
}
int_ptr_t CMemFile::Read(void *buf,int_ptr_t rsize)
{
    int_ptr_t left,sum=0,read_size=0;
    int_ptr_t block_end;
    int_ptr_t block=this->mOffset >> this->shift_n;
    int_ptr_t off=this->mOffset & this->mod_n;

    if(this->mOffset + rsize > this->mSize)
        rsize=this->mSize - this->mOffset;

    left = rsize;
    while(sum<rsize)
    {
        block_end = (block << this->shift_n) + this->mPageSize;
        if(this->mOffset + left < block_end)
        {
            read_size=left;
        }
        else
        {
            read_size = block_end - this->mOffset;
        }
        crt_memcpy((char*)buf+sum , this->mBase[block] + off, read_size);
        this->mOffset += read_size;
        sum += read_size;
        left -= read_size;
        block = this->mOffset >> this->shift_n;
        off = this->mOffset & this->mod_n;
    }
    return sum;
}
int_ptr_t CMemFile::Write(const void *buf,int_ptr_t wsize)
{
    int_ptr_t sum=0, write_size=0;
    int_ptr_t block_end;
    int_ptr_t left=wsize;
    int_ptr_t block= this->mOffset >> this->shift_n;
    int_ptr_t off=this->mOffset & this->mod_n;

    if(wsize <= 0) return 0;

    if(wsize + this->mOffset > this->mSize)
    {
        this->AddBlock(wsize + this->mOffset - this->mSize);
    }

    while(sum<wsize)
    {
        block_end = (block << this->shift_n) + this->mPageSize;
        if(this->mOffset + left < block_end)
        {
            write_size=left;
        }
        else
        {
            write_size=block_end - this->mOffset;
        }
        crt_memcpy(this->mBase[block]+off,(char*)buf+sum,write_size);
        this->mOffset += write_size;
        sum += write_size;
        left -= write_size;
        block = this->mOffset >> this->shift_n;
        off = this->mOffset & this->mod_n;
    }
    return sum;
}

status_t CMemFile::Print()
{
    return OK;
}

fsize_t CMemFile::Seek(fsize_t off)
{
    if(off > this->mSize)off = this->mSize;
    if(off < 0 )off = 0;
    this->mOffset = (int_ptr_t)off;
    return this->mOffset;
}

status_t CMemFile::SetSize(fsize_t s)
{
    if(s < 0) return ERROR;
    if(s > this->GetMaxSize())
        return ERROR;
    this->mSize = (int_ptr_t)s;
    this->mOffset = 0;
    return OK;
}

fsize_t CMemFile::GetOffset()
{
    return this->mOffset;
}

fsize_t CMemFile::GetSize()
{
    return this->mSize;
}

fsize_t CMemFile::GetMaxSize()
{
    return this->mMaxPages * this->mPageSize;
}

status_t CMemFile::Transfer(CMemFile *from)
{
    ASSERT(from);

    this->Destroy();

    this->mSize = from->mSize;
    this->mBase = from->mBase;
    this->mMaxPages = from->mMaxPages;
    this->mOffset = from->mOffset;
    this->mPages = from->mPages;
    this->mPageSize = from->mPageSize;
    this->mod_n = from->mod_n;
    this->shift_n = from->shift_n;

    from->InitBasic();

    return OK;
}

char CMemFile::CharAt(fsize_t index)
{
    BLOCK_AND_OFFSET(index,block,off);
    if(block >= mPages)return 0;
    return this->mBase[block][off];
}

status_t CMemFile::SetChar(fsize_t index,char ch)
{
    BLOCK_AND_OFFSET(index,block,off);
    if(block >= mPages)return ERROR;
    this->mBase[block][off] = ch;
    return OK;
}

char CMemFile::Getc()
{
    if(mOffset >= 0 && mOffset < mSize)
    {
        BLOCK_AND_OFFSET(mOffset,block,off);
        mOffset++;
        return this->mBase[block][off];
    }
    return 0;
}
char CMemFile::UnGetc()
{
    if(mOffset > 0 && mOffset <= mSize)
    {           
        mOffset--;
        BLOCK_AND_OFFSET(mOffset,block,off);
        return this->mBase[block][off];
    }
    return 0;
}

fsize_t CMemFile::Putc(char ch)
{
    if(1 + this->mOffset > this->mSize)
    {
        this->AddBlock(1 + this->mOffset - this->mSize);
    }
    BLOCK_AND_OFFSET(mOffset,block,off);
    this->mBase[block][off] = ch;
    mOffset++;
    return 1;
}
