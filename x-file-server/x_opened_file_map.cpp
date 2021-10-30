#include "x_opened_file_map.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "misc.h"


xOpenedFileMap_HashEntry::xOpenedFileMap_HashEntry()
{

    m_RawPtr = NULL;
    next = NULL;
}

xOpenedFileMap_HashEntry::~xOpenedFileMap_HashEntry()
{

    DEL(m_RawPtr);
    next = NULL;
}
    
CFile* xOpenedFileMap_HashEntry::get()
{
    return m_RawPtr;
}

status_t xOpenedFileMap_HashEntry::set(CFile *node)
{
    ASSERT(m_RawPtr == NULL);
    m_RawPtr = node;
    return OK;
}

/*********************************************/

CxOpenedFileMap::CxOpenedFileMap()
{
    this->InitBasic();
}
CxOpenedFileMap::~CxOpenedFileMap()
{
    this->Destroy();
}
status_t CxOpenedFileMap::InitBasic()
{

    this->m_Capacity = 0;
    this->m_Data = 0;
    this->m_Size = 0;
    return OK;
}
status_t CxOpenedFileMap::Init(int capacity)
{
    this->InitBasic();

    this->m_Capacity = capacity;
    MALLOC(this->m_Data,xOpenedFileMap_HashEntry*,capacity);
    memset(this->m_Data,0,capacity*sizeof(xOpenedFileMap_HashEntry*));
    return OK;
}
status_t CxOpenedFileMap::Destroy()
{
    int i;
    xOpenedFileMap_HashEntry *q,*p;


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

status_t CxOpenedFileMap::PutPtr(CFile *ptr)
{
    int code;
    xOpenedFileMap_HashEntry *p;

    ASSERT(ptr);

    if(this->Get(ptr) != NULL)
        return ERROR;

    code = this->HashCode(ptr);
    ASSERT(code >= 0 && code < this->m_Capacity);
    p = this->m_Data[code];

    xOpenedFileMap_HashEntry *ptr_entry;
    NEW(ptr_entry,xOpenedFileMap_HashEntry);
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

status_t CxOpenedFileMap::Put(CFile *hashentry)
{
    CFile *tmp = this->CloneNode(hashentry);
    if(!this->PutPtr(tmp))
    {
        DEL(tmp);
        return ERROR;
    }
    return OK;
}

CFile* CxOpenedFileMap::Get(CFile *key)
{
    int code;
    xOpenedFileMap_HashEntry *p;

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

status_t CxOpenedFileMap::EnumAll(CClosure *closure)
{
    int i,_contine;
    xOpenedFileMap_HashEntry *pre,*p,*next;
    
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

xOpenedFileMap_HashEntry* CxOpenedFileMap::Remove(CFile *key)
{
    int code;
    xOpenedFileMap_HashEntry *pre,*p;

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
status_t CxOpenedFileMap::Del(CFile *key)
{
    xOpenedFileMap_HashEntry *p = this->Remove(key);
    if(p != NULL)
    {
        DEL(p);
        return OK;
    }
    return ERROR;
}
int CxOpenedFileMap::GetSize()
{
    return this->m_Size;
}
int CxOpenedFileMap::GetCapacity()
{
    return this->m_Capacity;
}
bool CxOpenedFileMap::IsEmpty()
{
    return this->GetSize() <= 0;
}
status_t CxOpenedFileMap::DiscardAll()
{
    m_Data = NULL;
    return OK;
}
int CxOpenedFileMap::HashCode(CFile *hashentry)
{
    return HashCode(hashentry,m_Capacity);
}

status_t CxOpenedFileMap::Clear()
{
    int capacity = m_Capacity;
    this->Destroy();
    this->Init(capacity);
    return OK;
}

status_t CxOpenedFileMap::Copy(CxOpenedFileMap *p)
{
    ASSERT(p);
    this->Destroy();
    this->Init(p->GetCapacity());

    BEGIN_CLOSURE(on_copy)
    {
        CLOSURE_PARAM_PTR(CxOpenedFileMap*,self,10);
        CLOSURE_PARAM_PTR(CFile*,node,0);
        self->Put(node);
        return OK;
    }
    END_CLOSURE(on_copy);
    
    on_copy.SetParamPointer(10,this);
    p->EnumAll(&on_copy);
    return OK;
}

status_t CxOpenedFileMap::Comp(CxOpenedFileMap *p)
{
    ASSERT(p);
    if(p == this)
        return 0;
    ASSERT(0);
    return 0;
}

status_t CxOpenedFileMap::Print(CFileBase *_buf)
{
    _buf->Log("capacity is %d", m_Capacity);
    _buf->Log("total size is %d",m_Size);
    return OK;
}

status_t CxOpenedFileMap::ToArray(CFile *arr[], int *len)
{    
    ASSERT(arr && len);   
    int max_len = *len;    
    *len = 0;    
       
    BEGIN_CLOSURE(on_enum)        
    {        
        CLOSURE_PARAM_PTR(CFile*,node,0);        
        CLOSURE_PARAM_PTR(CxOpenedFileMap*,self,10);        
        CLOSURE_PARAM_PTR(CFile**,arr,11);        
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
int CxOpenedFileMap::HashCode(CFile *hashentry,int capacity)
{
    ASSERT(hashentry);
    return bob_32bit_integer_hash(hashentry->GetFileHandle()) % capacity;
}

bool CxOpenedFileMap::Equals(CFile *hashentry1, CFile *hashentry2)
{
    ASSERT(hashentry1 && hashentry2);
    return hashentry1->GetFileHandle() == hashentry2->GetFileHandle();
}

CFile* CxOpenedFileMap::CloneNode(CFile *hashentry)
{
    ASSERT(0);
    return NULL;
}

CFile *CxOpenedFileMap::GetByHandle(int handle)
{
    CFile tmp;
    tmp.Init();
    tmp.fp = handle;
    CFile *file = this->Get(&tmp);
    tmp.fp = ERROR_FILE_HANDLE;
    return file;
}

FILE_HANDLE CxOpenedFileMap::OpenFile(const char *filename, const char *mode)
{
    ASSERT(filename && mode);

    CFile *file;
    NEW(file,CFile);
    file->Init();
    if(file->OpenFile(filename,mode))
    {
        this->PutPtr(file);
        return file->GetFileHandle();
    }
    else
    {
        DEL(file);
        return ERROR_FILE_HANDLE;
    }
}

status_t CxOpenedFileMap::CloseFile(int handle)
{
    CFile *file = this->GetByHandle(handle);
    if(file)
    {
        file->CloseFile();
        this->Del(file);
        return OK;
    }
    return ERROR;
}

