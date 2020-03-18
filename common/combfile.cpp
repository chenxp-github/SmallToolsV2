#include "combfile.h"
#include "syslog.h"
#include "mem_tool.h"

CCombFile::CCombFile()
{
    this->InitBasic();
}
CCombFile::~CCombFile()
{
    this->Destroy();
}
status_t CCombFile::InitBasic()
{
    CFileBase::InitBasic();
    this->mOffset = 0;
    this->mAllFiles = NULL;
    this->mFileNum = 0;
    return OK;
}
status_t CCombFile::Init(int max)
{
    this->InitBasic();
    CFileBase::Init();
    MALLOC(this->mAllFiles,CFileBase*,max);
    this->mMaxFileNum = max;
    return OK;
}
status_t CCombFile::Destroy()
{
    FREE(this->mAllFiles);
    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}

fsize_t CCombFile::GetOffset()
{
    return this->mOffset;
}

fsize_t CCombFile::GetSize()
{
    fsize_t size = 0;
    for(int i = 0; i < this->mFileNum; i++)
    {
        size += this->mAllFiles[i]->GetSize();
    }
    return size;
}

int_ptr_t CCombFile::Read(void *buf,int_ptr_t n)
{
    int_ptr_t read_size = 0;
    
    while(read_size < n)
    {
        fsize_t off = 0;
        int i = this->InWhichFile(this->mOffset,&off);
        if(i < 0) break;
        fsize_t old_off = this->mAllFiles[i]->GetOffset();
        this->mAllFiles[i]->Seek(off);
        int_ptr_t rs = this->mAllFiles[i]->Read(
            ((uint8_t*)buf)+read_size,n-read_size
        );
        this->mAllFiles[i]->Seek(old_off);
        if(rs <= 0) break;
        this->mOffset += rs;
        read_size += rs;
    }
    return read_size;
}

int_ptr_t CCombFile::Write(const void *buf,int_ptr_t n)
{
    return 0;
}

status_t CCombFile::SetSize(fsize_t ssize)
{
    this->mOffset = 0;
    return OK;
}

fsize_t CCombFile::Seek(fsize_t off)
{
    if(off > this->GetSize())off = this->GetSize();
    if(off < 0 )off = 0;
    this->mOffset = (int_ptr_t)off;
    return this->mOffset;
}

status_t CCombFile::AddBlock(fsize_t bsize)
{
    return OK;
}

fsize_t CCombFile::GetMaxSize()
{
    return this->GetSize();
}

int CCombFile::InWhichFile(fsize_t off,fsize_t *subOff)
{
    fsize_t begin,size = 0;
    for(int i = 0; i < this->mFileNum; i++)
    {
        begin = size;
        size += this->mAllFiles[i]->GetSize();
        if(size > off)
        {
            *subOff = off - begin;
            return i;
        }
    }
    return -1;
}

status_t CCombFile::AddFile(CFileBase *file)
{
    ASSERT(this->mFileNum < this->mMaxFileNum);
    this->mAllFiles[this->mFileNum] = file;
    this->mFileNum++;
    return OK;
}

status_t CCombFile::Clear()
{
    this->mFileNum = 0;
    this->SetSize(0);
    return OK;
}
