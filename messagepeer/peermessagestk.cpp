#include "peermessagestk.h"
#include "mem_tool.h"
#include "syslog.h"

CPeerMessageStk::CPeerMessageStk()
{
    this->InitBasic();
}
CPeerMessageStk::~CPeerMessageStk()
{
    this->Destroy();
}
status_t CPeerMessageStk::InitBasic()
{
    this->mIndex = NULL;
    this->mTop = 0;
    this->mSize = 0;
    this->mExistTable = NULL;
    return OK;
}
status_t CPeerMessageStk::Init(int init_size)
{
    int i;
    this->InitBasic();
    this->mSize = init_size;
    MALLOC(this->mIndex,CPeerMessage * ,this->mSize);
    for(i = 0; i < this->mSize; i++)
        this->mIndex[i] = NULL;

    NEW(this->mExistTable,CPtrHashTable);
    this->mExistTable->Init(init_size*2);

    return OK;
}
status_t CPeerMessageStk::Destroy()
{
    int i;
    if(this->mIndex == NULL)
        return ERROR;
    DEL(this->mExistTable);
    for(i = 0; i < this->mTop; i++)
    {
        DelNode(this->mIndex[i]);
    }
    FREE(this->mIndex);
    this->InitBasic();
    return OK;
}
bool CPeerMessageStk::IsEmpty()
{
    return this->mTop <= 0;
}
bool CPeerMessageStk::IsFull()
{
    return this->mTop >= this->mSize;
}

status_t CPeerMessageStk::AutoResize()
{
    return OK;
}

status_t CPeerMessageStk::Push(CPeerMessage *peermessage)
{
    ASSERT(peermessage);
    CPeerMessage *tmp = this->CloneNode(peermessage);
    if(!this->PushPtr(tmp))
    {
        DelNode(tmp);
        return ERROR;
    }

    return OK;
}

status_t CPeerMessageStk::PushPtr(CPeerMessage *peermessage)
{
    ASSERT(peermessage);
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mIndex[this->mTop] = peermessage;
    this->mTop++;
    this->mExistTable->Put(peermessage);
    return OK;
}

CPeerMessage * CPeerMessageStk::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->mTop--;
    this->mExistTable->Del(this->mIndex[this->mTop]);
    return this->mIndex[this->mTop];
}

status_t CPeerMessageStk::Pop(CPeerMessage *peermessage)
{
    ASSERT(!this->IsEmpty());
    this->mTop--;
    this->mExistTable->Del(this->mIndex[this->mTop]);
    CopyNode(peermessage,mIndex[mTop]);
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPeerMessageStk::Print()
{
    int i;
    for(i = 0; i <this->mTop; i++)
    {
        this->mIndex[i]->Print();
        syslog_printf(",");
    }
    syslog_printf("mSize=%d",this->mSize);
    syslog_printf("mTop=%d",this->mTop);
    syslog_printf("bottom=%d",0);
    return OK;
}
int CPeerMessageStk::GetLen()
{
    return this->mTop;
}

int CPeerMessageStk::SearchPos(CPeerMessage *peermessage)
{
    int i;
    for(i=0;i<this->mTop;i++)
    {
        if(CompNode(mIndex[i],peermessage) == 0)
            return i;
    }
    return -1;
}

CPeerMessage * CPeerMessageStk::Search(CPeerMessage *peermessage)
{
    int pos = this->SearchPos(peermessage);
    if(pos >= 0 && pos < this->mTop)
        return this->mIndex[pos];;
    return NULL;
}
CPeerMessage * CPeerMessageStk::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->mIndex[this->mTop - 1];
}
status_t CPeerMessageStk::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->mTop--;
    this->mExistTable->Del(this->mIndex[this->mTop]);
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CPeerMessageStk::Clear()
{
    while(this->DelTop());
    return OK;
}

CPeerMessage * CPeerMessageStk::GetElem(int mIndex)
{
    if(mIndex < 0 || mIndex >= this->mTop)
        return NULL;
    return this->mIndex[mIndex];
}

CPeerMessage * CPeerMessageStk::BSearchNode(CPeerMessage *peermessage,int order)
{
    return this->GetElem(this->BSearch(peermessage,order));
}
int CPeerMessageStk::BSearch(CPeerMessage *peermessage,int order)
{
    int find,pos;
    pos = this->BSearchPos(peermessage,order,&find);
    if(find) return pos;
    return -1;
}

status_t CPeerMessageStk::InsertElemPtr(int i, CPeerMessage *peermessage)
{
    ASSERT(peermessage);
    ASSERT(i >= 0 && i <= this->mTop);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(int k = this->mTop; k > i; k--)
    {
        this->mIndex[k] = this->mIndex[k - 1];
    }
    this->mIndex[i] = peermessage;
    this->mTop++;
    this->mExistTable->Put(peermessage);
    return OK;
}

status_t CPeerMessageStk::InsertElem(int i, CPeerMessage *peermessage)
{
    ASSERT(peermessage);
    CPeerMessage *tmp = this->CloneNode(peermessage);
    if(!this->InsertElemPtr(i,tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CPeerMessage * CPeerMessageStk::RemoveElem(int index)
{
    CPeerMessage *p;

    ASSERT(index >= 0 && index < this->mTop);
    p = this->GetElem(index);

    for(int k = index; k < this->mTop-1; k++)
    {
        this->mIndex[k] = this->mIndex[k + 1];
    }
    this->mTop --;
    this->mExistTable->Del(p);
    this->mIndex[mTop] = NULL;
    return p;
}

status_t CPeerMessageStk::DelElem(int i)
{
    CPeerMessage *p = this->RemoveElem(i);
    if(p != NULL)
    {
        DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CPeerMessageStk::InsOrderedPtr(CPeerMessage *peermessage, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(peermessage,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElemPtr(pos,peermessage);
}

status_t CPeerMessageStk::InsOrdered(CPeerMessage *peermessage, int order,int unique)
{
    ASSERT(peermessage);
    int pos,find;
    pos = this->BSearchPos(peermessage,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElem(pos,peermessage);
}

int CPeerMessageStk::BSearchPos(CPeerMessage *peermessage, int order, int *find_flag)
{
    int low,high,mid,comp;

    low = 0;
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = CompNode(mIndex[mid],peermessage);
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

status_t CPeerMessageStk::Sort(int order)
{
    int i,len;
    CPeerMessageStk tmp;

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
status_t CPeerMessageStk::Copy(CPeerMessageStk *stk)
{
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(int i = 0; i < stk->GetLen(); i++)
    {
        CPeerMessage *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CPeerMessageStk::CopyNode(CPeerMessage *dst, CPeerMessage *src)
{
    return dst->Copy(src);
}

CPeerMessage * CPeerMessageStk::CloneNode(CPeerMessage *peermessage)
{
    CPeerMessage *tmp;
    NEW(tmp,CPeerMessage);
    tmp->Init();
    CopyNode(tmp,peermessage);
    return tmp;
}

status_t CPeerMessageStk::DelNode(CPeerMessage *peermessage)
{
    if(peermessage)
        peermessage->Release();
    return OK;
}

int CPeerMessageStk::CompNode(CPeerMessage *peermessage1, CPeerMessage *peermessage2)
{
    ASSERT(peermessage1 && peermessage2);
    return peermessage1->Comp(peermessage2);
}

bool CPeerMessageStk::IsPtrExist(CPeerMessage *msg)
{
    return this->mExistTable->IsExist(msg);
}
