#include "ftpconfiglist.h"
#include "sys_log.h"
#include "mem_tool.h"

CFtpConfigList::CFtpConfigList()
{
	this->InitBasic();  
}
CFtpConfigList::~CFtpConfigList()
{
	this->Destroy();
}
status_t CFtpConfigList::InitBasic()
{
	this->mIndex = NULL;
	this->mTop = 0;;
	this->mSize = 0;
	this->callback_comp = def_comp;
	
	for(int i = 0; i < MAX_CALLBACK_PARAMS; i++)
	{
		this->param_comp[i] = NULL;
	}

	return OK;
}
status_t CFtpConfigList::Init(int32_t init_size)
{
	int32_t i;
	this->InitBasic();  
	this->mSize = init_size;
	MALLOC(this->mIndex,CFtpConfig * ,this->mSize);
	for(i = 0; i < this->mSize; i++)
		this->mIndex[i] = NULL;
	return OK;
}
status_t CFtpConfigList::Destroy()
{
	int32_t i;
	if(this->mIndex == NULL)
		return ERROR;
	for(i = 0; i < this->mSize; i++)
	{
		DEL(this->mIndex[i]);
	}
	FREE(this->mIndex);
	this->InitBasic();	
	return OK;
}
status_t CFtpConfigList::IsEmpty()
{
	return this->mTop <= 0;
}
status_t CFtpConfigList::IsFull()
{
	return this->mTop >= this->mSize;
}

status_t CFtpConfigList::AutoResize()
{
	if(this->IsFull())
	{
		REALLOC(this->mIndex,CFtpConfig *,this->mSize,this->mSize * 2);
		this->mSize *= 2;
		for(int32_t i = this->mTop; i < this->mSize; i++)
		{
			this->mIndex[i] = NULL;
		}
	}

	return OK;
}

status_t CFtpConfigList::Push(CFtpConfig *node)
{
	ASSERT(node);

	CFtpConfig *new_node;
	NEW(new_node,CFtpConfig);
	new_node->Init();
	new_node->Copy(node);
	
	return this->PushPtr(new_node);
}
status_t CFtpConfigList::PushPtr(CFtpConfig *node)
{
	ASSERT(node);	
	this->AutoResize();
	ASSERT(!this->IsFull());
	this->mIndex[this->mTop] = node;
	this->mTop++;
	return OK;
}

CFtpConfig * CFtpConfigList::PopPtr()
{
	ASSERT(!this->IsEmpty());
	this->mTop--;
	return this->mIndex[this->mTop];
}

status_t CFtpConfigList::Pop(CFtpConfig *node)
{
	ASSERT(!this->IsEmpty());
	this->mTop--;
	node->Copy(this->mIndex[this->mTop]);
	DEL(this->mIndex[this->mTop]);
	return OK;
}
status_t CFtpConfigList::Print()
{
	return OK;
}
int32_t CFtpConfigList::GetLen()
{
	return this->mTop;
}

int32_t CFtpConfigList::Search_Pos(CFtpConfig *node)
{
	int32_t i;

	ASSERT(this->callback_comp);	

	for(i=0;i<this->mTop;i++)
	{
		this->param_comp[0] = this->mIndex[i];
		this->param_comp[1] = node;
		if(this->callback_comp(this->param_comp) == 0)
			return i;
	}
	return -1;
}

CFtpConfig * CFtpConfigList::Search(CFtpConfig *node)
{
	int32_t pos = this->Search_Pos(node);
	if(pos >= 0 && pos < this->mTop)
		return this->mIndex[pos];;
	return NULL;
}
CFtpConfig * CFtpConfigList::GetTopPtr()
{
	if(this->IsEmpty())
		return NULL;
	return this->mIndex[this->mTop - 1];
}
status_t CFtpConfigList::DelTop()
{
	if(this->IsEmpty())
		return ERROR;
	this->mTop--;
	DEL(this->mIndex[this->mTop]);
	return OK;
}
status_t CFtpConfigList::Clear()
{
	while(this->DelTop());
	return OK;
}

CFtpConfig * CFtpConfigList::GetElem(int32_t mIndex)
{
	if(mIndex < 0 || mIndex >= this->mTop)
		return NULL;
	return this->mIndex[mIndex];
}

CFtpConfig * CFtpConfigList::BSearch_CFtpConfig(CFtpConfig *node,int32_t order)
{
	int32_t i;
	i = this->BSearch(node,order);
	return this->GetElem(i);
}
int32_t CFtpConfigList::BSearch(CFtpConfig *node,int32_t order)
{
	int32_t find,pos;
	pos = this->BSearch_Pos(node,order,&find);
	if(find) return pos;
	return -1;
}

status_t CFtpConfigList::InsertElemPtr(int32_t i, CFtpConfig *node)
{
	ASSERT(node);
	ASSERT(i >= 0 && i <= this->mTop);

	this->AutoResize();
	ASSERT(!this->IsFull());
	for(int32_t k = this->mTop; k > i; k--)
	{
		this->mIndex[k] = this->mIndex[k - 1];
	}
	this->mIndex[i] = node;
	this->mTop++;
	return OK;
}

status_t CFtpConfigList::InsertElem(int32_t i, CFtpConfig *node)
{
	ASSERT(node);
	CFtpConfig *new_node;
	
	NEW(new_node,CFtpConfig);
	new_node->Init();
	new_node->Copy(node);
	return this->InsertElemPtr(i,new_node);
}

CFtpConfig * CFtpConfigList::RemoveElem(int32_t index)
{
	CFtpConfig *p;
	
	ASSERT(index >= 0 && index < this->mTop);
	p = this->GetElem(index);

	for(int32_t k = index; k < this->mTop-1; k++)
	{
		this->mIndex[k] = this->mIndex[k + 1];
	}
	this->mTop --;
	this->mIndex[mTop] = NULL;
	return p;
}

status_t CFtpConfigList::DelElem(int32_t i)
{
	CFtpConfig *p = this->RemoveElem(i);
	if(p != NULL)
	{
		DEL(p);
		return OK;
	}
	return ERROR;
}

status_t CFtpConfigList::InsOrderedPtr(CFtpConfig *node, int32_t order,int32_t unique)
{
	int32_t pos,find;
	pos = this->BSearch_Pos(node,order,&find);
	if(find && unique)
		return ERROR;
	return this->InsertElemPtr(pos,node);
}

status_t CFtpConfigList::InsOrdered(CFtpConfig *node, int32_t order,int32_t unique)
{
	ASSERT(node);

	CFtpConfig *new_node;
	
	NEW(new_node,CFtpConfig);
	new_node->Init();
	new_node->Copy(node);

	return InsOrderedPtr(new_node,order,unique);
}

int32_t CFtpConfigList::BSearch_Pos(CFtpConfig *node, int32_t order, int32_t *find_flag)
{
	int32_t low,high,mid,c;

	low = 0; high=this->GetLen() - 1;
	ASSERT(this->callback_comp);	

	while(low<=high)
	{
		mid = (low+high) >> 1;

		this->param_comp[0] = node;
		this->param_comp[1] = this->mIndex[mid];

		if(this->callback_comp(this->param_comp) == 0)
		{
			*find_flag = TRUE;
			return mid;
		}

		this->param_comp[0] = this->mIndex[mid];
		this->param_comp[1] = node;
		
		if(order == 0)
			c = this->callback_comp(this->param_comp) > 0;
		else
			c = this->callback_comp(this->param_comp) < 0;

		if(c)
			high = mid-1;
		else
			low = mid+1;
	}
	*find_flag = FALSE;
	return low;
}

status_t CFtpConfigList::Sort(int32_t order)
{
	int i,len;
	CFtpConfigList tmp;

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

status_t CFtpConfigList::def_comp(void **p)
{
	ASSERT(p);
	ASSERT(p[0] && p[1]);

	CFtpConfig *node0 = (CFtpConfig *)p[0];
	CFtpConfig *node1 = (CFtpConfig *)p[1];
	return node0->Comp(node1);
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
CFtpConfig * CFtpConfigList::SearchByUserNameAndPassword(CFtpConfig *cfg)
{
	ASSERT(cfg);

	CFtpConfig *pt;
	
	for(int i = 0; i < this->GetLen(); i++)
	{
		pt = this->GetElem(i);
		ASSERT(pt);

		if(pt->mUserName->StrCmp(cfg->mUserName) == 0)
		{
			if(pt->mPassword->StrLen() <= 0)
				return pt;
			else if(pt->mPassword->StrCmp(cfg->mPassword) == 0)
				return pt;
		}
	}		

	return NULL;
}
