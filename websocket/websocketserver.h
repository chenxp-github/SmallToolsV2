#ifndef __WEB_SOCKET_SERVER_H
#define __WEB_SOCKET_SERVER_H

#include "mem.h"
#include "memfile.h"
#include "taskmgr.h"
#include "websocketmessagequeue.h"
#include "taskwebsocketserver.h"

class CWebSocketServer{
public:
	enum{
		EVENT_TASK_STOPPED=1,
		EVENT_GOT_PAYLOAD,
	};
    WEAK_REF_ID_DEFINE();
    TASK_CONTAINER_DEFINE();
public:
	CClosure m_Callback;
public:
    CWebSocketMessageQueue m_SendingQueue;
    int m_TaskWebSocketServer;
public:
	bool IsConnected();
	status_t SendMessage(CWebSocketMessage *msg);
	CClosure* Callback();
	CWebSocketMessageQueue* GetSendingQueue();
	status_t OnNewClientArrive(int socket_fd,bool clear);
    CWebSocketServer();
    ~CWebSocketServer();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

};

#endif
