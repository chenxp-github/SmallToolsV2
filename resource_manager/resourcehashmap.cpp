#include "resourcehashmap.h"
#include "syslog.h"
#include "mem_tool.h"
#include "misc.h"

CResourceHashMap::CResourceHashMap()
{
    this->InitBasic();
}
CResourceHashMap::~CResourceHashMap()
{
    this->Destroy();
}
status_t CResourceHashMap::InitBasic()
{
    this->m_Capacity = 0;
    this->m_Data = 0;
    this->m_Size = 0;
    return OK;
}
status_t CResourceHashMap::Init(int capacity)
{
    this->InitBasic();
    this->m_Capacity = capacity;
    MALLOC(this->m_Data,CResource*,capacity);
    memset(this->m_Data,0,capacity*sizeof(CResource*));
    return OK;
}
status_t CResourceHashMap::Destroy()
{
    int i;
    CResource *q,*p;

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

status_t CResourceHashMap::PutPtr(CResource *ptr)
{
    int code;
    CResource *p;

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

status_t CResourceHashMap::Put(CResource *resource)
{
    CResource *tmp = this->CloneNode(resource);
    if(!this->PutPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CResource* CResourceHashMap::Get(CResource *key)
{
    int code;
    CResource *p;

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

status_t CResourceHashMap::EnumAll(CClosure *closure)
{
    int i,_contine;
    CResource *pre,*p,*next;
    
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

CResource* CResourceHashMap::Remove(CResource *key)
{
    int code;
    CResource *pre,*p;

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
status_t CResourceHashMap::Del(CResource *key)
{
    CResource *p = this->Remove(key);
    if(p != NULL)
    {
        return this->DelNode(p);
    }
    return ERROR;
}
int CResourceHashMap::GetSize()
{
    return this->m_Size;
}
int CResourceHashMap::GetCapacity()
{
    return this->m_Capacity;
}
bool CResourceHashMap::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CResourceHashMap::DiscardAll()
{
    m_Data = NULL;
    return OK;
}
int CResourceHashMap::HashCode(CResource *resource)
{
    return HashCode(resource,m_Capacity);
}
status_t CResourceHashMap::Print(CFileBase *_buf)
{
    int i;
    int collision = 0;
    int maxLength = 0;

    for(i = 0; i < this->m_Capacity; i++)
    {
        CResource *p = this->m_Data[i];

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
int CResourceHashMap::HashCode(CResource *resource,int capacity)
{
    ASSERT(resource);
    LOCAL_MEM(mem);
    mem.Puts(resource->GetId());
    mem.Puts(resource->GetTheme());
    mem.Puts(resource->GetLanguage());
    uint32_t code = ac_x31_hash_string(mem.CStr());
    return code % capacity;
}

bool CResourceHashMap::Equals(CResource *resource1, CResource *resource2)
{
    ASSERT(resource1 && resource2);
    return resource1->Comp(resource2) == 0;
}

CResource* CResourceHashMap::CloneNode(CResource *resource)
{
    CResource *ptr;
    NEW(ptr,CResource);
    ptr->Init();
    ptr->Copy(resource);
    return ptr;
}

status_t CResourceHashMap::DelNode(CResource *resource)
{
    DEL(resource);
    return OK;
}
