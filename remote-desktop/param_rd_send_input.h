#ifndef __PARAM_RD_SEND_INPUT_H
#define __PARAM_RD_SEND_INPUT_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
#include "remote_desktop_input.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_RdSendInput
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CRemoteDesktopInput m_input;
/*##End Members##*/

public:
    CParam_RdSendInput();
    virtual ~CParam_RdSendInput();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_RdSendInput *_p);
    int Comp(CParam_RdSendInput *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CRemoteDesktopInput* GetInput();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetInput(CRemoteDesktopInput *_input);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
