#ifndef __S2_STREAM_H
#define __S2_STREAM_H

#include "cruntime.h"
#include "filebase.h"
#include "mem.h"

#define LOCAL_STREAM_WITH_SIZE(name,size) char __##name[size];\
CStream name;\
name.Init();\
name.SetRawBuf(__##name,size,false);\
name.SetSize(0)\

#define LOCAL_STREAM(name) LOCAL_STREAM_WITH_SIZE(name,LBUF_SIZE)

class CStream:public CMem{
public:
	status_t GetAsString(CMem *str, int len);
	status_t PutString(const char *str);
	status_t GetString(CMem *str);	
	status_t PutString(CMem *str);
	status_t GetBinary(CMem *out);
	status_t PutBinary(CMem *bin);
	status_t GetZeroEndString(CMem *str);
	status_t PutZeroEndString(const char *str);
	status_t PutDouble(double d);
	double GetDouble();
	status_t PutBool(bool b);
	bool GetBool();
	
	status_t PutInt8(int8_t i);
	int8_t GetInt8();
	status_t PutInt16(int16_t i);
	int16_t GetInt16();
	status_t PutInt32(int32_t i);
	int32_t GetInt32();
	status_t PutInt64(int64_t i);
	int64_t GetInt64();

	status_t PutUInt8(uint8_t i);
	uint8_t GetUInt8();
	status_t PutUInt16(uint16_t i);
	uint16_t GetUInt16();
	status_t PutUInt32(uint32_t i);
	uint32_t GetUInt32();
	status_t PutUInt64(uint64_t i);
	uint64_t GetUInt64();

    CStream();
    virtual ~CStream();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CStream *_p);
    int Comp(CStream *_p);
    status_t Print(CFileBase *_buf);
};

#endif
