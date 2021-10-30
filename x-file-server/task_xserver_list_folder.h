#ifndef __TASKXSERVERLISTFOLDER_H
#define __TASKXSERVERLISTFOLDER_H

#include "taskmgr.h"
#include "closure.h"

/*##Begin Inlcudes##*/
#include "x_file_client.h"
#include "x_file_info_list.h"
/*##End Inlcudes##*/

class CTaskXServerListFolder:public CTask{
public:
    enum{
        ERROR_NONE = 0,
        ERROR_NO_SOURCE_FOLDER,
        ERROR_LIST_FOLDER_FAIL,
    };
    enum{
        EVENT_NONE = 0,
        EVENT_STOPPED,
        EVENT_RECV_FILE_INFO,
    };
    int m_Step;
    CClosure m_Callback;
/*##Begin Members##*/
public:
    CMem m_folder_name;
    CWeakPointer<CxFileClient> m_file_client;
    CxFileInfoList m_pending_queue;
/*##End Members##*/
public:
    CTaskXServerListFolder();
    virtual ~CTaskXServerListFolder();
    status_t Init(CTaskMgr *mgr);
    status_t Destroy();
    status_t InitBasic();
    status_t Run(uint32_t interval);
    const char* ErrStr(int err);
    status_t ReportError(int err);    
    status_t Start();
    status_t Stop(int err);
    CClosure* Callback();
/*##Begin Getter_H##*/
    CMem* GetFolderName();
    const char* GetFolderNameStr();
    CxFileClient* GetFileClient();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetFolderName(CMem *_folder_name);
    status_t SetFolderName(const char *_folder_name);
    status_t SetFileClient(CxFileClient *_file_client);
/*##End Setter_H##*/

    status_t AddPendingFolder(CMem *path);
    status_t OnRecvFolderListResult(CxFileInfoList *list);
};

#endif

