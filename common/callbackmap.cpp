#include "callbackmap.h"
#include "syslog.h"
#include "mem_tool.h"
#include "tasktimer.h"

CCallbackMap::CCallbackMap()
{
	this->InitBasic();
}
CCallbackMap::~CCallbackMap()
{
	this->Destroy();
}
status_t CCallbackMap::InitBasic()
{
	TASK_CONTAINER_CLEAR();
    WEAK_REF_CLEAR();

	m_AutoTimeoutTimer = 0;
	this->m_Capacity = 0;
	this->m_Data = 0;
	this->m_Size = 0;
	this->m_UniqueId = 0;
	return OK;
}
status_t CCallbackMap::Init(CTaskMgr *mgr,int capacity)
{
	this->InitBasic();    
    
    TASK_CONTAINER_INIT(mgr);

    this->m_Capacity = capacity;
	MALLOC(this->m_Data,CCallback*,capacity);
	memset(this->m_Data,0,capacity*sizeof(CCallback*));
	return OK;
}
status_t CCallbackMap::Destroy()
{
	int i;
	CCallback *q,*p;
	WEAK_REF_DESTROY();
	QuitTask(&m_AutoTimeoutTimer);

	if(this->m_Data == NULL)
		return OK;

	for(i = 0; i < this->m_Capacity; i++)
	{
		p  = this->m_Data[i];
		while(p)
		{
			q = p->next;
			this->DelNode(p);
			p = q;			
		}			
	}
	FREE(this->m_Data);
	this->InitBasic();
	return OK;
}

status_t CCallbackMap::PutPtr(CCallback *ptr)
{
	int code;
	CCallback *p;

	ASSERT(ptr);

	if(this->Get(ptr) != NULL)
		return ERROR;

	code = this->HashCode(ptr);
	ASSERT(code >= 0 && code < this->m_Capacity);
	p = this->m_Data[code];
	if (p == NULL)
	{
		this->m_Data[code] = ptr;
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
	this->m_Size++;
	return OK;
}

status_t CCallbackMap::Put(CCallback *callback)
{
	CCallback *tmp = this->CloneNode(callback);
	if(!this->PutPtr(tmp))
	{
		this->DelNode(tmp);
		return ERROR;
	}
	return OK;
}

CCallback* CCallbackMap::Get(CCallback *key)
{
	int code;
	CCallback *p;

	ASSERT(key);

	code = this->HashCode(key);
	ASSERT(code >= 0 && code < this->m_Capacity);
	p = this->m_Data[code];
	while(p)
	{
		if(this->Equals(p,key))
			return p;
		p = p->next;
	}
	return NULL;
}

status_t CCallbackMap::EnumAll(CClosure *closure)
{
	int i,_contine;
	CCallback *pre,*p,*next;

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
			closure->SetParamPointer(0,p);
			_contine = closure->Run();
			if(closure->GetParamPointer(0) == NULL)
			{
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
		closure->SetParamPointer(0,p);
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

CCallback* CCallbackMap::Remove(CCallback *key)
{
	int code;
	CCallback *pre,*p;

	ASSERT(key);
	code = this->HashCode(key);
	if(code < 0 || code >= this->m_Capacity)
		return NULL;
	p = this->m_Data[code];
	if(p == NULL) return NULL;

	if(this->Equals(p,key))
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
			if(p && this->Equals(p,key))
			{
				pre->next = p->next;
				this->m_Size --;
				return p;
			}
		}
	}

	return NULL;
}
status_t CCallbackMap::Del(CCallback *key)
{
	CCallback *p = this->Remove(key);
	if(p != NULL)
	{
		return this->DelNode(p);
	}
	return ERROR;
}
int CCallbackMap::GetSize()
{
	return this->m_Size;
}
bool CCallbackMap::IsEmpty()
{
	return this->GetSize() <= 0;
}
status_t CCallbackMap::Print(CFileBase *_buf)
{
	int i;
	int collision = 0;
	int maxLength = 0;

	for(i = 0; i < this->m_Capacity; i++)
	{
		CCallback *p = this->m_Data[i];
		_buf->Log("[%d]->",i);

		if(p != NULL)
		{
			int len = 0;
			collision--;
			while(p)
			{
				p->Print(_buf);
				_buf->Log("->");
				p = p->next;
				len ++;
				collision++;
			}
			if(len > maxLength)
				maxLength = len;
		}
		_buf->Log("");
	}

	_buf->Log("capacity is %d", m_Capacity);
	_buf->Log("total size is %d",m_Size);
	_buf->Log("maximum linked list length is %d",maxLength);
	_buf->Log("total collison is %d",collision);
	return OK;
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
int CCallbackMap::HashCode(CCallback *callback)
{
	ASSERT(callback);
	//Bob Jenkins' 32 bit integer hash function
	uint32_t a = (uint32_t)callback->GetCallbackId();
	a = (a+0x7ed55d16) + (a<<12);
	a = (a^0xc761c23c) ^ (a>>19);
	a = (a+0x165667b1) + (a<<5);
	a = (a+0xd3a2646c) ^ (a<<9);
	a = (a+0xfd7046c5) + (a<<3);
	a = (a^0xb55a4f09) ^ (a>>16); 
	return a % m_Capacity;
}
bool CCallbackMap::Equals(CCallback *callback1, CCallback *callback2)
{
	ASSERT(callback1 && callback2);
	return callback1->Comp(callback2) == 0;
}

CCallback* CCallbackMap::CloneNode(CCallback *callback)
{
	CCallback *ptr;
	NEW(ptr,CCallback);
	ptr->Init();
	ptr->Copy(callback);
	return ptr;
}

status_t CCallbackMap::DelNode(CCallback *callback)
{
	DEL(callback);
	return OK;
}

#define _TIMEOUT 1000
status_t CCallbackMap::StartAutoTimeoutTimer()
{
	ASSERT(!IsTask(m_AutoTimeoutTimer));

	BEGIN_CLOSURE_FUNC(on_timeout)
	{
		CLOSURE_PARAM_PTR(CCallbackMap*,self,10);
		self->IncAllCallbackTime(_TIMEOUT);
		self->AutoRemoveTimeoutCallback();
		return OK;
	}
	END_CLOSURE_FUNC(on_timeout);

	CTaskTimer *timer = CTaskTimer::NewTimer(GetTaskMgr(),_TIMEOUT,false);
	timer->Callback()->SetFunc(on_timeout);
	timer->Callback()->SetParamPointer(10,this);
	m_AutoTimeoutTimer = timer->GetId();
	return OK;
}

status_t CCallbackMap::IncAllCallbackTime(int ms)
{
	BEGIN_CLOSURE(on_add)
	{
		CLOSURE_PARAM_PTR(CCallback*,cb,0);
		CLOSURE_PARAM_INT(ms,10);
		cb->IncTime(ms);
		return OK;
	}
	END_CLOSURE(on_add);
	on_add.SetParamInt(10,ms);
	this->EnumAll(&on_add);
	return OK;
}

status_t CCallbackMap::AutoRemoveTimeoutCallback()
{
	BEGIN_CLOSURE(on_remove)
	{
		CLOSURE_PARAM_PTR(CCallback*,cb,0);
		if(cb->IsTimeout())
		{		
			cb->OnTimeout();
			cb->DelClosure();
			DEL(cb);
			closure->SetParamPointer(0,NULL);
		}
		return OK;
	}
	END_CLOSURE(on_remove);
	this->EnumAll(&on_remove);
	return OK;
}

CCallback * CCallbackMap::Get(int cbid)
{
	CCallback tmp;
	tmp.Init();
	tmp.SetCallbackId(cbid);
	return this->Get(&tmp);
}
status_t CCallbackMap::AddClosure(CClosure *closure, int cbid)
{
    ASSERT(closure);
    ASSERT(closure->IsOnHeap());   
    ASSERT(!this->Get(cbid));
    CCallback *callback;
    NEW(callback,CCallback);
    callback->Init();
    callback->SetClosure(closure);
    callback->SetCallbackId(cbid);
    return this->PutPtr(callback);
}

int CCallbackMap::AllocUniqueId()
{
	m_UniqueId++;
	if(m_UniqueId == 0)
		m_UniqueId++;
    return m_UniqueId;
}

