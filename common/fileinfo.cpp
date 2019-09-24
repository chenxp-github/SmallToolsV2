#include "fileinfo.h"
#include "syslog.h"
#include "mem_tool.h"

#define BSON_TAG_NAME               "n"
#define BSON_TAG_SIZE               "s"
#define BSON_TAG_IS_DIR             "d"
#define BSON_TAG_LAST_WRITE_TIME    "l"
#define BSON_TAG_MD5                "5"

CFileInfo::CFileInfo()
{
    this->InitBasic();
}
CFileInfo::~CFileInfo()
{
    this->Destroy();
}
status_t CFileInfo::InitBasic()
{
    this->mName = NULL;
    this->mLastWriteTime = 0;
    this->mSize = 0;
    this->mMd5Sum = NULL;
    return OK;
}
status_t CFileInfo::Init()
{
    this->InitBasic();

    NEW(this->mName,CMem);
    this->mName->Init();
    this->SetName("");

    NEW(this->mMd5Sum,CMem);
    this->mMd5Sum->Init();
    this->SetMd5Sum("");

    return OK;
}
status_t CFileInfo::Destroy()
{
    DEL(this->mName);
    DEL(this->mMd5Sum);
    this->InitBasic();
    return OK;
}
status_t CFileInfo::Copy(CFileInfo *p)
{
    ASSERT(p && this != p);
    this->SetName(p->mName);
    this->mLastWriteTime = p->mLastWriteTime;
    this->mSize = p->mSize;
    this->SetMd5Sum(p->mMd5Sum);
    this->mIsDir = p->mIsDir;
    return OK;
}
int CFileInfo::Comp(CFileInfo *p)
{
    ASSERT(p);
    return this->mName->StrCmp(p->mName);
}
status_t CFileInfo::Print(CFileBase *_buf)
{
    _buf->Log("filename = %s",this->GetName());
    char str[128];
    crt_fsize_to_str(mSize,str);
    _buf->Log("size = %s",str);
    if(mMd5Sum->StrLen() > 0)
        _buf->Log("md5 = %s",this->GetMd5Sum());
    crt_fsize_to_str(this->mLastWriteTime,str);
    _buf->Log("last_write_time = %s",str);
    _buf->Log("is_dir=%d",this->IsDir());
    return OK;
}

const char * CFileInfo::GetName()
{
    return this->mName->CStr();
}
int64_t CFileInfo::GetLastWriteTime()
{
    return this->mLastWriteTime;
}
fsize_t CFileInfo::GetSize()
{
    return this->mSize;
}
const char * CFileInfo::GetMd5Sum()
{
    return this->mMd5Sum->CStr();
}
status_t CFileInfo::SetName(const char *name)
{
    CMem mem(name);
    return this->SetName(&mem);
}
status_t CFileInfo::SetName(CMem *name)
{
    ASSERT(name);
    this->mName->Destroy();
    this->mName->Init();
    this->mName->Copy(name);
    return OK;
}
status_t CFileInfo::SetLastWriteTime(int64_t lastwritetime)
{
    this->mLastWriteTime = lastwritetime;
    return OK;
}
status_t CFileInfo::SetSize(fsize_t size)
{
    this->mSize = size;
    return OK;
}
status_t CFileInfo::SetMd5Sum(const char *md5sum)
{
    CMem mem(md5sum);
    return this->SetMd5Sum(&mem);
}
status_t CFileInfo::SetMd5Sum(CMem *md5sum)
{
    ASSERT(md5sum);
    this->mMd5Sum->Destroy();
    this->mMd5Sum->Init();
    this->mMd5Sum->Copy(md5sum);
    return OK;
}

status_t CFileInfo::LoadFromBson(CMiniBson *bson)
{
    ASSERT(bson);
    CMem mem;
    bson->GetString(BSON_TAG_NAME,&mem);
    this->SetName(&mem);

    int64_t s = 0;
    bson->GetInt64(BSON_TAG_SIZE,&s);
    this->SetSize((fsize_t)s);

    bson->GetBoolean(BSON_TAG_IS_DIR,&this->mIsDir);

    for(int i = 0; i < 2; i++)
    {
        LOCAL_MEM(name);
        int type;
        if(!bson->PeekNext(&type,&name))
            break;
        if(name.StrCmp(BSON_TAG_LAST_WRITE_TIME) == 0)
        {
            bson->GetInt64(NULL,&this->mLastWriteTime);
        }
        else if(name.StrCmp(BSON_TAG_MD5) == 0)
        {
            bson->GetString(NULL,&mem);
            this->SetMd5Sum(&mem);
        }
    }

    return OK;
}

status_t CFileInfo::SaveToBson(CMiniBson *bson)
{
    ASSERT(bson);

    bson->PutString(BSON_TAG_NAME,this->mName);
    bson->PutInt64(BSON_TAG_SIZE,this->GetSize());
    bson->PutBoolean(BSON_TAG_IS_DIR,this->GetIsDir());
    if(this->GetLastWriteTime() > 0)
    {
        bson->PutInt64(BSON_TAG_LAST_WRITE_TIME,this->GetLastWriteTime());
    }
    if(this->mMd5Sum->StrLen() > 0)
    {
        bson->PutString(BSON_TAG_MD5,this->mMd5Sum);
    }
    return OK;
}

bool CFileInfo::IsDir()
{
    return this->GetIsDir();
}
bool CFileInfo::GetIsDir()
{
    return this->mIsDir;
}
status_t CFileInfo::SetIsDir(bool isdir)
{
    this->mIsDir = isdir;
    return OK;
}
