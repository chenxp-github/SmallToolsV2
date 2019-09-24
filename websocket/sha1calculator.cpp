#include "sha1calculator.h"
#include "syslog.h"
#include "mem_tool.h"
#include "mem.h"

CSha1Calculator::CSha1Calculator()
{
    this->InitBasic();
}
CSha1Calculator::~CSha1Calculator()
{
    this->Destroy();
}
status_t CSha1Calculator::InitBasic()
{
    memset(&m_Context,0,sizeof(m_Context));
    memset(&m_Result,0,sizeof(m_Result));
    return OK;
}
status_t CSha1Calculator::Init()
{
    this->InitBasic();
    return OK;
}
status_t CSha1Calculator::Destroy()
{
    this->InitBasic();
    return OK;
}

status_t CSha1Calculator::Reset()
{
    return SHA1Reset(&m_Context) == shaSuccess;
}

status_t CSha1Calculator::Input(const void *buf, int_ptr_t size)
{
    return SHA1Input(&m_Context,(const uint8_t*)buf,size) == shaSuccess;
}

uint8_t* CSha1Calculator::GetResult()
{
    return m_Result;
}

status_t CSha1Calculator::CalcuHash(CFileBase *file)
{
    ASSERT(file);

    this->Reset();

    CMem buf;
    buf.Init();
    buf.Malloc(64*1024);

    file->Seek(0);
    while(!file->IsEnd())
    {
        int_ptr_t size = file->Read(buf.GetRawBuf(),(int_ptr_t)buf.GetMaxSize());
        this->Input(buf.GetRawBuf(),size);
    }

    return SHA1Result(&m_Context,m_Result) == shaSuccess;
}

status_t CSha1Calculator::CalcuHash(CMem *file)
{
    ASSERT(file);
    this->Reset();
    this->Input(file->GetRawBuf(),(int_ptr_t)file->GetSize());
    return SHA1Result(&m_Context,m_Result) == shaSuccess;
}

status_t CSha1Calculator::GetStringResult(CMem *out)
{
    ASSERT(out);
    for(int i = 0; i < SHA1HashSize; i++)
    {
        out->Printf("%02x",m_Result[i]);
    }
    return OK;
}
