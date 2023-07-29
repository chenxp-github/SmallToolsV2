#ifndef __PARAM_RD_LOGIN_H
#define __PARAM_RD_LOGIN_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_RdLogin
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_version;
/*##End Members##*/

public:
    CParam_RdLogin();
    virtual ~CParam_RdLogin();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_RdLogin *_p);
    int Comp(CParam_RdLogin *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetVersion();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetVersion(int _version);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
