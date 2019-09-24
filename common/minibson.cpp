#include "minibson.h"
#include "memstk.h"
#include "memfile.h"
#include "file.h"
#include "syslog.h"
#include "mem_tool.h"

#define CHECK_TYPE_AND_NAME(_t,_name) do{\
fsize_t _old_off = mData->GetOffset();\
int _type = this->ReadByte();\
if(_type != _t)\
{\
    mData->Seek(_old_off);\
    return ERROR;\
}\
if(!this->CheckName(_name))\
{\
    mData->Seek(_old_off); \
    return ERROR;\
}\
}while(0)\

CMiniBson::CMiniBson()
{
    this->InitBasic();
}
CMiniBson::~CMiniBson()
{
    this->Destroy();
}
status_t CMiniBson::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    this->mData = NULL;
    return OK;
}
status_t CMiniBson::Init()
{
    this->InitBasic();
    WEAK_REF_ID_INIT();

    NEW(this->mData,CMem);
    this->mData->Init();

    return OK;
}
status_t CMiniBson::Destroy()
{
    DEL(this->mData);
    this->InitBasic();
    return OK;
}
status_t CMiniBson::Copy(CMiniBson *p)
{
    ASSERT(p && this != p);
    this->mData->Copy(p->GetRawData());
    return OK;
}
int CMiniBson::Comp(CMiniBson *p)
{
    return 0;
}
status_t CMiniBson::Print(CFileBase *_buf)
{
    this->ResetPointer();
    this->ToJson(_buf,true);
    return OK;
}
CMem * CMiniBson::GetRawData()
{
    return this->mData;
}

status_t CMiniBson::AllocBuf(int_ptr_t size)
{
    this->mData->Destroy();
    this->mData->Init();
    this->mData->Malloc(size);
    this->mData->SetSize(0);
    return OK;
}

status_t CMiniBson::SetRawBuf(void *buf, int_ptr_t size)
{
    this->mData->Destroy();
    this->mData->Init();
    this->mData->SetRawBuf(buf,size,false);
    this->mData->SetSize(0);
    return OK;
}

status_t CMiniBson::SetRawBuf(CMem *buf)
{
    ASSERT(buf);
    return this->SetRawBuf(buf->GetRawBuf(),(int_ptr_t)buf->GetMaxSize());
}

status_t CMiniBson::LoadBson(CFileBase *data)
{
    ASSERT(data);
    this->mData->Copy(data);
    this->ResetPointer();
    return OK;
}
status_t CMiniBson::LoadBson(const char *fn)
{
    ASSERT(fn);

    CFile file;
    file.Init();
    if(file.OpenFile(fn,"rb"))
    {
        return this->LoadBson(&file);
    }
    return ERROR;
}

status_t CMiniBson::LoadBson()
{
    this->ResetPointer();
    return OK;
}

status_t CMiniBson::WriteCString(CMem *str)
{
    ASSERT(str);
    if(str->StrLen() > 0)
    {
        this->Write(str->GetRawBuf(),str->StrLen());
    }
    this->WriteByte(0);
    return OK;
}

status_t CMiniBson::WriteCString(const char *str)
{
    ASSERT(str);
    this->Write(str,crt_strlen(str)+1);
    return OK;
}

status_t CMiniBson::UpdateDocumentSize()
{
    return this->UpdateDocumentSize(0,this->GetDocumentSize());
}

status_t CMiniBson::UpdateDocumentSize(fsize_t offset,fsize_t size)
{
    fsize_t off = this->mData->GetOffset();
    this->mData->Seek(offset);
    this->WriteInt32((int32_t)size);
    this->mData->Seek(off);
    return OK;
}

status_t CMiniBson::UpdateArraySize(fsize_t offset,fsize_t size,int32_t array_length)
{
    fsize_t off = this->mData->GetOffset();
    this->mData->Seek(offset);
    this->WriteInt32(array_length);
    this->WriteInt32((int32_t)size);
    this->mData->Seek(off);
    return OK;
}

status_t CMiniBson::Write(const void *buf, int_ptr_t size)
{
    ASSERT(buf);
    CMem mem;
    mem.Init();
    mem.SetRawBuf((void*)buf,size,true);
    return this->WriteFile(&mem);
}

status_t CMiniBson::WriteFile(CFileBase *file)
{
    ASSERT(file);
    int_ptr_t needSize = (int_ptr_t)(this->mData->GetSize()+file->GetSize());
    if(needSize < 512)needSize = 512;
    if(needSize > this->mData->GetMaxSize())
    {
        fsize_t old_off = this->mData->GetOffset();
        this->mData->Realloc(needSize*2);
        this->mData->Seek(old_off);
    }
    this->mData->WriteFile(file);
    return OK;
}

status_t CMiniBson::WriteInt32(int32_t i)
{
    this->Write(&i,sizeof(i));
    return OK;
}

status_t CMiniBson::WriteInt64(int64_t i)
{
    this->Write(&i,sizeof(i));
    return OK;
}

status_t CMiniBson::WriteByte(int8_t b)
{
    this->Write(&b,sizeof(b));
    return OK;
}

int32_t CMiniBson::ReadInt32()
{
    int32_t t;
    this->mData->Read(&t,sizeof(t));
    return t;
}

status_t CMiniBson::PutInt32(CMem *name,int32_t i)
{
    ASSERT(name);
    this->WriteByte(BSON_TYPE_INT32);
    this->WriteCString(name);
    this->WriteInt32(i);
    return OK;
}

status_t CMiniBson::PutUInt32(CMem *name,uint32_t i)
{
    return this->PutInt32(name,(int32_t)i);
}

status_t CMiniBson::PutInt32(const char *name, int32_t i)
{
    CMem mem(name);
    return this->PutInt32(&mem,i);
}

status_t CMiniBson::PutUInt32(const char *name, uint32_t i)
{
    return this->PutInt32(name,(int32_t)i);
}

status_t CMiniBson::PutInt16(const char *name, int16_t i)
{
    CMem mem(name);
    return this->PutInt32(&mem,i);
}
status_t CMiniBson::PutInt8(const char *name, int8_t i)
{
    CMem mem(name);
    return this->PutInt32(&mem,i);
}
status_t CMiniBson::PutString(CMem *name, CMem *str)
{
    ASSERT(name && str);
    this->WriteByte(BSON_TYPE_STRING);
    this->WriteCString(name);
    this->WriteInt32(str->StrLen());
    this->WriteCString(str);
    return OK;
}

status_t CMiniBson::PutString(const char *name, CMem *str)
{
    CMem mem(name);
    return this->PutString(&mem,str);
}

status_t CMiniBson::PutString(const char *name, const char *str)
{
    CMem mem1(name), mem2(str);
    return this->PutString(&mem1,&mem2);
}

status_t CMiniBson::PutBinary(CMem *name, const void *bin, int_ptr_t size)
{
    ASSERT(name);
    this->WriteByte(BSON_TYPE_BINARY);
    this->WriteCString(name);
    this->WriteInt32((int32_t)size);
    if(bin)this->Write(bin,size);
    return OK;
}

status_t CMiniBson::PutBinary(const char *name, const void *bin, int_ptr_t size)
{
    CMem mem(name);
    return this->PutBinary(&mem,bin,size);
}

status_t CMiniBson::PutBinary(CMem *name, CFileBase *bin)
{
    ASSERT(name && bin);
    this->WriteByte(BSON_TYPE_BINARY);
    this->WriteCString(name);
    this->WriteInt32((int32_t)bin->GetSize());
    this->WriteFile(bin);
    return OK;
}

status_t CMiniBson::PutBinary(const char *name, CFileBase *bin)
{
    CMem mem(name);
    return this->PutBinary(&mem,bin);
}

status_t CMiniBson::PutInt64(CMem *name,int64_t i)
{
    ASSERT(name);
    this->WriteByte(BSON_TYPE_INT64);
    this->WriteCString(name);
    this->WriteInt64(i);
    return OK;
}

status_t CMiniBson::PutInt64(const char *name, int64_t i)
{
    CMem mem(name);
    return this->PutInt64(&mem,i);
}

int32_t CMiniBson::GetDocumentSize()
{
    return (int32_t)this->mData->GetSize();
}

status_t CMiniBson::PutDocument(CMem *name, CMiniBson *bson)
{
    ASSERT(name && bson);
    this->WriteByte(BSON_TYPE_DOCUMENT);
    this->WriteCString(name);
    this->WriteFile(bson->GetRawData());
    this->WriteByte(0);
    return OK;
}

status_t CMiniBson::PutDocument(const char *name, CMiniBson *bson)
{
    CMem mem(name);
    return this->PutDocument(&mem,bson);
}

status_t CMiniBson::StartDocument(CMem *name,fsize_t *offset)
{
    ASSERT(name && offset);
    this->WriteByte(BSON_TYPE_DOCUMENT);
    this->WriteCString(name);
    *offset = this->mData->GetOffset();
    this->WriteInt32(0); //size
    return OK;
}

status_t CMiniBson::StartDocument(const char *name,fsize_t *offset)
{
    CMem mem_name(name);
    return this->StartDocument(&mem_name,offset);
}

status_t CMiniBson::StartArray(CMem *name,fsize_t *offset)
{
    ASSERT(name && offset); 
    this->WriteByte(BSON_TYPE_ARRAY);
    this->WriteCString(name);
    *offset = this->mData->GetOffset();
    this->WriteInt32(0); //array length
    this->WriteInt32(0); //size
    return OK;
}

status_t CMiniBson::StartArray(const char *name,fsize_t *offset)
{
    CMem mem_name(name);
    return this->StartArray(&mem_name,offset);
}

status_t CMiniBson::EndArray(fsize_t offset,int32_t array_len)
{
    fsize_t size = this->mData->GetOffset()-offset;
    this->UpdateArraySize(offset,size,array_len);
    this->WriteByte(0);
    return OK;
}

status_t CMiniBson::EndDocument(fsize_t offset)
{
    fsize_t size = this->mData->GetOffset()-offset;
    this->UpdateDocumentSize(offset,size);
    this->WriteByte(0);
    return OK;
}

status_t CMiniBson::PutBoolean(CMem *name,bool b)
{
    ASSERT(name);
    this->WriteByte(BSON_TYPE_BOOLEAN);
    this->WriteCString(name);
    this->WriteByte(b);
    return OK;
}

status_t CMiniBson::PutBoolean(const char *name, bool b)
{
    CMem mem(name);
    return this->PutBoolean(&mem,b);
}

int8_t CMiniBson::ReadByte()
{
    int8_t b;
    this->mData->Read(&b,sizeof(b));
    return b;
}

status_t CMiniBson::ReadString(CMem *str)
{
    ASSERT(str);

    str->SetSize(0);

    int_ptr_t len = (int_ptr_t)this->mData->GetSize();
    int_ptr_t i = (int_ptr_t)this->mData->GetOffset();
    char *buf = this->mData->GetRawBuf();
    char ch;

    while(i < len)
    {
        ch = buf[i];
        if(ch == 0) break;
        str->Putc(ch);
        i++;
    }
    this->mData->Seek(i+1);
    return OK;
}

status_t CMiniBson::SkipString()
{
    int_ptr_t len = (int_ptr_t)this->mData->GetSize();
    int_ptr_t i = (int_ptr_t)this->mData->GetOffset();
    char *buf = this->mData->GetRawBuf();
    while(i < len)
    {
        if(buf[i] == 0)break;
        i++;
    }
    this->mData->Seek(i+1);
    return OK;
}

status_t CMiniBson::PutDouble(CMem *name, double d)
{
    ASSERT(name);
    this->WriteByte(BSON_TYPE_DOUBLE);
    this->WriteCString(name);
    this->Write(&d,sizeof(d));
    return OK;
}

status_t CMiniBson::PutDouble(const char *name, double d)
{
    CMem mem(name);
    return this->PutDouble(&mem,d);
}

status_t CMiniBson::GetInt32(const char *name, int32_t *pInt)
{
    ASSERT(pInt);
    CHECK_TYPE_AND_NAME(BSON_TYPE_INT32,name);
    this->mData->Read(pInt,sizeof(int32_t));
    return OK;
}

status_t CMiniBson::GetUInt32(const char *name, uint32_t *pInt)
{
    return this->GetInt32(name,(int32_t*)pInt);
}

status_t CMiniBson::GetInt16(const char *name, int16_t *pInt)
{
    int32_t tmp;
    status_t ret = GetInt32(name,&tmp);
    *pInt = (int16_t)tmp;
    return ret;
}
status_t CMiniBson::GetInt8(const char *name, int8_t *pInt)
{
    int32_t tmp;
    status_t ret = GetInt32(name,&tmp);
    *pInt = (int8_t)tmp;
    return ret;
}
status_t CMiniBson::GetInt64(const char *name, int64_t *pInt)
{
    ASSERT(pInt);    
    CHECK_TYPE_AND_NAME(BSON_TYPE_INT64,name);
    this->mData->Read(pInt,sizeof(int64_t));
    return OK;
}

status_t CMiniBson::GetDouble(const char *name, double *d)
{
    ASSERT(d);    
    CHECK_TYPE_AND_NAME(BSON_TYPE_DOUBLE,name);
    this->mData->Read(d,sizeof(double));
    return OK;
}

bool CMiniBson::CheckName(const char *name)
{
    if(name)
    {
        LOCAL_MEM(mem);
        this->ReadString(&mem);
        return crt_strcmp(name,mem.CStr()) == 0;
    }
    else
    {
        this->SkipString();
    }

    return true;
}

status_t CMiniBson::GetBoolean(const char *name, bool *b)
{
    ASSERT(b);    
    CHECK_TYPE_AND_NAME(BSON_TYPE_BOOLEAN,name);
    int8_t t = 0;
    this->mData->Read(&t,sizeof(t));
    *b = (t != 0);
    return OK;
}

status_t CMiniBson::GetString(const char *name, CMem *str)
{
    ASSERT(str);
	
	SAVE_WEAK_REF_ID(*str,w);
    str->Destroy();
    str->Init();
	RESTORE_WEAK_REF_ID(*str,w);

    CHECK_TYPE_AND_NAME(BSON_TYPE_STRING,name);
    int32_t size=this->ReadInt32();
    char *buf = (char*)this->mData->GetRawBuf();
    int32_t off = (int32_t)this->mData->GetOffset();
    str->SetRawBuf(buf+off,size,true);
    this->mData->Seek(size+off+1); // '\0' at end
    return OK;
}

status_t CMiniBson::GetBinary(const char *name, CMem *bin)
{
    ASSERT(bin);

	SAVE_WEAK_REF_ID(*bin,w);
    bin->Destroy();
    bin->Init();
	RESTORE_WEAK_REF_ID(*bin,w);
    
    CHECK_TYPE_AND_NAME(BSON_TYPE_BINARY,name);
    int32_t size=this->ReadInt32();
    char *buf = (char*)this->mData->GetRawBuf();
    int32_t off = (int32_t)this->mData->GetOffset();
    bin->SetRawBuf(buf+off,size,true);
    this->mData->Seek(off+size);
    return OK;
}

status_t CMiniBson::ResetPointer()
{
    this->mData->Seek(sizeof(int32_t));
    return OK;
}

status_t CMiniBson::GetDocument(const char *name, CMiniBson *doc)
{
    ASSERT(doc);
    
    CHECK_TYPE_AND_NAME(BSON_TYPE_DOCUMENT,name);
    int32_t size;
    fsize_t off = this->mData->GetOffset();
    this->mData->Read(&size,sizeof(size));

	SAVE_WEAK_REF_ID(*doc,w);
    doc->Destroy();
    doc->Init();
	RESTORE_WEAK_REF_ID(*doc,w);

    CMem *data = doc->GetRawData();
    char *buf = this->mData->GetRawBuf();

    data->SetRawBuf(buf+off,size,false);
    doc->ResetPointer();
    this->mData->Seek(off+size+1); // '\0' end
    return OK;
}

status_t CMiniBson::GetArray(const char *name, CMiniBson *doc,int32_t *array_length)
{
    ASSERT(doc && array_length);
    *array_length = 0;

    CHECK_TYPE_AND_NAME(BSON_TYPE_ARRAY,name);

    int32_t size;
	    
	this->mData->Read(array_length,sizeof(int32_t));    
	fsize_t off = this->mData->GetOffset();
    this->mData->Read(&size,sizeof(size));
    
    size -= sizeof(int32_t);

	SAVE_WEAK_REF_ID(*doc,w);
    doc->Destroy();
    doc->Init();
	RESTORE_WEAK_REF_ID(*doc,w);

    CMem *data = doc->GetRawData();
    char *buf = this->mData->GetRawBuf();
    
    data->SetRawBuf(buf+off,size,false);
    doc->ResetPointer();
    this->mData->Seek(off+size+1); // '\0' end
    return OK;
}

bool CMiniBson::IsEnd()
{
    return this->mData->IsEnd();
}

status_t CMiniBson::StartDocument()
{
    this->mData->SetSize(0);
    this->WriteInt32(0);
    return OK;
}

status_t CMiniBson::EndDocument()
{
    this->UpdateDocumentSize(0,this->mData->GetSize());
    return OK;
}

status_t CMiniBson::Traverse(bool recursive,int level,CClosure *closure)
{
    LOCAL_MEM(name);
    int8_t type;
    fsize_t old_off;
    
    CLOSURE_PARAM_PTR_CAN_BE_NULL(CMemStk*,path,INDEX_PATH);
    int is_in_array = closure->GetParamInt(INDEX_IS_IN_ARRAY);

    while(!this->IsEnd())
    {
        old_off = this->mData->GetOffset();
        type = this->ReadByte();
        if(type == 0)break;
        this->ReadString(&name);

        closure->SetParamInt(INDEX_TYPE,type);
        closure->SetParamPointer(INDEX_NAME,(void*)name.CStr());
        closure->SetParamInt(INDEX_LEVEL,level);

        if(path)path->Push(&name);

        switch(type)
        {       
            case BSON_TYPE_INT32:
            {
                int32_t t;
                this->mData->Read(&t,sizeof(t));
                closure->SetParamInt(INDEX_VALUE,t);
                closure->Run(EVENT_SINGLE_VALUE);
                break;
            }
            case BSON_TYPE_INT64:
            {
                int64_t t;
                this->mData->Read(&t,sizeof(t));
                closure->SetParamInt64(INDEX_VALUE,t);
                closure->Run(EVENT_SINGLE_VALUE);
                break;
            }
            case BSON_TYPE_DOUBLE:
            {
                double t;
                this->mData->Read(&t,sizeof(t));
                closure->SetParamDouble(INDEX_VALUE,t);
                closure->Run(EVENT_SINGLE_VALUE);
                break;
            }
            case BSON_TYPE_STRING:
            {
                LOCAL_MEM(mem);
                this->ReadInt32(); //size
                this->ReadString(&mem);
                closure->SetParamPointer(INDEX_VALUE,&mem);
                closure->Run(EVENT_SINGLE_VALUE);
                break;
            }
            case BSON_TYPE_DOCUMENT:
            {
                CMiniBson sub;
                sub.Init();
                this->mData->Seek(old_off);
                this->GetDocument(NULL,&sub);
                closure->SetParamInt(INDEX_IS_ARRAY,0);
                closure->Run(EVENT_BEGIN_DOCUMENT);
            
                closure->SetParamInt(INDEX_IS_IN_ARRAY,0);
                if(recursive)
                {                   
                    sub.Traverse(recursive,level+1,closure);                    
                }

                sub.ResetPointer();

                closure->SetParamInt(INDEX_IS_IN_ARRAY,is_in_array);
                closure->SetParamInt(INDEX_TYPE,type);
                closure->SetParamPointer(INDEX_NAME,(void*)name.CStr());
                closure->SetParamPointer(INDEX_VALUE,&sub);
                closure->SetParamInt(INDEX_IS_ARRAY,0);
                closure->SetParamInt(INDEX_LEVEL,level);
                closure->Run(EVENT_END_DOCUMENT);
                break;
            }
            case BSON_TYPE_BOOLEAN:
            {
                int8_t b;
                this->mData->Read(&b,sizeof(b));
                closure->SetParamInt(INDEX_VALUE,b);
                closure->Run(EVENT_SINGLE_VALUE);
                break;
            }
            case BSON_TYPE_BINARY:
            {
                int32_t t = this->ReadInt32();
                fsize_t off = this->mData->GetOffset();
				CMem bin;
				bin.Init();
				bin.SetRawBuf(mData->GetRawBuf()+off,t,true);
                this->mData->Seek(t+off);
                closure->SetParamPointer(INDEX_VALUE,&bin);
                closure->Run(EVENT_SINGLE_VALUE);
                break;
            }
            case BSON_TYPE_ARRAY:
            {
                CMiniBson sub;
                sub.Init();
                this->mData->Seek(old_off);
                int32_t len = 0;
                this->GetArray(NULL,&sub,&len);

                closure->SetParamInt(INDEX_IS_ARRAY,1);
                closure->Run(EVENT_BEGIN_DOCUMENT);

                is_in_array = 1;
                closure->SetParamInt(INDEX_IS_IN_ARRAY,is_in_array);            
                closure->SetParamInt(INDEX_IS_ARRAY,0);

                if(recursive)
                {
                    sub.Traverse(recursive,level+1,closure);
                }

                sub.ResetPointer();
                closure->SetParamInt(INDEX_TYPE,type);
                closure->SetParamPointer(INDEX_NAME,(void*)name.CStr());
                closure->SetParamPointer(INDEX_VALUE,&sub);
                closure->SetParamInt(INDEX_ARRAY_LEN,len);
                closure->SetParamInt(INDEX_IS_ARRAY,1);
                closure->SetParamInt(INDEX_IS_IN_ARRAY,is_in_array);
                closure->SetParamInt(INDEX_LEVEL,level);
                closure->Run(EVENT_END_DOCUMENT);

                is_in_array = 0;
                closure->SetParamInt(INDEX_IS_IN_ARRAY,is_in_array);
                break;
            }
            default:
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                    "MiniBson: unknow element type %d",type
                );
                break;
            }           
        }
    
        if(path)path->DelTop();
    }
    return OK;
}

#define INDEX_JSON_FILE 10
#define INDEX_CAN_PUT_COMMA 11

#define PUT_TAB(n) do{\
for(int ii = 0; ii < n+1; ii ++) \
    json->Puts("  ");\
}while(0)\

static status_t _to_json(CClosure *closure)
{
    CLOSURE_PARAM_INT(e,CMiniBson::INDEX_EVENT);
    CLOSURE_PARAM_INT(type,CMiniBson::INDEX_TYPE);
    CLOSURE_PARAM_PTR(const char*,name,CMiniBson::INDEX_NAME);
    CLOSURE_PARAM_INT(is_in_array,CMiniBson::INDEX_IS_IN_ARRAY);
    CLOSURE_PARAM_INT(level,CMiniBson::INDEX_LEVEL);
    CLOSURE_PARAM_PTR(CFileBase*,json,INDEX_JSON_FILE);
    CLOSURE_PARAM_INT(can_put_comma,INDEX_CAN_PUT_COMMA);

    if(e == CMiniBson::EVENT_BEGIN_DOCUMENT)
    {
        CLOSURE_PARAM_INT(is_array,CMiniBson::INDEX_IS_ARRAY);

        if(!can_put_comma)
        {
            can_put_comma = 1;
            closure->SetParamInt(INDEX_CAN_PUT_COMMA,can_put_comma);
        }
        else
        {
            json->Puts(",\r\n");            
        }
        
        PUT_TAB(level);

        if(!is_in_array)
        {
            json->Puts("\"");
            json->Puts(name);
            json->Puts("\":");
        }

        if(is_array)
            json->Puts("[\r\n");
        else
            json->Puts("{\r\n");    

        can_put_comma = 0;
        closure->SetParamInt(INDEX_CAN_PUT_COMMA,can_put_comma);        
    }

    else if(e == CMiniBson::EVENT_SINGLE_VALUE)
    {
        if(!can_put_comma)
        {
            can_put_comma = 1;
            closure->SetParamInt(INDEX_CAN_PUT_COMMA,can_put_comma);
        }
        else
        {
            json->Puts(",\r\n");
        }       

        PUT_TAB(level);

        if(!is_in_array)
        {
            json->Puts("\"");
            json->Puts(name);
            json->Puts("\":");
        }

        if(type == CMiniBson::BSON_TYPE_INT32)
        {
            CLOSURE_PARAM_INT(t,CMiniBson::INDEX_VALUE);
            json->Printf("%d",t);
        }
        else if(type == CMiniBson::BSON_TYPE_INT64)
        {
            char str[100];
            CLOSURE_PARAM_INT64(t,CMiniBson::INDEX_VALUE);
            crt_int64_to_str(t,str);
            json->Printf("%s",str);
        }
        else if(type == CMiniBson::BSON_TYPE_DOUBLE)
        {
            CLOSURE_PARAM_DOUBLE(t,CMiniBson::INDEX_VALUE);
            json->Printf("%f",t);
        }
        else if(type == CMiniBson::BSON_TYPE_STRING)
        {
            CLOSURE_PARAM_PTR(CMem*,mem,CMiniBson::INDEX_VALUE);
            json->Puts("\"");
            json->Puts(mem);
            json->Puts("\"");    
        }
        else if(type == CMiniBson::BSON_TYPE_BOOLEAN)
        {
            CLOSURE_PARAM_INT(b,CMiniBson::INDEX_VALUE);
            json->Puts(b?"true":"false");
        }
        else if(type == CMiniBson::BSON_TYPE_BINARY)
        {
            CLOSURE_PARAM_PTR(CMem*,bin,CMiniBson::INDEX_VALUE);
            json->Printf("<binary %d bytes>",bin->GetSize());
        }
    }

    else if(e == CMiniBson::EVENT_END_DOCUMENT)
    {
        CLOSURE_PARAM_INT(is_array,CMiniBson::INDEX_IS_ARRAY);

        json->Puts("\r\n");
        PUT_TAB(level);

        if(is_array)
            json->Puts("]");
        else
            json->Puts("}");     
    }

    return TRUE;
}

status_t CMiniBson::ToJson(CFileBase *json,bool bracket)
{
    ASSERT(json);
    if(bracket)json->Puts("{\r\n");
    CClosure closure(_to_json);
    closure.SetParamPointer(INDEX_JSON_FILE,json);
    closure.SetParamInt(INDEX_CAN_PUT_COMMA,0);
    this->ResetPointer();

    CMemStk path;
    path.Init(1024);

    closure.SetParamInt(INDEX_IS_IN_ARRAY,0);
    closure.SetParamPointer(INDEX_PATH,&path);
    this->Traverse(true,0,&closure);
    if(bracket)json->Puts("\r\n}\r\n");
    return OK;
}

fsize_t CMiniBson::GetPointerPos(void)
{
    return this->mData->GetOffset();
}

status_t CMiniBson::SetPointerPos(fsize_t pos)
{
    this->mData->Seek(pos);
    return OK;
}

status_t CMiniBson::PeekNext(int *type, CMem *name)
{
    ASSERT(type && name);
    if(this->IsEnd())return ERROR;
    fsize_t old = this->mData->GetOffset();
    *type = this->ReadByte();
    this->ReadString(name);
    this->mData->Seek(old);
    return OK;
}

status_t CMiniBson::LoadRawBuf(CMem *buf)
{
    ASSERT(buf);
    this->mData->Destroy();
    this->mData->Init();

    if(buf->GetSize() > 0)
    {
        this->mData->SetRawBuf(buf->GetRawBuf(),(int_ptr_t)buf->GetSize(),true);
        this->ResetPointer();
    }

    return OK;
}






