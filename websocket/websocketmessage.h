#ifndef __WEB_SOCKET_MESSAGE_H
#define __WEB_SOCKET_MESSAGE_H

#include "mem.h"
#include "memfile.h"
#include "minibson.h"
#include "mem.h"

#define WEB_SOCKET_MESSAGE_FUNC_FLAG(func,bit) FLAG_FUNC(m_Flags,func,bit)

class CWebSocketMessage{
public:
    enum{
        TYPE_UNKNOWN = 0,
        TYPE_BSON,
        TYPE_STRING,
    };
    enum{
		REQUEST =  0x01,
		RESPONSE  = 0x02,
		SINGAL = 0x03,
		PART_RESPONSE = 0x04,
    };
	WEAK_REF_ID_DEFINE();
public:
    int32_t	m_Method;
    int32_t m_CallbackId;
    int32_t m_DataType;
	int32_t m_MessageType;
    CMem m_Data;
	uint32_t m_Flags;
public:
	status_t TransferData(CMem *from);
	int GetMessageType();
	status_t SetMessageType(int _type);
	status_t AttachPayload(CMem *payload);
	int HeaderSize();
	int Size();
	status_t DeserializeHeader(CFileBase *file);
	status_t SerializeHeader(CFileBase *file);
    CWebSocketMessage();
    ~CWebSocketMessage();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CWebSocketMessage *_p);
    int Comp(CWebSocketMessage *_p);
    status_t Print(CFileBase *_buf);

    int GetMethod();
    int GetCallbackId();
    int GetDataType();
    CMem* GetData();
    status_t SetMethod(int _method);
    status_t SetCallbackId(int _callbackid);
    status_t SetDataType(int _type);
    status_t SetData(CMem *_data);

	WEB_SOCKET_MESSAGE_FUNC_FLAG(IsOnHeap,0x00000001);

};

#endif
