#ifndef __WEB_SOCKET_PARAM_BASE_H
#define __WEB_SOCKET_PARAM_BASE_H

#include "mem.h"
#include "memfile.h"
#include "minibson.h"

class CWebSocketParamBase{

public:
    CWebSocketParamBase();
    ~CWebSocketParamBase();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CWebSocketParamBase *_p);
    int Comp(CWebSocketParamBase *_p);
    status_t Print(CFileBase *_buf);

    virtual status_t SaveBson(CMiniBson *_bson);
    virtual status_t SaveBson(CMem *_mem);
    virtual status_t LoadBson(CMiniBson *_bson);
    virtual status_t LoadBson(CFileBase *_file);
};

#endif
