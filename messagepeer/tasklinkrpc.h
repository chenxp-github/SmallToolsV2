#ifndef __TASKLINKRPC_H
#define __TASKLINKRPC_H

#include "tasklinkrpcreader.h"
#include "tasklinkrpcwriter.h"
#include "taskmgr.h"
#include "socketreaderwriter.h"

class CTaskLinkRpc:public CTask{
public:
    enum{
        ERROR_NONE = 0,
        ERROR_LINKRPC_READER_STOPPED,
        ERROR_LINKRPC_WRITER_STOPPED,
        ERROR_EXCEED_MAX_RETRIES,
        ERROR_SOCKET_ERROR,
        ERROR_CONNECT_ERROR,
        ERROR_WRONG_HEADER_FORMAT,
    };
public:
    CSocket *mSocket;
    CSocketReaderWriter *mSocketRwer;
    int mTaskReader;
    int mTaskWriter;
    CTaskLinkRpcReader *mReader;
    CTaskLinkRpcWriter *mWriter;
    CMem *iHeaderBuf;
    CFileBase *iDataBuf;
    int mStep;
    int mRetries;
    int mMaxRetries;
public:
    CSocket* GetSocket();
    status_t WriteImmediately();
    status_t Retry(int err);
    status_t SetMaxRetries(int max);
    int GetRetries();
    status_t SetRetries(int retry);
    status_t Start();
    status_t SendResponse(CFileBase *header, CFileBase *data);
    status_t SendRequest(CFileBase *header, CFileBase *data);
    bool IsAlive();
    virtual status_t Stop(int err);
    virtual status_t OnSocketError();
    status_t SetTimeout(int to);
    status_t StartReaderAndWriter();
    status_t TransferSocket(CSocket *from);
    status_t SetSocket(int32_t fd);
    status_t SetDataBuf(CFileBase *iBuf);
    status_t SetHeaderBuf(CMem *iBuf);
    status_t StartSending();
    status_t CreateWriter();
    status_t CreateReader();
    CTaskLinkRpc();
    virtual ~CTaskLinkRpc();
    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t Run(uint32_t interval);
    virtual const char * ErrorToString(int err);
    status_t ReportError(int err);
    status_t Reset();

    //connection
    virtual status_t OnGetSocket();
    //reading package
    virtual bool CanReadNextPackage();
    virtual status_t OnGotPackageHeader(LINKRPC_HEADER *header,CMem *header_data);
    virtual status_t OnGotPackageData(LINKRPC_HEADER *header,CMem *header_data,CFileBase *data);
    //writing package
    virtual status_t OnPreparePackageToSend();
    virtual status_t OnPackageSendOk();
};

#endif
