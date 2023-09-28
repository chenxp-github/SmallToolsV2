// TaskFtpSocketReader.cpp: implementation of the CTaskFtpSocketReader class.
//
//////////////////////////////////////////////////////////////////////
#include "taskftpsocketreader.h"
#include "sys_log.h"
#include "mem_tool.h"

int CTaskFtpSocketReader::EVENT_FINISH = 1;
int CTaskFtpSocketReader::EVENT_QUIT = 2;

status_t CTaskFtpSocketReader::ERROR_NONE = 0;
status_t CTaskFtpSocketReader::ERROR_CONNECTION_CLOSED	= 1;
status_t CTaskFtpSocketReader::ERROR_READ_TIMEOUT = 2;
status_t CTaskFtpSocketReader::ERROR_WAIT_BUF_TIME_OUT = 3;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTaskFtpSocketReader::CTaskFtpSocketReader()
{
    this->InitBasic();
}
CTaskFtpSocketReader::~CTaskFtpSocketReader()
{
    this->Destroy();
}
status_t CTaskFtpSocketReader::InitBasic()
{
    CTask::InitBasic();

    this->mQBuf = NULL;
	this->iSocket = NULL;
	this->iDstFile = NULL;
	this->mFileLen = 0;
	this->mTimeout = 60*1024;
	this->mReadTime = 0;
	this->mWaitBufTime = 0;
	this->mIsFinishMsgSend = false;
	m_Callback.InitBasic();

    return OK;
}
status_t CTaskFtpSocketReader::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    
	NEW(this->mQBuf,CQueueFile);
	this->mQBuf->Init(64*1024);

	m_Callback.Init();
    return OK;
}
status_t CTaskFtpSocketReader::Destroy()
{
    DEL(this->mQBuf);
	m_Callback.Destroy();
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskFtpSocketReader::Run(uint32_t interval)
{
	ASSERT(this->iSocket);
	fsize_t left_size,size,need_read;

	this->PutDataToDesFile();
	if(!this->iSocket->IsConnected())
	{
		if(this->IsComplete())
		{
			if(this->mQBuf->GetSize() <= 0)
			{
				this->Stop(ERROR_NONE);
			}
			else
			{
				this->mWaitBufTime += interval;
				if(this->mWaitBufTime >= this->mTimeout)
					this->Stop(ERROR_WAIT_BUF_TIME_OUT);
			}
		}
		else
		{
			this->Stop(ERROR_CONNECTION_CLOSED);
		}
		return ERROR;
	}

	left_size = this->mQBuf->GetFreeSize();
	if(left_size <= 0)
		return ERROR;

	LOCAL_MEM_WITH_SIZE(mem,64*1024);
	need_read = mem.GetMaxSize();
	if(need_read > left_size)
		need_read = left_size;
	size = this->iSocket->Read(mem.GetRawBuf(),(int_ptr_t)need_read);
	if(size > 0)
	{
		this->mReadTime = 0;
		this->mQBuf->Write(mem.GetRawBuf(),(int_ptr_t)size);
	}
	else if(size == 0)
	{
		this->mReadTime += interval;
		if(this->mReadTime >= this->mTimeout)
		{
			this->iSocket->CloseConnect();
			this->Stop(ERROR_READ_TIMEOUT);
		}
	}
	else
	{
		ASSERT(0);
	}

    return OK;
}

status_t CTaskFtpSocketReader::SetDstFile(CFileBase *i_file, fsize_t read_len)
{
	this->mIsFinishMsgSend = false;
	this->iDstFile = i_file;
	this->mFileLen = read_len;
	return OK;
}
status_t CTaskFtpSocketReader::IsComplete()
{
	return this->mFileLen <= 0;
}
status_t CTaskFtpSocketReader::PutDataToDesFile()
{
	if(this->iDstFile == NULL)
		return ERROR;
	
	fsize_t size = this->mQBuf->GetSize();
	if(size > 0)
	{
		if(size > this->mFileLen)
			size = this->mFileLen;
		this->mFileLen -= size;

		char buf[FILEBASE_LBUF_SIZE];
		while(size > 0)
		{
			int_ptr_t rs = this->mQBuf->Read(buf,FILEBASE_LBUF_SIZE);
			if(rs > 0)
			{
				this->iDstFile->Write(buf,rs);
				size -= rs;
			}
		}
	}	

	if(this->IsComplete() && (!mIsFinishMsgSend))
	{	
		this->mIsFinishMsgSend = true;
		m_Callback.SetParamPointer(1,this);
		m_Callback.Run(EVENT_FINISH);			
		return OK;
	}
	return OK;
}
status_t CTaskFtpSocketReader::SetSocket(CSocket *i_socket)
{
	ASSERT(i_socket);
	this->iSocket = i_socket;
	return OK;
}
status_t CTaskFtpSocketReader::IsBufEmpty()
{
	return this->mQBuf->GetSize() <= 0;
}

status_t CTaskFtpSocketReader::Stop(status_t err)
{
	if(this->IsDead())return ERROR;
	this->Quit();
	this->ReportError(err);
	this->PutDataToDesFile();
	m_Callback.SetParamInt(1,err);
	m_Callback.SetParamPointer(2,this);
	m_Callback.Run(EVENT_QUIT);
	return OK;
}

status_t CTaskFtpSocketReader::Start()
{
	this->mIsFinishMsgSend = false;
	this->mWaitBufTime = 0;
	this->mReadTime = 0;
	this->Resume();
	return OK;
}

const char * CTaskFtpSocketReader::ErrorToString(int32_t err)
{
	if(err == ERROR_NONE)
		return "none";
	if(err == ERROR_CONNECTION_CLOSED)
		return "connection closed";
	if(err ==  ERROR_READ_TIMEOUT)
		return "read timeout";		
	if(err == ERROR_WAIT_BUF_TIME_OUT)
		return "wait buf time out";
	return "unknown error";
}

status_t CTaskFtpSocketReader::ReportError(int32_t err)
{
	LOCAL_MEM(mem);

	mem.Puts("CTaskFtpSocketReader ");
	mem.Printf("(%d) ",this->GetId());

	if(err != ERROR_NONE)
	{
		mem.Puts("exit with error:");
		mem.Puts(this->ErrorToString(err));		
	}
	else 
	{
		mem.Puts("exit successfully!");
	}
	if(err != ERROR_NONE)
	{
		XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,"%s",mem.CStr());
	}
	return OK;
}
status_t CTaskFtpSocketReader::SetTimeout(int32_t timeout)
{
	this->mTimeout=timeout;
	return OK;
}

bool CTaskFtpSocketReader::BufHasEmptyLine()
{
	return this->mQBuf->HasEmptyLine();
}

bool CTaskFtpSocketReader::BufHasWholeLine()
{
	return this->mQBuf->HasWholeLine();
}

CQueueFile * CTaskFtpSocketReader::GetQueueBuf()
{
	return this->mQBuf;
}

CFileBase * CTaskFtpSocketReader::GetDstFile()
{
	return this->iDstFile;
}
