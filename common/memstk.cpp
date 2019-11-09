#include "memstk.h"
#include "mem_tool.h"
#include "syslog.h"

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
            this->mIndex[i] = NULL;
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
    this->Destroy();
    this->Init(stk->GetLen());
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

#if _UNICODE_
status_t CMemStk::PushW(const wchar_t *str)
{
    CMem mem(str);
    return this->Push(&mem);
}
#endif


