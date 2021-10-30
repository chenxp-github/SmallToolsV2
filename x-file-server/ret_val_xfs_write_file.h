#ifndef __RET_VAL_XFS_WRITE_FILE_H
#define __RET_VAL_XFS_WRITE_FILE_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_XfsWriteFile
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_result;
    int m_ws;
/*##End Members##*/

public:
    CRetVal_XfsWriteFile();
    virtual ~CRetVal_XfsWriteFile();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_XfsWriteFile *_p);
    int Comp(CRetVal_XfsWriteFile *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetResult();
    int GetWs();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetResult(int _result);
    status_t SetWs(int _ws);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
