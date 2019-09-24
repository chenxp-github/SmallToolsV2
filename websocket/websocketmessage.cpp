#include "websocketmessage.h"
#include "syslog.h"
#include "mem_tool.h"

CWebSocketMessage::CWebSocketMessage()
{
    this->InitBasic();
}

CWebSocketMessage::~CWebSocketMessage()
{
    this->Destroy();
}

status_t CWebSocketMessage::InitBasic()
{
	WEAK_REF_ID_CLEAR();
    this->m_Method = 0;
    this->m_CallbackId = 0;
    this->m_DataType = 0;
    this->m_Data.InitBasic();
	m_Flags = 0;
	m_MessageType = 0;
    return OK;
}

status_t CWebSocketMessage::Init()
{
    this->InitBasic();
	WEAK_REF_ID_INIT();
    this->m_Data.Init();
    return OK;
}

status_t CWebSocketMessage::Destroy()
{
    this->m_Data.Destroy();
    this->InitBasic();
    return OK;
}

status_t CWebSocketMessage::Copy(CWebSocketMessage *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

    this->m_Method = _p->m_Method;
    this->m_CallbackId = _p->m_CallbackId;
    this->m_DataType = _p->m_DataType;
    this->m_Data.Copy(&_p->m_Data);
	m_Flags = _p->m_Flags;
	m_MessageType = _p->m_MessageType;
    return OK;
}

status_t CWebSocketMessage::Comp(CWebSocketMessage *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CWebSocketMessage::Print(CFileBase *_buf)
{
    _buf->Log("Method = %d",m_Method);
    _buf->Log("CallbackId = %d",m_CallbackId);
    _buf->Log("DataType = %d",m_DataType);
	_buf->Log("MessageType = %d",m_MessageType);
    _buf->Log("Data = <%d bytes>",m_Data.GetSize());
    return OK;
}

int CWebSocketMessage::GetMethod()
{
    return m_Method;
}

int CWebSocketMessage::GetCallbackId()
{
    return m_CallbackId;
}

int CWebSocketMessage::GetDataType()
{
    return m_DataType;
}

CMem* CWebSocketMessage::GetData()
{
    return &m_Data;
}

status_t CWebSocketMessage::SetMethod(int _method)
{
    this->m_Method = _method;
    return OK;
}

status_t CWebSocketMessage::SetCallbackId(int _callbackid)
{
    this->m_CallbackId = _callbackid;
    return OK;
}

status_t CWebSocketMessage::SetDataType(int _type)
{
    this->m_DataType = _type;
    return OK;
}

status_t CWebSocketMessage::SetData(CMem *_data)
{
    ASSERT(_data);
    return this->m_Data.Copy(_data);
}

status_t CWebSocketMessage::SerializeHeader(CFileBase *file)
{
	ASSERT(file);
	file->Write(&m_Method,sizeof(m_Method));
	file->Write(&m_CallbackId,sizeof(m_CallbackId));
	file->Write(&m_DataType,sizeof(m_DataType));
	file->Write(&m_MessageType,sizeof(m_MessageType));
	return OK;
}

status_t CWebSocketMessage::DeserializeHeader(CFileBase *file)
{
	file->Read(&m_Method,sizeof(m_Method));
	file->Read(&m_CallbackId,sizeof(m_CallbackId));
	file->Read(&m_DataType,sizeof(m_DataType));
	file->Read(&m_MessageType,sizeof(m_MessageType));
	return OK;
}

int CWebSocketMessage::HeaderSize()
{
    return 4*sizeof(int32_t);
}

int CWebSocketMessage::Size()
{
	return HeaderSize() +(int)m_Data.GetSize();
}

status_t CWebSocketMessage::AttachPayload(CMem *payload)
{
    ASSERT(payload);

    payload->Seek(0);
    this->DeserializeHeader(payload);
    m_Data.Destroy();
    m_Data.Init();
    m_Data.SetRawBuf(
        payload->GetRawBuf()+HeaderSize(),
        (int_ptr_t)(payload->GetSize()-HeaderSize()),
        true
    );
    return OK;
}

int CWebSocketMessage::GetMessageType()
{
    return m_MessageType;
}

status_t CWebSocketMessage::SetMessageType(int _type)
{
    this->m_MessageType = _type;
    return OK;
}

status_t CWebSocketMessage::TransferData(CMem *from)
{
	ASSERT(from);
	return m_Data.Transfer(from);
}	
