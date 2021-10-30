#ifndef __RET_VAL_XFS_CREATE_FOLDER_H
#define __RET_VAL_XFS_CREATE_FOLDER_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_XfsCreateFolder
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_result;
/*##End Members##*/

public:
    CRetVal_XfsCreateFolder();
    virtual ~CRetVal_XfsCreateFolder();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_XfsCreateFolder *_p);
    int Comp(CRetVal_XfsCreateFolder *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetResult();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetResult(int _result);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
