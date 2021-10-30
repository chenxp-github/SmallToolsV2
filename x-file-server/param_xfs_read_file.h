#ifndef __PARAM_XFS_READ_FILE_H
#define __PARAM_XFS_READ_FILE_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_XfsReadFile
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_handle;
    int64_t m_offset;
    int m_size;
/*##End Members##*/

public:
    CParam_XfsReadFile();
    virtual ~CParam_XfsReadFile();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_XfsReadFile *_p);
    int Comp(CParam_XfsReadFile *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetHandle();
    int64_t GetOffset();
    int GetSize();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetHandle(int _handle);
    status_t SetOffset(int64_t _offset);
    status_t SetSize(int _size);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
