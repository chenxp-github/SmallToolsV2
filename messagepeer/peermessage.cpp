#include "peermessage.h"
#include "syslog.h"
#include "mem_tool.h"

#define BODY_TYPE_MASK  0x000000f0
#define MSG_TYPE_MASK   0x0000000f

CPeerMessage::CPeerMessage()
{
    REF_COUNT_INIT();
    this->InitBasic();
}
CPeerMessage::~CPeerMessage()
{
    this->Destroy();
}
status_t CPeerMessage::InitBasic()
{
    this->mFrom = NULL;
    this->mTo = NULL;
    this->mFunc = 0;
    this->mBody = NULL;
    this->mCallbackId = 0;
    this->mFlags = 0;
    return OK;
}
status_t CPeerMessage::Init()
{
    this->InitBasic();
    NEW(this->mFrom,CMem);
    this->mFrom->Init();

    NEW(this->mTo,CMem);
    this->mTo->Init();

    NEW(this->mBody,CMem);
    this->mBody->Init();

    return OK;
}
status_t CPeerMessage::Destroy()
{
    DEL(this->mFrom);
    DEL(this->mTo);
    DEL(this->mBody);
    this->InitBasic();
    return OK;
}
status_t CPeerMessage::Copy(CPeerMessage *p)
{
    ASSERT(p && this != p);
    this->mFrom->Copy(p->mFrom);
    this->mTo->Copy(p->mTo);
    this->mFunc = p->mFunc;
    this->mBody->Copy(p->mBody);
    this->mFlags = p->mFlags;
    this->mCallbackId = p->mCallbackId;
    return OK;
}
int CPeerMessage::Comp(CPeerMessage *p)
{
    ASSERT(p);
    return 0;
}
status_t CPeerMessage::Print()
{
    if(this->mFrom->StrLen()>0)
        syslog_printf("From = %s",this->mFrom->CStr());
    if(this->mTo->StrLen()>0)
        syslog_printf("To = %s",this->mTo->CStr());
    syslog_printf("Func = %d",this->mFunc);
    syslog_printf("Body = <binary %d bytes>",(int)this->mBody->GetSize());
    syslog_printf("Flags=0x%08x",mFlags);
    syslog_printf("CallbackId=%d",mCallbackId);
    return OK;
}
status_t CPeerMessage::Clear()
{
    this->Destroy();
    this->Init();
    return OK;
}
status_t CPeerMessage::SetFrom(CMem *from)
{
    ASSERT(from);
    this->mFrom->Copy(from);
    return OK;
}
status_t CPeerMessage::SetFrom(const char *from)
{
    ASSERT(from);
    CMem mem_from(from);
    return this->SetFrom(&mem_from);
}
status_t CPeerMessage::SetTo(CMem *to)
{
    ASSERT(to);
    this->mTo->Copy(to);
    return OK;
}
status_t CPeerMessage::SetTo(const char *to)
{
    ASSERT(to);
    CMem mem_to(to);
    return this->SetTo(&mem_to);
}
status_t CPeerMessage::SetFunc(int func)
{
    this->mFunc = func;
    return OK;
}
status_t CPeerMessage::SetBody(CMem *body)
{
    ASSERT(body);
    this->SetBodyType(RAW);
    this->mBody->Copy(body);
    return OK;
}
status_t CPeerMessage::SetBody(const void *body, int_ptr_t size)
{
    ASSERT(body);
    CMem mem_body;
    mem_body.Init();
    mem_body.SetRawBuf((void*)body,size,true);
    return this->SetBody(&mem_body);
}
CMem* CPeerMessage::GetFrom()
{
    return this->mFrom;
}
CMem* CPeerMessage::GetTo()
{
    return this->mTo;
}
int CPeerMessage::GetFunc()
{
    return this->mFunc;
}
CMem* CPeerMessage::GetBody()
{
    return this->mBody;
}
status_t CPeerMessage::TransferBody(CMem *body)
{
    ASSERT(body);
    return this->mBody->Transfer(body);
}

status_t CPeerMessage::SetBodyString(const char *str)
{
    ASSERT(str);
    CMem mem(str);
    return this->SetBodyString(&mem);
}
status_t CPeerMessage::SetBodyString(CMem *str)
{
    ASSERT(str);
    this->SetBody(str);
    this->SetBodyType(STRING);
    return OK;
}
status_t CPeerMessage::SetCallbackId(int cbid)
{
    mCallbackId = cbid;
    return OK;
}

int CPeerMessage::GetCallbackId()
{
    return mCallbackId;
}

status_t CPeerMessage::SetMessageType(int type)
{
    this->mFlags &= (~MSG_TYPE_MASK);
    this->mFlags |= (type&MSG_TYPE_MASK);
    return OK;
}

status_t CPeerMessage::SetBodyType(int type)
{
    this->mFlags &= (~BODY_TYPE_MASK);
    this->mFlags |= (type&BODY_TYPE_MASK);
    return OK;
}

int CPeerMessage::GetMessageType()
{
    return mFlags&MSG_TYPE_MASK;
}

int CPeerMessage::GetBodyType()
{
    return mFlags&BODY_TYPE_MASK;
}

status_t CPeerMessage::SerializeHeader(CMem *buf)
{
    ASSERT(buf);
    buf->Write(&mFunc,sizeof(mFunc));
    buf->Write(&mCallbackId,sizeof(mCallbackId));
    buf->Write(&mFlags,sizeof(mFlags));
    if(mFrom->StrLen() > 0)
        buf->Write(mFrom->CStr(),mFrom->StrLen()+1);
    else
        buf->Putc(0);

    if(mTo->StrLen()>0)
        buf->Write(mTo->CStr(),mTo->StrLen()+1);
    else
        buf->Putc(0);

    return OK;
}
status_t CPeerMessage::UnSerializeHeader(CMem *buf)
{
    ASSERT(buf);
    buf->Read(&mFunc,sizeof(mFunc));
    buf->Read(&mCallbackId,sizeof(mCallbackId));
    buf->Read(&mFlags,sizeof(mFlags));

    char ch;
    LOCAL_MEM(mem);

    while(!buf->IsEnd())
    {
        ch = buf->Getc();
        if(ch == 0)break;
        mem.Putc(ch);
    }

    this->SetFrom(&mem);

    mem.SetSize(0);
    while(!buf->IsEnd())
    {
        ch = buf->Getc();
        if(ch == 0)break;
        mem.Putc(ch);
    }
    this->SetTo(&mem);
    return OK;
}

status_t CPeerMessage::CheckHeader()
{
    ASSERT(mFunc > 0);
    ASSERT(mCallbackId >= 0);

    int type = GetBodyType();
    ASSERT(
          type==BSON
        ||type==STRING
        ||type ==JSON
        ||type == RAW
    );

    return TRUE;
}

uint32_t CPeerMessage::GetFlags()
{
    return mFlags;
}
status_t CPeerMessage::SetFlags(uint32_t flags)
{
    mFlags = flags;
    return OK;
}
CPeerMessage* CPeerMessage::NewStringMessage(const char *to, int func,int cbid, CMem *str)
{
    ASSERT(to);
    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->SetTo(to);
    msg->SetFunc(func);
    msg->SetCallbackId(cbid);
    if(str)
    {
        msg->SetBodyString(str);
    }
    return msg;
}

CPeerMessage* CPeerMessage::NewStringMessage(const char *to, int func, int cbid, const char *str)
{
    ASSERT(to && str);
    CMem tmp(str);
    return NewStringMessage(to,func,cbid,&tmp);
}

CPeerMessage* CPeerMessage::NewBsonMessage(const char *to, int func, int cbid, CMem *bson)
{
    ASSERT(to);
    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->SetTo(to);
    msg->SetFunc(func);
    msg->SetCallbackId(cbid);
    msg->SetBodyType(CPeerMessage::BSON);
    if(bson)
    {
        ASSERT(bson->IsMalloc());
        msg->TransferBody(bson);
    }
    return msg;
}

CPeerMessage* CPeerMessage::NewBsonMessage(const char *to,int func, int cbid, CMiniBson *bson)
{
    CMem *data = NULL;
    if(bson)data = bson->GetRawData();
    return NewBsonMessage(to,func,cbid,data);
}

CPeerMessage* CPeerMessage::NewJsonMessage(const char *to, int func,int cbid, CMem *str)
{
    ASSERT(to);
    CPeerMessage *msg;
    NEW(msg,CPeerMessage);
    msg->Init();
    msg->SetTo(to);
    msg->SetFunc(func);
    msg->SetCallbackId(cbid);
    msg->SetBodyType(CPeerMessage::JSON);
    if(str)
    {
        msg->SetBodyString(str);
    }
    return msg;
}

CPeerMessage* CPeerMessage::NewJsonMessage(const char *to, int func, int cbid, const char *str)
{
    ASSERT(to && str);
    CMem tmp(str);
    return NewJsonMessage(to,func,cbid,&tmp);
}

CPeerMessage* CPeerMessage::NewEmptyMessage(const char *to, int func, int cbid)
{
    return NewBsonMessage(to,func,cbid,(CMiniBson*)NULL);
}
