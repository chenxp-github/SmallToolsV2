#ifndef __PARAM_XFS_OPEN_FILE_H
#define __PARAM_XFS_OPEN_FILE_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_XfsOpenFile
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CMem m_path;
    CMem m_mode;
/*##End Members##*/

public:
    CParam_XfsOpenFile();
    virtual ~CParam_XfsOpenFile();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_XfsOpenFile *_p);
    int Comp(CParam_XfsOpenFile *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetPath();
    const char* GetPathStr();
    CMem* GetMode();
    const char* GetModeStr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetPath(CMem *_path);
    status_t SetPath(const char *_path);
    status_t SetMode(CMem *_mode);
    status_t SetMode(const char *_mode);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
