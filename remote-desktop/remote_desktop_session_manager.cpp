#include "remote_desktop_session_manager.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "minitask.h"

#define MAX_LIFETIME  (60*1000)

CRemoteDesktopSessionManager::CRemoteDesktopSessionManager()
{
    this->InitBasic();  
}
CRemoteDesktopSessionManager::~CRemoteDesktopSessionManager()
{
    this->Destroy();
}
status_t CRemoteDesktopSessionManager::InitBasic()
{
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();

    this->m_Index = NULL;
    this->m_Top = 0;
    this->m_Size = 0;    
	m_task_auto_clear = 0;
    return OK;
}
status_t CRemoteDesktopSessionManager::Init(CTaskMgr *_taskmgr,int init_size)
{
    int i;
    this->InitBasic();  
    TASK_CONTAINER_INIT(_taskmgr);

    this->m_Size = init_size;
    MALLOC(this->m_Index,CRemoteDesktopSession* ,this->m_Size);
    for(i = 0; i < this->m_Size; i++)
        this->m_Index[i] = NULL;
    return OK;
}

status_t CRemoteDesktopSessionManager::Destroy()
{
    int i;
    WEAK_REF_DESTROY();
	QuitTask(&m_task_auto_clear);

    if(this->m_Index == NULL)
        return ERROR;

    for(i = 0; i < this->m_Top; i++)
    {
        this->DelNode(this->m_Index[i]);
    }
    FREE(this->m_Index);
    this->InitBasic();    
    return OK;
}
bool CRemoteDesktopSessionManager::IsEmpty()
{
    return this->m_Top <= 0;
}
bool CRemoteDesktopSessionManager::IsFull()
{
    return this->m_Top >= this->m_Size;
}

status_t CRemoteDesktopSessionManager::AutoResize()
{
    int i;

    if(this->IsFull())
    {
        REALLOC(this->m_Index,CRemoteDesktopSession*,this->m_Size,this->m_Size*2);
        this->m_Size *= 2;
        for(i = this->m_Top; i < this->m_Size; i++)
        {
            this->m_Index[i] = NULL;
        }
    }
    return OK;
}

status_t CRemoteDesktopSessionManager::Push(CRemoteDesktopSession *node)
{
    CRemoteDesktopSession *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->PushPtr(tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    
    return OK;
}

status_t CRemoteDesktopSessionManager::PushPtr(CRemoteDesktopSession *node)
{
    ASSERT(node);    
    this->AutoResize();
    ASSERT(!this->IsFull());
    this->m_Index[this->m_Top] = node;
    this->m_Top++;
    return OK;
}

CRemoteDesktopSession * CRemoteDesktopSessionManager::PopPtr()
{
    if(this->IsEmpty())
        return NULL;
    this->m_Top--;
    return this->m_Index[this->m_Top];
}

status_t CRemoteDesktopSessionManager::Pop(CRemoteDesktopSession *node)
{
    ASSERT(!this->IsEmpty());    
    this->m_Top--;
    this->CopyNode(node,this->m_Index[this->m_Top]);
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
int CRemoteDesktopSessionManager::GetLen()
{
    return this->m_Top;
}

int CRemoteDesktopSessionManager::SearchPos(CRemoteDesktopSession *node)
{
    int i;
    for(i=0;i<this->m_Top;i++)
    {
        if(this->CompNode(this->m_Index[i],node) == 0)
            return i;
    }
    return -1;
}

CRemoteDesktopSession* CRemoteDesktopSessionManager::Search(CRemoteDesktopSession *node)
{
    int pos = this->SearchPos(node);
    if(pos >= 0 && pos < this->m_Top)
        return this->m_Index[pos];;
    return NULL;
}
CRemoteDesktopSession* CRemoteDesktopSessionManager::GetTopPtr()
{
    if(this->IsEmpty())
        return NULL;
    return this->m_Index[this->m_Top - 1];
}
status_t CRemoteDesktopSessionManager::DelTop()
{
    if(this->IsEmpty())
        return ERROR;
    this->m_Top--;
    this->DelNode(this->m_Index[this->m_Top]);
    return OK;
}
status_t CRemoteDesktopSessionManager::Clear()
{
    while(this->DelTop());
    return OK;
}

CRemoteDesktopSession* CRemoteDesktopSessionManager::GetElem(int index)
{
    if(index < 0 || index >= this->m_Top)
        return NULL;
    return this->m_Index[index];
}

CRemoteDesktopSession* CRemoteDesktopSessionManager::BSearchNode(CRemoteDesktopSession *node,int order)
{
    return this->GetElem(this->BSearch(node,order));
}
int CRemoteDesktopSessionManager::BSearch(CRemoteDesktopSession *node,int order)
{
    int find,pos;
    pos = this->BSearchPos(node,order,&find);
    if(find) return pos;
    return -1;
}

status_t CRemoteDesktopSessionManager::InsElemPtr(int i, CRemoteDesktopSession *node)
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

status_t CRemoteDesktopSessionManager::InsElem(int i, CRemoteDesktopSession *node)
{
    CRemoteDesktopSession *tmp;
    ASSERT(node);
    tmp = this->CloneNode(node);
    if(!this->InsElemPtr(i,tmp))
    {
        this->DelNode(tmp);
        return ERROR;
    }
    return OK;
}

CRemoteDesktopSession* CRemoteDesktopSessionManager::RemoveElem(int index)
{
    CRemoteDesktopSession *p;
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

status_t CRemoteDesktopSessionManager::DelElem(int i)
{
    CRemoteDesktopSession *p = this->RemoveElem(i);
    if(p != NULL)
    {
        this->DelNode(p);
        return OK;
    }
    return ERROR;
}

status_t CRemoteDesktopSessionManager::DelElems(int index, int size)
{
    int k;
    ASSERT(index >= 0 && index < this->m_Top);
    ASSERT(size >= 0);
    if (index + size > m_Top)
        size = m_Top - index;

    for(k = index; k < index+size; k++)
    {
        DEL(m_Index[k]);
    }

    for (k = index+size; k < this->m_Top; k++)
    {
        this->m_Index[k-size] = this->m_Index[k];
    }
    this->m_Top -= size;
    return OK;
}

status_t CRemoteDesktopSessionManager::InsOrderedPtr(CRemoteDesktopSession *node, int order,int unique)
{
    int pos,find;
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElemPtr(pos,node);
}

status_t CRemoteDesktopSessionManager::InsOrdered(CRemoteDesktopSession *node, int order,int unique)
{
    int pos,find;
    ASSERT(node);
    pos = this->BSearchPos(node,order,&find);
    if(find && unique)
        return ERROR;
    return this->InsElem(pos,node);
}

int CRemoteDesktopSessionManager::BSearchPos(CRemoteDesktopSession *node, int order, int *find_flag)
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

status_t CRemoteDesktopSessionManager::Sort(int order)
{
    int i,len;
    CRemoteDesktopSessionManager tmp;

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

int CRemoteDesktopSessionManager::Comp(CRemoteDesktopSessionManager *stk)
{
    ASSERT(stk);
    if(this == stk)
        return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopSessionManager::Copy(CRemoteDesktopSessionManager *_p)
{
    int i;

    ASSERT(_p);
    this->Clear();
    for(i = 0; i < _p->GetLen(); i++)
    {
        CRemoteDesktopSession *p = _p->GetElem(i);
        ASSERT(p);
        this->Push(p);
    }
    return OK;
}
status_t CRemoteDesktopSessionManager::Print(CFileBase *_buf)
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
status_t CRemoteDesktopSessionManager::CopyNode(CRemoteDesktopSession *dst, CRemoteDesktopSession *src)
{
    return dst->Copy(src);
}

CRemoteDesktopSession* CRemoteDesktopSessionManager::CloneNode(CRemoteDesktopSession *node)
{
    CRemoteDesktopSession *tmp;
    NEW(tmp,CRemoteDesktopSession);
    tmp->Init(GetTaskMgr());
    this->CopyNode(tmp,node);
    return tmp;
}

status_t CRemoteDesktopSessionManager::DelNode(CRemoteDesktopSession *node)
{
    DEL(node);
    return OK;
}

int CRemoteDesktopSessionManager::CompNode(CRemoteDesktopSession *node1, CRemoteDesktopSession *node2)
{
    ASSERT(node1 && node2);
    return node1->Comp(node2);
}
/////////////////////////////////////////////////////////////////////////////////////
status_t CRemoteDesktopSessionManager::StartAutoClearTask()
{
	ASSERT(!IsTask(m_task_auto_clear));
	
	BEGIN_MINI_TASK(task)
	{
		CRemoteDesktopSessionManager *self;
		status_t Run(uint32_t interval)
		{
			self->OnAutoClear(interval);
			this->Sleep(1000);
			return OK;
		}
	}
	END_MINI_TASK(task);
	
	task->Init(GetTaskMgr());
	task->self = this;
	task->Start(1);    
	m_task_auto_clear = task->GetId();
	return OK;

}

status_t CRemoteDesktopSessionManager::OnAutoClear(uint32_t interval)
{
	for(int i = GetLen()-1; i >= 0; i--)
	{
		CRemoteDesktopSession *session = this->GetElem(i);
		session->IncLifetime(interval);
		if(session->GetLifetime() > MAX_LIFETIME)
		{
			this->DelElem(i);
		}
	}
	return OK;
}


CRemoteDesktopSession *CRemoteDesktopSessionManager::CreateSession(CMem *name)
{
	ASSERT(name);
	CRemoteDesktopSession *session;
	NEW(session,CRemoteDesktopSession);
	session->Init(GetTaskMgr());
	session->SetName(name);

	if(this->InsOrderedPtr(session,0,1))
	{
		return session;
	}
	else
	{
		DEL(session);
		return NULL;
	}
}

CRemoteDesktopSession *CRemoteDesktopSessionManager::GetSession(CMem *name)
{
	ASSERT(name);
	CRemoteDesktopSession tmp;
	tmp.Init(GetTaskMgr());
	tmp.SetName(name);
	return this->BSearchNode(&tmp,0);
}

status_t CRemoteDesktopSessionManager::DelSession(CMem *name)
{
	ASSERT(name);

	CRemoteDesktopSession tmp;
	tmp.Init(GetTaskMgr());
	tmp.SetName(name);
	
	int found = 0;
	int index = this->BSearchPos(&tmp,0,&found);
	if(found)
	{
		this->DelElem(index);	
	}

	return OK;
}

status_t CRemoteDesktopSessionManager::SessionReactive(CMem *name)
{
	ASSERT(name);

	CRemoteDesktopSession *session = this->GetSession(name);
	if(session)
	{
		session->SetLifetime(0);
	}
	return OK;
}

