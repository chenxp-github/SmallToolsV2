#ifndef __RPC_CALL_CONTEXT_H
#define __RPC_CALL_CONTEXT_H

#include "mem.h"
#include "memfile.h"
#include "stream.h"
#include "peermessage.h"

class CRpcCallContext{
public:
    CMem m_from;
    int32_t m_callback_id;
    int32_t m_method;

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
    int32_t GetCallbackId();
    int32_t GetMethod();
    status_t SetFrom(CMem *_from);
    status_t SetFrom(const char *_from);
    status_t SetCallbackId(int32_t _callback_id);
    status_t SetMethod(int32_t _method);

    status_t Searialize(CStream *stream);
    status_t Searialize(CClosure *closure, int index);
    status_t Desearialize(CStream *stream);
    status_t Desearialize(const void *buf, size_t size);
    status_t Desearialize(CClosure *closure, int index);

    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);

    status_t Clear();
};

#endif
