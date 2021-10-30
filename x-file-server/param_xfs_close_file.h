#ifndef __PARAM_XFS_CLOSE_FILE_H
#define __PARAM_XFS_CLOSE_FILE_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_XfsCloseFile
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_handle;
/*##End Members##*/

public:
    CParam_XfsCloseFile();
    virtual ~CParam_XfsCloseFile();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_XfsCloseFile *_p);
    int Comp(CParam_XfsCloseFile *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetHandle();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetHandle(int _handle);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
