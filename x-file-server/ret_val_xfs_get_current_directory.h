#ifndef __RET_VAL_XFS_GET_CURRENT_DIRECTORY_H
#define __RET_VAL_XFS_GET_CURRENT_DIRECTORY_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_XfsGetCurrentDirectory
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_result;
    CMem m_dir;
/*##End Members##*/

public:
    CRetVal_XfsGetCurrentDirectory();
    virtual ~CRetVal_XfsGetCurrentDirectory();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_XfsGetCurrentDirectory *_p);
    int Comp(CRetVal_XfsGetCurrentDirectory *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetResult();
    CMem* GetDir();
    const char* GetDirStr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetResult(int _result);
    status_t SetDir(CMem *_dir);
    status_t SetDir(const char *_dir);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
