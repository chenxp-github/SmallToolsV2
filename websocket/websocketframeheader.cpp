#include "websocketframeheader.h"
#include "syslog.h"
#include "mem_tool.h"
#include "bigendian.h"

const size_t kMaskingKeyLength = 4;
const uint8_t kFinalBit = 0x80;
const uint8_t kReserved1Bit = 0x40;
const uint8_t kReserved2Bit = 0x20;
const uint8_t kReserved3Bit = 0x10;
const uint8_t kOpCodeMask = 0xF;
const uint8_t kMaskBit = 0x80;
const uint8_t kPayloadLengthMask = 0x7F;
const uint64_t kMaxPayloadLengthWithoutExtendedLengthField = 125;
const uint64_t kPayloadLengthWithTwoByteExtendedLengthField = 126;
const uint64_t kPayloadLengthWithEightByteExtendedLengthField = 127;

const uint8_t  kuint8max  =  0xFF;
const uint16_t kuint16max =  0xFFFF;
const uint32_t kuint32max =  0xFFFFFFFF;
const  int8_t  kint8min   = -0x7F - 1;
const  int8_t  kint8max   =  0x7F;
const  int16_t kint16min  = -0x7FFF - 1;
const  int16_t kint16max  =  0x7FFF;
const  int32_t kint32min  = -0x7FFFFFFF - 1;
const  int32_t kint32max  =  0x7FFFFFFF;

CWebSocketFrameHeader::CWebSocketFrameHeader()
{
	this->InitBasic();
}
CWebSocketFrameHeader::~CWebSocketFrameHeader()
{
	this->Destroy();
}
status_t CWebSocketFrameHeader::InitBasic()
{
	this->m_Fin = 0;
	this->m_OpCode = 0;
	this->m_Mask = 0;
	this->m_PayloadLen = 0;
	this->m_MaskingKey = 0;
	this->m_Reserved1 = 0;
	this->m_Reserved2 = 0;
	this->m_Reserved3 = 0;
	this->m_FrameOffset = 0;
	return OK;
}
status_t CWebSocketFrameHeader::Init()
{
	this->InitBasic();
	return OK;
}
status_t CWebSocketFrameHeader::Destroy()
{
	this->InitBasic();
	return OK;
}
int CWebSocketFrameHeader::Comp(CWebSocketFrameHeader *_p)
{
	ASSERT(_p);
	if(this==_p)return 0;
	return 0;
}
status_t CWebSocketFrameHeader::Copy(CWebSocketFrameHeader *_p)
{
	ASSERT(_p);
	if(this == _p)return OK;
	this->m_Fin = _p->m_Fin;
	this->m_OpCode = _p->m_OpCode;
	this->m_Mask = _p->m_Mask;
	this->m_PayloadLen = _p->m_PayloadLen;
	this->m_MaskingKey = _p->m_MaskingKey;
	this->m_Reserved1 = _p->m_Reserved1;
	this->m_Reserved2 = _p->m_Reserved2;
	this->m_Reserved3 = _p->m_Reserved3;
	this->m_FrameOffset = _p->m_FrameOffset;
	return OK;
}
status_t CWebSocketFrameHeader::Print(CFileBase *_buf)
{
	_buf->Log("Fin = %d",this->m_Fin);
	_buf->Log("OpCode = %d",this->m_OpCode);
	_buf->Log("Mask = %d",this->m_Mask);
	_buf->Log("PayloadLen = %lld",this->m_PayloadLen);
	_buf->Log("MaskingKey = 0x%x",this->m_MaskingKey);
	_buf->Log("FrameOffset = %lld",this->m_FrameOffset);
	return OK;
}
status_t CWebSocketFrameHeader::Reset()
{
	return this->InitBasic();
}

uint8_t CWebSocketFrameHeader::GetFin()
{
	return this->m_Fin;
}
uint8_t CWebSocketFrameHeader::GetOpCode()
{
	return this->m_OpCode;
}
uint8_t CWebSocketFrameHeader::GetMask()
{
	return this->m_Mask;
}
int64_t CWebSocketFrameHeader::GetPayloadLen()
{
	return this->m_PayloadLen;
}
uint32_t CWebSocketFrameHeader::GetMaskingKey()
{
	return this->m_MaskingKey;
}
status_t CWebSocketFrameHeader::SetFin(uint8_t _fin)
{
	this->m_Fin = _fin;
	return OK;
}
status_t CWebSocketFrameHeader::SetOpCode(uint8_t _opcode)
{
	this->m_OpCode = _opcode;
	return OK;
}
status_t CWebSocketFrameHeader::SetMask(uint8_t _mask)
{
	this->m_Mask = _mask;
	return OK;
}
status_t CWebSocketFrameHeader::SetPayloadLen(int64_t _payloadlen)
{
	this->m_PayloadLen = _payloadlen;
	return OK;
}
status_t CWebSocketFrameHeader::SetMaskingKey(uint32_t _maskingkey)
{
	this->m_MaskingKey = _maskingkey;
	return OK;
}

status_t CWebSocketFrameHeader::SetFrameOffset(int64_t offset)
{
	this->m_FrameOffset = offset;
	return OK;
}

int64_t CWebSocketFrameHeader::GetFrameOffset()
{
	return this->m_FrameOffset;
}

status_t CWebSocketFrameHeader::Serialize(CFileBase *file)
{
	ASSERT(file);
	
	CBigEndian stream;
	stream.Init(file);
	
	//first byte
	uint8_t b = 0;
	b |= (m_Fin?kFinalBit:0);
	b |= (m_OpCode&kOpCodeMask);
	stream.WriteU8(b);
	
	b=0;
	b |= (m_Mask?kMaskBit:0);
	if((uint64_t)m_PayloadLen <= kMaxPayloadLengthWithoutExtendedLengthField)
	{
		b |= m_PayloadLen;
		stream.WriteU8(b);
	}
	else if(m_PayloadLen <= kuint16max)
	{
		b |= kPayloadLengthWithTwoByteExtendedLengthField;
		stream.WriteU8(b);
		stream.WriteU16((uint16_t)m_PayloadLen);
	}
	else
	{
		b |= kPayloadLengthWithEightByteExtendedLengthField;
		stream.WriteU8(b);
		stream.WriteU64((uint64_t)m_PayloadLen);
	}

	if(m_Mask)
	{
		stream.WriteBytes(&m_MaskingKey,4);
	}

	return OK;
}

status_t CWebSocketFrameHeader::Deserialize(CFileBase *file)
{
	ASSERT(file);

	CBigEndian stream;
	stream.Init(file);
	
	uint8_t first_byte = 0;
	if(stream.ReadU8(&first_byte) < (int_ptr_t)sizeof(first_byte))
		return ERROR;

	uint8_t second_byte = 0;
	if(stream.ReadU8(&second_byte) < (int_ptr_t)sizeof(second_byte))
		return ERROR;
	
	bool final = (first_byte & kFinalBit) != 0;
	bool reserved1 = (first_byte & kReserved1Bit) != 0;
	bool reserved2 = (first_byte & kReserved2Bit) != 0;
	bool reserved3 = (first_byte & kReserved3Bit) != 0;
	uint8_t opcode = first_byte & kOpCodeMask;
	bool masked = (second_byte & kMaskBit) != 0;
	uint64_t payload_length = second_byte & kPayloadLengthMask;
	if (payload_length == kPayloadLengthWithTwoByteExtendedLengthField) 
	{
		uint16_t payload_length_16;
		if(stream.ReadU16(&payload_length_16) < (int_ptr_t)sizeof(payload_length_16))
			return ERROR;
		payload_length = payload_length_16;
		if(payload_length <= kMaxPayloadLengthWithoutExtendedLengthField)
			return ERROR;

	} 
	else if (payload_length == kPayloadLengthWithEightByteExtendedLengthField) 
	{
		if(stream.ReadU64(&payload_length) < (int_ptr_t)sizeof(payload_length))
			return ERROR;
		if(payload_length <= kuint16max || payload_length >= (uint64_t)kint32max)
			return ERROR;
	}

	if (masked) 
	{
		if (stream.GetLeftSize() < (int_ptr_t)kMaskingKeyLength)
			return ERROR;
		if(stream.ReadBytes(&m_MaskingKey,kMaskingKeyLength) < (int_ptr_t)kMaskingKeyLength)
			return ERROR;
	}
	else 
	{
		m_MaskingKey = 0;
	}
	
	m_OpCode = opcode;
	m_Fin = final;
	m_Reserved1 = reserved1;
	m_Reserved2 = reserved2;
	m_Reserved3 = reserved3;
	m_Mask = masked;
	m_PayloadLen = payload_length;

	return OK;
}


inline void MaskWebSocketFramePayloadByBytes( const uint32_t masking_key, size_t masking_key_offset, char* const begin, char* const end) 
{
	const uint8_t *key = (const uint8_t*)(&masking_key);
	for (char* masked = begin; masked != end; ++masked) 
	{
		*masked ^= key[masking_key_offset++];
		if (masking_key_offset == 4)
			masking_key_offset = 0;
	}
}

static status_t MaskWebSocketFramePayload(uint32_t masking_key,uint64_t frame_offset,char* const data,int_ptr_t data_size) 
{

	ASSERT(data && data_size > 0);

	typedef size_t PackedMaskType;
	PackedMaskType packed_mask_key = 0;

	static const size_t kPackedMaskKeySize = sizeof(packed_mask_key);

	ASSERT((kPackedMaskKeySize >= kMaskingKeyLength &&
		kPackedMaskKeySize % kMaskingKeyLength == 0));

	char* const end = data + data_size;

	if (data_size <= static_cast<int>(kPackedMaskKeySize * 2)) 
	{
		MaskWebSocketFramePayloadByBytes(
			masking_key, 
			(size_t)frame_offset % kMaskingKeyLength, 
			data, end
		);
		return OK;
	}

	const size_t data_modulus =
		reinterpret_cast<size_t>(data) % kPackedMaskKeySize;

	char* const aligned_begin =
		data_modulus == 0 ? data : (data + kPackedMaskKeySize - data_modulus);

	ASSERT(aligned_begin < end);

	MaskWebSocketFramePayloadByBytes(
		masking_key, 
		(size_t)frame_offset % kMaskingKeyLength, 
		data, aligned_begin
	);

	const size_t end_modulus = reinterpret_cast<size_t>(end) % kPackedMaskKeySize;
	char* const aligned_end = end - end_modulus;

	// Guaranteed by the above check for small data_size.
	ASSERT(aligned_end > aligned_begin);
	
	// Create a version of the mask which is rotated by the appropriate offset
	// for our alignment. The "trick" here is that 0 XORed with the mask will
	// give the value of the mask for the appropriate byte.
	char realigned_mask[kMaskingKeyLength] = {0};

	MaskWebSocketFramePayloadByBytes(
		masking_key,
		(frame_offset + aligned_begin - data) % kMaskingKeyLength,
		realigned_mask,
		realigned_mask + kMaskingKeyLength
	);
	
	for (size_t i = 0; i < kPackedMaskKeySize; i += kMaskingKeyLength) 
	{
		memcpy(reinterpret_cast<char*>(&packed_mask_key) + i,
			realigned_mask,
			kMaskingKeyLength
		);
	}
	
	// The main loop.
	for (char* merged = aligned_begin; merged != aligned_end; merged += kPackedMaskKeySize) 
	{
		// This is not quite standard-compliant C++. However, the standard-compliant
		// equivalent (using memcpy()) compiles to slower code using g++. In
		// practice, this will work for the compilers and architectures currently
		// supported by Chromium, and the tests are extremely unlikely to pass if a
		// future compiler/architecture breaks it.
		*reinterpret_cast<PackedMaskType*>(merged) ^= packed_mask_key;
	}
	
	MaskWebSocketFramePayloadByBytes(
		masking_key,
		(size_t)(frame_offset + (aligned_end - data)) % kMaskingKeyLength,
		aligned_end,
		end
	);

	return OK;
}

status_t CWebSocketFrameHeader::GenerateWebSocketMaskingKey()
{
	uint8_t *p = (uint8_t*)&m_MaskingKey;
	p[0] = rand()&0xff;
	p[1] = rand()&0xff;
	p[2] = rand()&0xff;
	p[3] = rand()&0xff;
	return OK;
}

status_t CWebSocketFrameHeader::DecodeFramePayload(const void *indata,size_t size, CMem *out)
{
	ASSERT(indata && out);

	size_t next_size = size;	
	size_t offset = (size_t)out->GetOffset();
	size_t left_size = (size_t)out->GetMaxSize() - offset;
	
	if(left_size < next_size)
		next_size = left_size;

	if(next_size > 0)
	{
		char *des = out->GetRawBuf() + offset;	
		memcpy(des,indata,next_size);
		MaskWebSocketFramePayload(m_MaskingKey, m_FrameOffset, des,next_size);
		m_FrameOffset += next_size;
		if((size_t)out->GetSize()<offset + next_size)
			out->SetSize(offset + next_size);
		out->Seek(offset + next_size);
	}
	return OK;
}

int32_t CWebSocketFrameHeader::PredictHeaderSize(CFileBase *file)
{
	ASSERT(file);
	
	int32_t size = 0;

	CBigEndian stream;
	stream.Init(file);
	
	uint8_t first_byte = 0;
	if(stream.ReadU8(&first_byte) < (int_ptr_t)sizeof(first_byte))
		return size;
	
	uint8_t second_byte = 0;
	if(stream.ReadU8(&second_byte) < (int_ptr_t)sizeof(second_byte))
		return size;
	
	bool masked = (second_byte & kMaskBit) != 0;
	uint64_t payload_length = second_byte & kPayloadLengthMask;
	
	if (payload_length == kPayloadLengthWithTwoByteExtendedLengthField) 
	{
		size += 2;	
	} 
	else if (payload_length == kPayloadLengthWithEightByteExtendedLengthField) 
	{
		size += 8;
	}
	
	if (masked) 
	{
		size += 4;
	}

	return size;
}
