#include "memstk.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "encoder.h"
#include "xml.h"

CLOSURE_COMMON_OBJECT_OPS_DEFINE_CPP(CMemStk,memstk)

CMemStk::CMemStk()
{
    this->InitBasic();  
}
CMemStk::~CMemStk()
{
    this->Destroy();
}
status_t CMemStk::InitBasic()
{
    CUserFunc::InitBasic();
    this->mIndex = NULL;
    this->mTop = 0;
    this->mSize = 0;    
    this->mBsonMode = BSON_MODE_RAW;
    return OK;
}
status_t CMemStk::Init(int init_size)
{
    int i;
    this->InitBasic();  
    CUserFunc::Init();
    this->mSize = init_size;
    MALLOC(this->mIndex,CMem* ,this->mSize);
    for(i = 0; i < this->mSize; i++)
        this->mIndex[i] = NULL;
    return OK;
}
status_t CMemStk::Destroy()
{
    int i;
    if(this->mIndex == NULL)
        return ERROR;
    for(i = 0; i < this->mTop; i++)
    {
        this->DelNode(this->mIndex[i]);
    }
    FREE(this->mIndex);
    CUserFunc::Destroy();
    this->InitBasic();  
    return OK;
}
bool CMemStk::IsEmpty()
{
    return this->mTop <= 0;
}
bool CMemStk::IsFull()
{
    return this->mTop >= this->mSize;
}

status_t CMemStk::AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->mIndex,CMem*,this->mSize,this->mSize*2);
        this->mSize *= 2;
        for(i = this->mTop; i < this->mSize; i++)
        {
            if(mIndex)this->mIndex[i] = NULL;
        }
    }
    return OK;
}

status_t CMemStk::Push(CMem *mem)
{
    CMem *tmp;
    ASSERT(mem);
    tmp = this->CloneNode(mem);
    if(!this->PushPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CMemStk::PushPtr(CMem *mem)
{
    ASSERT(mem);    
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mIndex[this->mTop] = mem;
    this->mTop++;
    return OK;
}

CMem * CMemStk::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->mTop--;
    return this->mIndex[this->mTop];
}

status_t CMemStk::Pop(CMem *mem)
{
    ASSERT(!this->IsEmpty());   
    this->mTop--;
    this->CopyNode(mem,this->mIndex[this->mTop]);
    this->DelNode(this->mIndex[this->mTop]);
    return OK;
}
int CMemStk::GetLen()
{
    return this->mTop;
}

int CMemStk::SearchPos(CMem *mem)
{
    int i;
    for(i=0;i<this->mTop;i++)
    {
        if(this->CompNode(this->mIndex[i],mem) == 0)
            return i;
    }
    return -1;
}

CMem* CMemStk::Search(CMem *mem)
{
    int pos = this->SearchPos(mem);
    if(pos >= 0 && pos < this->mTop)
        return this->mIndex[pos];
    return NULL;
}
CMem* CMemStk::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->mIndex[this->mTop - 1];
}
status_t CMemStk::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->mTop--;
    this->DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CMemStk::Clear()
{
    while(this->DelTop());
    return OK;
}

CMem* CMemStk::GetElem(int index)
{
    if(index < 0 || index >= this->mTop)
        return NULL;
    return this->mIndex[index];
}

CMem* CMemStk::BSearchNode(CMem *mem,int order)
{
    return this->GetElem(this->BSearch(mem,order));
}
int CMemStk::BSearch(CMem *mem,int order)
{
    int find,pos;
    pos = this->BSearchPos(mem,order,&find);
    if(find) return pos;
    return -1;
}

status_t CMemStk::InsElemPtr(int i, CMem *mem)
{
    int k;

    ASSERT(mem);
    ASSERT(i >= 0 && i <= this->mTop);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(k = this->mTop; k > i; k--)
    {
        this->mIndex[k] = this->mIndex[k - 1];
    }
    this->mIndex[i] = mem;
    this->mTop++;
    return OK;
}

status_t CMemStk::InsElem(int i, CMem *mem)
{
    CMem *tmp;
    ASSERT(mem);
    tmp = this->CloneNode(mem);
    if(!this->InsElemPtr(i,tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CMem* CMemStk::RemoveElem(int index)
{
    CMem *p;
    int k;
    
    ASSERT(index >= 0 && index < this->mTop);
    p = this->GetElem(index);

    for(k = index; k < this->mTop-1; k++)
    {
        this->mIndex[k] = this->mIndex[k + 1];
    }
    this->mTop --;
    this->mIndex[this->mTop] = NULL;
    return p;
}

status_t CMemStk::DelElem(int i)
{
    CMem *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CMemStk::InsOrderedPtr(CMem *mem, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(mem,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,mem);
}

status_t CMemStk::InsOrdered(CMem *mem, int order,int unique)
{
    int pos,find;
    ASSERT(mem);
    pos = this->BSearchPos(mem,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElem(pos,mem);
}

int CMemStk::BSearchPos(CMem *mem, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = this->CompNode(this->mIndex[mid],mem);
        if(comp == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
    
        if(order != 0) comp = -comp;
        if(comp>0)high=mid-1;else low=mid+1;
    }
    *find_flag = FALSE;
    return low;
}

status_t CMemStk::Sort(int order)
{
    int i,len;
    CMemStk tmp;

    len = this->GetLen();
    tmp.Init(len);

    for(i = 0; i < len; i++)
    {
        tmp.PushPtr(this->GetElem(i));
    }

    this->mTop = 0;
    for(i = 0; i < len; i++)
    {
        this->InsOrderedPtr(tmp.PopPtr(),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.mIndex[i] = NULL;
    }

    return OK;
}
status_t CMemStk::Copy(CMemStk *stk)
{
    int i;

    ASSERT(stk);
    this->Clear();
    CUserFunc::Copy((CUserFunc*)stk);
    for(i = 0; i < stk->GetLen(); i++)
    {
        CMem *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
status_t CMemStk::Print(CFileBase *_buf)
{
    for(int i = 0; i < this->mTop; i++)
    {
        if(this->mIndex[i])
        {
            _buf->Log("[%d]=%s",i,mIndex[i]->CStr());
        }
    }
    _buf->Log("top = %d",this->mTop);
    _buf->Log("size = %d",this->mSize);
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CMemStk::CopyNode(CMem *dst, CMem *src)
{
    if(this->IsUserFunc(HOW_TO_COPY))
    {
        ASSERT(Callback()->GetFunc());
        Callback()->SetParamPointer(1,dst);
        Callback()->SetParamPointer(2,src);
        return Callback()->Run(HOW_TO_COPY);
    }
    else
    {
        return dst->Copy(src);
    }
}

CMem* CMemStk::CloneNode(CMem *mem)
{
    CMem *tmp;
    NEW(tmp,CMem);
    tmp->Init();
    this->CopyNode(tmp,mem);
    return tmp;
}

status_t CMemStk::DelNode(CMem *mem)
{
    if(this->IsUserFunc(HOW_TO_DEL))
    {
        ASSERT(Callback()->GetFunc());
        Callback()->SetParamPointer(1,mem);
        return Callback()->Run(HOW_TO_DEL);
    }
    else
    {
        DEL(mem);
        return OK;
    }   
}

int CMemStk::CompNode(CMem *mem1, CMem *mem2)
{
    ASSERT(mem1 && mem2);

    if(this->IsUserFunc(HOW_TO_COMP))
    {
        ASSERT(Callback()->GetFunc());
        Callback()->SetParamPointer(1,mem1);
        Callback()->SetParamPointer(2,mem2);
        return Callback()->Run(HOW_TO_COMP);
    }
    else
    {
        return mem1->StrCmp(mem2);
    }
}

status_t CMemStk::LoadLines(CFileBase *file)
{
    ASSERT(file);
    
    LOCAL_MEM(mem);
    
    file->Seek(0);
    while(file->ReadLine(&mem))
    {
        if(mem.C(0) == 0)continue;
        this->Push(&mem);
    }
    
    return OK;
}

status_t CMemStk::SaveLines(CFileBase *file)
{
    ASSERT(file);    
    for(int32_t i = 0; i < this->GetLen(); i++)
    {
        CMem *p = this->GetElem(i);
        ASSERT(p);
        file->Puts(p);
        file->Puts("\r\n");
    }
    
    return OK;
}

bool CMemStk::Equal(CMemStk *stk)
{
    ASSERT(stk);
    
    if(this->GetLen() != stk->GetLen())
        return false;
    int len = this->GetLen();
    for(int i = 0; i < len; i++)
    {
        if(this->GetElem(i)->StrLen()!=stk->GetElem(i)->StrLen())
            return false;   
        if(CompNode(this->GetElem(i),stk->GetElem(i)) != 0)
            return false;
    }
    return true;
}

status_t CMemStk::LoadPath(CFileBase* path)
{
    LOCAL_MEM(mem);
    path->SetSplitChars("/\\");
    path->Seek(0);
    while(path->ReadString(&mem))
    {
        if(mem.StrICmp("..") == 0)
        {
            this->DelTop();
        }
        else if(mem.StrICmp(".") != 0)
        {
            this->Push(&mem);
        }   
    }   
    return OK;
}

status_t CMemStk::SavePath(CFileBase *file)
{
    CMem *pstr;
    ASSERT(file);
    
    file->SetSize(0);
    if(this->IsEmpty())
    {
        file->Putc('/');
        return OK;
    }
    for(int32_t i = 0 ; i < this->GetLen() ;i++)
    {
        pstr = this->GetElem(i);
        ASSERT(pstr);
        if(pstr->C(0) != 0)
        {
            file->Putc('/');
            file->Puts(pstr);
        }
    }   
    return OK;
}

status_t CMemStk::PushFile(CFileBase *file)
{
    ASSERT(file);
    CMem *new_node;
    NEW(new_node,CMem);
    new_node->Init();
    new_node->Copy(file);
    return this->PushPtr(new_node);
}

status_t CMemStk::Push(const char *str)
{
    CMem mem(str);
    return this->Push(&mem);
}

status_t CMemStk::Push(const void *data, size_t size)
{
    CMem tmp;
    tmp.Init();
    tmp.SetRawBuf((void*)data,size,true);
    return this->Push(&tmp);
}

#if _UNICODE_
status_t CMemStk::PushW(const wchar_t *str)
{
    CMem mem(str);
    return this->Push(&mem);
}
#endif


status_t CMemStk::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);

    fsize_t off;
    _bson->StartArray("_array_",&off);
    char name[256];
    for(int i = 0; i < this->GetLen(); i++)
    {
        CMem *p = this->GetElem(i);
        ASSERT(p);        
        crt_snprintf(name,sizeof(name)-1,"%d",i);
    
        if(mBsonMode == BSON_MODE_RAW)
        {
            fsize_t _off;
            _bson->StartDocument(name,&_off);      
            _bson->PutString("file_name",
                p->file_name?p->file_name:""
            );
            _bson->PutBinary("data",p);        
            _bson->EndDocument(_off);
        }

        if(mBsonMode == BSON_MODE_STR)
        {
            _bson->PutString(name,p);
        }
    }
    _bson->EndArray(off,this->GetLen());    

    return OK;
}

status_t CMemStk::SaveBson(CMem *_mem)
{
    ASSERT(_mem);
    CMiniBson _bson;
    _bson.Init();
    _bson.SetRawBuf(_mem);
    _bson.StartDocument();
    this->SaveBson(&_bson);
    _bson.EndDocument();
    _mem->SetSize(_bson.GetDocumentSize());
    return OK;
}

status_t CMemStk::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);

    CMiniBson doc;
    doc.Init();
    
    int32_t len = 0;
    BSON_CHECK(_bson->GetArray("_array_",&doc,&len));
    this->Clear();
    
    doc.ResetPointer();
    for(int i = 0; i < len; i++)
    {
        if(mBsonMode == BSON_MODE_RAW)
        {
            CMiniBson sub_doc;
            sub_doc.Init();        

            doc.GetDocument(NULL,&sub_doc);

            CMem *p=NULL;
            NEW(p,CMem);
            p->Init();

            CMem fn,data;
            fn.Init();
            data.Init();

            BSON_CHECK(sub_doc.GetString("file_name",&fn));
            BSON_CHECK(sub_doc.GetBinary("data",&data));

            p->Copy(&data);
            if(fn.StrLen() > 0) //must be after Copy
            {
                p->SetFileName(fn.CStr());
            }
        
            this->PushPtr(p);      
        }  

        if(mBsonMode == BSON_MODE_STR)
        {
            CMem tmp;
            tmp.Init();
            doc.GetString(NULL,&tmp);
            this->Push(&tmp);
        }

    }

    return OK;
}

status_t CMemStk::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

status_t CMemStk::LoadXml(const char *fn, const char *path)
{
    ASSERT(fn && path);

    CXml xml;
    xml.Init();
    ASSERT(xml.LoadXml(fn));

    CXmlNode *px = xml.GetNodeByPath(path);
    ASSERT(px);

    return this->LoadXml(px);
}

status_t CMemStk::SaveXml(const char *fn, const char *node_name)
{
    ASSERT(fn && node_name);

    CMemFile mf;
    mf.Init();
    mf.Log("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    mf.Log("<%s size=\"%d\">",node_name,this->GetLen());
    mf.IncLogLevel(1);
    this->SaveXml(&mf);
    mf.IncLogLevel(-1);
    mf.Log("</%s>",node_name);

    return mf.WriteToFile(fn) > 0;
}

status_t CMemStk::LoadXml(CXmlNode *_root)
{
    ASSERT(_root);
    CXmlNode *px;
	
	LOCAL_MEM(tmp);

    px = _root->GetChild();
	while(px)
	{	
		CMem *val = px->GetValue();
		if(val)
		{
			tmp.SetSize(0);
			tmp.Puts(val);
			CEncoder::UnEscapeXmlStr(&tmp);
			this->Push(&tmp);
		}
		px = px->GetNext();
	}

    return OK;
}

status_t CMemStk::SaveXml(CFileBase *_xml)
{
    ASSERT(_xml);
    LOCAL_MEM(tmp);
    for(int i = 0; i < this->GetLen(); i++)
    {
        CMem *p = this->GetElem(i);
        ASSERT(p);
        tmp.SetSize(0);
        tmp.Puts(p);
        CEncoder::EscapeXmlStr(&tmp);
        _xml->Log("<e>%s</e>",tmp.CStr());
    }
    return OK;
}

status_t CMemStk::SetBsonMode(int mode)
{
    mBsonMode = mode;
    return OK;
}

