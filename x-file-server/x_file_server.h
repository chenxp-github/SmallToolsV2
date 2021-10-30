#ifndef __X_FILE_SERVER_H
#define __X_FILE_SERVER_H

#include "mem.h"
#include "memfile.h"
#include "rpc_service_base.h"
#include "rpc_call_context.h"
#include "x_file_session_map.h"

class CxFileServer:public CRpcServiceBase{

public:
    CMem m_RootPath;
    CxFileSessionMap m_SessionMap;
    int m_TaskAutoClear;
public:
    CxFileServer();
    virtual ~CxFileServer();

    status_t InitBasic();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();    
    virtual status_t OnSocketConnected();
    virtual status_t OnSocketDisconnected();
    virtual status_t OnRequest(CPeerMessage *msg);

/*##Begin ServiceFunctions_H ##*/
    status_t OnListFolder(CPeerMessage *_message);
    status_t OnGetInfo(CPeerMessage *_message);
    status_t OnChangeCurrentDirectory(CPeerMessage *_message);
    status_t OnGetCurrentDirectory(CPeerMessage *_message);
    status_t OnOpenFile(CPeerMessage *_message);
    status_t OnCloseFile(CPeerMessage *_message);
    status_t OnReadFile(CPeerMessage *_message);
    status_t OnWriteFile(CPeerMessage *_message);
    status_t OnCreateFolder(CPeerMessage *_message);
    status_t OnDeleteFolder(CPeerMessage *_message);
    status_t OnDeleteFile(CPeerMessage *_message);
/*##End ServiceFunctions_H ##*/

    CxFileSession *GetSession(CMem *name);
    CxFileSession *GetOrCreateSession(CMem *name);
    status_t SetRootPath(CMem *path);
    status_t SetRootPath(const char *path);
    const char *GetRootPathStr();
    CMem *GetRootPath();

    status_t BuildLocalPath(CxFileSession *session, CMem *rpath, CMem *out);

    status_t StartAutoClearSessionTask();
    status_t OnAutoClearSession(uint32_t interval);
};

#endif



