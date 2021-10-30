#ifndef __PARAM_XFS_WRITE_FILE_H
#define __PARAM_XFS_WRITE_FILE_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_XfsWriteFile
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_handle;
    int64_t m_offset;
    uint8_t *m_data;
    int m_data_size;
/*##End Members##*/

public:
    CParam_XfsWriteFile();
    virtual ~CParam_XfsWriteFile();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_XfsWriteFile *_p);
    int Comp(CParam_XfsWriteFile *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetHandle();
    int64_t GetOffset();
    uint8_t* GetData();
    int GetDataSize();
    uint8_t GetDataElem(int _index);
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetHandle(int _handle);
    status_t SetOffset(int64_t _offset);
    status_t AllocData(int _len);
    status_t SetData(uint8_t _data[], int _len);
    status_t SetData(uint8_t *_data[], int _len);
    status_t SetDataElem(int _index,uint8_t _data);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
