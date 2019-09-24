#ifndef __LINKRPCCOMMON_H
#define __LINKRPCCOMMON_H

#include "cruntime.h"

#define LINKRPC_TAG 0x6b6e694c

typedef struct {
    int32_t tag;
    int32_t cmd;
    int32_t header_size;
    int32_t data_size;
}LINKRPC_HEADER;

enum{
    LINKRPC_CMD_REQUEST = 1000,
    LINKRPC_CMD_RESPONSE,
    LINKRPC_CMD_HEARTBEAT,
};

#define LINKRPC_HEADER_DEFINE(header)\
    LINKRPC_HEADER header; \
    memset(&header,0,sizeof(header));\
    header.tag = LINKRPC_TAG


#endif
