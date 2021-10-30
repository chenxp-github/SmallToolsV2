#include "x_file_session_map.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "misc.h"


xFileSessionMap_HashEntry::xFileSessionMap_HashEntry()
{

    m_RawPtr = NULL;
    next = NULL;
}

xFileSessionMap_HashEntry::~xFileSessionMap_HashEntry()
{

    DEL(m_RawPtr);
    next = NULL;
}
    
CxFileSession* xFileSessionMap_HashEntry::get()
{
    return m_RawPtr;
}

status_t xFileSessionMap_HashEntry::set(CxFileSession *node)
{
    ASSERT(m_RawPtr == NULL);
    m_RawPtr = node;
    return OK;
}

/*********************************************/

CxFileSessionMap::CxFileSessionMap()
{
    this->InitBasic();
}
CxFileSessionMap::~CxFileSessionMap()
{
    this->Destroy();
}
status_t CxFileSessionMap::InitBasic()
{

    this->m_Capacity = 0;
    this->m_Data = 0;
    this->m_Size = 0;
    return OK;
}
status_t CxFileSessionMap::Init(int capacity)
{
    this->InitBasic();

    this->m_Capacity = capacity;
    MALLOC(this->m_Data,xFileSessionMap_HashEntry*,capacity);
    memset(this->m_Data,0,capacity*sizeof(xFileSessionMap_HashEntry*));
    return OK;
}
status_t CxFileSessionMap::Destroy()
{
    int i;
    xFileSessionMap_HashEntry *q,*p;


    if(this->m_Data == NULL)
        return OK;

    for(i = 0; i < this->m_Capacity; i++)
    {
        p  = this->m_Data[i];
        while(p)
        {
            q = p->next;
            DEL(p);
            p = q;            
        }            
    }
    FREE(this->m_Data);
    this->InitBasic();
    return OK;
}

status_t CxFileSessionMap::PutPtr(CxFileSession *ptr)
{
    int code;
    xFileSessionMap_HashEntry *p;

    ASSERT(ptr);

    if(this->Get(ptr) != NULL)
        return ERROR;

    code = this->HashCode(ptr);
    ASSERT(code >= 0 && code < this->m_Capacity);
    p = this->m_Data[code];

    xFileSessionMap_HashEntry *ptr_entry;
    NEW(ptr_entry,xFileSessionMap_HashEntry);
    ptr_entry->set(ptr);

    if (p == NULL)
    {
        this->m_Data[code] = ptr_entry;
        ptr_entry->next = NULL;
    }
    else
    {
        while(p->next)
        {
            p = p->next;
        }
        p->next = ptr_entry;
        ptr_entry->next = NULL;
    }
    this->m_Size++;
    return OK;
}

status_t CxFileSessionMap::Put(CxFileSession *hashentry)
{
    CxFileSession *tmp = this->CloneNode(hashentry);
    if(!this->PutPtr(tmp))
    {
        DEL(tmp);
        return ERROR;
    }
    return OK;
}

CxFileSession* CxFileSessionMap::Get(CxFileSession *key)
{
    int code;
    xFileSessionMap_HashEntry *p;

    ASSERT(key);

    code = this->HashCode(key);
    ASSERT(code >= 0 && code < this->m_Capacity);
    p = this->m_Data[code];
    while(p)
    {
        if(this->Equals(p->get(),key))
            return p->get();
        p = p->next;
    }
    return NULL;
}

status_t CxFileSessionMap::EnumAll(CClosure *closure)
{
    int i,_contine;
    xFileSessionMap_HashEntry *pre,*p,*next;
    
    ASSERT(closure);
    
    for(i = 0; i < this->m_Capacity; i++)
    {
        p = this->m_Data[i];
        if(p == NULL)continue;
        
        pre = p;
        p = p->next;
        while(p)
        {            
            next = p->next;
            closure->SetParamPointer(0,p->get());
            closure->SetParamInt(1,p==m_Data[i]); //is first
            _contine = closure->Run();
            //mark param 0 to NULL mean to delete the entry
            if(closure->GetParamPointer(0) == NULL) 
            {
                p->m_RawPtr = NULL;
                DEL(p);
                pre->next = next;
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
        next = p->next;
        closure->SetParamPointer(0,p->get());
        closure->SetParamInt(1,p==m_Data[i]);
        _contine = closure->Run();
        if(closure->GetParamPointer(0) == NULL)
        {
            p->m_RawPtr = NULL;
            DEL(p);
            this->m_Data[i] = next;
            this->m_Size --;
        }
        if(!_contine)
            goto end;
    }
    
end:
    return OK;
}

xFileSessionMap_HashEntry* CxFileSessionMap::Remove(CxFileSession *key)
{
    int code;
    xFileSessionMap_HashEntry *pre,*p;

    ASSERT(key);
    code = this->HashCode(key);
    if(code < 0 || code >= this->m_Capacity)
        return NULL;
    p = this->m_Data[code];
    if(p == NULL) return NULL;

    if(this->Equals(p->get(),key))
    {
        this->m_Data[code] = p->next;
        this->m_Size --;
        return p;
    }
    else
    {
        while(p)
        {
            pre = p;
            p = p->next;
            if(p && this->Equals(p->get(),key))
            {
                pre->next = p->next;
                this->m_Size --;
                return p;
            }
        }
    }

    return NULL;
}
status_t CxFileSessionMap::Del(CxFileSession *key)
{
    xFileSessionMap_HashEntry *p = this->Remove(key);
    if(p != NULL)
    {
        DEL(p);
        return OK;
    }
    return ERROR;
}
int CxFileSessionMap::GetSize()
{
    return this->m_Size;
}
int CxFileSessionMap::GetCapacity()
{
    return this->m_Capacity;
}
bool CxFileSessionMap::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CxFileSessionMap::DiscardAll()
{
    m_Data = NULL;
    return OK;
}
int CxFileSessionMap::HashCode(CxFileSession *hashentry)
{
    return HashCode(hashentry,m_Capacity);
}

status_t CxFileSessionMap::Clear()
{
    int capacity = m_Capacity;
    this->Destroy();
    this->Init(capacity);
    return OK;
}

status_t CxFileSessionMap::Copy(CxFileSessionMap *p)
{
    ASSERT(p);
    this->Destroy();
    this->Init(p->GetCapacity());

    BEGIN_CLOSURE(on_copy)
    {
        CLOSURE_PARAM_PTR(CxFileSessionMap*,self,10);
        CLOSURE_PARAM_PTR(CxFileSession*,node,0);
        self->Put(node);
        return OK;
    }
    END_CLOSURE(on_copy);
    
    on_copy.SetParamPointer(10,this);
    p->EnumAll(&on_copy);
    return OK;
}

status_t CxFileSessionMap::Comp(CxFileSessionMap *p)
{
    ASSERT(p);
    if(p == this)
        return 0;
    ASSERT(0);
    return 0;
}

status_t CxFileSessionMap::Print(CFileBase *_buf)
{
    int i;
    int collision = 0;
    int maxLength = 0;

    for(i = 0; i < this->m_Capacity; i++)
    {
        xFileSessionMap_HashEntry *p = this->m_Data[i];
        if(p != NULL)
        {
            _buf->Log("[%d]->",i);
            int len = 0;
            collision--;
            while(p)
            {
                if(p->get())
                {
                    p->get()->Print(_buf);
                }
                _buf->Log("->");
                p = p->next;
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

status_t CxFileSessionMap::ToArray(CxFileSession *arr[], int *len)
{    
    ASSERT(arr && len);   
    int max_len = *len;    
    *len = 0;    
       
    BEGIN_CLOSURE(on_enum)        
    {        
        CLOSURE_PARAM_PTR(CxFileSession*,node,0);        
        CLOSURE_PARAM_PTR(CxFileSessionMap*,self,10);        
        CLOSURE_PARAM_PTR(CxFileSession**,arr,11);        
        CLOSURE_PARAM_PTR(int*,len,12);        
        CLOSURE_PARAM_INT(max_len,13);        
        ASSERT(*len < max_len);        
        arr[(*len)++] = node;           
        return OK;        
    }    
    END_CLOSURE(on_enum);
            
    on_enum.SetParamPointer(10,this);    
    on_enum.SetParamPointer(11,arr);    
    on_enum.SetParamPointer(12,len);    
    on_enum.SetParamInt(13,max_len);        
    this->EnumAll(&on_enum);    
    return OK;
}
/*********************************************/
/*********************************************/
int CxFileSessionMap::HashCode(CxFileSession *hashentry,int capacity)
{
    ASSERT(hashentry);
    return ac_x31_hash_string(hashentry->GetNameStr()) % capacity;
}

bool CxFileSessionMap::Equals(CxFileSession *hashentry1, CxFileSession *hashentry2)
{
    ASSERT(hashentry1 && hashentry2);
    return hashentry1->Comp(hashentry2) == 0;
}

CxFileSession* CxFileSessionMap::CloneNode(CxFileSession *hashentry)
{
    CxFileSession *ptr;
    NEW(ptr,CxFileSession);
    ptr->Init();
    ptr->Copy(hashentry);
    return ptr;
}

CxFileSession *CxFileSessionMap::GetByName(CMem *name)
{
    ASSERT(name);

    CxFileSession tmp;
    tmp.Init();
    tmp.SetName(name);
    return this->Get(&tmp);
}