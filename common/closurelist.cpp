#include "closurelist.h"
#include "mem_tool.h"
#include "syslog.h"

CClosureList::CClosureList()
{
    this->InitBasic();  
}
CClosureList::~CClosureList()
{
    this->Destroy();
}
status_t CClosureList::InitBasic()
{
    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;   
    return OK;
}
status_t CClosureList::Init(int init_size)
{
    int i;
    this->InitBasic();  
    this->m_Size = init_size;
    MALLOC(this->m_Index,CClosure* ,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}
status_t CClosureList::Destroy()
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
bool CClosureList::IsEmpty()
{
    return this->m_Top <= 0;
}
bool CClosureList::IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t CClosureList::AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,CClosure*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t CClosureList::Push(CClosure *closure)
{
    CClosure *tmp;
    ASSERT(closure);
    tmp = this->CloneNode(closure);
    if(!this->PushPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CClosureList::PushPtr(CClosure *closure)
{
    ASSERT(closure);    
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = closure;
    this->m_Top++;
    return OK;
}

CClosure * CClosureList::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

status_t CClosureList::Pop(CClosure *closure)
{
    ASSERT(!this->IsEmpty());   
    this->m_Top--;
    this->CopyNode(closure,this->m_Index[this->m_Top]);
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
int CClosureList::GetLen()
{
    return this->m_Top;
}

int CClosureList::SearchPos(CClosure *closure)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],closure) == 0)
            return i;
    }
    return -1;
}

CClosure* CClosureList::Search(CClosure *closure)
{
    int pos = this->SearchPos(closure);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}
CClosure* CClosureList::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}
status_t CClosureList::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t CClosureList::Clear()
{
    while(this->DelTop());
    return OK;
}

CClosure* CClosureList::GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

CClosure* CClosureList::BSearchNode(CClosure *closure,int order)
{
    return this->GetElem(this->BSearch(closure,order));
}
int CClosureList::BSearch(CClosure *closure,int order)
{
    int find,pos;
    pos = this->BSearchPos(closure,order,&find);
    if(find) return pos;
    return -1;
}

status_t CClosureList::InsElemPtr(int i, CClosure *closure)
{
    int k;

    ASSERT(closure);
    ASSERT(i >= 0 && i <= this->m_Top);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(k = this->m_Top; k > i; k--)
    {
        this->m_Index[k] = this->m_Index[k - 1];
    }
    this->m_Index[i] = closure;
    this->m_Top++;
    return OK;
}

status_t CClosureList::InsElem(int i, CClosure *closure)
{
    CClosure *tmp;
    ASSERT(closure);
    tmp = this->CloneNode(closure);
    if(!this->InsElemPtr(i,tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CClosure* CClosureList::RemoveElem(int index)
{
    CClosure *p;
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

status_t CClosureList::DelElem(int i)
{
    CClosure *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CClosureList::InsOrderedPtr(CClosure *closure, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(closure,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,closure);
}

status_t CClosureList::InsOrdered(CClosure *closure, int order,int unique)
{
    int pos,find;
    ASSERT(closure);
    pos = this->BSearchPos(closure,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElem(pos,closure);
}

int CClosureList::BSearchPos(CClosure *closure, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = this->CompNode(this->m_Index[mid],closure);
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

status_t CClosureList::Sort(int order)
{
    int i,len;
    CClosureList tmp;

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
status_t CClosureList::Copy(CClosureList *stk)
{
    int i;

    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(i = 0; i < stk->GetLen(); i++)
    {
        CClosure *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
status_t CClosureList::Print(int _level,CFileBase *_buf)
{
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CClosureList::CopyNode(CClosure *dst, CClosure *src)
{
    return dst->Copy(src);
}

CClosure* CClosureList::CloneNode(CClosure *closure)
{
    CClosure *tmp;
    NEW(tmp,CClosure);
    tmp->Init();
    this->CopyNode(tmp,closure);
    return tmp;
}

status_t CClosureList::DelNode(CClosure *closure)
{
    DEL(closure);
    return OK;
}

int CClosureList::CompNode(CClosure *closure1, CClosure *closure2)
{
    ASSERT(closure1 && closure2);
    ASSERT(0);
    return 0;
}
