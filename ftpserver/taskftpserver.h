#ifndef __TASKFTPSERVER_H
#define __TASKFTPSERVER_H

#include "taskmgr.h"
#include "tcp.h"
#include "taskftpsocketreader.h"
#include "taskftpsocketwriter.h"
#include "ftpconfiglist.h"
#include "file.h"
#include "memfile.h"

class CTaskFtpServer:public CTask{
public:
	static status_t ERROR_NONE;
	static status_t ERROR_SOCKET_CLOSED;
	static status_t ERROR_PERMISSION_DENIED;
	static status_t ERROR_CONNECT_TIMEOUT;

	int mTaskFtpSocketReader;
	int mTaskFtpSocketWriter;
	CMem *mRequest;
	CMem *mResponse;
	CSocket *iSocket;
	CTcpServer *mServer;
	int mStep;
	CFtpConfigList *iConfigList;
	CFtpConfig *mCurConfig;
	status_t mAfterResponseParam;
	CMemFile *mTmpFile;
	int mTaskFtpSocketPort;
	CTcpClient *mDataClientSocket;
	CTcpServer *mDataServerSocket;
	CSocket *iDataSocket;
	int mTaskDataSocketReader;
	int mTaskDataSocketWriter;
	CMem *mCurDir;
	CFile *mIoFile;
	int mTransferType;
	CMem *mClientIP;
	CMem *mFixServerIp;
	CMem *mFileNameToBeRenamed;
	status_t mAllowAnonymous;
	CMem *mAnonymousRootPath;
	fsize_t mRestPos;
	uint32_t mTimeout;
	uint32_t mConnectTime;
	int mTimerSyncTimeout;
	int mEncoding;
public:
	static status_t on_sync_timeout(CClosure *closure);
	status_t SyncTimeout();
	const char * ErrorToString(int err);
	status_t ReportError(int err);
	static status_t on_reader_event(CClosure *closure);
	static status_t on_writer_event(CClosure *closure);
	static status_t on_data_reader_event(CClosure *closure);
	static status_t on_data_writer_event(CClosure *closure);
	status_t SetTimeout(int timeout);
	status_t SetAnonymouseRootPath(const char *str);
	status_t AllowAnonymouse(status_t enable);
	status_t SetServerSocket(int snum, const char *client_ip);
	status_t MakeDirList(const char *dir, CFileBase *list);
	status_t ChangeDir(const char *dir);
	status_t ToAbsPath(CFileBase *file);
	status_t MakeFileList(const char *dir, CFileBase *list);
	CTaskFtpSocketReader * GetDataReader(status_t renew);
	CTaskFtpSocketWriter * GetDataWritter(status_t renew);
	status_t AfterResponse();
	status_t SetConfigs(CFtpConfigList *i_list);
	status_t PrepareResponse(const char *response);
	status_t PrepareResponse(CFileBase *file);
	status_t HandleRequest();
	status_t UpdateCurConfig();
	status_t PrepareRequest(const char *request);
	status_t PrepareRequest(CFileBase *file);
	status_t Start();
	status_t Stop(status_t err);
	CTaskFtpSocketWriter * GetWritter(status_t renew);
	CTaskFtpSocketReader * GetReader(status_t renew);
	CTaskFtpServer();
	virtual ~CTaskFtpServer();
	status_t Init(CTaskMgr *mgr);
	status_t Destroy();
	status_t InitBasic();
  	virtual status_t Run(uint32_t interval);
	status_t SetEncoding(int encoding);
	status_t SetFixServerIp(const char *server_ip);
};

#endif
