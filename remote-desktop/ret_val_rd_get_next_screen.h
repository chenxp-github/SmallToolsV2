#ifndef __RET_VAL_RD_GET_NEXT_SCREEN_H
#define __RET_VAL_RD_GET_NEXT_SCREEN_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
#include "remote_desktop_pixel_buffer_list.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_RdGetNextScreen
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_res;
    int m_full;
    int m_frameNo;
    CRemoteDesktopPixelBufferList m_diff;
/*##End Members##*/

public:
    CRetVal_RdGetNextScreen();
    virtual ~CRetVal_RdGetNextScreen();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_RdGetNextScreen *_p);
    int Comp(CRetVal_RdGetNextScreen *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetRes();
    int GetFull();
    int GetFrameNo();
    CRemoteDesktopPixelBufferList* GetDiff();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetRes(int _res);
    status_t SetFull(int _full);
    status_t SetFrameNo(int _frameno);
    status_t SetDiff(CRemoteDesktopPixelBufferList *_diff);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
