#ifndef __TASKXSERVERWRITEFILE_H
#define __TASKXSERVERWRITEFILE_H

#include "taskmgr.h"
#include "closure.h"
#include "file.h"
#include "x_file_client.h"
#include "mem.h"

class CTaskXServerWriteFile:public CTask{
public:
    enum{
        ERROR_NONE = 0,
        ERROR_NO_SOURCE_FILE,
        ERROR_NO_OUT_FILE,
        ERROR_OPEN_SOURCE_FILE_FAIL,
        ERROR_OPEN_OUT_FILE_FAIL,
        ERROR_READ_FILE_FAIL,
        ERROR_WRITE_FILE_FAIL,
        ERROR_SOCKET_ERROR,        
    };
    enum{
        EVENT_NONE = 0,
        EVENT_STOPPED,
    };
    int m_Step;
    CClosure m_Callback;
/*##Begin Members##*/
public:
    CMem m_src_file_name;
    CFileBase *m_src_file;
    fsize_t m_start_offset;
    fsize_t m_write_size;
    fsize_t m_already_write_size;
    int m_out_handle;
    CMem m_out_file_name;
    CWeakPointer<CxFileClient> m_file_client;
/*##End Members##*/
    CFile m_tmp_src_file;
    CMem m_tmp_buffer;
public:
    CTaskXServerWriteFile();
    virtual ~CTaskXServerWriteFile();
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
    CMem* GetSrcFileName();
    const char* GetSrcFileNameStr();
    CFileBase* GetSrcFile();
    fsize_t GetStartOffset();
    fsize_t GetWriteSize();
    fsize_t GetAlreadyWriteSize();
    int GetOutHandle();
    CMem* GetOutFileName();
    const char* GetOutFileNameStr();
    CxFileClient* GetFileClient();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetSrcFileName(CMem *_src_file_name);
    status_t SetSrcFileName(const char *_src_file_name);
    status_t SetSrcFile(CFileBase *_src_file);
    status_t SetStartOffset(fsize_t _start_offset);
    status_t SetWriteSize(fsize_t _write_size);
    status_t SetOutHandle(int _out_handle);
    status_t SetOutFileName(CMem *_out_file_name);
    status_t SetOutFileName(const char *_out_file_name);
    status_t SetFileClient(CxFileClient *_file_client);
/*##End Setter_H##*/

    status_t CloseRemoteFile();
};

#endif

