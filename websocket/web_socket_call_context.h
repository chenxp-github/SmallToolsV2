#ifndef __WEB_SOCKET_CALL_CONTEXT_H
#define __WEB_SOCKET_CALL_CONTEXT_H

#include "mem.h"
#include "memfile.h"
#include "minibson.h"
#include "websocketmessage.h"

class CWebSocketCallContext{
public:
    int m_Method;
    int m_CallbackId;

public:
	status_t FromMessage(CWebSocketMessage *msg);
    CWebSocketCallContext();
    ~CWebSocketCallContext();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CWebSocketCallContext *_p);
    int Comp(CWebSocketCallContext *_p);
    status_t Print(CFileBase *_buf);
    int GetMethod();
    int GetCallbackId();
    status_t SetMethod(int _method);
    status_t SetCallbackId(int _callbackid);

    status_t Searialize(CMem *stream);
    status_t Desearialize(CMem *stream);
    status_t Desearialize(const void *buf, size_t size);
    status_t Searialize(CClosure *closure, int index);
    status_t Desearialize(CClosure *closure, int index);

};

#endif
