#include "peerproxystk.h"
#include "mem_tool.h"
#include "syslog.h"

CPeerProxyStk::CPeerProxyStk()
{
    this->InitBasic();
}
CPeerProxyStk::~CPeerProxyStk()
{
    this->Destroy();
}
status_t CPeerProxyStk::InitBasic()
{
    this->mIndex = NULL;
    this->mTop = 0;;
    this->mSize = 0;
    return OK;
}
status_t CPeerProxyStk::Init(int init_size)
{
    int i;
    this->InitBasic();
    this->mSize = init_size;
    MALLOC(this->mIndex,CPeerProxy * ,this->mSize);
    for(i = 0; i < this->mSize; i++)
        this->mIndex[i] = NULL;
    return OK;
}
status_t CPeerProxyStk::Destroy()
{
    int i;
    if(this->mIndex == NULL)
        return ERROR;
    for(i = 0; i < this->mTop; i++)
    {
        DelNode(this->mIndex[i]);
    }
    FREE(this->mIndex);
    this->InitBasic();
    return OK;
}
bool CPeerProxyStk::IsEmpty()
{
    return this->mTop <= 0;
}
bool CPeerProxyStk::IsFull()
{
    return this->mTop >= this->mSize;
}

status_t CPeerProxyStk::AutoResize()
{
    if(this->IsFull())
    {
        REALLOC(this->mIndex,CPeerProxy*,this->mSize,this->mSize*2);
        this->mSize *= 2;
        for(int i = this->mTop; i < this->mSize; i++)
        {
            this->mIndex[i] = NULL;
        }
    }
    return OK;
}

status_t CPeerProxyStk::Push(CPeerProxy *peerproxy)
{
    ASSERT(peerproxy);
    CPeerProxy *tmp = this->CloneNode(peerproxy);
    if(!this->PushPtr(tmp))
    {
        DelNode(tmp);
        return ERROR;
    }

    return OK;
}

status_t CPeerProxyStk::PushPtr(CPeerProxy *peerproxy)
{
    ASSERT(peerproxy);
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mIndex[this->mTop] = peerproxy;
    this->mTop++;
    return OK;
}

CPeerProxy * CPeerProxyStk::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->mTop--;
    return this->mIndex[this->mTop];
}

status_t CPeerProxyStk::Pop(CPeerProxy *peerproxy)
{
    ASSERT(!this->IsEmpty());
    this->mTop--;
    CopyNode(peerproxy,mIndex[mTop]);
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPeerProxyStk::Print()
{
    int i;
    for(i = 0; i <this->mTop; i++)
    {
        this->mIndex[i]->Print();
        XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,",");
    }
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,"mSize=%d",this->mSize);
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,"mTop=%d",this->mTop);
    XLOG(LOG_MODULE_MESSAGEPEER,LOG_LEVEL_INFO,"bottom=%d",0);
    return OK;
}
int CPeerProxyStk::GetLen()
{
    return this->mTop;
}

int CPeerProxyStk::SearchPos(CPeerProxy *peerproxy)
{
    int i;
    for(i=0;i<this->mTop;i++)
    {
        if(CompNode(mIndex[i],peerproxy) == 0)
            return i;
    }
    return -1;
}

CPeerProxy * CPeerProxyStk::Search(CPeerProxy *peerproxy)
{
    int pos = this->SearchPos(peerproxy);
    if(pos >= 0 && pos < this->mTop)
        return this->mIndex[pos];;
    return NULL;
}
CPeerProxy * CPeerProxyStk::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->mIndex[this->mTop - 1];
}
status_t CPeerProxyStk::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->mTop--;
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPeerProxyStk::Clear()
{
    while(this->DelTop());
    return OK;
}

CPeerProxy * CPeerProxyStk::GetElem(int mIndex)
{
    if(mIndex < 0 || mIndex >= this->mTop)
        return NULL;
    return this->mIndex[mIndex];
}

CPeerProxy * CPeerProxyStk::BSearchNode(CPeerProxy *peerproxy,int order)
{
    return this->GetElem(this->BSearch(peerproxy,order));
}
int CPeerProxyStk::BSearch(CPeerProxy *peerproxy,int order)
{
    int find,pos;
    pos = this->BSearchPos(peerproxy,order,&find);
    if(find) return pos;
    return -1;
}

status_t CPeerProxyStk::InsertElemPtr(int i, CPeerProxy *peerproxy)
{
    ASSERT(peerproxy);
    ASSERT(i >= 0 && i <= this->mTop);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(int k = this->mTop; k > i; k--)
    {
        this->mIndex[k] = this->mIndex[k - 1];
    }
    this->mIndex[i] = peerproxy;
    this->mTop++;
    return OK;
}

status_t CPeerProxyStk::InsertElem(int i, CPeerProxy *peerproxy)
{
    ASSERT(peerproxy);
    CPeerProxy *tmp = this->CloneNode(peerproxy);
    if(!this->InsertElemPtr(i,tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CPeerProxy * CPeerProxyStk::RemoveElem(int index)
{
    CPeerProxy *p;

    ASSERT(index >= 0 && index < this->mTop);
    p = this->GetElem(index);

    for(int k = index; k < this->mTop-1; k++)
    {
        this->mIndex[k] = this->mIndex[k + 1];
    }
    this->mTop --;
    this->mIndex[mTop] = NULL;
    return p;
}

status_t CPeerProxyStk::DelElem(int i)
{
    CPeerProxy *p = this->RemoveElem(i);
    if(p != NULL)
    {
        DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CPeerProxyStk::InsOrderedPtr(CPeerProxy *peerproxy, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(peerproxy,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElemPtr(pos,peerproxy);
}

status_t CPeerProxyStk::InsOrdered(CPeerProxy *peerproxy, int order,int unique)
{
    ASSERT(peerproxy);
    int pos,find;
    pos = this->BSearchPos(peerproxy,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElem(pos,peerproxy);
}

int CPeerProxyStk::BSearchPos(CPeerProxy *peerproxy, int order, int *find_flag)
{
    int low,high,mid,comp;

    low = 0;
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = CompNode(mIndex[mid],peerproxy);
        if(comp == 0)
        {
            *find_flag = TRUE;
            return mid;
        }

        if(order != 0) comp = -comp;

        if(comp > 0)
            high = mid-1;
        else
            low = mid+1;
    }
    *find_flag = FALSE;
    return low;
}

status_t CPeerProxyStk::Sort(int order)
{
    int i,len;
    CPeerProxyStk tmp;

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
status_t CPeerProxyStk::Copy(CPeerProxyStk *stk)
{
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(int i = 0; i < stk->GetLen(); i++)
    {
        CPeerProxy *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CPeerProxyStk::CopyNode(CPeerProxy *dst, CPeerProxy *src)
{
    return dst->Copy(src);
}

CPeerProxy * CPeerProxyStk::CloneNode(CPeerProxy *peerproxy)
{
    ASSERT(0);
    return NULL;
}

status_t CPeerProxyStk::DelNode(CPeerProxy *peerproxy)
{
    DEL(peerproxy);
    return OK;
}

int CPeerProxyStk::CompNode(CPeerProxy *peerproxy1, CPeerProxy *peerproxy2)
{
    ASSERT(peerproxy1 && peerproxy2);
    return peerproxy1->Comp(peerproxy2);
}
