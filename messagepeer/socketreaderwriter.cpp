#include "socketreaderwriter.h"
#include "syslog.h"
#include "mem_tool.h"

#define MAX_LINE_LENGTH (64*1024)

CSocketReaderWriter::CSocketReaderWriter()
{
    this->InitBasic();
}
CSocketReaderWriter::~CSocketReaderWriter()
{
    this->Destroy();
}
status_t CSocketReaderWriter::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    this->iSocket = NULL;
    this->iReadDestFile = NULL;
    this->iWriteSrcFile = NULL;
    this->mReadTimer = 0;
    this->mTimeout = SOCKETRW_TIMEOUT;
    this->mWriteTimer = 0;
    this->mTotalReadLength = 0;
    this->mWriteEndOffset = 0;
    this->mWriteOffset = 0;
    this->mReadLength = 0;
    this->mCallback = NULL;
    this->mReadSizeThisTime = 0;
    this->mWriteSizeThisTime = 0;
	this->iTaskMgr = NULL;
    return OK;
}
status_t CSocketReaderWriter::Init(CTaskMgr *taskmgr)
{
    this->Destroy();
    WEAK_REF_ID_INIT();
    NEW(mCallback,CClosure);
    mCallback->Init();
	iTaskMgr = taskmgr;
    return OK;
}
status_t CSocketReaderWriter::Destroy()
{
    DEL(mCallback);
    this->InitBasic();
    return OK;
}

status_t CSocketReaderWriter::SetTimeout(uint32_t timeout)
{
    this->mTimeout = timeout;
    return OK;
}

status_t CSocketReaderWriter::PrepareFileForWrite(CFileBase *ifile, fsize_t start, fsize_t size)
{
    this->mWriteOffset = start;
    this->iWriteSrcFile = ifile;    
    this->mWriteEndOffset = size-start;
    this->mWriteTimer = 0;
    return OK;
}

status_t CSocketReaderWriter::DoWrite(uint32_t interval)
{   
    this->mWriteSizeThisTime = 0;

    if(this->mWriteEndOffset <= 0 || this->iWriteSrcFile == NULL)
    {
        return OK;
    }
    
    ASSERT(this->iSocket);

    if(!this->iSocket->IsConnected())
    {
        this->Error(ERROR_CONNECTION_CLOSED);
        return ERROR;
    }
    
    if(this->mWriteOffset >= this->mWriteEndOffset)
    {
        return OK;
    }

    char buf[64*1024];

    int_ptr_t need_write= (int_ptr_t)(this->mWriteEndOffset - this->mWriteOffset);  
    if(need_write > (int_ptr_t)sizeof(buf))need_write = sizeof(buf);

    this->iWriteSrcFile->Seek(this->mWriteOffset);
    if(this->iWriteSrcFile->IsEnd())
    {
        this->Error(ERROR_REACH_END_OF_FILE);
        return ERROR;
    }
    
    if(!iSocket->CanWrite())
        return ERROR;

    int_ptr_t rs = this->iWriteSrcFile->Read(buf,need_write);
    int_ptr_t ws = this->iSocket->Write(buf,rs);
    this->mWriteSizeThisTime = ws;

    if(ws > 0)
    {
        this->mWriteOffset += ws;
        this->mWriteTimer = 0;
        if(this->mWriteOffset >= this->mWriteEndOffset)
        {
            return OK;
        }
		ASSERT(iTaskMgr);
		iTaskMgr->TurboOn();
    }
    else if(ws == 0)
    {
        this->mWriteOffset += ws;
        if(interval < mTimeout)
            this->mWriteTimer += interval;
        if(this->mWriteTimer >= this->mTimeout)
        {
            this->iSocket->CloseConnect();
            this->Error(ERROR_WRITE_TIMEOUT);
            return ERROR;
        }
    }
    else
    {
        this->iSocket->CloseConnect();
        this->Error(ERROR_WRITE_ERROR);
        return ERROR;
    }

    return ERROR;
}

status_t CSocketReaderWriter::Error(int err)
{
    const char *err_str="unknown";

    if(err == ERROR_CONNECT_ERROR)
        err_str="connect error";
    if(err == ERROR_READ_TIMEOUT)
        err_str="read timeout";
    if(err == ERROR_CONNECTION_CLOSED)
        err_str="connection closed";
    if(err == ERROR_WRITE_TIMEOUT)
        err_str="write timeout";
    if(err == ERROR_WRITE_ERROR)
        err_str="write error";
    if(err == ERROR_READ_ERROR)
        err_str="read error";
    if(err == ERROR_REACH_END_OF_FILE)
        err_str="reach end of file";
    if(err == ERROR_LINE_IS_TOO_LONG)
        err_str="line is too long";

    mCallback->SetParamInt(1,err);
    mCallback->SetParamPointer(2,(void*)err_str);
    mCallback->Run(EVENT_ERROR);

    return OK;
}

status_t CSocketReaderWriter::PrepareFileForRead(CFileBase *ifile, fsize_t size,bool reset)
{
    if(ifile&&reset)ifile->SetSize(0);
    this->iReadDestFile = ifile;
    this->mReadTimer = 0;
    this->mTotalReadLength = size;
    this->mReadLength = 0;
    return OK;
}

status_t CSocketReaderWriter::DoRead(uint32_t interval)
{
    this->mReadSizeThisTime = 0;
    if(this->mTotalReadLength == 0 || this->iReadDestFile == NULL)
    {
        return OK;
    }
    
    ASSERT(this->iSocket);
    
    if(!this->iSocket->IsConnected())
    {
        this->Error(ERROR_CONNECTION_CLOSED);
        return ERROR;
    }

    if(mTotalReadLength>0 && this->mReadLength>=this->mTotalReadLength)
    {
        return OK;
    }
    
    char buf[64*1024];
    int_ptr_t need_read;
    if(mTotalReadLength > 0)
    {
        need_read = (int_ptr_t)(mTotalReadLength - this->mReadLength);
        if(need_read > (int_ptr_t)sizeof(buf))need_read = sizeof(buf);  
    }
    else
    {
        need_read = sizeof(buf);
    }
    
    int_ptr_t rs = this->iSocket->Read(buf,need_read);
    this->mReadSizeThisTime = rs;

    if(rs > 0)
    {
        this->mReadTimer = 0;
        this->mReadLength += rs;
        this->iReadDestFile->Write(buf,rs);
        if(mTotalReadLength>0 && this->mReadLength>=this->mTotalReadLength)
        {
            return OK;
        }
    }
    else if(rs == 0)
    {
        if(interval < mTimeout)
            this->mReadTimer += interval;
        if(this->mReadTimer >= this->mTimeout)
        {
            this->iSocket->CloseConnect();
            this->Error(ERROR_READ_TIMEOUT);
            return ERROR;
        }
    }
    else
    {
        this->iSocket->CloseConnect();
        this->Error(ERROR_READ_ERROR);
        
        return ERROR;
    }

    return ERROR;
}

status_t CSocketReaderWriter::DoReadUntilEol(uint32_t interval,bool only_lf)
{
    this->mReadSizeThisTime = 0;
    ASSERT(this->iReadDestFile);
    if(!this->iSocket->IsConnected())
    {
        this->Error(ERROR_CONNECTION_CLOSED);
        return ERROR;
    }

    char buf[8];

    while(true)
    {
        int_ptr_t rs = this->iSocket->Read(buf,1);
        if(rs == 1)
        {
            this->mReadSizeThisTime += rs;
            this->mReadTimer = 0;
            this->iReadDestFile->Putc(buf[0]);
            fsize_t size = this->iReadDestFile->GetSize();
            
            if(size > MAX_LINE_LENGTH)
            {
                this->Error(ERROR_LINE_IS_TOO_LONG);
                return ERROR;
            }

            if(only_lf)
            {               
                if(buf[0] == '\n')
                {
                    return OK;
                }
            }

            if(size >= 2 && (!only_lf))
            {
                char last[2];
                fsize_t offset = this->iReadDestFile->GetOffset();
                
                this->iReadDestFile->Seek(size-2);
                this->iReadDestFile->Read(last,2);
                this->iReadDestFile->Seek(offset);

                if(last[0]=='\r'&&last[1]=='\n')
                {
                    return OK;
                }
            }
        }
        else if(rs == 0)
        {
            if(interval < mTimeout)
                this->mReadTimer += interval;
            if(this->mReadTimer > this->mTimeout)
            {
                this->Error(ERROR_READ_TIMEOUT);
            }
            return ERROR;
        }
        else
        {
            this->iSocket->CloseConnect();
            this->Error(ERROR_READ_ERROR);
            return ERROR;
        }
    }

    return ERROR;
}

status_t CSocketReaderWriter::DoReadUntilEmptyLine(uint32_t interval)
{
    this->mReadSizeThisTime = 0;
    ASSERT(this->iReadDestFile);
    if(!this->iSocket->IsConnected())
    {
        this->Error(ERROR_CONNECTION_CLOSED);
        return ERROR;
    }

    char buf[8];

    while(true)
    {
        int_ptr_t rs = this->iSocket->Read(buf,1);
        if(rs == 1)
        {
            this->mReadSizeThisTime += rs;
            this->mReadTimer = 0;
            this->iReadDestFile->Putc(buf[0]);
            fsize_t size = this->iReadDestFile->GetSize();
            if(size > MAX_LINE_LENGTH)
            {
                this->Error(ERROR_LINE_IS_TOO_LONG);
                return ERROR;
            }

            if(size >= 4)
            {
                char last[4];
                fsize_t offset = this->iReadDestFile->GetOffset();
                
                this->iReadDestFile->Seek(size-4);
                this->iReadDestFile->Read(last,4);
                this->iReadDestFile->Seek(offset);

                if(last[0]=='\r'&&last[1]=='\n'&&last[2]=='\r'&&last[3]=='\n')
                {
                    return OK;
                }
            }
        }
        else if(rs == 0)
        {
            if(interval < mTimeout)
                this->mReadTimer += interval;
            if(this->mReadTimer > this->mTimeout)
            {
                this->Error(ERROR_READ_TIMEOUT);
            }
            return ERROR;
        }
        else
        {
            this->iSocket->CloseConnect();
            this->Error(ERROR_READ_ERROR);
            return ERROR;
        }
    }

    return ERROR;
}

status_t CSocketReaderWriter::SetSocket(CSocket *isocket)
{
    this->iSocket = isocket;
    return OK;
}

uint32_t CSocketReaderWriter::GetTimeout()
{
    return this->mTimeout;
}
CClosure* CSocketReaderWriter::Callback()
{
    return mCallback;
}

fsize_t CSocketReaderWriter::ReadSizeThisTime()
{
    return mReadSizeThisTime;
}

fsize_t CSocketReaderWriter::WriteSizeThisTime()
{
    return mWriteSizeThisTime;
}

bool CSocketReaderWriter::IsConnected()
{
    ASSERT(iSocket);
    return iSocket->IsConnected();
}
