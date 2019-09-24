#include "mem.h"
#include "syslog.h"
#include "mem_tool.h"

CMem::CMem()
{
    this->Init();
}

CMem::~CMem()
{
    this->Destroy();
}

CMem::CMem(const char *str)
{
    this->Init();
    this->SetStr(str);
}

status_t CMem::InitBasic()
{
    this->mBuf = NULL;
    this->mSelfAlloc = false;
    this->mSize = 0;
    this->mMaxSize = 0;
    this->mOffset = 0;
    this->mIsConst = true;
    return OK;
}
status_t CMem::Init()
{
    CFileBase::Init();
    this->InitBasic();
    return OK;
}

status_t CMem::Destroy()
{
    this->Free();
    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CMem::Malloc(int_ptr_t asize)
{
    asize += sizeof(int);
    ASSERT(asize > 0);
    ASSERT(this->mBuf == NULL);
    MALLOC(this->mBuf,char,asize);
    this->mSelfAlloc = true;
    this->mSize = 0;
    this->mOffset = 0;
    this->mMaxSize = asize;
    this->mIsConst = false;
    return OK;
}

status_t CMem::Free()
{
    if(this->mBuf == NULL)
        return ERROR;
    if(this->mSelfAlloc)
    {        
        FREE(this->mBuf);
        this->mSelfAlloc = false;
    }
    return OK;
}


status_t CMem::Realloc(int_ptr_t newSize)
{
    CMem new_mem;
	if(newSize == mSize)
		return OK;
    fsize_t old_off = this->GetOffset();
    new_mem.Malloc(newSize);
    new_mem.WriteFile(this); //may change this offset
    
    this->Free();

    this->mBuf = new_mem.mBuf;
    this->mMaxSize = (int_ptr_t)new_mem.GetMaxSize();
    if(this->mSize > newSize)
        this->mSize = newSize;
    this->mIsConst = false;
    this->mSelfAlloc = true;
    new_mem.mBuf = NULL;
    this->Seek(old_off);
    return OK;
}

int_ptr_t CMem::Read(void *buf,int_ptr_t n)
{
    int_ptr_t  copy_length;
    ASSERT(this->mBuf && buf);
    if(this->mSize - this->mOffset > n)
        copy_length=n;
    else
        copy_length=(int_ptr_t)(this->mSize - this->mOffset);
    if(copy_length > 0)
    {
        crt_memcpy(buf,(this->mBuf+this->mOffset),copy_length);
        this->mOffset += copy_length;
        return copy_length;
    }
    return 0;
}

int_ptr_t CMem::Write(const void *buf,int_ptr_t n)
{
    int_ptr_t  copy_length;
    
    ASSERT(!this->mIsConst);
    ASSERT(this->mBuf && buf);

    if(n + this->mOffset > this->mSize)
    {
        this->AddBlock(n + this->mOffset - this->mSize);
    }

    if(this->mSize - this->mOffset > n)
        copy_length=n;
    else
        copy_length=(int_ptr_t)(this->mSize - this->mOffset);
    if(copy_length > 0)
    {
        crt_memcpy((this->mBuf+this->mOffset),buf,copy_length);
        this->mOffset += copy_length;
        return copy_length;
    }

    return 0;
}

status_t CMem::Zero()
{
    ASSERT(this->mBuf);
    ASSERT(!this->mIsConst);
    crt_memset(this->mBuf,0,this->mMaxSize);
    return OK;
}

status_t CMem::Print(CFileBase *_buf)
{
    if(StrLen() > 0)
    {
        _buf->FillBlock(4*_buf->log_tab_level_,' ');
        _buf->Puts(this);
    }
    return OK;
}

status_t CMem::SetRawBuf(void *p,int_ptr_t s,bool is_const)
{
    ASSERT(this->mBuf == NULL);
    this->mBuf = (char*)p;
    this->mSize = s;
    this->mMaxSize = s;
    this->mIsConst = is_const;
    this->mSelfAlloc = false;
    this->mOffset = 0;
    return OK;
}

fsize_t CMem::Seek(fsize_t off)
{
    if(off > this->mSize)off = this->mSize;
    if(off < 0 )off = 0;
    this->mOffset = (int_ptr_t)off;
    return this->mOffset;
}

fsize_t CMem::GetOffset()
{
    return this->mOffset;
}

fsize_t CMem::GetSize()
{
    return this->mSize;
}

status_t CMem::SetSize(fsize_t ssize)
{
    if(ssize >= 0 && ssize <= this->mMaxSize)
    {
        this->mSize = (int_ptr_t)ssize;
        this->mOffset = 0;
        return OK;
    }
    return ERROR;
}

status_t CMem::AddBlock(fsize_t bsize)
{
    if(this->mBuf == NULL)
    {
        this->Malloc((int_ptr_t)bsize);
        return OK;
    }

    this->mSize += (int_ptr_t)bsize;
    if(this->mSize < 0)
        this->mSize = 0;
    if(this->mSize > this->mMaxSize)
        this->mSize = this->mMaxSize;
    return OK;
}

fsize_t CMem::GetMaxSize()
{
    return this->mMaxSize;
}

status_t CMem::Copy(CFileBase *file)
{
    ASSERT(file);
    if(this == file) return OK;

    SAVE_WEAK_REF_ID(*this,w);
    this->Destroy();
    this->Init();
    RESTORE_WEAK_REF_ID(*this,w);

    if(file->file_name)
        this->SetFileName(file->file_name);

    this->SetUserData(file->GetUserData());
    
    if(file->GetSize() > 0)
    {
        this->Malloc((int_ptr_t)file->GetSize());
        this->WriteFile(file);
    }
    return OK;
}

status_t CMem::SetStr(const char *p)
{
    ASSERT(p);
    return this->SetRawBuf((void*)p,crt_strlen(p),true);
}

const char *CMem::CStr()
{
    ASSERT(this->mBuf);
    if(!this->mIsConst)
    {
        if(this->mSize >= 0 && this->mSize < this->mMaxSize)
            this->mBuf[this->mSize] = 0;
        else if(this->mSize >= this->mMaxSize && this->mMaxSize > 0)
            this->mBuf[this->mMaxSize-1] = 0;
    }
    return this->mBuf;
}

char * CMem::GetRawBuf()
{
    return this->mBuf;
}

char CMem::C(int index)
{
    return this->CStr()[index];
}

bool CMem::StrEqu(CMem *str, bool case_sensitive)
{
    ASSERT(str);

    if(str->StrLen() != this->StrLen())
        return false;
    if(case_sensitive)
        return crt_strcmp(str->CStr(),this->CStr()) == 0;
    else
        return crt_stricmp(str->CStr(),this->CStr()) == 0;
}

status_t CMem::Transfer(CMem *from)
{
    ASSERT(from);
    this->Destroy();
    this->Init();

    this->mBuf = from->mBuf;
    this->mSelfAlloc = from->mSelfAlloc;
    this->mSize = from->mSize;
    this->mMaxSize = from->mMaxSize;
    this->mOffset = from->mOffset;
    this->mIsConst = from->mIsConst;

    from->mBuf = NULL;
    from->mSelfAlloc = false;
    from->mSize = 0;
    from->mMaxSize = 0;
    from->mOffset = 0;
    from->mIsConst = true;
    
    return OK;
}

bool CMem::IsMalloc()
{
    return this->mSelfAlloc;
}

char CMem::CharAt(fsize_t index)
{
    if(!mBuf)return ERROR;
    if(index >= 0 && index < mSize)
    {
        return mBuf[index];
    }
    return 0;
}
status_t CMem::SetChar(fsize_t index,char ch)
{
    if(!mBuf)return ERROR;
    if(index >= 0 && index < mSize)
    {
        mBuf[index] = ch;
    }
    return OK;
}
char CMem::Getc()
{
    if(mBuf)
    {
        if(mOffset >= 0 && mOffset < mSize)
            return mBuf[mOffset++];
    }
    return 0;
}
char CMem::UnGetc()
{
    if(mBuf)
    {
        if(mOffset > 0 && mOffset <= mSize)
            return mBuf[--mOffset];
    }
    return 0;
}

fsize_t CMem::Putc(char ch)
{
    if(!mBuf)return 0;

    if(1 + this->mOffset > this->mSize)
    {
        this->AddBlock(1 + this->mOffset - this->mSize);
    }

    if(mOffset >= 0 && mOffset < mSize)
    {
        mBuf[mOffset++] = ch;
        return 1;
    }
    return 0;
}

bool CMem::IsReadOnly()
{
	return mIsConst;
}

status_t CMem::SetIsReadOnly(bool read_only)
{
    mIsConst = read_only;
    return OK;
}
////////////////////////////////////////////////////////////////////////////
#if _UNICODE_
////////////////////////////////////////////////////////////////////////////
CMem::CMem(const wchar_t *str)
{
    this->Init();
    this->SetStrW(str);
}

status_t CMem::SetStrW(const wchar_t *p)
{
    ASSERT(p);
    return this->SetRawBuf((void*)p,crt_strlen_w(p)*sizeof(wchar_t),true);
}
const wchar_t * CMem::CStrW()
{
    ASSERT(this->mBuf);
    if(!this->mIsConst)
    {
        wchar_t *p = (wchar_t*)this->mBuf;
        fsize_t len = this->StrLenW();
        fsize_t max_len = this->mMaxSize/sizeof(wchar_t);
        if(len >= 0 && len < max_len)
            p[len] = 0;
        else if(len >= max_len && max_len > 0)
            p[max_len-1] = 0;
    }
    return (const wchar_t*)this->mBuf;
}
wchar_t CMem::CW(int index)
{
    return this->CStrW()[index];
}

////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////
