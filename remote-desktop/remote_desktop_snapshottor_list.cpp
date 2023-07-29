#include "remote_desktop_snapshottor_list.h"
#include "mem_tool.h"
#include "sys_log.h"


CRemoteDesktopSnapshottorList::CRemoteDesktopSnapshottorList()
{
    this->InitBasic();  
}
CRemoteDesktopSnapshottorList::~CRemoteDesktopSnapshottorList()
{
    this->Destroy();
}
status_t CRemoteDesktopSnapshottorList::InitBasic()
{
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();

    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;    
	m_Flags = 0;
    return OK;
}
status_t CRemoteDesktopSnapshottorList::Init(CTaskMgr *_taskmgr,int init_size)
{
    int i;
    this->InitBasic();  
    TASK_CONTAINER_INIT(_taskmgr);

    this->m_Size = init_size;
    MALLOC(this->m_Index,CRemoteDesktopSnapshottor* ,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}

status_t CRemoteDesktopSnapshottorList::Destroy()
{
    int i;
    WEAK_REF_DESTROY();

    if(this->m_Index == NULL)
        return ERROR;

    for(i = 0; i < this->m_Top; i++)
    {
        this->DelNode(this->m_Index[i]);
		m_Index[i] = NULL;
	}
    FREE(this->m_Index);
    this->InitBasic();    
    return OK;
}
bool CRemoteDesktopSnapshottorList::IsEmpty()
{
    return this->m_Top <= 0;
}
bool CRemoteDesktopSnapshottorList::IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t CRemoteDesktopSnapshottorList::AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,CRemoteDesktopSnapshottor*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t CRemoteDesktopSnapshottorList::Push(CRemoteDesktopSnapshottor *node)
{
    CRemoteDesktopSnapshottor *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->PushPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CRemoteDesktopSnapshottorList::PushPtr(CRemoteDesktopSnapshottor *node)
{
    ASSERT(node);    
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = node;
    this->m_Top++;
    return OK;
}

CRemoteDesktopSnapshottor * CRemoteDesktopSnapshottorList::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

status_t CRemoteDesktopSnapshottorList::Pop(CRemoteDesktopSnapshottor *node)
{
    ASSERT(!this->IsEmpty());    
    this->m_Top--;
    this->CopyNode(node,this->m_Index[this->m_Top]);
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
int CRemoteDesktopSnapshottorList::GetLen()
{
    return this->m_Top;
}

int CRemoteDesktopSnapshottorList::SearchPos(CRemoteDesktopSnapshottor *node)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],node) == 0)
            return i;
    }
    return -1;
}

CRemoteDesktopSnapshottor* CRemoteDesktopSnapshottorList::Search(CRemoteDesktopSnapshottor *node)
{
    int pos = this->SearchPos(node);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}
CRemoteDesktopSnapshottor* CRemoteDesktopSnapshottorList::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}
status_t CRemoteDesktopSnapshottorList::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t CRemoteDesktopSnapshottorList::Clear()
{
    while(this->DelTop());
    return OK;
}

CRemoteDesktopSnapshottor* CRemoteDesktopSnapshottorList::GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

CRemoteDesktopSnapshottor* CRemoteDesktopSnapshottorList::BSearchNode(CRemoteDesktopSnapshottor *node,int order)
{
    return this->GetElem(this->BSearch(node,order));
}
int CRemoteDesktopSnapshottorList::BSearch(CRemoteDesktopSnapshottor *node,int order)
{
    int find,pos;
    pos = this->BSearchPos(node,order,&find);
    if(find) return pos;
    return -1;
}

status_t CRemoteDesktopSnapshottorList::InsElemPtr(int i, CRemoteDesktopSnapshottor *node)
{
    int k;

    ASSERT(node);
    ASSERT(i >= 0 && i <= this->m_Top);

    this->AutoResize();
    ASSERT(!this->IsFull());
    for(k = this->m_Top; k > i; k--)
    {
        this->m_Index[k] = this->m_Index[k - 1];
    }
    this->m_Index[i] = node;
    this->m_Top++;
    return OK;
}

status_t CRemoteDesktopSnapshottorList::InsElem(int i, CRemoteDesktopSnapshottor *node)
{
    CRemoteDesktopSnapshottor *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->InsElemPtr(i,tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CRemoteDesktopSnapshottor* CRemoteDesktopSnapshottorList::RemoveElem(int index)
{
    CRemoteDesktopSnapshottor *p;
    int k;
    
    ASSERT(index >= 0 && index < this->m_Top);
    p = this->GetElem(index);

    for(k = index; k < this->m_Top-1; k++)
    {
        this->m_Index[k] = this->m_Index[k + 1];
    }
    this->m_Top --;
    this->m_Index[this->m_Top] = NULL;
    return p;
}

status_t CRemoteDesktopSnapshottorList::DelElem(int i)
{
    CRemoteDesktopSnapshottor *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CRemoteDesktopSnapshottorList::InsOrderedPtr(CRemoteDesktopSnapshottor *node, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,node);
}

status_t CRemoteDesktopSnapshottorList::InsOrdered(CRemoteDesktopSnapshottor *node, int order,int unique)
{
    int pos,find;
    ASSERT(node);
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElem(pos,node);
}

int CRemoteDesktopSnapshottorList::BSearchPos(CRemoteDesktopSnapshottor *node, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = this->GetLen() - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = this->CompNode(this->m_Index[mid],node);
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

status_t CRemoteDesktopSnapshottorList::Sort(int order)
{
    int i,len;
    CRemoteDesktopSnapshottorList tmp;

    len = this->GetLen();
    tmp.Init(GetTaskMgr(),len);

    for(i = 0; i < len; i++)
    {
        tmp.PushPtr(this->GetElem(i));
    }

    this->m_Top = 0;
    for(i = 0; i < len; i++)
    {
        this->InsOrderedPtr(tmp.PopPtr(),order,0);
    }
    
    for(i = 0; i < len; i++)
    {
        tmp.m_Index[i] = NULL;
    }

    return OK;
}

int CRemoteDesktopSnapshottorList::Comp(CRemoteDesktopSnapshottorList *stk)
{
    ASSERT(stk);
    if(this == stk)
        return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopSnapshottorList::Copy(CRemoteDesktopSnapshottorList *_p)
{
    int i;

    ASSERT(_p);
    this->Clear();
    for(i = 0; i < _p->GetLen(); i++)
    {
        CRemoteDesktopSnapshottor *p = _p->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
status_t CRemoteDesktopSnapshottorList::Print(CFileBase *_buf)
{
    for(int i = 0; i < this->m_Top; i++)
    {
        if(this->m_Index[i])
        {
            _buf->Log("[%d]={",i);
            _buf->IncLogLevel(1);
            this->m_Index[i]->Print(_buf);
            _buf->IncLogLevel(-1);
            _buf->Log("}");
        }
    }
    _buf->Log("top = %d",this->m_Top);
    _buf->Log("size = %d",this->m_Size);
    return OK;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CRemoteDesktopSnapshottorList::CopyNode(CRemoteDesktopSnapshottor *dst, CRemoteDesktopSnapshottor *src)
{
    return dst->Copy(src);
}

CRemoteDesktopSnapshottor* CRemoteDesktopSnapshottorList::CloneNode(CRemoteDesktopSnapshottor *node)
{
    CRemoteDesktopSnapshottor *tmp;
    NEW(tmp,CRemoteDesktopSnapshottor);
    tmp->Init(GetTaskMgr());
    this->CopyNode(tmp,node);
    return tmp;
}

status_t CRemoteDesktopSnapshottorList::DelNode(CRemoteDesktopSnapshottor *node)
{
	if(!IsWeakTable())
    {
		DEL(node);
	}
    return OK;
}

int CRemoteDesktopSnapshottorList::CompNode(CRemoteDesktopSnapshottor *node1, CRemoteDesktopSnapshottor *node2)
{
    ASSERT(node1 && node2);
    return node1->Comp(node2);
}
/////////////////////////////////////////////////////////////////////////////////////

CRemoteDesktopSnapshottor *CRemoteDesktopSnapshottorList::GetByName(CMem *name)
{
	ASSERT(name);
	CRemoteDesktopSnapshottor tmp;
	tmp.Init(GetTaskMgr());
	tmp.SetName(name);
	return this->BSearchNode(&tmp,0);
}

//snapshotter must be on heap.
status_t CRemoteDesktopSnapshottorList::AddSnapshotter(CRemoteDesktopSnapshottor *snapshotter)
{
	ASSERT(snapshotter);
	if(!this->InsOrderedPtr(snapshotter,0,1))
	{
		if(!IsWeakTable())
		{
			DEL(snapshotter);
		}
		return ERROR;
	}
	return OK;
}
