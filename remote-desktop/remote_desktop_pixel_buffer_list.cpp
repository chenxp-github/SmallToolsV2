#include "remote_desktop_pixel_buffer_list.h"
#include "mem_tool.h"
#include "sys_log.h"


CRemoteDesktopPixelBufferList::CRemoteDesktopPixelBufferList()
{
    this->InitBasic();  
}
CRemoteDesktopPixelBufferList::~CRemoteDesktopPixelBufferList()
{
    this->Destroy();
}
status_t CRemoteDesktopPixelBufferList::InitBasic()
{
    WEAK_REF_CLEAR();

    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;    
    return OK;
}
status_t CRemoteDesktopPixelBufferList::Init(int init_size)
{
    int i;
    this->InitBasic();  

    this->m_Size = init_size;
    MALLOC(this->m_Index,CRemoteDesktopPixelBuffer* ,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}

status_t CRemoteDesktopPixelBufferList::Destroy()
{
    int i;
    WEAK_REF_DESTROY();

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
bool CRemoteDesktopPixelBufferList::IsEmpty()
{
    return this->m_Top <= 0;
}
bool CRemoteDesktopPixelBufferList::IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t CRemoteDesktopPixelBufferList::AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,CRemoteDesktopPixelBuffer*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t CRemoteDesktopPixelBufferList::Push(CRemoteDesktopPixelBuffer *node)
{
    CRemoteDesktopPixelBuffer *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->PushPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CRemoteDesktopPixelBufferList::PushPtr(CRemoteDesktopPixelBuffer *node)
{
    ASSERT(node);    
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = node;
    this->m_Top++;
    return OK;
}

CRemoteDesktopPixelBuffer * CRemoteDesktopPixelBufferList::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

status_t CRemoteDesktopPixelBufferList::Pop(CRemoteDesktopPixelBuffer *node)
{
    ASSERT(!this->IsEmpty());    
    this->m_Top--;
    this->CopyNode(node,this->m_Index[this->m_Top]);
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
int CRemoteDesktopPixelBufferList::GetLen()
{
    return this->m_Top;
}

int CRemoteDesktopPixelBufferList::SearchPos(CRemoteDesktopPixelBuffer *node)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],node) == 0)
            return i;
    }
    return -1;
}

CRemoteDesktopPixelBuffer* CRemoteDesktopPixelBufferList::Search(CRemoteDesktopPixelBuffer *node)
{
    int pos = this->SearchPos(node);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}
CRemoteDesktopPixelBuffer* CRemoteDesktopPixelBufferList::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}
status_t CRemoteDesktopPixelBufferList::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t CRemoteDesktopPixelBufferList::Clear()
{
    while(this->DelTop());
    return OK;
}

CRemoteDesktopPixelBuffer* CRemoteDesktopPixelBufferList::GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

CRemoteDesktopPixelBuffer* CRemoteDesktopPixelBufferList::BSearchNode(CRemoteDesktopPixelBuffer *node,int order)
{
    return this->GetElem(this->BSearch(node,order));
}
int CRemoteDesktopPixelBufferList::BSearch(CRemoteDesktopPixelBuffer *node,int order)
{
    int find,pos;
    pos = this->BSearchPos(node,order,&find);
    if(find) return pos;
    return -1;
}

status_t CRemoteDesktopPixelBufferList::InsElemPtr(int i, CRemoteDesktopPixelBuffer *node)
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

status_t CRemoteDesktopPixelBufferList::InsElem(int i, CRemoteDesktopPixelBuffer *node)
{
    CRemoteDesktopPixelBuffer *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->InsElemPtr(i,tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CRemoteDesktopPixelBuffer* CRemoteDesktopPixelBufferList::RemoveElem(int index)
{
    CRemoteDesktopPixelBuffer *p;
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

status_t CRemoteDesktopPixelBufferList::DelElem(int i)
{
    CRemoteDesktopPixelBuffer *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CRemoteDesktopPixelBufferList::InsOrderedPtr(CRemoteDesktopPixelBuffer *node, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,node);
}

status_t CRemoteDesktopPixelBufferList::InsOrdered(CRemoteDesktopPixelBuffer *node, int order,int unique)
{
    int pos,find;
    ASSERT(node);
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElem(pos,node);
}

int CRemoteDesktopPixelBufferList::BSearchPos(CRemoteDesktopPixelBuffer *node, int order, int *find_flag)
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

status_t CRemoteDesktopPixelBufferList::Sort(int order)
{
    int i,len;
    CRemoteDesktopPixelBufferList tmp;

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

int CRemoteDesktopPixelBufferList::Comp(CRemoteDesktopPixelBufferList *stk)
{
    ASSERT(stk);
    if(this == stk)
        return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopPixelBufferList::Copy(CRemoteDesktopPixelBufferList *_p)
{
    int i;

    ASSERT(_p);
    this->Clear();
    for(i = 0; i < _p->GetLen(); i++)
    {
        CRemoteDesktopPixelBuffer *p = _p->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
status_t CRemoteDesktopPixelBufferList::Print(CFileBase *_buf)
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

status_t CRemoteDesktopPixelBufferList::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    
    fsize_t off;
    _bson->StartArray("_array_",&off);
    char name[256];
    for(int i = 0; i < this->GetLen(); i++)
    {
        CRemoteDesktopPixelBuffer *p = this->GetElem(i);
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

status_t CRemoteDesktopPixelBufferList::SaveBson(CMem *_mem)
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

status_t CRemoteDesktopPixelBufferList::LoadBson(CMiniBson *_bson)
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
        CRemoteDesktopPixelBuffer tmp;
        tmp.Init();
        BSON_CHECK(tmp.LoadBson(&sub_doc));
        this->Push(&tmp);
    }

    return OK;
}

status_t CRemoteDesktopPixelBufferList::LoadBson(CFileBase *_file)
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
status_t CRemoteDesktopPixelBufferList::CopyNode(CRemoteDesktopPixelBuffer *dst, CRemoteDesktopPixelBuffer *src)
{
    return dst->Copy(src);
}

CRemoteDesktopPixelBuffer* CRemoteDesktopPixelBufferList::CloneNode(CRemoteDesktopPixelBuffer *node)
{
    CRemoteDesktopPixelBuffer *tmp;
    NEW(tmp,CRemoteDesktopPixelBuffer);
    tmp->Init();
    this->CopyNode(tmp,node);
    return tmp;
}

status_t CRemoteDesktopPixelBufferList::DelNode(CRemoteDesktopPixelBuffer *node)
{
    DEL(node);
    return OK;
}

int CRemoteDesktopPixelBufferList::CompNode(CRemoteDesktopPixelBuffer *node1, CRemoteDesktopPixelBuffer *node2)
{
    ASSERT(node1 && node2);
    return node1->Comp(node2);
}
/////////////////////////////////////////////////////////////////////////////////////

status_t CRemoteDesktopPixelBufferList::SaveAllTiles(const char *prefix)
{
	ASSERT(prefix);

	for(int i = 0; i < this->GetLen(); i++)
	{
		CRemoteDesktopPixelBuffer *pbuf = this->GetElem(i);
		LOCAL_MEM(fn);
		fn.Printf("%s%04d.png",prefix,i);
		pbuf->SaveToXimage(fn.CStr());
	}
	return OK;
}

size_t CRemoteDesktopPixelBufferList::GetTotalPixelDataSize()
{
	size_t total = 0;
	for(int i = 0; i < this->GetLen(); i++)
	{
		CRemoteDesktopPixelBuffer *pbuf = this->GetElem(i);
		total += pbuf->GetPixelDataSize();
	}
	return total;
}

status_t CRemoteDesktopPixelBufferList::CompressAll()
{
	for(int i = 0; i < this->GetLen(); i++)
	{
		CRemoteDesktopPixelBuffer *pbuf = this->GetElem(i);
		pbuf->CompressData();
	}
	return OK;
}

status_t CRemoteDesktopPixelBufferList::DecompressAll()
{
	for(int i = 0; i < this->GetLen(); i++)
	{
		CRemoteDesktopPixelBuffer *pbuf = this->GetElem(i);
		pbuf->DecompressData();
	}
	return OK;
}

status_t CRemoteDesktopPixelBufferList::CopySingleScreen(CRemoteDesktopPixelBuffer *screen)
{
	ASSERT(screen);
	this->Clear();
	CRemoteDesktopPixelBuffer *p;
	NEW(p,CRemoteDesktopPixelBuffer);
	p->Init();
	p->Copy(screen);
	return this->PushPtr(p);
}

