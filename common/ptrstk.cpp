#include "ptrstk.h"
#include "mem_tool.h"
#include "syslog.h"

CPtrStk::CPtrStk()
{
    this->InitBasic();
}
CPtrStk::~CPtrStk()
{
    this->Destroy();
}

status_t CPtrStk::InitBasic()
{
    CUserFunc::InitBasic();

    this->mData = NULL;
    this->mTop = 0;
    this->mSize = 0;
    this->mUseFree = false;
    return OK;
}
status_t CPtrStk::Init(int init_size)
{
    this->InitBasic();
    CUserFunc::Init();
    MALLOC(this->mData,any_ptr,init_size);
    this->mSize = init_size;
    return OK;
}
status_t CPtrStk::Destroy()
{
    this->DestroyAllNodes();
    FREE(this->mData);
    CUserFunc::Destroy();
    this->InitBasic();
    return OK;
}
status_t CPtrStk::Clear()
{
    this->DestroyAllNodes();
    this->mTop = 0;
    return OK;
}
bool CPtrStk::IsEmpty()
{
    return this->mTop <= 0;
}
bool CPtrStk::IsFull()
{
    return this->mTop >= this->mSize;
}
int CPtrStk::GetLen()
{
    return this->mTop;
}
status_t CPtrStk::AutoResize()
{
    if(this->IsFull())
    {
        REALLOC(this->mData,any_ptr,this->mSize,this->mSize * 2);
        this->mSize *= 2;
    }
    return OK;
}

status_t CPtrStk::Push(any_ptr node)
{   
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mData[mTop]= node;
    this->mTop ++;  
    return OK;
}

any_ptr CPtrStk::Pop()
{
    ASSERT(!this->IsEmpty());
    this->mTop --;
    return this->mData[mTop];
}

any_ptr CPtrStk::GetElem(int index)
{
    ASSERT(index >= 0 && index <= this->mTop);  
    return this->mData[index];
}

int CPtrStk::BSearchPos(any_ptr node, int order, int *find_flag)
{
    int low,high,mid;
    int comp;

    low = 0; high=this->GetLen() - 1;
    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = CompNode(this->mData[mid],node);
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
status_t CPtrStk::InsertElem(int index, any_ptr node)
{
    ASSERT(index >= 0 && index <= this->mTop);  

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(int k = this->mTop; k > index; k--)
    {
        this->mData[k] = this->mData[k - 1];
    }
    this->mData[index] = node;
    this->mTop++;
    return OK;
}
status_t CPtrStk::DelElem(int index)
{
    ASSERT(index >= 0 && index < this->mTop);
    for(int k = index; k < this->mTop-1; k++)
    {
        this->mData[k] = this->mData[k + 1];
    }
    this->mTop --;
    return OK;
}
status_t CPtrStk::InsOrdered(any_ptr node, int order, int unique)
{
    int pos,find;
    
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    
    return this->InsertElem(pos,node);
}
int CPtrStk::SearchPos(any_ptr node)
{
    int i;
    for(i=0;i<this->mTop;i++)
    {
        if(CompNode(this->mData[i],node) == 0)
            return i;
    }
    return -1;
}
any_ptr CPtrStk::GetTop()
{
    ASSERT(!this->IsEmpty());
    return this->mData[mTop - 1];
}
status_t CPtrStk::SetElem(int index, any_ptr node)
{
    ASSERT(index >= 0 && index < this->mTop);
    this->mData[index] = node;
    return OK;
}
status_t CPtrStk::Sort(int order)
{
    int i;
    CPtrStk tmp;

    tmp.Init(this->GetLen());
    for(i = 0; i < this->GetLen(); i++)
    {
        tmp.Push(this->GetElem(i));
    }
    
    this->mTop = 0;
    for(i = 0; i < tmp.GetLen(); i++)
    {
        this->InsOrdered(tmp.GetElem(i),order,0);
    }
    return OK;
}
status_t CPtrStk::Copy(CPtrStk *stk)
{
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(int i = 0; i < stk->GetLen(); i++)
    {
        this->Push(stk->GetElem(i));
    }
    return OK;
}
/***************************************************/
status_t CPtrStk::Print(CFileBase *_buf)
{
    ASSERT(_buf);

    for(int i = 0; i< mTop; i++)
    {
        if(IsUserFunc(HOW_TO_PRINT))
        {
            this->Callback()->SetParamPointer(1,GetElem(i));
            this->Callback()->SetParamPointer(2,_buf);
            this->Callback()->Run(HOW_TO_PRINT);
        }
        else
        {
            _buf->Log("%p",GetElem(i));
        }

        _buf->Printf(",");
    }
    return TRUE;
}

int CPtrStk::CompNode(any_ptr node1, any_ptr node2)
{
    if(IsUserFunc(HOW_TO_COMP))
    {
        Callback()->SetParamPointer(1,node1);
        Callback()->SetParamPointer(2,node2);   
        return Callback()->Run(HOW_TO_COMP);
    }
    return ((int_ptr_t)node1) - ((int_ptr_t)node2);
}

status_t CPtrStk::DestroyAllNodes()
{   
    if(this->mData)
    {
        for(int i = 0; i < this->GetLen(); i++)
        {
            this->free_mem(i);
        }
    }
    this->mTop = 0;
    return OK;
}

status_t CPtrStk::UseFreeWhenDelete(bool use_free)
{
    this->mUseFree = use_free;
    return OK;
}

status_t CPtrStk::free_mem(int index)
{
    if(!mUseFree)
    {

        if(IsUserFunc(HOW_TO_DEL))
        {
            Callback()->SetParamPointer(1,GetElem(index));
            return Callback()->Run(HOW_TO_DEL);
        }
    }
    else
    {
        FREE(this->mData[index]);
    }

    return OK;
}

int_ptr_t CPtrStk::GetInt(int index)
{
    return (int_ptr_t)this->GetElem(index);
}

status_t CPtrStk::PushInt(int_ptr_t i)
{
    return this->Push((any_ptr)i);
}
