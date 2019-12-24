#include "queuefile.h"
#include "misc.h"
#include "mem_tool.h"
#include "syslog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQueueFile::CQueueFile()
{
    this->InitBasic();
}
CQueueFile::~CQueueFile()
{
    this->Destroy();
}
status_t CQueueFile::InitBasic()
{
    this->head = 0;
    this->tail = 0;
    this->max_size = 0;
    this->pdata = NULL;
    this->size_shift = -1;
    this->is_full = 0;
    this->size_mod = 0;
    return OK;
}
status_t CQueueFile::Init(int_ptr_t bytes)
{
    int_ptr_t shift;
    this->InitBasic();
    CFileBase::Init();
    shift = find_shift(bytes);
    ASSERT(shift >= 0 && shift <= 31);
    this->size_shift = shift;
    this->size_mod = (1<<shift) - 1;
    MALLOC(this->pdata,char,bytes);
    this->max_size = bytes;
    return OK;
}
status_t CQueueFile::Destroy()
{
    FREE(this->pdata);
    CFileBase::Destroy();
    this->InitBasic();
    return OK;
}
fsize_t CQueueFile::GetOffset()
{
    return 0;
}
fsize_t CQueueFile::GetSize()
{
    if(this->is_full)
        return this->max_size;
    return (this->tail - this->head +this->max_size) & this->size_mod;
}
int_ptr_t CQueueFile::Read(void *buf,int_ptr_t n)
{
    int_ptr_t rs,ret = 0;

    if(n > this->GetSize())
        n = (int_ptr_t)this->GetSize();
    if(n <= 0)
        return 0;
    
    if(this->tail > this->head)
    {
        if(buf)memcpy(buf,pdata + head,n);      
        ret = n;
    }
    else
    {
        rs = n;
        if(rs > max_size - head)
            rs = max_size - head;
        if(rs > 0)
        {
            if(buf)memcpy(buf,pdata + head,rs);
            ret += rs;
        }
        rs = n - rs;
        if(rs > 0)
        {
            if(buf)memcpy((char*)buf+ret, pdata, rs);
            ret += rs;
        }
    }
    
    this->head=(this->head+ret) & this->size_mod;
    this->is_full = 0;
    return ret;
}

int_ptr_t CQueueFile::Write(const void *buf,int_ptr_t n)
{
    int_ptr_t ws,ret = 0;
    if(n > this->GetFreeSize())
        n = (int_ptr_t)this->GetFreeSize();
    if(n <= 0)
        return 0;
    if(this->tail + n <= this->max_size)
    {
        if(buf)memcpy(pdata + tail,buf,n);
        ret = n;
    }
    else
    {
        ws = n;
        if(ws > max_size - tail)
            ws = max_size - tail;
        if(ws > 0)
        {
            if(buf)memcpy(pdata + tail,buf,ws);
            ret += ws;
        }
        ws = n - ws;
        if(ws > 0)
        {
            if(buf)memcpy(pdata, (char*)buf + ret, ws);
            ret += ws;
        }
    }
    
    this->tail=(this->tail+ret) & this->size_mod;
    if(this->tail == this->head)
        is_full = 1;
    return ret;
}
status_t CQueueFile::SetSize(fsize_t ssize)
{
    return 0;
}
fsize_t CQueueFile::Seek(fsize_t off)
{
    return 0;
}
status_t CQueueFile::AddBlock(fsize_t bsize)
{
    return 0;
}
fsize_t CQueueFile::GetMaxSize()
{
    return this->max_size;
}
fsize_t CQueueFile::GetFreeSize()
{
    return this->max_size - this->GetSize();
}

status_t CQueueFile::Dump()
{
    QUEUE_FILE_CONTEXT(context);
    this->SaveContext(context);
    CFileBase::Dump();
    this->RestoreContext(context);
    return OK;
}
int CQueueFile::Clear()
{
    this->head = 0;
    this->tail = 0;
    this->is_full = 0;
    return OK;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
bool CQueueFile::HasEmptyLine()
{
    QUEUE_FILE_CONTEXT(context);    
    this->SaveContext(context); 
    bool ret = false;
    LOCAL_MEM(mem);
    while(this->ReadLine(&mem))
    {
        if(mem.C(0) == 0)
        {
            ret = TRUE;
            break;
        }
    }
    this->RestoreContext(context);
    return ret;
}
bool CQueueFile::HasWholeLine()
{
    bool ret = false;
    char ch;

    QUEUE_FILE_CONTEXT(context);
    this->SaveContext(context); 
    LOCAL_MEM(mem);
    while(this->Read(&ch,1))
    {
        if(!CFileBase::IsEmptyChar(ch))
            mem.Putc(ch);
        if(ch == '\n' && mem.GetSize() > 0)
        {
            ret = TRUE;
            break;
        }
    }
    this->RestoreContext(context);
    return ret;
}

int_ptr_t CQueueFile::PeekData(void *data, int_ptr_t size)
{	
	QUEUE_FILE_CONTEXT(context);
	this->SaveContext(context);
	int_ptr_t rs = this->Read(data,size);
	this->RestoreContext(context);
	return rs;
}

status_t CQueueFile::SaveContext(void *buf)
{
    ASSERT(buf);
    int_ptr_t *p = (int_ptr_t*)buf;
    p[0] = this->tail;
    p[1] = this->head;
    p[2] = this->is_full;
    return OK;
}
status_t CQueueFile::RestoreContext(void *buf)
{
    ASSERT(buf);
    int_ptr_t *p = (int_ptr_t*)buf; 
    this->tail = p[0];
    this->head = p[1];
    this->is_full = (p[2] !=0);
    return OK;
}

int_ptr_t CQueueFile::FindTag(const void *tag, int size)
{
    BEGIN_CLOSURE(binary_match)
    {
        CLOSURE_PARAM_PTR(CQueueFile*,self,0);
        CLOSURE_PARAM_PTR(char*,tag,10);
        CLOSURE_PARAM_INT(size,11);
        
        char tmp[1024];
        ASSERT((size_t)size <= sizeof(tmp));
        
        if(self->Read(tmp,size) < size)
            return FALSE;
        
        return memcmp(tag,tmp,size) == 0;
    }
    END_CLOSURE(binary_match);

	binary_match.SetParamPointer(10,(void*)tag);
	binary_match.SetParamInt(11,size);
	return this->FindTag(&binary_match);
}

int_ptr_t CQueueFile::FindTag(CClosure *match)
{
    ASSERT(match);

    QUEUE_FILE_CONTEXT(context);
    QUEUE_FILE_CONTEXT(tmp_ctx);

    this->SaveContext(context);
    int_ptr_t pos = 0;
    bool find = false;
    while(true)
    {       
        this->SaveContext(tmp_ctx);

        match->SetParamPointer(0,this);
        if(match->Run())
            find = true;

        this->RestoreContext(tmp_ctx);

        if(find)break;
        if(this->Read(NULL,1) <= 0)
            break;
        pos++;
    }
    this->RestoreContext(context);
    return find?pos:-1;
}
