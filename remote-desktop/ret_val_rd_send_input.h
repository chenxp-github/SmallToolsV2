#ifndef __RET_VAL_RD_SEND_INPUT_H
#define __RET_VAL_RD_SEND_INPUT_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_RdSendInput
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_res;
/*##End Members##*/

public:
    CRetVal_RdSendInput();
    virtual ~CRetVal_RdSendInput();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_RdSendInput *_p);
    int Comp(CRetVal_RdSendInput *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetRes();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetRes(int _res);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
