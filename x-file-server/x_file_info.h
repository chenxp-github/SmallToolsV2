#ifndef __X_FILE_INFO_H
#define __X_FILE_INFO_H

#include "minibson.h"
/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CxFileInfo
/*##Begin Bases##*/
/*##End Bases##*/
{
public:
    enum{
        OPERATION_NONE=0,
        OPERATION_CREATE,
        OPERATION_DELETE,
        OPERATION_OVERWRITE,
    };
/*##Begin Members##*/
public:
    int64_t m_size;
    CMem m_full_path;
    CMem m_file_name;
    uint32_t m_flags;
    int64_t m_modify_time;
    int m_operation;
    int64_t m_current_size;
/*##End Members##*/

public:
    CxFileInfo();
    virtual ~CxFileInfo();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CxFileInfo *_p);
    int Comp(CxFileInfo *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int64_t GetSize();
    CMem* GetFullPath();
    const char* GetFullPathStr();
    CMem* GetFileName();
    const char* GetFileNameStr();
    uint32_t GetFlags();
    int64_t GetModifyTime();
    int GetOperation();
    int64_t GetCurrentSize();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetSize(int64_t _size);
    status_t SetFullPath(CMem *_full_path);
    status_t SetFullPath(const char *_full_path);
    status_t SetFileName(CMem *_file_name);
    status_t SetFileName(const char *_file_name);
    status_t SetFlags(uint32_t _flags);
    status_t SetModifyTime(int64_t _modify_time);
    status_t SetOperation(int _operation);
    status_t SetCurrentSize(int64_t _current_size);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/

    status_t SaveBson_Javascript(CMiniBson *_bson);

    FLAG_FUNC(m_flags,IsDir,0x00000001);

};

#endif
