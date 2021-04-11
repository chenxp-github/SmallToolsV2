#include "websocketserver.h"
#include "syslog.h"
#include "mem_tool.h"

#define MAX_SENDING_QUEUE_LEN 4096

CWebSocketServer::CWebSocketServer()
{
    this->InitBasic();
}

CWebSocketServer::~CWebSocketServer()
{
    this->Destroy();
}

status_t CWebSocketServer::InitBasic()
{
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();
    m_TaskWebSocketServer = 0;
    m_SendingQueue.InitBasic();
	m_Callback.InitBasic();
    return OK;
}

status_t CWebSocketServer::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
    
    TASK_CONTAINER_INIT(_taskmgr);
    m_SendingQueue.Init(MAX_SENDING_QUEUE_LEN);
	m_Callback.Init();
    return OK;
}

status_t CWebSocketServer::Destroy()
{
    WEAK_REF_DESTROY();
	m_Callback.Destroy();
    m_SendingQueue.Destroy();
    QuitTask(&m_TaskWebSocketServer);
    this->InitBasic();
    return OK;
}

status_t CWebSocketServer::OnNewClientArrive(int socket_fd,bool clear)
{
    ASSERT(!IsTask(m_TaskWebSocketServer));

    if(clear)
    {
        m_SendingQueue.Clear();
    }

    CTaskWebSocketServer *ws_server;
    NEW(ws_server,CTaskWebSocketServer);
    ws_server->Init(GetTaskMgr());
    ws_server->SetSocketFd(socket_fd);
    ws_server->m_HostPtr.WeakRef(this);
	ws_server->Start();
    m_TaskWebSocketServer = ws_server->GetId();

    return OK;
}

CWebSocketMessageQueue* CWebSocketServer::GetSendingQueue()
{
    return &m_SendingQueue;
}

CClosure* CWebSocketServer::Callback()
{
	return &m_Callback;
}

status_t CWebSocketServer::SendMessage(CWebSocketMessage *msg)
{
	ASSERT(msg);
	ASSERT(msg->IsOnHeap());
	if(!m_SendingQueue.PushPtr(msg))
	{
		DEL(msg);
		return ERROR;
	}

	return OK;
}

bool CWebSocketServer::IsConnected()
{
	if(!IsTask(m_TaskWebSocketServer))
		return false;
	CTaskWebSocketServer *task = (CTaskWebSocketServer*)GetTask(m_TaskWebSocketServer);
	ASSERT(task);
	return task->IsConnected();
}
