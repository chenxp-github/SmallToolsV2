#include "mmapfile.h"
#include "syslog.h"
#include "mem_tool.h"
#include "dirmgr.h"

#if !HAVE_WINDOWS_H
#include <sys/mman.h>
#endif

CMMapFile::CMMapFile()
{
    this->InitBasic();
}
CMMapFile::~CMMapFile()
{
    this->Destroy();
}
status_t CMMapFile::InitBasic()
{
    WEAK_REF_CLEAR();
    m_Data = NULL;
    m_DataSize = 0;
#if HAVE_WINDOWS_H
    m_MapHandle = NULL;
    m_FileHandle = NULL;
#else
    m_FileHandle = ERROR_FILE_HANDLE;
#endif
    m_FileName.InitBasic();
    return OK;
}
status_t CMMapFile::Init()
{
    this->InitBasic();
    
    return OK;
}
status_t CMMapFile::Destroy()
{
    WEAK_REF_DESTROY();
    this->Close();
    m_FileName.Destroy();
    this->InitBasic();
    return OK;
}
int CMMapFile::Comp(CMMapFile *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CMMapFile::Copy(CMMapFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    return OK;
}
status_t CMMapFile::Print(CFileBase *_buf)
{
    return OK;
}

void* CMMapFile::GetData()
{
    return m_Data;
}

int_ptr_t CMMapFile::GetSize()
{
    return m_DataSize;
}

status_t CMMapFile::SetFileName(CMem* _filename)
{
    this->m_FileName.Copy(_filename);
    return OK;
}
status_t CMMapFile::SetFileName(const char *_filename)
{
    CMem _mem(_filename);
    return this->SetFileName(&_mem);
}
/////////////////////////////////////////////////////////////
#if HAVE_WINDOWS_H  //windows
/////////////////////////////////////////////////////////////
status_t CMMapFile::Close()
{
    if(m_Data)
    {
        UnmapViewOfFile(m_Data);
        m_Data = NULL;
        m_DataSize = 0;
    }

    if(m_MapHandle)
    {
        CloseHandle(m_MapHandle);
        m_MapHandle = NULL;
    }

    if(m_FileHandle)
    {
        CloseHandle(m_FileHandle);
        m_FileHandle = NULL;
    }
    return OK;
}

status_t CMMapFile::OpenReadOnly(const char *filename)
{
    ASSERT(filename);
    
    this->Close();

    m_FileHandle = CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_READONLY,
        NULL
    );

    if(m_FileHandle == INVALID_HANDLE_VALUE)
    {
        return ERROR;
    }

    LOCAL_MEM(mapping_file_name);
    this->MakeFileMappingName(filename,&mapping_file_name);

    m_MapHandle = CreateFileMapping(m_FileHandle,NULL,PAGE_READONLY,0,0,mapping_file_name.CStr());

    if(!m_MapHandle)
    {
        return ERROR;
    }

    m_Data = MapViewOfFile(m_MapHandle,FILE_MAP_READ,0,0,0);
    ASSERT(m_Data);

    m_DataSize = ::GetFileSize(m_FileHandle,NULL);

    this->SetFileName(filename);
    return OK;
}

status_t CMMapFile::OpenCreate(const char *filename, int_ptr_t size)
{
    ASSERT(filename && size > 0);
    
    this->Close();

    m_FileHandle = CreateFile(
        filename,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if(m_FileHandle == INVALID_HANDLE_VALUE)
    {
        return ERROR;
    }

    SetFilePointer(m_FileHandle,size,NULL,FILE_BEGIN);
    SetEndOfFile(m_FileHandle);
    
    LOCAL_MEM(mapping_file_name);
    this->MakeFileMappingName(filename,&mapping_file_name);
    m_MapHandle = CreateFileMapping(m_FileHandle,NULL,PAGE_READWRITE,0,0,mapping_file_name.CStr());

    if(!m_MapHandle)
    {
        return ERROR;
    }
    
    m_Data = MapViewOfFile(m_MapHandle,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
    ASSERT(m_Data);
    m_DataSize = ::GetFileSize(m_FileHandle,NULL);
    this->SetFileName(filename);
    return OK;
}

status_t CMMapFile::OpenReadWrite(const char *filename)
{
    ASSERT(filename);
    
    this->Close();
    
    m_FileHandle = CreateFile(
        filename,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if(m_FileHandle == INVALID_HANDLE_VALUE)
    {
        return ERROR;
    }

    LOCAL_MEM(mapping_file_name);
    this->MakeFileMappingName(filename,&mapping_file_name);

    m_MapHandle = CreateFileMapping(m_FileHandle,NULL,PAGE_READWRITE,0,0,mapping_file_name.CStr());
    
    if(!m_MapHandle)
    {
        return ERROR;
    }
    
    m_Data = MapViewOfFile(m_MapHandle,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
    ASSERT(m_Data);
    
    m_DataSize = ::GetFileSize(m_FileHandle,NULL);
    this->SetFileName(filename);
    
    return OK;
}
status_t CMMapFile::Unlink()
{
    return DeleteFile(m_FileName.CStr());
}
status_t CMMapFile::Sync()
{
    return OK;
}
status_t CMMapFile::MakeFileMappingName(const char *filename,CMem *obj_name)
{
    ASSERT(filename && obj_name);
    obj_name->SetSize(0);
    
    obj_name->Puts("mmapfile::");

    int len = strlen(filename);
    for(int i = 0; i < len; i++)
    {
        char ch = filename[i];
        if(obj_name->IsSpChar(ch) || obj_name->IsEmptyChar(ch))
        {
            obj_name->Putc('_');
        }
        else
        {
            obj_name->Putc(ch);
        }
    }
    return OK;
}
/////////////////////////////////////////////////////////////
#else  //linux
/////////////////////////////////////////////////////////////
status_t CMMapFile::Close()
{
    if(m_Data)
    {
        munmap(m_Data,m_DataSize);
        m_Data = NULL;
        m_DataSize = 0;
    }
    if(m_FileHandle != ERROR_FILE_HANDLE)
    {
        close(m_FileHandle);
        m_FileHandle = ERROR_FILE_HANDLE;
    }
    return OK;
}

status_t CMMapFile::OpenReadOnly(const char *filename)
{
    ASSERT(filename);

    this->Close();

    m_FileHandle = open(filename,O_RDONLY);
    if(!crt_is_file_handle(m_FileHandle))
    {
        return ERROR;
    }

    struct stat sb;
    
    ASSERT((fstat(m_FileHandle, &sb) != -1));
    ASSERT(sb.st_size > 0);
    
    m_Data = mmap(NULL,sb.st_size,PROT_READ,MAP_SHARED,m_FileHandle,0);
    ASSERT(m_Data);
    
    m_DataSize = sb.st_size;
    this->SetFileName(filename);

    return OK;
}

status_t CMMapFile::OpenCreate(const char *filename, int_ptr_t size)
{
    ASSERT(filename && size > 0);
    
    this->Close();
    
    m_FileHandle = open(filename,O_CREAT|O_RDWR,0755);
    if(!crt_is_file_handle(m_FileHandle))
    {
        return ERROR;
    }
    
    ASSERT(ftruncate(m_FileHandle,size) == 0);
    
    m_Data = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,m_FileHandle,0);
    ASSERT(m_Data);
    
    m_DataSize = size;
    this->SetFileName(filename);

    return OK;
}

status_t CMMapFile::Sync()
{
    if(!m_Data)
        return ERROR;
    if(m_DataSize <= 0)
        return ERROR;
    return msync(m_Data,m_DataSize,MS_SYNC) == 0;
}

status_t CMMapFile::OpenReadWrite(const char *filename)
{
    ASSERT(filename);
    
    this->Close();
    
    m_FileHandle = open(filename,O_RDWR);
    if(!crt_is_file_handle(m_FileHandle))
    {
        return ERROR;
    }
    
    struct stat sb;
    
    ASSERT((fstat(m_FileHandle, &sb) != -1));
    ASSERT(sb.st_size > 0);
    
    m_Data = mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,m_FileHandle,0);
    ASSERT(m_Data);
    
    m_DataSize = sb.st_size;
    this->SetFileName(filename);

    return OK;
}

status_t CMMapFile::Unlink()
{
    return unlink(m_FileName.CStr()) == 0;
}

#endif
/////////////////////////////////////////////////////////////
