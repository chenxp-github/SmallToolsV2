#include "taskwebsocketserver.h"
#include "mem_tool.h"
#include "syslog.h"
#include "encoder.h"
#include "sha1calculator.h"
#include "websocketframeheader.h"
#include "minitask.h"

#define MAX_WS_HEADER_SIZE 1024

#define EXTRA_MEM_SIZE sizeof(int)

#define ASSURE_MEM_SIZE(mem,size)do{\
	if((uint32_t)(mem).GetMaxSize()<((size)+EXTRA_MEM_SIZE))\
	{\
		(mem).Realloc((size));\
	}\
}while(0)\

#define FREE_MEM_IF_BIGGER_THAN(mem,size) do{\
if((uint32_t)(mem).GetMaxSize() > ((size)+EXTRA_MEM_SIZE))\
{\
	(mem).Destroy();\
	(mem).Init();\
}}while(0)\

#define DEFAULT_READER_BUFFER_SIZE (64*1024)

enum{
	STEP_PREPARE = 1,
	STEP_READ_CLIENT_HAND_SHAKE,
	STEP_SEND_SERVER_HAND_SHAKE,
	
	STEP_PREDICT_PAYLOAD_HEADER_SIZE_1,
	STEP_PREDICT_PAYLOAD_HEADER_SIZE_2,

	STEP_PREPARE_READ_PAYLOAD_HEADER,
	STEP_READ_PAYLOAD_HEADER,
	STEP_READ_PAYLOAD,

    STEP_PREPARE_PAYLOAD_TO_SEND,
	STEP_SEND_PAYLOAD,

	STEP_STOP_FIRST_STAGE,
};

CTaskWebSocketServer::CTaskWebSocketServer()
{
	this->InitBasic();
}
CTaskWebSocketServer::~CTaskWebSocketServer()
{
	this->Destroy();
}
status_t CTaskWebSocketServer::InitBasic()
{
	CTask::InitBasic();
	this->m_Step = 0;
	m_Socket.InitBasic();
	m_SocketRw.InitBasic();
	m_TmpBufForRead.InitBasic();
	m_HttpHeader.InitBasic();
	m_TmpHeader.InitBasic();
    m_HostPtr.InitBasic();
	m_DecodedBuffer.InitBasic();
    m_SubTaskWriter = 0;
    m_SubTaskReader = 0;
    m_ReaderStep = 0;
    m_WriterStep = 0;
    m_CombDataFile.InitBasic();
	return OK;
}

static status_t on_socket_rw_event(CClosure *closure)
{
	CLOSURE_PARAM_INT(event,0);
	CLOSURE_PARAM_PTR(CTaskWebSocketServer*,self,10);

	if(event == CSocketReaderWriter::EVENT_ERROR)
	{
		CLOSURE_PARAM_INT(error,1);
		CLOSURE_PARAM_PTR(char*,err_str,2);
		XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
			"socket error: %d:%s",error,err_str
		);
		self->Stop(CTaskWebSocketServer::ERROR_SOCKET_ERROR);
	}
	return OK;
}

status_t CTaskWebSocketServer::Init(CTaskMgr *mgr)
{
	this->Destroy();
	CTask::Init(mgr);
    m_CombDataFile.Init(4);
	m_HostPtr.Init();
	m_TmpHeader.Init();
	m_HttpHeader.Init();
	m_TmpBufForRead.Init();
    m_TmpBufForWrite.Init();
	m_DecodedBuffer.Init();
	m_Socket.Init();
	m_SocketRw.Init(mgr);
	m_SocketRw.SetSocket(&m_Socket);
	m_SocketRw.Callback()->SetParamPointer(10,this);
	m_SocketRw.Callback()->SetFunc(on_socket_rw_event);
    m_SocketRw.NeverTimeout();
	return OK;
}
status_t CTaskWebSocketServer::Destroy()
{
    QuitTask(&m_SubTaskReader);
    QuitTask(&m_SubTaskWriter);
    m_CombDataFile.Destroy();
    m_HostPtr.Destroy();
	m_TmpHeader.Destroy();
	m_HttpHeader.Destroy();
	m_TmpBufForRead.Destroy();
    m_TmpBufForWrite.Destroy();
	m_DecodedBuffer.Destroy();
	m_SocketRw.Destroy();
	m_Socket.Destroy();	
	CTask::Destroy();
	this->InitBasic();
	return OK;
}
const char * CTaskWebSocketServer::ErrStr(int err)
{
	ERR_STR(ERROR_NONE,"no error");
	ERR_STR(ERROR_SOCKET_ERROR,"socket error");
	ERR_STR(ERROR_ERROR_HEADER_SIZE,"error header size");
	ERR_STR(ERROR_ERROR_HEADER_FORMAT,"error header format");
	ERR_STR(ERROR_WEB_SOCKET_CLOSED,"web socket session closed");
	return "unknown error";
}
status_t CTaskWebSocketServer::ReportError(int err)
{
	XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
		"CTaskWebSocketServer(%d): exit with error:%s",
		this->GetId(),this->ErrStr(err)
	);
	return OK;
}
status_t CTaskWebSocketServer::Start()
{
	this->Resume();
	GOTO_STATE(0,STEP_PREPARE);
	return OK;
}
status_t CTaskWebSocketServer::Stop(int err)
{
	if(this->IsDead())return OK;
	this->Quit();
	this->ReportError(err);

	CClosure *callback = m_HostPtr->Callback();
	callback->SetParamInt(1,err);
	callback->Run(CWebSocketServer::EVENT_TASK_STOPPED);

	return OK;
}

status_t CTaskWebSocketServer::Run(uint32_t interval)
{
    ASSERT(m_HostPtr.get());

	BEGIN_FSM()
	/*****************/
	BEGIN_STATE(STEP_PREPARE)
	{
		ASSURE_MEM_SIZE(m_TmpBufForRead,DEFAULT_READER_BUFFER_SIZE);
		m_SocketRw.PrepareFileForRead(&m_TmpBufForRead,0,true);
		GOTO_STATE(0,STEP_READ_CLIENT_HAND_SHAKE);
	}
	END_STATE(STEP_PREPARE);
	/*****************/
	BEGIN_STATE(STEP_READ_CLIENT_HAND_SHAKE)
	{
		if(m_SocketRw.DoReadUntilEmptyLine(interval))
		{
			m_HttpHeader.Load(&m_TmpBufForRead);
			m_TmpBufForRead.SetSize(0);
			this->MakeHandShakeResponse(&m_TmpBufForRead);
			m_SocketRw.PrepareFileForWrite(&m_TmpBufForRead,0,m_TmpBufForRead.GetSize());
			GOTO_STATE(0,STEP_SEND_SERVER_HAND_SHAKE);
		}
	}
	END_STATE(STEP_READ_CLIENT_HAND_SHAKE);
	/*****************/
	BEGIN_STATE(STEP_SEND_SERVER_HAND_SHAKE)
	{
		if(m_SocketRw.DoWrite(interval))
		{
			GOTO_STATE(0,STEP_STOP_FIRST_STAGE);
		}		
	}
	END_STATE(STEP_SEND_SERVER_HAND_SHAKE);
	/*****************/
	BEGIN_STATE(STEP_STOP_FIRST_STAGE)
	{
		this->StartSubTaskReader();
		this->StartSubTaskWriter();
		this->Suspend(); //stop main task running
		return OK;
	}
	END_STATE(STEP_STOP_FIRST_STAGE);
	/*****************/
	END_FSM()
	return OK;
}

status_t CTaskWebSocketServer::SetSocketFd(int socket_fd)
{
	m_Socket.AttachSocketFd(socket_fd);
	m_Socket.SetBlocking(false);
	return OK;
}

status_t CTaskWebSocketServer::MakeHandShakeResponse(CFileBase *out)
{
	ASSERT(out);

	out->Puts("HTTP/1.1 101 Switching Protocols\r\n");
	out->Puts("Upgrade: websocket\r\n");
	out->Puts("Connection: Upgrade\r\n");
	out->Puts("Sec-WebSocket-Accept: ");
	this->CalcuSecWebSocketAccept(out);
	out->Puts("\r\n");
	out->Puts("\r\n");

	return OK;
}

status_t CTaskWebSocketServer::CalcuSecWebSocketAccept(CFileBase *out)
{
	ASSERT(out);

	CHttpPair *pair = m_HttpHeader.GetPair("Sec-WebSocket-Key");
	ASSERT(pair);

	LOCAL_MEM(mem);
	
	mem.Puts(pair->GetValue());
	mem.Puts("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
	
	CSha1Calculator sha1;
	sha1.Init();
	sha1.CalcuHash(&mem);
	
	uint8_t *r = sha1.GetResult();

	CMem tmp;
	tmp.Init();
	tmp.SetRawBuf(r,SHA1HashSize,true);
		
	mem.SetSize(0);
	CEncoder::Base64_Encode(&tmp,&mem);
	
	out->Puts(&mem);
	return OK;
}

status_t CTaskWebSocketServer::WebSocketFrameHeaderReadComplete()
{
	m_TmpHeader.Reset();
	m_TmpBufForRead.Seek(0);

	if(m_TmpHeader.Deserialize(&m_TmpBufForRead))
	{
		return OK;
	}
	return ERROR;
}

status_t CTaskWebSocketServer::StartSubTaskReader()
{
    ASSERT(!IsTask(m_SubTaskReader));

    BEGIN_MINI_TASK(reader)
    {
        CTaskWebSocketServer *self;
        status_t Run(uint32_t interval)
        {
            self->RunReader(interval);
            return OK;
        }
    }
    END_MINI_TASK(reader);
    
    reader->Init(GetTaskMgr());
    reader->self = this;
    reader->SetParentTask(this);
    reader->Start(1);

    m_ReaderStep = STEP_PREDICT_PAYLOAD_HEADER_SIZE_1;
    m_SubTaskReader = reader->GetId();
    return OK;
}

status_t CTaskWebSocketServer::StartSubTaskWriter()
{
    ASSERT(!IsTask(m_SubTaskWriter));
    
    BEGIN_MINI_TASK(writer)
    {
        CTaskWebSocketServer *self;
        status_t Run(uint32_t interval)
        {
            self->RunWriter(interval);
            return OK;
        }
    }
    END_MINI_TASK(writer);
    
    writer->Init(GetTaskMgr());
    writer->self = this;
    writer->SetParentTask(this);
    writer->Start(1);
    m_WriterStep = STEP_PREPARE_PAYLOAD_TO_SEND;

    m_SubTaskWriter = writer->GetId();
    return OK;
}

status_t CTaskWebSocketServer::RunReader(uint32_t interval)
{
    ASSERT(m_HostPtr.get());

    if(m_ReaderStep == STEP_PREDICT_PAYLOAD_HEADER_SIZE_1)
    {
		ASSURE_MEM_SIZE(m_TmpBufForRead,DEFAULT_READER_BUFFER_SIZE);
        m_SocketRw.PrepareFileForRead(&m_TmpBufForRead,2,true);
        m_ReaderStep = STEP_PREDICT_PAYLOAD_HEADER_SIZE_2;
	}

    if(m_ReaderStep == STEP_PREDICT_PAYLOAD_HEADER_SIZE_2)
    {		
        if(m_SocketRw.DoRead(interval))
        {
            m_TmpBufForRead.Seek(0);
            int32_t next_header_size = m_TmpHeader.PredictHeaderSize(&m_TmpBufForRead);
            if(next_header_size <= 0)
            {
                this->Stop(ERROR_ERROR_HEADER_SIZE);
                return ERROR;
            }

            ASSURE_MEM_SIZE(m_TmpBufForRead,next_header_size+2);
            m_SocketRw.PrepareFileForRead(&m_TmpBufForRead,next_header_size,false);
            m_ReaderStep = STEP_READ_PAYLOAD_HEADER;
        }
    }

    if(m_ReaderStep == STEP_READ_PAYLOAD_HEADER)
    {
        if(m_SocketRw.DoRead(interval))
        {
            if(!WebSocketFrameHeaderReadComplete())
            {
                this->Stop(ERROR_ERROR_HEADER_FORMAT);
                return ERROR;
            }
            
            if(m_TmpHeader.GetOpCode()==CWebSocketFrameHeader::kOpCodeClose)
            {
                this->Stop(ERROR_WEB_SOCKET_CLOSED);
                return ERROR;
            }

			ASSURE_MEM_SIZE(m_TmpBufForRead,(int)m_TmpHeader.GetPayloadLen());
            
            m_SocketRw.PrepareFileForRead(
                &m_TmpBufForRead,
                m_TmpHeader.GetPayloadLen(),
                true
            );

            m_ReaderStep = STEP_READ_PAYLOAD;
        }
    }

    if(m_ReaderStep == STEP_READ_PAYLOAD)
    {
        if(m_SocketRw.DoRead(interval))
        {
			char *data = m_TmpBufForRead.GetRawBuf();
			int32_t len = (int32_t)m_TmpHeader.GetPayloadLen();
			
			ASSURE_MEM_SIZE(m_DecodedBuffer,len*2);

			m_DecodedBuffer.SetSize(0);
			m_TmpHeader.DecodeFramePayload(data,len,&m_DecodedBuffer);

			CClosure *callback = m_HostPtr->Callback();
			callback->SetParamPointer(1,&m_DecodedBuffer);
            callback->SetParamPointer(2,&m_TmpHeader);
			callback->Run(CWebSocketServer::EVENT_GOT_PAYLOAD);

			FREE_MEM_IF_BIGGER_THAN(m_TmpBufForRead,DEFAULT_READER_BUFFER_SIZE);
			FREE_MEM_IF_BIGGER_THAN(m_DecodedBuffer,DEFAULT_READER_BUFFER_SIZE*2);
			m_ReaderStep = STEP_PREDICT_PAYLOAD_HEADER_SIZE_1;
        }
    }

    return OK;
}

status_t CTaskWebSocketServer::RunWriter(uint32_t interval)
{
	ASSERT(m_HostPtr.get());

    if(m_WriterStep == STEP_PREPARE_PAYLOAD_TO_SEND)
    {
        CWebSocketMessageQueue *queue = m_HostPtr->GetSendingQueue();
        ASSERT(queue);
        CWebSocketMessage *msg = queue->GetElem(0);        
        if(!msg)return ERROR;

        CWebSocketFrameHeader header;
        header.Init();
        header.SetFin(1);
        header.SetMask(0);
        header.SetOpCode(CWebSocketFrameHeader::kOpCodeBinary);
    
		header.SetPayloadLen(msg->Size());
		
		ASSURE_MEM_SIZE(m_TmpBufForWrite,MAX_WS_HEADER_SIZE);
		m_TmpBufForWrite.SetSize(0);

		header.Serialize(&m_TmpBufForWrite);
		m_TmpBufForWrite.SeekEnd();
		msg->SerializeHeader(&m_TmpBufForWrite);

		m_CombDataFile.Clear();
		m_CombDataFile.AddFile(&m_TmpBufForWrite);
		m_CombDataFile.AddFile(msg->GetData());
       
        this->m_SocketRw.PrepareFileForWrite(&m_CombDataFile,0,m_CombDataFile.GetSize());
        m_WriterStep = STEP_SEND_PAYLOAD;
    }

	if(m_WriterStep == STEP_SEND_PAYLOAD)
	{
		if(m_SocketRw.DoWrite(interval))
		{
			CWebSocketMessageQueue *queue = m_HostPtr->GetSendingQueue();
			ASSERT(queue);
			queue->DelElem(0); //delete first message
			m_WriterStep = STEP_PREPARE_PAYLOAD_TO_SEND;
		}
	}

    return OK;
}
