#ifndef __PARAM_RD_GET_NEXT_SCREEN_H
#define __PARAM_RD_GET_NEXT_SCREEN_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CParam_RdGetNextScreen
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CMem m_monitorName;
    int m_curFrameNo;
    int m_bytePerPixel;
    int m_full;
/*##End Members##*/

public:
    CParam_RdGetNextScreen();
    virtual ~CParam_RdGetNextScreen();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CParam_RdGetNextScreen *_p);
    int Comp(CParam_RdGetNextScreen *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetMonitorName();
    const char* GetMonitorNameStr();
    int GetCurFrameNo();
    int GetBytePerPixel();
    int GetFull();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetMonitorName(CMem *_monitorname);
    status_t SetMonitorName(const char *_monitorname);
    status_t SetCurFrameNo(int _curframeno);
    status_t SetBytePerPixel(int _byteperpixel);
    status_t SetFull(int _full);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
