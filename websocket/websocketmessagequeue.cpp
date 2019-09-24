#include "websocketmessagequeue.h"
#include "mem_tool.h"
#include "syslog.h"

CWebSocketMessageQueue::CWebSocketMessageQueue()
{
	this->InitBasic();  
}
CWebSocketMessageQueue::~CWebSocketMessageQueue()
{
	this->Destroy();
}
status_t CWebSocketMessageQueue::InitBasic()
{
	this->m_Index = NULL;
	this->m_Top = 0;
	this->m_Size = 0;	
	return OK;
}
status_t CWebSocketMessageQueue::Init(int init_size)
{
	int i;
	this->InitBasic();  
	this->m_Size = init_size;
	MALLOC(this->m_Index,CWebSocketMessage* ,this->m_Size);
	for(i = 0; i < this->m_Size; i++)
		this->m_Index[i] = NULL;
	return OK;
}
status_t CWebSocketMessageQueue::Destroy()
{
	int i;
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
bool CWebSocketMessageQueue::IsEmpty()
{
	return this->m_Top <= 0;
}
bool CWebSocketMessageQueue::IsFull()
{
	return this->m_Top >= this->m_Size;
}

status_t CWebSocketMessageQueue::AutoResize()
{
	int i;

	if(this->IsFull())
	{
		REALLOC(this->m_Index,CWebSocketMessage*,this->m_Size,this->m_Size*2);
		this->m_Size *= 2;
		for(i = this->m_Top; i < this->m_Size; i++)
		{
			this->m_Index[i] = NULL;
		}
	}
	return OK;
}

status_t CWebSocketMessageQueue::Push(CWebSocketMessage *node)
{
	CWebSocketMessage *tmp;
	ASSERT(node);
	tmp = this->CloneNode(node);
	if(!this->PushPtr(tmp))
	{
		this->DelNode(tmp);
		return ERROR;
	}
	
	return OK;
}

status_t CWebSocketMessageQueue::PushPtr(CWebSocketMessage *node)
{
	ASSERT(node);	
	this->AutoResize();
	ASSERT(!this->IsFull());
	this->m_Index[this->m_Top] = node;
	this->m_Top++;
	return OK;
}

CWebSocketMessage * CWebSocketMessageQueue::PopPtr()
{
	if(this->IsEmpty())
		return NULL;
	this->m_Top--;
	return this->m_Index[this->m_Top];
}

status_t CWebSocketMessageQueue::Pop(CWebSocketMessage *node)
{
	ASSERT(!this->IsEmpty());	
	this->m_Top--;
	this->CopyNode(node,this->m_Index[this->m_Top]);
	this->DelNode(this->m_Index[this->m_Top]);
	return OK;
}
int CWebSocketMessageQueue::GetLen()
{
	return this->m_Top;
}

int CWebSocketMessageQueue::SearchPos(CWebSocketMessage *node)
{
	int i;
	for(i=0;i<this->m_Top;i++)
	{
		if(this->CompNode(this->m_Index[i],node) == 0)
			return i;
	}
	return -1;
}

CWebSocketMessage* CWebSocketMessageQueue::Search(CWebSocketMessage *node)
{
	int pos = this->SearchPos(node);
	if(pos >= 0 && pos < this->m_Top)
		return this->m_Index[pos];;
	return NULL;
}
CWebSocketMessage* CWebSocketMessageQueue::GetTopPtr()
{
	if(this->IsEmpty())
		return NULL;
	return this->m_Index[this->m_Top - 1];
}
status_t CWebSocketMessageQueue::DelTop()
{
	if(this->IsEmpty())
		return ERROR;
	this->m_Top--;
	this->DelNode(this->m_Index[this->m_Top]);
	return OK;
}
status_t CWebSocketMessageQueue::Clear()
{
	while(this->DelTop());
	return OK;
}

CWebSocketMessage* CWebSocketMessageQueue::GetElem(int index)
{
	if(index < 0 || index >= this->m_Top)
		return NULL;
	return this->m_Index[index];
}

CWebSocketMessage* CWebSocketMessageQueue::BSearchNode(CWebSocketMessage *node,int order)
{
	return this->GetElem(this->BSearch(node,order));
}
int CWebSocketMessageQueue::BSearch(CWebSocketMessage *node,int order)
{
	int find,pos;
	pos = this->BSearchPos(node,order,&find);
	if(find) return pos;
	return -1;
}

status_t CWebSocketMessageQueue::InsElemPtr(int i, CWebSocketMessage *node)
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

status_t CWebSocketMessageQueue::InsElem(int i, CWebSocketMessage *node)
{
	CWebSocketMessage *tmp;
	ASSERT(node);
	tmp = this->CloneNode(node);
	if(!this->InsElemPtr(i,tmp))
	{
		this->DelNode(tmp);
		return ERROR;
	}
	return OK;
}

CWebSocketMessage* CWebSocketMessageQueue::RemoveElem(int index)
{
	CWebSocketMessage *p;
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

status_t CWebSocketMessageQueue::DelElem(int i)
{
	CWebSocketMessage *p = this->RemoveElem(i);
	if(p != NULL)
	{
		this->DelNode(p);
		return OK;
	}
	return ERROR;
}

status_t CWebSocketMessageQueue::InsOrderedPtr(CWebSocketMessage *node, int order,int unique)
{
	int pos,find;
	pos = this->BSearchPos(node,order,&find);
	if(find && unique)
		return ERROR;
	return this->InsElemPtr(pos,node);
}

status_t CWebSocketMessageQueue::InsOrdered(CWebSocketMessage *node, int order,int unique)
{
	int pos,find;
	ASSERT(node);
	pos = this->BSearchPos(node,order,&find);
	if(find && unique)
		return ERROR;
	return this->InsElem(pos,node);
}

int CWebSocketMessageQueue::BSearchPos(CWebSocketMessage *node, int order, int *find_flag)
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

status_t CWebSocketMessageQueue::Sort(int order)
{
	int i,len;
	CWebSocketMessageQueue tmp;

	len = this->GetLen();
	tmp.Init(len);

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
status_t CWebSocketMessageQueue::Copy(CWebSocketMessageQueue *_p)
{
	int i;

	ASSERT(_p);
	this->Destroy();
	this->Init(_p->GetLen());
	for(i = 0; i < _p->GetLen(); i++)
	{
		CWebSocketMessage *p = _p->GetElem(i);
		ASSERT(p);
		this->Push(p);
	}
	return OK;
}
status_t CWebSocketMessageQueue::Print(CFileBase *_buf)
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
status_t CWebSocketMessageQueue::CopyNode(CWebSocketMessage *dst, CWebSocketMessage *src)
{
	return dst->Copy(src);
}

CWebSocketMessage* CWebSocketMessageQueue::CloneNode(CWebSocketMessage *node)
{
	CWebSocketMessage *tmp;
	NEW(tmp,CWebSocketMessage);
	tmp->Init();
	this->CopyNode(tmp,node);
	return tmp;
}

status_t CWebSocketMessageQueue::DelNode(CWebSocketMessage *node)
{
	DEL(node);
	return OK;
}

int CWebSocketMessageQueue::CompNode(CWebSocketMessage *node1, CWebSocketMessage *node2)
{
	ASSERT(node1 && node2);
	return node1->Comp(node2);
}
