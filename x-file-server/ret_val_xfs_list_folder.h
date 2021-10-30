#ifndef __RET_VAL_XFS_LIST_FOLDER_H
#define __RET_VAL_XFS_LIST_FOLDER_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "rpc_param_base.h"
#include "x_file_info_list.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRetVal_XfsListFolder
/*##Begin Bases##*/
:public CRpcParamBase
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    int m_result;
    CxFileInfoList m_list;
/*##End Members##*/

public:
    CRetVal_XfsListFolder();
    virtual ~CRetVal_XfsListFolder();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRetVal_XfsListFolder *_p);
    int Comp(CRetVal_XfsListFolder *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetResult();
    CxFileInfoList* GetList();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetResult(int _result);
    status_t SetList(CxFileInfoList *_list);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
