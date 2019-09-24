#ifndef __SOCKETREADERWRITER_H
#define __SOCKETREADERWRITER_H

#include "cruntime.h"
#include "tcp.h"
#include "closure.h"
#include "taskmgr.h"

class CSocketReaderWriter{
public:
    enum{
        EVENT_ERROR = 1,
    };
    
    enum{
        ERROR_NONE = 0,
        ERROR_CONNECT_ERROR = 1000,
        ERROR_READ_TIMEOUT,
        ERROR_CONNECTION_CLOSED,
        ERROR_WRITE_TIMEOUT,
        ERROR_WRITE_ERROR,
        ERROR_READ_ERROR,
        ERROR_REACH_END_OF_FILE,
        ERROR_LINE_IS_TOO_LONG,
    };

    WEAK_REF_ID_DEFINE();
private:
	CTaskMgr *iTaskMgr;
    CSocket *iSocket;
    CFileBase *iReadDestFile;
    CFileBase *iWriteSrcFile;
    fsize_t mTotalReadLength;
    fsize_t mWriteEndOffset;
    fsize_t mWriteOffset;
    fsize_t mReadLength;
    fsize_t mReadSizeThisTime;
    fsize_t mWriteSizeThisTime;
    uint32_t mWriteTimer;
    uint32_t mReadTimer;
    uint32_t mTimeout;
    CClosure *mCallback;
public:
    bool IsConnected();
    fsize_t WriteSizeThisTime();
    fsize_t ReadSizeThisTime();
    CClosure* Callback();
    uint32_t GetTimeout();
    status_t SetSocket(CSocket *isocket);
    status_t DoReadUntilEmptyLine(uint32_t interval);
    status_t DoReadUntilEol(uint32_t interval, bool only_lf=false);
    status_t DoRead(uint32_t interval);
    status_t PrepareFileForRead(CFileBase *ifile, fsize_t size,bool reset);
    status_t Error(int err);
    status_t DoWrite(uint32_t interval);
    status_t PrepareFileForWrite(CFileBase *ifile, fsize_t start, fsize_t size);
    status_t SetTimeout(uint32_t timeout);
    CSocketReaderWriter();
    virtual ~CSocketReaderWriter();
    status_t InitBasic();
    status_t Init(CTaskMgr *taskmgr);
    status_t Destroy();
};

#endif
