#ifndef __RPC_CALL_CONTEXT_H
#define __RPC_CALL_CONTEXT_H

#include "mem.h"
#include "memfile.h"
#include "stream.h"
#include "peermessage.h"

class CRpcCallContext{
public:
    CMem m_from;
    int m_callback_id;
    int m_method;

public:
	status_t FromPeerMessage(CPeerMessage *msg);
    CRpcCallContext();
    ~CRpcCallContext();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRpcCallContext *_p);
    int Comp(CRpcCallContext *_p);
    status_t Print(CFileBase *_buf);

    CMem* GetFrom();
    const char* GetFromStr();
    int GetCallbackId();
    int GetMethod();
    status_t SetFrom(CMem *_from);
    status_t SetFrom(const char *_from);
    status_t SetCallbackId(int _callback_id);
    status_t SetMethod(int _method);

    status_t Searialize(CStream *stream);
    status_t Searialize(CClosure *closure, int index);
    status_t Desearialize(CStream *stream);
    status_t Desearialize(const void *buf, size_t size);
    status_t Desearialize(CClosure *closure, int index);
};

#endif
