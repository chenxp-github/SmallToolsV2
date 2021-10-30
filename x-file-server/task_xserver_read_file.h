#ifndef __TASKXSERVERREADFILE_H
#define __TASKXSERVERREADFILE_H

#include "taskmgr.h"
#include "closure.h"
#include "x_file_client.h"
#include "mem.h"
#include "file.h"

class CTaskXServerReadFile:public CTask{
public:
    enum{
        ERROR_NONE = 0,
        ERROR_NO_SOURCE_FILE,
        ERROR_NO_OUT_FILE,
        ERROR_OPEN_SOURCE_FILE_FAIL,
        ERROR_OPEN_OUT_FILE_FAIL,
        ERROR_READ_FILE_FAIL,
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
    int m_src_handle;
    fsize_t m_start_offset;
    fsize_t m_read_size;
    fsize_t m_already_read_size;
    CFileBase *m_out_file;
    CMem m_out_file_name;
    CWeakPointer<CxFileClient> m_file_client;
/*##End Members##*/
    CFile m_tmp_out_file;
public:
    CTaskXServerReadFile();
    virtual ~CTaskXServerReadFile();
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
    int GetSrcHandle();
    fsize_t GetStartOffset();
    fsize_t GetReadSize();
    fsize_t GetAlreadyReadSize();
    CFileBase* GetOutFile();
    CMem* GetOutFileName();
    const char* GetOutFileNameStr();
    CxFileClient* GetFileClient();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetSrcFileName(CMem *_src_file_name);
    status_t SetSrcFileName(const char *_src_file_name);
    status_t SetSrcHandle(int _src_handle);
    status_t SetStartOffset(fsize_t _start_offset);
    status_t SetReadSize(fsize_t _read_size);
    status_t SetOutFile(CFileBase *_out_file);
    status_t SetOutFileName(CMem *_out_file_name);
    status_t SetOutFileName(const char *_out_file_name);
    status_t SetFileClient(CxFileClient *_file_client);
/*##End Setter_H##*/

    status_t MakeSureOutFile();
};

#endif

