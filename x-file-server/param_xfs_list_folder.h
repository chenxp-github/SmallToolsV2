#ifndef __PARAM_XFS_LIST_FOLDER_H
#define __PARAM_XFS_LIST_FOLDER_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_XfsListFolder
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CMem m_path;
/*##End Members##*/

public:
    CParam_XfsListFolder();
    virtual ~CParam_XfsListFolder();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_XfsListFolder *_p);
    int Comp(CParam_XfsListFolder *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetPath();
    const char* GetPathStr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetPath(CMem *_path);
    status_t SetPath(const char *_path);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif