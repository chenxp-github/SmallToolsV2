#include "ptrhashtable.h"
#include "syslog.h"
#include "mem_tool.h"

CPtrHashTable::CPtrHashTable()
{
    this->InitBasic();
}
CPtrHashTable::~CPtrHashTable()
{
    this->Destroy();
}
status_t CPtrHashTable::InitBasic()
{
    this->mCapacity = 0;
    this->mData = 0;
    this->mSize = 0;
    return OK;
}
status_t CPtrHashTable::Init(int capacity)
{
    this->InitBasic();
    this->mCapacity = capacity;
    MALLOC(this->mData,CPtrHashNode*,capacity);
    memset(this->mData,0,capacity*sizeof(CPtrHashNode*));
    return OK;
}
status_t CPtrHashTable::Destroy()
{
    int i;
    CPtrHashNode *q,*p;

    if(this->mData == NULL)
        return OK;

    for(i = 0; i < this->mCapacity; i++)
    {
        p  = this->mData[i];
        while(p)
        {
            q = p->next;
            this->DelNode(p);
            p = q;          
        }           
    }
    FREE(this->mData);
    this->InitBasic();
    return OK;
}

status_t CPtrHashTable::PutPtr(CPtrHashNode *ptr)
{
    int code;
    CPtrHashNode *p;

    ASSERT(ptr);

    if(this->get(ptr) != NULL)
        return ERROR;

    code = this->HashCode(ptr);
    ASSERT(code >= 0 && code < this->mCapacity);
    p = this->mData[code];
    if (p == NULL)
    {
        this->mData[code] = ptr;
        ptr->next = NULL;
    }
    else
    {
        while(p->next)
        {
            p = p->next;
        }
        p->next = ptr;
        ptr->next = NULL;
    }
    this->mSize++;
    return OK;
}

status_t CPtrHashTable::put(CPtrHashNode *ptrhashnode)
{
    CPtrHashNode *tmp = this->CloneNode(ptrhashnode);
    if(!this->PutPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CPtrHashNode* CPtrHashTable::get(CPtrHashNode *key)
{
    int code;
    CPtrHashNode *p;

    ASSERT(key);

    code = this->HashCode(key);
    ASSERT(code >= 0 && code < this->mCapacity);
    p = this->mData[code];
    while(p)
    {
        if(this->Equals(p,key))
            return p;
        p = p->next;
    }
    return NULL;
}
status_t CPtrHashTable::EnumAll(CClosure *closure)
{
    int i,_contine;
    CPtrHashNode *pre,*p,*next;

    ASSERT(closure);
    
    for(i = 0; i < this->mCapacity; i++)
    {
        p = this->mData[i];
        if(p == NULL)continue;

        pre = p;
        p = p->next;
        while(p)
        {           
            next = p->next;
            closure->SetParamPointer(0,p);
            _contine = closure->Run();
            if(closure->GetParamPointer(0) == NULL)
            {
                pre->next = next;
                p = next;
                this->mSize --;
            }
            else
            {
                pre = p;
                p = next;
            }
            
            if(!_contine)
                goto end;
        }
        
        p = this->mData[i];
        next = p->next;
        closure->SetParamPointer(0,p);
        _contine = closure->Run();
        if(closure->GetParamPointer(0) == NULL)
        {
            this->mData[i] = next;
            this->mSize --;
        }
        if(!_contine)
            goto end;
    }
    
end:
    return OK;
}
CPtrHashNode* CPtrHashTable::Remove(CPtrHashNode *key)
{
    int code;
    CPtrHashNode *pre,*p;

    ASSERT(key);
    code = this->HashCode(key);
    if(code < 0 || code >= this->mCapacity)
        return NULL;
    p = pre = this->mData[code];
    if(p != NULL)
    {
        if(this->Equals(p,key))
        {
            this->mData[code] = p->next;
            this->mSize --;
            return p;
        }
        else
        {
            while(p)
            {
                pre = p;
                p = p->next;
                if(this->Equals(p,key))
                {
                    pre->next = p->next;
                    this->mSize --;
                    return 0;
                }
            }
        }
    }

    return NULL;
}
status_t CPtrHashTable::del(CPtrHashNode *key)
{
    CPtrHashNode *p = this->Remove(key);
    if(p != NULL)
    {
        return this->DelNode(p);
    }
    return ERROR;
}
int CPtrHashTable::GetSize()
{
    return this->mSize;
}
bool CPtrHashTable::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CPtrHashTable::Print(CFileBase *_buf)
{
    int i;
    int collision = 0;
    int maxLength = 0;

    for(i = 0; i < this->mCapacity; i++)
    {
        CPtrHashNode *p = this->mData[i];
        if(p != NULL)
        {
            int len = 0;
            collision--;
            while(p)
            {               
                p = p->next;
                len ++;
                collision++;
            }
            if(len > maxLength)
                maxLength = len;
        }
    }

    _buf->Log("capacity is %d\r\n", mCapacity);
    _buf->Log("total size is %d\r\n",mSize);
    _buf->Log("maximum linked list length is %d\r\n",maxLength);
    _buf->Log("total collison is %d\r\n",collision);
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
int CPtrHashTable::HashCode(CPtrHashNode *ptrhashnode)
{
    ASSERT(ptrhashnode);
    //Bob Jenkins' 32 bit integer hash function
    uint32_t a = (uint32_t)ptrhashnode->GetIntValue();
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16); 
    return a % mCapacity;
}
bool CPtrHashTable::Equals(CPtrHashNode *ptrhashnode1, CPtrHashNode *ptrhashnode2)
{
    ASSERT(ptrhashnode1 && ptrhashnode2);
    return ptrhashnode1->Comp(ptrhashnode2) == 0;
}

CPtrHashNode* CPtrHashTable::CloneNode(CPtrHashNode *ptrhashnode)
{
    CPtrHashNode *ptr;
    NEW(ptr,CPtrHashNode);
    ptr->Init();
    ptr->Copy(ptrhashnode);
    return ptr;
}

status_t CPtrHashTable::DelNode(CPtrHashNode *ptrhashnode)
{
    DEL(ptrhashnode);
    return OK;
}

status_t CPtrHashTable::Put(void *ptr)
{
    CPtrHashNode tmp;
    tmp.Init();
    tmp.Set(ptr);
    return this->put(&tmp);
}

status_t CPtrHashTable::Del(void *ptr)
{
    CPtrHashNode tmp;
    tmp.Init();
    tmp.Set(ptr);
    return this->del(&tmp);
}

bool CPtrHashTable::IsExist(void *ptr)
{
    CPtrHashNode tmp;
    tmp.Init();
    tmp.Set(ptr);
    return this->get(&tmp) != NULL;
}
