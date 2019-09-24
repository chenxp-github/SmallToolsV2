#ifndef __TASKWEBSOCKETSERVER_H
#define __TASKWEBSOCKETSERVER_H

#include "taskmgr.h"
#include "closure.h"
#include "tcp.h"
#include "socketreaderwriter.h"
#include "memfile.h"
#include "httpheader.h"
#include "websocketframeheader.h"
#include "weakptr.h"
#include "websocketserver.h"
#include "combfile.h"

class CWebSocketServer;
class CTaskWebSocketServer:public CTask{
public:
	enum{
		ERROR_NONE,
		ERROR_SOCKET_ERROR,
		ERROR_ERROR_HEADER_SIZE,
		ERROR_ERROR_HEADER_FORMAT,
		ERROR_WEB_SOCKET_CLOSED,
	};
    enum{
        EVENT_STOPPED = 1,
    };

public:
	int m_Step;
    int m_ReaderStep;
    int m_WriterStep;

	CSocket m_Socket;
	CSocketReaderWriter m_SocketRw;
	CMem m_TmpBufForRead;
    CMem m_TmpBufForWrite;
    CCombFile m_CombDataFile;
	CMem m_DecodedBuffer;
	CHttpHeader m_HttpHeader;
	CWebSocketFrameHeader m_TmpHeader;
    int m_SubTaskReader;
    int m_SubTaskWriter;
    CWeakPtr<CWebSocketServer> m_HostPtr;
public:
	status_t RunReader(uint32_t interval);
	status_t StartSubTaskReader();
    status_t RunWriter(uint32_t interval);
    status_t StartSubTaskWriter();

	status_t WebSocketFrameHeaderReadComplete();
	status_t CalcuSecWebSocketAccept(CFileBase *out);
	status_t MakeHandShakeResponse(CFileBase *out);
	status_t SetSocketFd(int socket_fd);
	CTaskWebSocketServer();
	virtual ~CTaskWebSocketServer();
	status_t Init(CTaskMgr *mgr);
	status_t Destroy();
	status_t InitBasic();
	status_t Run(uint32_t interval);
	const char* ErrStr(int err);
	status_t ReportError(int err);	
	status_t Start();
	status_t Stop(int err);
	CClosure* Callback();
};

#endif
