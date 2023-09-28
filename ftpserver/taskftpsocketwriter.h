// TaskFtpSocketWriter.h: interface for the CTaskFtpSocketWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKFTPSOCKETWRITER_H__67FE86F8_E1DC_48EF_B458_5698B3C211AF__INCLUDED_)
#define AFX_TASKFTPSOCKETWRITER_H__67FE86F8_E1DC_48EF_B458_5698B3C211AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tcp.h"
#include "filebase.h"
#include "taskmgr.h"
#include "mem.h"
#include "closure.h"

class CTaskFtpSocketWriter:public CTask{
public:
	static status_t ERROR_NONE;
	static status_t ERROR_CONNECTION_CLOSED;
	static status_t ERROR_READ_ERROR;
	static status_t ERROR_WRITE_TIMEOUT;
	
	static int EVENT_FINISH;
	static int EVENT_QUIT;

	CSocket *iSocket;
	CFileBase *iSrcFile;
	CMem *mBuf;
	fsize_t mBufOffset;
	uint32_t mTimeout;
	uint32_t mWriteTime;
	CClosure m_Callback;
public:
	status_t SetTimeout(int32_t timeout);
	const char * ErrorToString(int32_t err);
	status_t ReportError(int32_t err);
	status_t SetSrcFile(fsize_t start,CFileBase *i_file);
	status_t ClearBuf();
	status_t SetSrcFile(CFileBase *i_file);
	status_t SetSocket(CSocket *i_socket);
	status_t IsComplete();
	status_t Stop(status_t err);
	status_t Start();
	CTaskFtpSocketWriter();
	virtual ~CTaskFtpSocketWriter();
	status_t Init(CTaskMgr *mgr);
	status_t Destroy();
	status_t InitBasic();
	virtual status_t Run(uint32_t interval);
};

#endif // !defined(AFX_TASKFTPSOCKETWRITER_H__67FE86F8_E1DC_48EF_B458_5698B3C211AF__INCLUDED_)
