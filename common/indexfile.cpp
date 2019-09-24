#include "indexfile.h"
#include "syslog.h"
#include "mem_tool.h"

#define IDX_FILE_FLAG   0x584449
#define CURRENT_VERSION 2

CIndexFile::CIndexFile()
{
    this->InitBasic();
}
CIndexFile::~CIndexFile()
{
    this->Destroy();
}
status_t CIndexFile::InitBasic()
{
    this->iDstFile = NULL;
    this->mIndexFile= NULL;
    this->mHeaderSize = 32;
    this->mCurOffset = 0;
    this->mMaxBlocks = 0;
    return OK;
}
status_t CIndexFile::Init()
{
    this->InitBasic();
    
    NEW(this->mIndexFile,CMemFile);
    this->mIndexFile->Init();
    return OK;
}
status_t CIndexFile::Destroy()
{
    DEL(this->mIndexFile);
    this->InitBasic();
    return OK;
}
status_t CIndexFile::SetDstFile(CFileBase *i_file)
{
    ASSERT(i_file);
    this->iDstFile = i_file;
    i_file->SetSize(0);
    i_file->FillBlock(this->mHeaderSize,0);
    this->mCurOffset = this->mHeaderSize;
    this->mMaxBlocks = 0;
    return OK;
}
status_t CIndexFile::AddBlock(CFileBase *file)
{
    ASSERT(file && this->iDstFile);
    this->mIndexFile->Write(&mCurOffset,sizeof(int64_t));
    this->iDstFile->Seek(this->mCurOffset);
    this->iDstFile->WriteFile(file);
    this->mCurOffset += (int64_t)file->GetSize();
    this->mMaxBlocks++;
    return OK;
}
status_t CIndexFile::Save()
{
    ASSERT(this->iDstFile);
    
    this->iDstFile->Seek(0);

    int32_t flag = IDX_FILE_FLAG;
    this->iDstFile->Write(&flag,sizeof(flag));
    flag = CURRENT_VERSION;
    this->iDstFile->Write(&flag,sizeof(flag));

    int64_t tmp;
    tmp = this->mHeaderSize;
    this->iDstFile->Write(&tmp,sizeof(tmp));
    tmp = (int64_t)(this->mIndexFile->GetSize()/sizeof(tmp));
    this->iDstFile->Write(&tmp,sizeof(tmp));
    tmp = (int64_t)this->iDstFile->GetSize();
    this->iDstFile->Write(&tmp,sizeof(tmp));

    this->iDstFile->SeekEnd();
    this->iDstFile->WriteFile(mIndexFile);
    return OK;
}
status_t CIndexFile::LoadIndexFile(CFileBase *i_file)
{
    ASSERT(i_file);
    int64_t index_offset;
    int32_t tmp;
    this->iDstFile = i_file;

    i_file->Seek(0);
    i_file->Read(&tmp,sizeof(tmp));
    ASSERT(tmp == IDX_FILE_FLAG);
    i_file->Read(&tmp,sizeof(tmp));
    ASSERT(tmp == CURRENT_VERSION);
    
    i_file->Read(&mHeaderSize,sizeof(int64_t));
    i_file->Read(&mMaxBlocks,sizeof(int64_t));
    i_file->Read(&index_offset,sizeof(int64_t));
    
    this->mIndexFile->SetSize(0);
    i_file->WriteToFile(this->mIndexFile,index_offset,mMaxBlocks * sizeof(int64_t));
    return OK;
}
int64_t CIndexFile::GetBlockOffset(int64_t index)
{
    if(index < 0 || index >= mMaxBlocks)
        return -1;
    int64_t off=0;
    this->mIndexFile->Seek(index * sizeof(int64_t));
    this->mIndexFile->Read(&off,sizeof(int64_t));
    return off;
}
int64_t CIndexFile::GetDataSize()
{
    ASSERT(this->iDstFile);
    return  (int64_t)(iDstFile->GetSize()-this->mHeaderSize-this->mMaxBlocks*sizeof(int64_t));
}
int64_t CIndexFile::GetBlockSize(int64_t index)
{
    ASSERT(index >=0 && index < mMaxBlocks);
    if(index == mMaxBlocks -1)
    {
        return this->GetDataSize() - this->GetBlockOffset(index) + this->mHeaderSize;
    }
    else
    {
        return this->GetBlockOffset(index+1) - this->GetBlockOffset(index);
    }
}
status_t CIndexFile::GetBlock(int64_t index, CFileBase *file_dst)
{
    ASSERT(file_dst && this->iDstFile);
    
    file_dst->SetSize(0);
    int64_t off = this->GetBlockOffset(index);
    if(off > 0)
    {
        int64_t size = this->GetBlockSize(index);
        if(size > 0)
        {
            this->iDstFile->WriteToFile(file_dst,off,size);
            return OK;
        }
    }
    
    return ERROR;
}

int64_t CIndexFile::GetBlockCount()
{
    return this->mMaxBlocks;
}

status_t CIndexFile::GhostBlock(int64_t index, CPartFile* file)
{   
    ASSERT(file && this->iDstFile);
    
    int64_t off = this->GetBlockOffset(index);
    if(off > 0)
    {
        int64_t size = this->GetBlockSize(index);
        if(size > 0)
        {
            file->SetHostFile(this->iDstFile,(fsize_t)off,(fsize_t)size);
            return OK;
        }
    }
    return ERROR;
}

int64_t CIndexFile::GetCurOffset(void)
{
    return this->mCurOffset;
}

bool CIndexFile::IsIndexFile(CFileBase *file)
{
    int32_t tmp;
    file->Seek(0);
    file->Read(&tmp,sizeof(tmp));
    return tmp == IDX_FILE_FLAG;
}
