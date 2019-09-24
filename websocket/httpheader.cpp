#include "httpheader.h"
#include "syslog.h"
#include "mem_tool.h"

CHttpHeader::CHttpHeader()
{
    this->InitBasic();
}
CHttpHeader::~CHttpHeader()
{
    this->Destroy();
}
status_t CHttpHeader::InitBasic()
{
    this->mMethod = NULL;
    this->mRequestUrl = NULL;
    this->mHttpVersion = NULL;
    this->mRetCode = 0;
    this->mParis = NULL;
    this->mRetCode = -1;
    return OK;
}
status_t CHttpHeader::Init()
{
    this->Destroy();

    NEW(this->mParis,CCommonArray<CHttpPair>);
    this->mParis->Init(32);

    NEW(this->mMethod,CMem);
    this->mMethod->Init();

    NEW(this->mRequestUrl,CMem);
    this->mRequestUrl->Init();

    NEW(this->mHttpVersion,CMem);
    this->mHttpVersion->Init();

    this->SetHttpVersion("HTTP/1.1");
    return OK;
}
status_t CHttpHeader::Destroy()
{
    DEL(this->mParis);
    DEL(this->mMethod);
    DEL(this->mRequestUrl);
    DEL(this->mHttpVersion);

    this->InitBasic();
    return OK;
}
status_t CHttpHeader::Copy(CHttpHeader *p)
{
    ASSERT(p && this != p);
    this->mMethod->Copy(p->mMethod);
    this->mRequestUrl->Copy(p->mRequestUrl);
    this->mHttpVersion->Copy(p->mHttpVersion);
    this->mRetCode = p->mRetCode;
    
    this->mParis->Destroy();
    if(p->mParis->GetLen() > 0)
    {
        this->mParis->Init(p->mParis->GetLen());
        for(int i = 0; i < p->mParis->GetLen(); i++)
        {
            CHttpPair *pair;
            NEW(pair,CHttpPair);
            pair->Init();
            pair->Copy(p->mParis->GetElem(i));
            this->mParis->PushPtr(pair);
        }
    }

    return OK;
}
int CHttpHeader::Comp(CHttpHeader *p)
{
    ASSERT(p);
    return 0;
}
status_t CHttpHeader::Print(CFileBase *_buf)
{
    _buf->Log("Method = %s\r\n",this->mMethod->CStr());
    _buf->Log("RequestUrl = %s\r\n",this->mRequestUrl->CStr());
    _buf->Log("HttpVersion = %s\r\n",this->mHttpVersion->CStr());
    _buf->Log("RetCode = %d\r\n",this->mRetCode);
    _buf->Log("Paris = {\r\n");
    this->mParis->Print(_buf);
    _buf->Log("}\r\n");
    return OK;
}
status_t CHttpHeader::SetMethod(CMem *method)
{
    ASSERT(method);
    this->mMethod->Copy(method);
    return OK;
}
status_t CHttpHeader::SetMethod(const char *method)
{
    ASSERT(method);
    CMem mem_method(method);
    return this->SetMethod(&mem_method);
}
status_t CHttpHeader::SetRequestUrl(CMem *requesturl)
{
    ASSERT(requesturl);
    this->mRequestUrl->Copy(requesturl);
    return OK;
}
status_t CHttpHeader::SetRequestUrl(const char *requesturl)
{
    ASSERT(requesturl);
    CMem mem_requesturl(requesturl);
    return this->SetRequestUrl(&mem_requesturl);
}
status_t CHttpHeader::SetHttpVersion(CMem *httpversion)
{
    ASSERT(httpversion);
    this->mHttpVersion->Copy(httpversion);
    return OK;
}
status_t CHttpHeader::SetHttpVersion(const char *httpversion)
{
    ASSERT(httpversion);
    CMem mem_httpversion(httpversion);
    return this->SetHttpVersion(&mem_httpversion);
}
CMem* CHttpHeader::GetMethod()
{
    return this->mMethod;
}
const char* CHttpHeader::GetMethodStr()
{
    return this->mMethod->CStr();
}
CMem* CHttpHeader::GetRequestUrl()
{
    return this->mRequestUrl;
}
const char* CHttpHeader::GetRequestUrlStr()
{
    return this->mRequestUrl->CStr();
}
CMem* CHttpHeader::GetHttpVersion()
{
    return this->mHttpVersion;
}
const char* CHttpHeader::GetHttpVersionStr()
{
    return this->mHttpVersion->CStr();
}

CHttpPair* CHttpHeader::GetPairByIndex(int index)
{
    return this->mParis->GetElem(index);
}

int CHttpHeader::KeyToIndex(CMem *key)
{
    ASSERT(key);
    
    CHttpPair tmp;
    tmp.Init();
    tmp.SetKey(key);

    return this->mParis->BSearch(&tmp,0);
}

status_t CHttpHeader::SetPair(CMem *key, CMem *value)
{
    ASSERT(key && value);
    int index = this->KeyToIndex(key);
    CHttpPair *pair = this->mParis->GetElem(index);

    if(pair)
    {
        pair->SetKey(key);
        pair->SetValue(value);
    }
    else
    {
        NEW(pair,CHttpPair);
        pair->Init();
        pair->SetKey(key);
        pair->SetValue(value);
        mParis->InsOrderedPtr(pair,0,1);
    }
    return OK;
}

status_t CHttpHeader::Load(CFileBase *file)
{
    ASSERT(file);

    LOCAL_MEM(mem);
    LOCAL_MEM(tmp);

    file->Seek(0);
    file->ReadLine(&mem);
    mem.SetSplitChars(" \t");

    if(mem.StartWith("HTTP/",0,1))
    {   
        mem.Seek(0);
        mem.ReadString(&tmp);
        this->SetHttpVersion(&tmp); //version
        mem.ReadString(&tmp);
        this->mRetCode = atoi(tmp.CStr()); //code
    }
    else
    {
        mem.Seek(0);
        mem.ReadString(&tmp); //method
        this->SetMethod(&tmp);
        mem.ReadString(&tmp); //url
        this->SetRequestUrl(&tmp);
        mem.ReadString(&tmp); //http version
        this->SetHttpVersion(&tmp);
    }


    mem.SetSplitChars(": ");

    this->mParis->Clear();
    while(file->ReadLine(&mem))
    {
        if(mem.C(0) == 0)continue;
        mem.Seek(0);

        CHttpPair *pair;
        NEW(pair,CHttpPair);
        pair->Init();

        mem.ReadString(&tmp);
        pair->SetKey(&tmp);

        mem.ReadWord(&tmp); // ':'
        mem.ReadLeftStr(&tmp,1);
        pair->SetValue(&tmp);

        mParis->InsOrderedPtr(pair,0,1);
    }

    return OK;
}

status_t CHttpHeader::SetPair(const char *key, const char *value)
{
    ASSERT(key && value);
    CMem _key(key);
    CMem _value(value);
    return this->SetPair(&_key,&_value);
}

bool CHttpHeader::IsChunked()
{
    CHttpPair *pair = this->GetPair("Transfer-encoding");
    if(pair)
    {
        return pair->GetValue()->StrICmp("chunked") == 0;
    }
    return false;
}

CHttpPair* CHttpHeader::GetPair(CMem *key)
{
    ASSERT(key);
    int index = this->KeyToIndex(key);
    return this->GetPairByIndex(index);
}

CHttpPair* CHttpHeader::GetPair(const char *key)
{
    CMem _key(key);
    return this->GetPair(&_key);
}

fsize_t CHttpHeader::GetContentLength()
{
    CHttpPair *pair = this->GetPair("Content-Length");
    if(!pair)return -1;
    return crt_str_to_fsize(pair->GetValueStr());
}

status_t CHttpHeader::Save(CFileBase *file)
{
    ASSERT(file);

    file->SetSize(0);

    file->Printf("%s %s %s\r\n",this->GetMethodStr(),this->GetRequestUrlStr(),this->GetHttpVersionStr());

    LOCAL_MEM(tmp_key);
    LOCAL_MEM(mem);

    for(int i = 0; i < this->mParis->GetLen(); i++)
    {
        this->mParis->GetElem(i)->ToString(&mem);
        file->Puts(&mem);
        file->Puts("\r\n");
    }
    file->Puts("\r\n");
    return OK;
}

status_t CHttpHeader::DelPair(CMem *key)
{
    int index = this->KeyToIndex(key);
    if(index >= 0)
        return this->mParis->DelElem(index);
    return ERROR;
}   

status_t CHttpHeader::DelPair(const char *key)
{
    ASSERT(key);
    CMem _key(key);
    return this->DelPair(&_key);
}

status_t CHttpHeader::GetLocation(CMem *location)
{
    ASSERT(location);
    location->SetSize(0);
    CHttpPair *pair = this->GetPair("Location");
    if(!pair)return ERROR;
    location->Puts(pair->GetValue());
    return OK;
}

int CHttpHeader::GetRetCode()
{
    return this->mRetCode;
}

status_t CHttpHeader::Clear()
{
    this->Destroy();
    this->Init();
    return OK;
}

status_t CHttpHeader::GetContentRange(fsize_t *start, fsize_t *end)
{

    CHttpPair *pair = this->GetPair("Content-Range");
    if(!pair)
        return ERROR;

    CMem *mem = pair->GetValue();

    mem->SetSplitChars(" \t-/");
    
    LOCAL_MEM(buf);

    mem->Seek(0);
    mem->ReadString(&buf); //bytes

    mem->ReadString(&buf);
    *start = crt_str_to_fsize(buf.CStr());
    
    mem->ReadString(&buf);
    *end = crt_str_to_fsize(buf.CStr());

    return OK;
}
