#ifndef __COMMONARRAY_H
#define __COMMONARRAY_H

#include "filebase.h"
#include "syslog.h"
#include "mem_tool.h"

template <class T>
class CCommonArray{
    typedef status_t (*HOW_TO_COMP)(T* t1,T* t2);
public:
    WEAK_REF_ID_DEFINE();
private:
    T **m_Index;
    int m_Top;
    int m_Size;
    bool m_IsWeakTable;
    HOW_TO_COMP how_to_comp;
public:     
CCommonArray()
{
    this->InitBasic();  
}
~CCommonArray()
{
    this->Destroy();
}
status_t InitBasic()
{
    WEAK_REF_ID_CLEAR();
    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;   
    this->how_to_comp = NULL;
    this->m_IsWeakTable = false;
    return OK;
}
status_t Init(int init_size=256,bool is_weak_table=false)
{
    int i;
    this->InitBasic();  
    WEAK_REF_ID_INIT();
    this->m_IsWeakTable = is_weak_table;
    this->m_Size = init_size;
    MALLOC(this->m_Index,T* ,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}
status_t Destroy()
{
    int i;    
    if(this->m_Index == NULL)
        return ERROR;
    if(!m_IsWeakTable)
    {
        for(i = 0; i < this->m_Top; i++)
        {        
            this->DelNode(this->m_Index[i]);
        }
    }
    FREE(this->m_Index);
    this->InitBasic();  
    return OK;
}
bool IsEmpty()
{
    return this->m_Top <= 0;
}
bool IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,T*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t PushPtr(T *t)
{
    ASSERT(t);  
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = t;
    this->m_Top++;
    return OK;
}

T * PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

int GetLen()
{
    return this->m_Top;
}

int SearchPos(T *t)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],t) == 0)
            return i;
    }
    return -1;
}

T* Search(T *t)
{
    int pos = this->SearchPos(t);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}
T* GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}
status_t DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t Clear()
{
    while(this->DelTop());
    return OK;
}

T* GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

T* BSearchNode(T *t,int order)
{
    return this->GetElem(this->BSearch(t,order));
}

int BSearch(T *t,int order)
{
    int find,pos;
    pos = this->BSearchPos(t,order,&find);
    if(find) return pos;
    return -1;
}

status_t InsElemPtr(int i, T *t)
{
    int k;

    ASSERT(t);
    ASSERT(i >= 0 && i <= this->m_Top);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(k = this->m_Top; k > i; k--)
    {
        this->m_Index[k] = this->m_Index[k - 1];
    }
    this->m_Index[i] = t;
    this->m_Top++;
    return OK;
}

T* RemoveElem(int index)
{
    T *p;
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

status_t DelElem(int i)
{
    T *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t InsOrderedPtr(T *t, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(t,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,t);
}

int BSearchPos(T *t, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = this->CompNode(this->m_Index[mid],t);
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

status_t Sort(int order)
{
    int i,len;
    CCommonArray tmp;

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

status_t Print(CFileBase *_buf)
{
    for(int i = 0; i < this->m_Top; i++)
    {
        if(this->m_Index[i])
        {
            _buf->Log("[%d]={",i);
            _buf->IncLogLevel(1);
            this->PrintNode(this->m_Index[i],_buf);
            _buf->IncLogLevel(-1);
            _buf->Log("}");
        }
    }
    _buf->Log("top = %d",this->m_Top);
    _buf->Log("size = %d",this->m_Size);
    return OK;
}

bool IsWeakTable()
{
    return m_IsWeakTable;
}

status_t DelNode(T *t)
{
    DEL(t);
    return OK;
}

status_t PrintNode(T *t,CFileBase *_buf)
{
    t->Print(_buf);
    return OK;
}

int CompNode(T *t1, T *t2)
{
    ASSERT(t1 && t2);
    
    if(how_to_comp)
    {
        return how_to_comp(t1,t2);
    }   
    return t1->Comp(t2);
}

void SetCompFunc(HOW_TO_COMP func)
{
    how_to_comp = func;
}

HOW_TO_COMP GetCompFunc()
{
    return how_to_comp;
}

};

#endif
