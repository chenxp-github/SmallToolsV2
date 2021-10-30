#ifndef __X_FILE_CLIENT_H
#define __X_FILE_CLIENT_H

#include "mem.h"
#include "memfile.h"
#include "rpc_service_base.h"
#include "rpc_call_context.h"

class CxFileClient:public CRpcServiceBase{

public:
    CxFileClient();
    virtual ~CxFileClient();

    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();    
    virtual status_t OnSocketConnected();
    virtual status_t OnSocketDisconnected();
    virtual status_t OnRequest(CPeerMessage *msg);

/*##Begin ServiceFunctions_H ##*/
    status_t ListFolder(CMem *_path, CClosure *_callback);
    status_t GetInfo(CMem *_path, CClosure *_callback);
    status_t ChangeCurrentDirectory(CMem *_dir, CClosure *_callback);
    status_t GetCurrentDirectory(CClosure *_callback);
    status_t OpenFile(CMem *_path, CMem *_mode, CClosure *_callback);
    status_t CloseFile(int _handle, CClosure *_callback);
    status_t ReadFile(int _handle, int64_t _offset, int _size, CClosure *_callback);
    status_t WriteFile(int _handle, int64_t _offset, uint8_t *_data,int _data_len, CClosure *_callback);
    status_t CreateFolder(CMem *_path, CClosure *_callback);
    status_t DeleteFolder(CMem *_path, CClosure *_callback);
    status_t DeleteFile(CMem *_path, CClosure *_callback);
/*##End ServiceFunctions_H ##*/
};

#endif



