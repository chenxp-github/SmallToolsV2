#ifndef __S2_WEBSOCKETFRAMEHEADER_H
#define __S2_WEBSOCKETFRAMEHEADER_H

#include "cruntime.h"
#include "mem.h"

class CWebSocketFrameHeader{
public:
	enum {
		kOpCodeContinuation = 0x0,
		kOpCodeText = 0x1,
		kOpCodeBinary = 0x2,
		kOpCodeDataUnused = 0x3,
		kOpCodeClose = 0x8,
		kOpCodePing = 0x9,
		kOpCodePong = 0xA,
		kOpCodeControlUnused = 0xB,
	};
public:
	uint8_t m_Fin;
	uint8_t m_Reserved1;
	uint8_t m_Reserved2;
	uint8_t m_Reserved3;
	uint8_t m_OpCode;
	uint8_t m_Mask;
	int64_t m_PayloadLen;
	uint32_t m_MaskingKey;
	uint64_t m_FrameOffset;
public:
	int32_t PredictHeaderSize(CFileBase *file);
	status_t DecodeFramePayload(const void *indata,size_t size, CMem *out);
	status_t Reset();
	status_t GenerateWebSocketMaskingKey();
	CWebSocketFrameHeader();
	virtual ~CWebSocketFrameHeader();
	status_t InitBasic();
	status_t Init();
	status_t Destroy();
	status_t Copy(CWebSocketFrameHeader *_p);
	int Comp(CWebSocketFrameHeader *_p);
	status_t Print(CFileBase *_buf);
	status_t Serialize(CFileBase *file);
	status_t Deserialize(CFileBase *file);
	uint8_t GetFin();
	uint8_t GetOpCode();
	uint8_t GetMask();
	int64_t GetPayloadLen();
	uint32_t GetMaskingKey();
	status_t SetFin(uint8_t _fin);
	status_t SetOpCode(uint8_t _opcode);
	status_t SetMask(uint8_t _mask);
	status_t SetPayloadLen(int64_t _payloadlen);
	status_t SetMaskingKey(uint32_t _maskingkey);
	status_t SetFrameOffset(int64_t offset);
	int64_t GetFrameOffset();

};

#endif
