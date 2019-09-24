#ifndef __T_WEB_SOCKET_MESSAGE_QUEUE_H
#define __T_WEB_SOCKET_MESSAGE_QUEUE_H

#include "mem.h"
#include "websocketmessage.h"

class CWebSocketMessageQueue{
private:
	CWebSocketMessage **m_Index;
	int m_Top;
	int m_Size;
public:		
	int CompNode(CWebSocketMessage *node1, CWebSocketMessage *node2);
	status_t CopyNode(CWebSocketMessage *dst, CWebSocketMessage *src);
	status_t DelNode(CWebSocketMessage *node);
	CWebSocketMessage* CloneNode(CWebSocketMessage *node);
	status_t Copy(CWebSocketMessageQueue *stk);
	CWebSocketMessage* RemoveElem(int index);
	status_t InsElemPtr(int i, CWebSocketMessage *node);
	CWebSocketMessage* PopPtr();
	status_t AutoResize();
	status_t PushPtr(CWebSocketMessage *node);
	status_t InitBasic();  
	int BSearchPos(CWebSocketMessage *node,int order,int *find_flag);
	status_t InsOrderedPtr(CWebSocketMessage *node,int order,int unique);
	status_t InsOrdered(CWebSocketMessage *node,int order,int unique);
	status_t DelElem(int i);
	status_t InsElem(int i,CWebSocketMessage *node);
	int BSearch(CWebSocketMessage *node,int order);
	CWebSocketMessage* BSearchNode(CWebSocketMessage *node,int order);
	status_t Sort(int order);
	CWebSocketMessage* GetElem(int index);
	CWebSocketMessage* GetTopPtr();
	CWebSocketMessage* Search(CWebSocketMessage *node);
	int SearchPos(CWebSocketMessage *node);
	CWebSocketMessageQueue();
	status_t Clear();
	status_t DelTop();
	status_t Destroy();
	status_t Init(int init_size);
	bool IsEmpty();
	bool IsFull();
	status_t Pop(CWebSocketMessage *node);
	status_t Print(CFileBase *_buf);
	status_t Push(CWebSocketMessage *node);
	int GetLen();
	~CWebSocketMessageQueue();
};

#endif

