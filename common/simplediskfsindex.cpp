#include "simplediskfsindex.h"
#include "syslog.h"
#include "mem_tool.h"
#include "misc.h"

CSimpleDiskFsIndex::CSimpleDiskFsIndex()
{
    this->InitBasic();
}
CSimpleDiskFsIndex::~CSimpleDiskFsIndex()
{
    this->Destroy();
}
status_t CSimpleDiskFsIndex::InitBasic()
{
    this->m_Capacity = 0;
    this->m_Data = 0;
    this->m_Size = 0;
    return OK;
}
status_t CSimpleDiskFsIndex::Init(int capacity)
{
    this->InitBasic();
    this->m_Capacity = capacity;
    MALLOC(this->m_Data,CSimpleDiskFsEntry*,capacity);
    memset(this->m_Data,0,capacity*sizeof(CSimpleDiskFsEntry*));
    return OK;
}
status_t CSimpleDiskFsIndex::Destroy()
{
    int i;
    CSimpleDiskFsEntry *q,*p;

    if(this->m_Data == NULL)
        return OK;

    for(i = 0; i < this->m_Capacity; i++)
    {
        p  = this->m_Data[i];
        while(p)
        {
            q = p->__next;
            this->DelNode(p);
            p = q;          
        }           
    }
    FREE(this->m_Data);
    this->InitBasic();
    return OK;
}

status_t CSimpleDiskFsIndex::PutPtr(CSimpleDiskFsEntry *ptr)
{
    int code;
    CSimpleDiskFsEntry *p;

    ASSERT(ptr);

    if(this->Get(ptr) != NULL)
        return ERROR;

    code = this->HashCode(ptr);
    ASSERT(code >= 0 && code < this->m_Capacity);
    p = this->m_Data[code];
    if (p == NULL)
    {
        this->m_Data[code] = ptr;
        ptr->__next = NULL;
    }
    else
    {
        while(p->__next)
        {
            p = p->__next;
        }
        p->__next = ptr;
        ptr->__next = NULL;
    }
    this->m_Size++;
    return OK;
}

status_t CSimpleDiskFsIndex::Put(CSimpleDiskFsEntry *simplediskfsentry)
{
    CSimpleDiskFsEntry *tmp = this->CloneNode(simplediskfsentry);
    if(!this->PutPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CSimpleDiskFsEntry* CSimpleDiskFsIndex::Get(CSimpleDiskFsEntry *key)
{
    int code;
    CSimpleDiskFsEntry *p;

    ASSERT(key);

    code = this->HashCode(key);
    ASSERT(code >= 0 && code < this->m_Capacity);
    p = this->m_Data[code];
    while(p)
    {
        if(this->Equals(p,key))
            return p;
        p = p->__next;
    }
    return NULL;
}

status_t CSimpleDiskFsIndex::EnumAll(CClosure *closure)
{
    int i,_contine;
    CSimpleDiskFsEntry *pre,*p,*next;
    
    ASSERT(closure);
    
    for(i = 0; i < this->m_Capacity; i++)
    {
        p = this->m_Data[i];
        if(p == NULL)continue;
        
        pre = p;
        p = p->__next;
        while(p)
        {           
            next = p->__next;
            closure->SetParamPointer(0,p);
            closure->SetParamInt(1,p==m_Data[i]); //is first
            _contine = closure->Run();
            if(closure->GetParamPointer(0) == NULL)
            {
                pre->__next = next;
                p = next;
                this->m_Size --;
            }
            else
            {
                pre = p;
                p = next;
            }
            
            if(!_contine)
                goto end;
        }
        
        p = this->m_Data[i];
        next = p->__next;
        closure->SetParamPointer(0,p);
        closure->SetParamInt(1,p==m_Data[i]);
        _contine = closure->Run();
        if(closure->GetParamPointer(0) == NULL)
        {
            this->m_Data[i] = next;
            this->m_Size --;
        }
        if(!_contine)
            goto end;
    }
    
end:
    return OK;
}

CSimpleDiskFsEntry* CSimpleDiskFsIndex::Remove(CSimpleDiskFsEntry *key)
{
    int code;
    CSimpleDiskFsEntry *pre,*p;

    ASSERT(key);
    code = this->HashCode(key);
    if(code < 0 || code >= this->m_Capacity)
        return NULL;
    p = this->m_Data[code];
    if(p == NULL) return NULL;

    if(this->Equals(p,key))
    {
        this->m_Data[code] = p->__next;
        this->m_Size --;
        return p;
    }
    else
    {
        while(p)
        {
            pre = p;
            p = p->__next;
            if(p && this->Equals(p,key))
            {
                pre->__next = p->__next;
                this->m_Size --;
                return p;
            }
        }
    }

    return NULL;
}
status_t CSimpleDiskFsIndex::Del(CSimpleDiskFsEntry *key)
{
    CSimpleDiskFsEntry *p = this->Remove(key);
    if(p != NULL)
    {
        return this->DelNode(p);
    }
    return ERROR;
}
int CSimpleDiskFsIndex::GetSize()
{
    return this->m_Size;
}
int CSimpleDiskFsIndex::GetCapacity()
{
    return this->m_Capacity;
}
bool CSimpleDiskFsIndex::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CSimpleDiskFsIndex::DiscardAll()
{
    m_Data = NULL;
    return OK;
}
int CSimpleDiskFsIndex::HashCode(CSimpleDiskFsEntry *simplediskfsentry)
{
    return HashCode(simplediskfsentry,m_Capacity);
}
status_t CSimpleDiskFsIndex::Print(CFileBase *_buf)
{
    int i;
    int collision = 0;
    int maxLength = 0;

    for(i = 0; i < this->m_Capacity; i++)
    {
        CSimpleDiskFsEntry *p = this->m_Data[i];

        if(p != NULL)
        {
            _buf->Log("[%d]->",i);
            int len = 0;
            collision--;
            while(p)
            {
                p->Print(_buf);
                _buf->Log("->");
                p = p->__next;
                len ++;
                collision++;
            }
            if(len > maxLength)
                maxLength = len;
            _buf->Log("");
        }
    }

    _buf->Log("capacity is %d", m_Capacity);
    _buf->Log("total size is %d",m_Size);
    _buf->Log("maximum linked list length is %d",maxLength);
    _buf->Log("total collison is %d",collision);
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
int CSimpleDiskFsIndex::HashCode(CSimpleDiskFsEntry *simplediskfsentry,int capacity)
{
    ASSERT(simplediskfsentry);
    return ac_x31_hash_string(simplediskfsentry->GetFileNameStr()) % capacity;
}

bool CSimpleDiskFsIndex::Equals(CSimpleDiskFsEntry *simplediskfsentry1, CSimpleDiskFsEntry *simplediskfsentry2)
{
    ASSERT(simplediskfsentry1 && simplediskfsentry2);
    return simplediskfsentry1->Comp(simplediskfsentry2) == 0;
}

CSimpleDiskFsEntry* CSimpleDiskFsIndex::CloneNode(CSimpleDiskFsEntry *simplediskfsentry)
{
    CSimpleDiskFsEntry *ptr;
    NEW(ptr,CSimpleDiskFsEntry);
    ptr->Init();
    ptr->Copy(simplediskfsentry);
    return ptr;
}

status_t CSimpleDiskFsIndex::DelNode(CSimpleDiskFsEntry *simplediskfsentry)
{
    DEL(simplediskfsentry);
    return OK;
}
