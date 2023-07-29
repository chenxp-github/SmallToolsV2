#ifndef __RET_VAL_RD_LOGIN_H
#define __RET_VAL_RD_LOGIN_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_RdLogin
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    status_t m_RetVal0;
/*##End Members##*/

public:
    CRetVal_RdLogin();
    virtual ~CRetVal_RdLogin();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_RdLogin *_p);
    int Comp(CRetVal_RdLogin *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    status_t GetRetVal0();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetRetVal0(status_t _retval0);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
