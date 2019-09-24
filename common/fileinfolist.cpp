#include "fileinfolist.h"
#include "dirmgr.h"
#include "encoder.h"
#include "syslog.h"
#include "mem_tool.h"
#include "minibson.h"
#include "file.h"

#define BSON_TAG_FILE_INFO  "f"

CFileInfoList::CFileInfoList()
{
    this->InitBasic();  
}
CFileInfoList::~CFileInfoList()
{
    this->Destroy();
}
status_t CFileInfoList::InitBasic()
{
    CUserFunc::InitBasic();
    this->mIndex = NULL;
    this->mTop = 0;;
    this->mSize = 0;
    return OK;
}
status_t CFileInfoList::Init(int init_size)
{
    int i;
    this->InitBasic();  
    CUserFunc::Init();
    this->mSize = init_size;
    MALLOC(this->mIndex,CFileInfo * ,this->mSize);
    for(i = 0; i < this->mSize; i++)
        this->mIndex[i] = NULL;
    return OK;
}
status_t CFileInfoList::Destroy()
{
    int i;
    if(this->mIndex == NULL)
        return ERROR;
    for(i = 0; i < this->mTop; i++)
    {
        DelNode(this->mIndex[i]);
    }
    FREE(this->mIndex);
    CUserFunc::Destroy();
    this->InitBasic();  
    return OK;
}
bool CFileInfoList::IsEmpty()
{
    return this->mTop <= 0;
}
bool CFileInfoList::IsFull()
{
    return this->mTop >= this->mSize;
}

status_t CFileInfoList::AutoResize()
{
    if(this->IsFull())
    {
        REALLOC(this->mIndex,CFileInfo*,this->mSize,this->mSize*2);
        this->mSize *= 2;
        for(int i = this->mTop; i < this->mSize; i++)
        {
            this->mIndex[i] = NULL;
        }
    }
    return OK;
}

status_t CFileInfoList::Push(CFileInfo *fileinfo)
{
    ASSERT(fileinfo);
    CFileInfo *tmp = this->CloneNode(fileinfo);
    if(!this->PushPtr(tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CFileInfoList::PushPtr(CFileInfo *fileinfo)
{
    ASSERT(fileinfo);   
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->mIndex[this->mTop] = fileinfo;
    this->mTop++;
    return OK;
}

CFileInfo * CFileInfoList::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->mTop--;
    return this->mIndex[this->mTop];
}

status_t CFileInfoList::Pop(CFileInfo *fileinfo)
{
    ASSERT(!this->IsEmpty());   
    this->mTop--;
    CopyNode(fileinfo,mIndex[mTop]);
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CFileInfoList::Print(CFileBase *_buf)
{
    int i;
    for(i = 0; i <this->mTop; i++)
    {
        this->mIndex[i]->Print(_buf);
        _buf->Log(",");
    }
    _buf->Log("mSize=%ld",this->mSize);
    _buf->Log("mTop=%ld",this->mTop);
    _buf->Log("bottom=%ld",0);
    return OK;
}
int CFileInfoList::GetLen()
{
    return this->mTop;
}

int CFileInfoList::SearchPos(CFileInfo *fileinfo)
{
    int i;
    for(i=0;i<this->mTop;i++)
    {
        if(CompNode(mIndex[i],fileinfo) == 0)
            return i;
    }
    return -1;
}

CFileInfo * CFileInfoList::Search(CFileInfo *fileinfo)
{
    int pos = this->SearchPos(fileinfo);
    if(pos >= 0 && pos < this->mTop)
        return this->mIndex[pos];;
    return NULL;
}
CFileInfo * CFileInfoList::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->mIndex[this->mTop - 1];
}
status_t CFileInfoList::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->mTop--;
    DelNode(this->mIndex[this->mTop]);
    return OK;
}
status_t CFileInfoList::Clear()
{
    while(this->DelTop());
    return OK;
}

CFileInfo * CFileInfoList::GetElem(int mIndex)
{
    if(mIndex < 0 || mIndex >= this->mTop)
        return NULL;
    return this->mIndex[mIndex];
}

CFileInfo * CFileInfoList::BSearchNode(CFileInfo *fileinfo,int order)
{
    return this->GetElem(this->BSearch(fileinfo,order));
}
int CFileInfoList::BSearch(CFileInfo *fileinfo,int order)
{
    int find,pos;
    pos = this->BSearchPos(fileinfo,order,&find);
    if(find) return pos;
    return -1;
}

status_t CFileInfoList::InsertElemPtr(int i, CFileInfo *fileinfo)
{
    ASSERT(fileinfo);
    ASSERT(i >= 0 && i <= this->mTop);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(int k = this->mTop; k > i; k--)
    {
        this->mIndex[k] = this->mIndex[k - 1];
    }
    this->mIndex[i] = fileinfo;
    this->mTop++;
    return OK;
}

status_t CFileInfoList::InsertElem(int i, CFileInfo *fileinfo)
{
    ASSERT(fileinfo);
    CFileInfo *tmp = this->CloneNode(fileinfo);
    if(!this->InsertElemPtr(i,tmp))
    {
        DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CFileInfo * CFileInfoList::RemoveElem(int index)
{
    CFileInfo *p;
    
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

status_t CFileInfoList::DelElem(int i)
{
    CFileInfo *p = this->RemoveElem(i);
    if(p != NULL)
    {
        DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CFileInfoList::InsOrderedPtr(CFileInfo *fileinfo, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(fileinfo,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElemPtr(pos,fileinfo);
}

status_t CFileInfoList::InsOrdered(CFileInfo *fileinfo, int order,int unique)
{
    ASSERT(fileinfo);
    int pos,find;
    pos = this->BSearchPos(fileinfo,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsertElem(pos,fileinfo);
}

int CFileInfoList::BSearchPos(CFileInfo *fileinfo, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = CompNode(mIndex[mid],fileinfo);
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

status_t CFileInfoList::Sort(int order)
{
    int i,len;
    CFileInfoList tmp;

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
status_t CFileInfoList::Copy(CFileInfoList *stk)
{
    ASSERT(stk);
    this->Destroy();
    this->Init(stk->GetLen());
    for(int i = 0; i < stk->GetLen(); i++)
    {
        CFileInfo *p = stk->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CFileInfoList::CopyNode(CFileInfo *dst, CFileInfo *src)
{
    return dst->Copy(src);
}

CFileInfo * CFileInfoList::CloneNode(CFileInfo *fileinfo)
{
    CFileInfo *tmp;
    NEW(tmp,CFileInfo);
    tmp->Init();
    CopyNode(tmp,fileinfo);
    return tmp;
}

status_t CFileInfoList::DelNode(CFileInfo *fileinfo)
{
    DEL(fileinfo);
    return OK;
}

int CFileInfoList::CompNode(CFileInfo *fileinfo1, CFileInfo *fileinfo2)
{
    ASSERT(fileinfo1 && fileinfo2);
    if(this->IsUserFunc(HOW_TO_COMP))
    {
        Callback()->SetParamPointer(1,fileinfo1);
        Callback()->SetParamPointer(2,fileinfo2);
        return Callback()->Run(HOW_TO_COMP);
    }
    else
    {
        return fileinfo1->Comp(fileinfo2);
    }
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
status_t CFileInfoList::SearchDir(const char *dir,bool fullpath, bool recursive)
{
    this->Clear();
    
    ASSERT(dir);
    
    BEGIN_CLOSURE(pass)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(char*,filename,1);
        CLOSURE_PARAM_PTR(char*,fullname,2);
        CLOSURE_PARAM_INT(is_dir,3);
        CLOSURE_PARAM_INT64(size,4);
        CLOSURE_PARAM_INT64(last_write_time,5);

        CLOSURE_PARAM_PTR(CFileInfoList*,self,10);
        CLOSURE_PARAM_INT(fullpath,11);

        if(event == CDirMgr::EVENT_END_DIR)
            return OK;

        CFileInfo info;
        info.Init();        
        info.SetSize(size);
        info.SetIsDir(is_dir!=0);
        info.SetLastWriteTime(last_write_time);

        if(fullpath)
            info.SetName(fullname);
        else
            info.SetName(filename);

        self->Push(&info);

        return OK;
    }
    END_CLOSURE(pass);
    
    pass.SetParamPointer(10,this);
    pass.SetParamInt(11,fullpath);

    return CDirMgr::SearchDir(dir,recursive,&pass);
}

status_t CFileInfoList::SaveToBson(CFileBase *file)
{
    ASSERT(file);
    
    CMiniBson bson;
    bson.Init();
    bson.AllocBuf(64*1024*1024);
    bson.StartDocument();
    for(int i = 0; i < this->GetLen(); i++)
    {
        CFileInfo *p = this->GetElem(i);
        ASSERT(p);

        fsize_t off;
        bson.StartDocument(BSON_TAG_FILE_INFO,&off);
        p->SaveToBson(&bson);
        bson.EndDocument(off);
    }
    bson.EndDocument();
    file->SetSize(0);
    file->WriteFile(bson.GetRawData());
    return OK;
}

status_t CFileInfoList::SaveToBson(const char *filename)
{
    CFile file;
    file.Init();
    ASSERT(file.OpenFile(filename,"wb+"));
    return this->SaveToBson(&file);
}

status_t CFileInfoList::LoadFromBson(CFileBase *file)
{
    ASSERT(file);

    CMem data;
    data.Init();
    data.Copy(file);

    CMiniBson bson;
    bson.Init();
    bson.LoadBson(&data);

    this->Clear();
    
    while(!bson.IsEnd())
    {
        CMiniBson doc;
        doc.Init();
        bson.GetDocument(BSON_TAG_FILE_INFO,&doc);
        CFileInfo info;
        info.Init();
        info.LoadFromBson(&doc);
        this->Push(&info);
    }
    return OK;
}

status_t CFileInfoList::LoadFromBson(const char *filename)
{
    CFile file;
    file.Init();
    if(file.OpenFile(filename,"rb"))
    {
        return this->LoadFromBson(&file);
    }
    return ERROR;
}

status_t CFileInfoList::RemoveDirPrefix(const char *dir)
{
    ASSERT(dir);
    int len = strlen(dir);
    if(len <= 0)dir="/";
    
    if(dir[len-1] != '/' && dir[len-1] != '\\')
        len = len + 1;

    LOCAL_MEM(mem);

    for(int i = 0; i < this->GetLen(); i++)
    {
        CFileInfo *p = this->GetElem(i);
        ASSERT(p);

        if(p->mName->StartWith(dir,false,false))
        {
            mem.SetSize(0);
            mem.Puts(p->GetName()+len);
            p->SetName(&mem);
        }
    }
    return OK;
}

int CFileInfoList::SearchOnlyByName(const char *name)
{
    BEGIN_CLOSURE_FUNC(comp_only_by_name)
    {
        CLOSURE_PARAM_PTR(CFileInfo*,info1,1);
        CLOSURE_PARAM_PTR(CFileInfo*,info2,2);
        return crt_stricmp(info1->GetName(),info2->GetName());  
    }
    END_CLOSURE_FUNC(comp_only_by_name);

    CLOSURE_FUNC old_func = Callback()->GetFunc();
    uint32_t old_flags = this->GetUseUserFunc();

    Callback()->SetFunc(comp_only_by_name);
    this->SetUseUserFunc(HOW_TO_COMP);

    CFileInfo tmp;
    tmp.Init();
    tmp.SetName(name);

    int ret = this->SearchPos(&tmp);
    Callback()->SetFunc(old_func);
    this->SetUseUserFunc(old_flags);

    return ret;
}
