#ifndef __TASKXSERVERSYNCFOLDER_H
#define __TASKXSERVERSYNCFOLDER_H

#include "taskmgr.h"
#include "closure.h"
#include "mem.h"
#include "x_file_info_list.h"

/*##Begin Inlcudes##*/
#include "x_file_info_list.h"
#include "x_file_client.h"
/*##End Inlcudes##*/

class CTaskXServerSyncFolder:public CTask{
public:
    enum{
        ERROR_NONE = 0,
        ERROR_CREATE_DIR_FAIL,
        ERROR_OPEN_FILE_FAIL,
        ERROR_CLOSE_FILE_FAIL,
        ERROR_DELETE_FILE_FAIL,
        ERROR_DELETE_DIR_FAIL,
        ERROR_UPLOAD_FILE_FAIL,
        ERROR_USER_CANCEL,
    };
    enum{
        EVENT_NONE = 0,
        EVENT_STOPPED,
    };
    int m_Step;
    CClosure m_Callback;
/*##Begin Members##*/
public:
    CMem m_local_folder;
    CMem m_remote_folder;
    CxFileInfoList m_current_list;
    CxFileInfoList m_create_list;
    CxFileInfoList m_delete_list;
    CWeakPointer<CxFileClient> m_file_client;
    CMem m_snapshot_name_prefix;
/*##End Members##*/
    int m_task_upload;

public:
    CTaskXServerSyncFolder();
    virtual ~CTaskXServerSyncFolder();
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
    CMem* GetLocalFolder();
    const char* GetLocalFolderStr();
    CMem* GetRemoteFolder();
    const char* GetRemoteFolderStr();
    CxFileClient* GetFileClient();
    CMem* GetSnapshotNamePrefix();
    const char* GetSnapshotNamePrefixStr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetLocalFolder(CMem *_local_folder);
    status_t SetLocalFolder(const char *_local_folder);
    status_t SetRemoteFolder(CMem *_remote_folder);
    status_t SetRemoteFolder(const char *_remote_folder);
    status_t SetFileClient(CxFileClient *_file_client);
    status_t SetSnapshotNamePrefix(CMem *_snapshot_name_prefix);
    status_t SetSnapshotNamePrefix(const char *_snapshot_name_prefix);
/*##End Setter_H##*/

    status_t GetSnapshotFullPath(CMem *out);
    status_t DiffLocalFolder();
    status_t ResetLocalFolder();

    status_t GetCurrentProgress(CxFileInfo *progress);
};

#endif

