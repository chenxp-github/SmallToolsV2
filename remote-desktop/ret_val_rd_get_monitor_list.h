#ifndef __RET_VAL_RD_GET_MONITOR_LIST_H
#define __RET_VAL_RD_GET_MONITOR_LIST_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_RdGetMonitorList
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CMem *m_monitors;
    int m_monitors_size;
/*##End Members##*/

public:
    CRetVal_RdGetMonitorList();
    virtual ~CRetVal_RdGetMonitorList();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_RdGetMonitorList *_p);
    int Comp(CRetVal_RdGetMonitorList *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetMonitors();
    int GetMonitorsSize();
    CMem* GetMonitorsElem(int _index);
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t AllocMonitors(int _len);
    status_t SetMonitors(CMem _monitors[], int _len);
    status_t SetMonitors(CMem *_monitors[], int _len);
    status_t SetMonitorsElem(int _index,CMem *_monitors);
    status_t SetMonitorsElem(int _index,const char *_monitors);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
