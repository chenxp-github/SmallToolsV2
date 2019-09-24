#ifndef __PEERMESSAGE_H
#define __PEERMESSAGE_H

#include "minibson.h"
#include "refcnt.h"
#include "mem.h"

class CPeerMessage{
public:
    enum{
        REQUEST =  0x01,
        RESPONSE  = 0x02,
        SINGAL = 0x03,
		PART_RESPONSE = 0x04,
    };

    enum{
        STRING = 0x01<<4,
        BSON = 0x02<<4,
        RAW = 0x03<<4,
        JSON = 0x04<<4,
    };

public:
    REF_COUNT_DEFINE();
    CMem *mFrom;
    CMem *mTo;
    int32_t mFlags;
    int32_t mFunc;
    int32_t mCallbackId;
    CMem *mBody;
public:
    static CPeerMessage* NewJsonMessage(const char *to, int func,int cbid, CMem *str);
    static CPeerMessage* NewJsonMessage(const char *to, int func, int cbid, const char *str);
    static CPeerMessage* NewStringMessage(const char *to, int func,int cbid, CMem *str);
    static CPeerMessage* NewStringMessage(const char *to, int func, int cbid, const char *str);
    static CPeerMessage* NewBsonMessage(const char *to, int func, int cbid, CMem *bson);
    static CPeerMessage* NewBsonMessage(const char *to,int func, int cbid, CMiniBson *bson);
    static CPeerMessage* NewEmptyMessage(const char *to,int func, int cbid);
    uint32_t GetFlags();
    status_t SetFlags(uint32_t flags);
    status_t CheckHeader();
    status_t SerializeHeader(CMem *buf);
    status_t UnSerializeHeader(CMem *buf);
    int GetBodyType();
    int GetMessageType();
    status_t SetBodyType(int type);
    status_t SetMessageType(int type);
    int GetCallbackId();
    status_t SetCallbackId(int cbid);
    status_t SetBodyString(const char *str);
    status_t SetBodyString(CMem *str);
    status_t TransferBody(CMem *body);
    status_t SetFrom(CMem *from);
    status_t SetFrom(const char *from);
    status_t SetTo(CMem *to);
    status_t SetTo(const char *to);
    status_t SetFunc(int func);
    status_t SetBody(CMem *body);
    status_t SetBody(const void *body, int_ptr_t size);
    CMem* GetFrom();
    CMem* GetTo();
    int GetFunc();
    CMem* GetBody();
    CPeerMessage();
    virtual ~CPeerMessage();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CPeerMessage *p);
    int Comp(CPeerMessage *p);
    status_t Print();
    status_t Clear();
};

#endif
