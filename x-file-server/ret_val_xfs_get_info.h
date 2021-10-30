#ifndef __RET_VAL_XFS_GET_INFO_H
#define __RET_VAL_XFS_GET_INFO_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
#include "x_file_info.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_XfsGetInfo
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_result;
    CxFileInfo m_info;
/*##End Members##*/

public:
    CRetVal_XfsGetInfo();
    virtual ~CRetVal_XfsGetInfo();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_XfsGetInfo *_p);
    int Comp(CRetVal_XfsGetInfo *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetResult();
    CxFileInfo* GetInfo();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetResult(int _result);
    status_t SetInfo(CxFileInfo *_info);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
