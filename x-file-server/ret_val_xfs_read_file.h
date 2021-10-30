#ifndef __RET_VAL_XFS_READ_FILE_H
#define __RET_VAL_XFS_READ_FILE_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_XfsReadFile
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_result;
    uint8_t *m_data;
    int m_data_size;
/*##End Members##*/

public:
    CRetVal_XfsReadFile();
    virtual ~CRetVal_XfsReadFile();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_XfsReadFile *_p);
    int Comp(CRetVal_XfsReadFile *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetResult();
    uint8_t* GetData();
    int GetDataSize();
    uint8_t GetDataElem(int _index);
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetResult(int _result);
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
