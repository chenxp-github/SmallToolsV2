#ifndef __PARAM_XFS_CHANGE_CURRENT_DIRECTORY_H
#define __PARAM_XFS_CHANGE_CURRENT_DIRECTORY_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_XfsChangeCurrentDirectory
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CMem m_dir;
/*##End Members##*/

public:
    CParam_XfsChangeCurrentDirectory();
    virtual ~CParam_XfsChangeCurrentDirectory();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_XfsChangeCurrentDirectory *_p);
    int Comp(CParam_XfsChangeCurrentDirectory *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetDir();
    const char* GetDirStr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
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
