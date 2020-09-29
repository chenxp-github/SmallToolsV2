#ifndef __MINIBSON_H
#define __MINIBSON_H

#include "mem.h"
#include "closure.h"

CLOSURE_COMMON_OBJECT_OPS_DEFINE_H(CMiniBson,bson)

#define LOCAL_BSON(bson,size) char __##bson[size];\
CMiniBson bson;\
bson.Init();\
bson.SetRawBuf(__##bson,size);\

#define BSON_CHECK(f) do{if(!(f)){\
	__LOG("check fail \"%s\" at file=%s line=%d"),#f,__FILE__,__LINE__);\
    return 0;\
}}while(0)\

class CMiniBson{
public:
    enum{
        BSON_TYPE_DOUBLE=0x01,
        BSON_TYPE_STRING=0x02,
        BSON_TYPE_DOCUMENT=0x03,
        BSON_TYPE_ARRAY=0x04,
        BSON_TYPE_BINARY=0x05,
        BSON_TYPE_BOOLEAN=0x08,
        BSON_TYPE_INT32=0x10,
        BSON_TYPE_INT64=0x12,
    };

    enum{
        EVENT_BEGIN_DOCUMENT = 1,
        EVENT_END_DOCUMENT,
        EVENT_SINGLE_VALUE,
    };

    enum{
        INDEX_EVENT =   0,
        INDEX_TYPE  =   1,
        INDEX_NAME  =   2,
        INDEX_VALUE  =  3,
        INDEX_IS_ARRAY  = 4,
        INDEX_ARRAY_LEN = 5,
        INDEX_IS_IN_ARRAY = 6,
        INDEX_LEVEL = 7,
        INDEX_PATH = 8,
		//keep last
		INDEX_MAX,
    };
    WEAK_REF_DEFINE();
public:
    CMem *mData;
public:
    status_t LoadRawBuf(const void *buf, int_ptr_t size);
    status_t LoadRawBuf(CMem *buf);
    status_t PeekNext(int *type, CMem *name);
    CMiniBson();
    virtual ~CMiniBson();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CMiniBson *p);
    int Comp(CMiniBson *p);
    status_t Print(CFileBase *_buf);
    status_t EndDocument();
    status_t StartDocument();
    status_t WriteFile(CFileBase *file);
    status_t Write(const void *buf, int_ptr_t size);
    status_t LoadBson(CFileBase *data);
    status_t LoadBson(const char *fn);
    status_t LoadBson();
    int32_t ReadInt32();
    bool IsEnd();
    status_t GetDocument(const char *name, CMiniBson *doc);
    status_t StartDocument(CMem *name,fsize_t *offset);
    status_t StartDocument(const char *name,fsize_t *offset);
    status_t EndDocument(fsize_t offset);
    status_t ResetPointer();
    status_t GetBinary(const char *name, CMem *bin);
    status_t GetString(const char *name, CMem *str);
    status_t GetBoolean(const char *name, bool *b);
    bool CheckName(const char *name);
    status_t SkipString();
    status_t GetDouble(const char *name, double *d);
    status_t GetInt64(const char *name, int64_t *pInt);
    status_t ToJson(CFileBase *json,bool bracket);
    status_t GetInt32(const char *name, int32_t *pInt);
    status_t GetUInt32(const char *name, uint32_t *pInt);
    status_t GetInt16(const char *name, int16_t *pInt);
    status_t GetUInt16(const char *name, uint16_t *pInt);
    status_t GetInt8(const char *name, int8_t *pInt);
    status_t GetUInt8(const char *name, uint8_t *pInt);
    status_t PutDouble(const char *name, double d);
    status_t PutDouble(CMem *name, double d);
    status_t ReadString(CMem *str);
    int8_t ReadByte();
    status_t PutBoolean(CMem *name,bool b);
    status_t PutBoolean(const char *name, bool b);
    status_t PutDocument(const char *name, CMiniBson *bson);
    int32_t GetDocumentSize();
    status_t PutDocument(CMem *name, CMiniBson *bson);
    status_t WriteInt64(int64_t i);
    status_t PutInt64(CMem *name,int64_t i);
    status_t PutInt64(const char *name, int64_t i);
    status_t PutBinary(const char *name, CFileBase *bin);
    status_t PutBinary(CMem *name, CFileBase *bin);
    status_t PutBinary(const char *name, const void *bin, int_ptr_t size);
    status_t PutBinary(CMem *name, const void *bin, int_ptr_t size);
    status_t PutString(const char *name, const char *str);
    status_t PutString(const char *name, CMem *str);
    status_t PutString(CMem *name, CMem *str);
    status_t PutInt32(const char *name, int32_t i);
    status_t PutUInt32(const char *name, uint32_t i);
    status_t PutInt16(const char *name,int16_t i);
    status_t PutUInt16(const char *name,uint16_t i);
    status_t PutInt8(const char *name,int8_t i);
    status_t PutUInt8(const char *name,uint8_t i);
    status_t WriteByte(int8_t b);
    status_t PutInt32(CMem *name,int32_t i);
    status_t PutUInt32(CMem *name,uint32_t i);
    status_t UpdateDocumentSize();
    status_t UpdateDocumentSize(fsize_t offset,fsize_t size);
    status_t WriteInt32(int32_t i);
    status_t WriteCString(const char *str);
    status_t WriteCString(CMem *str);
    status_t SetRawBuf(void *buf, int_ptr_t size);
    status_t SetRawBuf(CMem *buf);
    status_t AllocBuf(int_ptr_t size);
    CMem * GetRawData();
    status_t Traverse(bool recursive,int level,CClosure *closure);
    fsize_t GetPointerPos(void);
    status_t SetPointerPos(fsize_t pos);
    status_t UpdateArraySize(fsize_t offset,fsize_t size,int32_t array_length);
    status_t StartArray(CMem *name,fsize_t *offset);
    status_t StartArray(const char *name,fsize_t *offset);
    status_t EndArray(fsize_t offset,int32_t array_len);
    status_t GetArray(const char *name, CMiniBson *doc,int32_t *array_length);
};

#endif
