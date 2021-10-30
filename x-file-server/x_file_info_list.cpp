#include "x_file_info_list.h"
#include "mem_tool.h"
#include "sys_log.h"


CxFileInfoList::CxFileInfoList()
{
    this->InitBasic();  
}
CxFileInfoList::~CxFileInfoList()
{
    this->Destroy();
}
status_t CxFileInfoList::InitBasic()
{

    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;    
    return OK;
}
status_t CxFileInfoList::Init(int init_size)
{
    int i;
    this->InitBasic();  

    this->m_Size = init_size;
    MALLOC(this->m_Index,CxFileInfo* ,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}

status_t CxFileInfoList::Destroy()
{
    int i;

    if(this->m_Index == NULL)
        return ERROR;

    for(i = 0; i < this->m_Top; i++)
    {
        this->DelNode(this->m_Index[i]);
    }
    FREE(this->m_Index);
    this->InitBasic();    
    return OK;
}
bool CxFileInfoList::IsEmpty()
{
    return this->m_Top <= 0;
}
bool CxFileInfoList::IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t CxFileInfoList::AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,CxFileInfo*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t CxFileInfoList::Push(CxFileInfo *node)
{
    CxFileInfo *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->PushPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CxFileInfoList::PushPtr(CxFileInfo *node)
{
    ASSERT(node);    
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = node;
    this->m_Top++;
    return OK;
}

CxFileInfo * CxFileInfoList::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

status_t CxFileInfoList::Pop(CxFileInfo *node)
{
    ASSERT(!this->IsEmpty());    
    this->m_Top--;
    this->CopyNode(node,this->m_Index[this->m_Top]);
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
int CxFileInfoList::GetLen()
{
    return this->m_Top;
}

int CxFileInfoList::SearchPos(CxFileInfo *node)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],node) == 0)
            return i;
    }
    return -1;
}

CxFileInfo* CxFileInfoList::Search(CxFileInfo *node)
{
    int pos = this->SearchPos(node);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}
CxFileInfo* CxFileInfoList::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}
status_t CxFileInfoList::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t CxFileInfoList::Clear()
{
    while(this->DelTop());
    return OK;
}

CxFileInfo* CxFileInfoList::GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

CxFileInfo* CxFileInfoList::BSearchNode(CxFileInfo *node,int order)
{
    return this->GetElem(this->BSearch(node,order));
}
int CxFileInfoList::BSearch(CxFileInfo *node,int order)
{
    int find,pos;
    pos = this->BSearchPos(node,order,&find);
    if(find) return pos;
    return -1;
}

status_t CxFileInfoList::InsElemPtr(int i, CxFileInfo *node)
{
    int k;

    ASSERT(node);
    ASSERT(i >= 0 && i <= this->m_Top);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(k = this->m_Top; k > i; k--)
    {
        this->m_Index[k] = this->m_Index[k - 1];
    }
    this->m_Index[i] = node;
    this->m_Top++;
    return OK;
}

status_t CxFileInfoList::InsElem(int i, CxFileInfo *node)
{
    CxFileInfo *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->InsElemPtr(i,tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CxFileInfo* CxFileInfoList::RemoveElem(int index)
{
    CxFileInfo *p;
    int k;
    
    ASSERT(index >= 0 && index < this->m_Top);
    p = this->GetElem(index);

    for(k = index; k < this->m_Top-1; k++)
    {
        this->m_Index[k] = this->m_Index[k + 1];
    }
    this->m_Top --;
    this->m_Index[this->m_Top] = NULL;
    return p;
}

status_t CxFileInfoList::DelElem(int i)
{
    CxFileInfo *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CxFileInfoList::InsOrderedPtr(CxFileInfo *node, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,node);
}

status_t CxFileInfoList::InsOrdered(CxFileInfo *node, int order,int unique)
{
    int pos,find;
    ASSERT(node);
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElem(pos,node);
}

int CxFileInfoList::BSearchPos(CxFileInfo *node, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = this->CompNode(this->m_Index[mid],node);
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

status_t CxFileInfoList::Sort(int order)
{
    int i,len;
    CxFileInfoList tmp;

    len = this->GetLen();
    tmp.Init(len);

    for(i = 0; i < len; i++)
    {
        tmp.PushPtr(this->GetElem(i));
    }

    this->m_Top = 0;
    for(i = 0; i < len; i++)
    {
        this->InsOrderedPtr(tmp.PopPtr(),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.m_Index[i] = NULL;
    }

    return OK;
}

int CxFileInfoList::Comp(CxFileInfoList *stk)
{
    ASSERT(stk);
    if(this == stk)
        return 0;
    ASSERT(0);
    return 0;
}

status_t CxFileInfoList::Copy(CxFileInfoList *_p)
{
    int i;

    ASSERT(_p);
    this->Destroy();
    this->Init();
    for(i = 0; i < _p->GetLen(); i++)
    {
        CxFileInfo *p = _p->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
status_t CxFileInfoList::Print(CFileBase *_buf)
{
    for(int i = 0; i < this->m_Top; i++)
    {
        if(this->m_Index[i])
        {
            _buf->Log("[%d]={",i);
            _buf->IncLogLevel(1);
            this->m_Index[i]->Print(_buf);
            _buf->IncLogLevel(-1);
            _buf->Log("}");
        }
    }
    _buf->Log("top = %d",this->m_Top);
    _buf->Log("size = %d",this->m_Size);
    return OK;
}

status_t CxFileInfoList::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    
    fsize_t off;
    _bson->StartArray("_array_",&off);
    char name[256];
    for(int i = 0; i < this->GetLen(); i++)
    {
        CxFileInfo *p = this->GetElem(i);
        ASSERT(p);        
        sprintf(name,"%d",i);
        fsize_t _off;
        _bson->StartDocument(name,&_off);
        p->SaveBson(_bson);
        _bson->EndDocument(_off);
    }
    _bson->EndArray(off,this->GetLen());    
    return OK;
}

status_t CxFileInfoList::SaveBson(CMem *_mem)
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

status_t CxFileInfoList::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);

    CMiniBson doc;
    doc.Init();

    int len = 0;
    BSON_CHECK(_bson->GetArray("_array_",&doc,&len));
    this->Clear();
    
    doc.ResetPointer();
    for(int i = 0; i < len; i++)
    {
        CMiniBson sub_doc;
        sub_doc.Init();
        doc.GetDocument(NULL,&sub_doc);
        CxFileInfo tmp;
        tmp.Init();
        BSON_CHECK(tmp.LoadBson(&sub_doc));
        this->Push(&tmp);
    }

    return OK;
}

status_t CxFileInfoList::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CxFileInfoList::CopyNode(CxFileInfo *dst, CxFileInfo *src)
{
    return dst->Copy(src);
}

CxFileInfo* CxFileInfoList::CloneNode(CxFileInfo *node)
{
    CxFileInfo *tmp;
    NEW(tmp,CxFileInfo);
    tmp->Init();
    this->CopyNode(tmp,node);
    return tmp;
}

status_t CxFileInfoList::DelNode(CxFileInfo *node)
{
    DEL(node);
    return OK;
}

int CxFileInfoList::CompNode(CxFileInfo *node1, CxFileInfo *node2)
{
    ASSERT(node1 && node2);
    return node1->Comp(node2);
}
/////////////////////////////////////////////////////////////////////////////////////


