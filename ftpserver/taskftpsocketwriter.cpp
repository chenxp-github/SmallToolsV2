// TaskFtpSocketWriter.cpp: implementation of the CTaskFtpSocketWriter class.
//
//////////////////////////////////////////////////////////////////////
#include "taskftpsocketwriter.h"
#include "mem.h"
#include "sys_log.h"
#include "mem_tool.h"

int CTaskFtpSocketWriter::EVENT_FINISH = 1;
int CTaskFtpSocketWriter::EVENT_QUIT = 2;

status_t CTaskFtpSocketWriter::ERROR_NONE = 0;
status_t CTaskFtpSocketWriter::ERROR_CONNECTION_CLOSED = 1;
status_t CTaskFtpSocketWriter::ERROR_READ_ERROR = 2;
status_t CTaskFtpSocketWriter::ERROR_WRITE_TIMEOUT = 3;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTaskFtpSocketWriter::CTaskFtpSocketWriter()
{
    this->InitBasic();
}
CTaskFtpSocketWriter::~CTaskFtpSocketWriter()
{
    this->Destroy();
}
status_t CTaskFtpSocketWriter::InitBasic()
{
    CTask::InitBasic();
    this->iSrcFile = NULL;
	this->iSocket = NULL;
	this->mBuf = NULL;
	this->mBufOffset = 0;
	this->mTimeout = 60*1024;
	this->mWriteTime = 0;	
	m_Callback.InitBasic();
    return OK;
}
status_t CTaskFtpSocketWriter::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    NEW(this->mBuf,CMem);
	this->mBuf->Init();
	this->mBuf->Malloc(256*1024);
	m_Callback.Init();
    return OK;
}
status_t CTaskFtpSocketWriter::Destroy()
{
	DEL(this->mBuf);
	m_Callback.Destroy();
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskFtpSocketWriter::Run(uint32_t interval)
{
	ASSERT(this->iSrcFile && this->iSocket);
	fsize_t rs,ws;

	if(!this->iSocket->IsConnected())
	{
		if(this->IsComplete())
			this->Stop(ERROR_NONE);
		else
			this->Stop(ERROR_CONNECTION_CLOSED);
		return ERROR;
	}

	if(mBufOffset >= this->mBuf->GetSize())
	{
		mBuf->SetSize(0);
		rs = this->iSrcFile->Read(mBuf->GetRawBuf(),(int_ptr_t)mBuf->GetMaxSize());
		if(rs <= 0)
		{
			this->Stop(ERROR_READ_ERROR);
			return ERROR;
		}
		this->mBuf->SetSize(rs);
		this->mBufOffset = 0;
	}	
	else
	{
		ws = this->iSocket->Write(mBuf->GetRawBuf() + mBufOffset,(int_ptr_t)(mBuf->GetSize() - mBufOffset));
		if(ws > 0)
		{
			mBufOffset += ws;
			this->mWriteTime = 0;
		}
		else if(ws == 0)
		{
			this->mWriteTime += interval;
			if(this->mWriteTime >= this->mTimeout)
			{
				this->iSocket->CloseConnect();
				this->Stop(ERROR_WRITE_TIMEOUT);
			}
		}
		else
		{
			ASSERT(0);
		}
	}

	if(this->IsComplete())
	{
		this->Stop(ERROR_NONE);
	}
    return OK;
}
status_t CTaskFtpSocketWriter::Start()
{
	this->mWriteTime = 0;
	this->ClearBuf();
	return this->Resume();
}
status_t CTaskFtpSocketWriter::Stop(status_t err)
{
	if(this->IsDead())return ERROR;
	this->ReportError(err);
	ASSERT(this->iSocket);
	if(!this->iSocket->IsConnected())
	{
		this->Quit();
		m_Callback.SetParamInt(1,err);
		m_Callback.SetParamPointer(2,this);
		m_Callback.Run(EVENT_QUIT);
		return OK;
	}
	
	m_Callback.SetParamInt(1,err);
	m_Callback.SetParamPointer(2,this);
	m_Callback.Run(EVENT_FINISH);
	return this->Suspend();
}
status_t CTaskFtpSocketWriter::IsComplete()
{
	ASSERT(this->iSrcFile);
	return this->iSrcFile->IsEnd() && (mBufOffset >= this->mBuf->GetSize());
}
status_t CTaskFtpSocketWriter::SetSocket(CSocket *i_socket)
{
	ASSERT(i_socket);
	this->iSocket = i_socket;
	return OK;
}

status_t CTaskFtpSocketWriter::SetSrcFile(CFileBase *i_file)
{
	return this->SetSrcFile(0,i_file);
}

status_t CTaskFtpSocketWriter::SetSrcFile(fsize_t start,CFileBase *i_file)
{
	ASSERT(i_file);
	this->ClearBuf();
	i_file->Seek(start);
	this->iSrcFile = i_file;
	return OK;
}

status_t CTaskFtpSocketWriter::ClearBuf()
{
	this->mBufOffset = 0;
	this->mBuf->SetSize(0);
	return OK;
}
const char * CTaskFtpSocketWriter::ErrorToString(int32_t err)
{
	if(err == ERROR_NONE)
		return "none";
	if(err == ERROR_CONNECTION_CLOSED)
		return "connection closed";
	if(err == ERROR_READ_ERROR)
		return "read error";
	if(err == ERROR_WRITE_TIMEOUT)
		return "write timeout";
	return "unknown error";
}

status_t CTaskFtpSocketWriter::ReportError(int32_t err)
{
	LOCAL_MEM(mem);

	mem.Puts("CTaskFtpSocketWriter ");
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
		XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR, "%s",mem.CStr());
	}
	return OK;
}
status_t CTaskFtpSocketWriter::SetTimeout(int32_t timeout)
{
	this->mTimeout=timeout;
	return OK;
}
