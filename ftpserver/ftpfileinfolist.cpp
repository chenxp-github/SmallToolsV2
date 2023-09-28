#include "ftpfileinfolist.h"
#include "dirmgr.h"
#include "encoder.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "ftputils.h"

typedef int (*CALLBACK_FUNC)(void **);
#define CALLBACK_PARAMS(param) void *param[MAX_CALLBACK_PARAMS]


#define CALLBACK_INIT(name) \
callback_##name = NULL; \
memset(param_##name,0,sizeof(param_##name))


CFtpFileInfoList::CFtpFileInfoList()
{
	this->InitBasic();  
}
CFtpFileInfoList::~CFtpFileInfoList()
{
	this->Destroy();
}
status_t CFtpFileInfoList::InitBasic()
{
	CALLBACK_INIT(comp);
	this->mIndex = NULL;
	this->mTop = 0;;
	this->mSize = 0;
	return OK;
}
status_t CFtpFileInfoList::Init(int init_size)
{
	int i;
	this->InitBasic();  
	this->mSize = init_size;
	MALLOC(this->mIndex,CFtpFileInfo * ,this->mSize);
	for(i = 0; i < this->mSize; i++)
		this->mIndex[i] = NULL;
	return OK;
}
status_t CFtpFileInfoList::Destroy()
{
	int i;
	if(this->mIndex == NULL)
		return ERROR;
	for(i = 0; i < this->mTop; i++)
	{
		DelNode(this->mIndex[i]);
	}
	FREE(this->mIndex);
	this->InitBasic();	
	return OK;
}
bool CFtpFileInfoList::IsEmpty()
{
	return this->mTop <= 0;
}
bool CFtpFileInfoList::IsFull()
{
	return this->mTop >= this->mSize;
}

status_t CFtpFileInfoList::AutoResize()
{
	if(this->IsFull())
	{
		REALLOC(this->mIndex,CFtpFileInfo*,this->mSize,this->mSize*2);
		this->mSize *= 2;
		for(int i = this->mTop; i < this->mSize; i++)
		{
			this->mIndex[i] = NULL;
		}
	}
	return OK;
}

status_t CFtpFileInfoList::Push(CFtpFileInfo *ftpfileinfo)
{
	ASSERT(ftpfileinfo);
	CFtpFileInfo *tmp = this->CloneNode(ftpfileinfo);
	if(!this->PushPtr(tmp))
	{
		DelNode(tmp);
		return ERROR;
	}
	
	return OK;
}

status_t CFtpFileInfoList::PushPtr(CFtpFileInfo *ftpfileinfo)
{
	ASSERT(ftpfileinfo);	
	this->AutoResize();
	ASSERT(!this->IsFull());
	this->mIndex[this->mTop] = ftpfileinfo;
	this->mTop++;
	return OK;
}

CFtpFileInfo * CFtpFileInfoList::PopPtr()
{
	if(this->IsEmpty())
		return NULL;
	this->mTop--;
	return this->mIndex[this->mTop];
}

status_t CFtpFileInfoList::Pop(CFtpFileInfo *ftpfileinfo)
{
	ASSERT(!this->IsEmpty());	
	this->mTop--;
	CopyNode(ftpfileinfo,mIndex[mTop]);
	DelNode(this->mIndex[this->mTop]);
	return OK;
}
status_t CFtpFileInfoList::Print()
{
	return OK;
}
int CFtpFileInfoList::GetLen()
{
	return this->mTop;
}

int CFtpFileInfoList::SearchPos(CFtpFileInfo *ftpfileinfo)
{
	int i;
	for(i=0;i<this->mTop;i++)
	{
		if(CompNode(mIndex[i],ftpfileinfo) == 0)
			return i;
	}
	return -1;
}

CFtpFileInfo * CFtpFileInfoList::Search(CFtpFileInfo *ftpfileinfo)
{
	int pos = this->SearchPos(ftpfileinfo);
	if(pos >= 0 && pos < this->mTop)
		return this->mIndex[pos];;
	return NULL;
}
CFtpFileInfo * CFtpFileInfoList::GetTopPtr()
{
	if(this->IsEmpty())
		return NULL;
	return this->mIndex[this->mTop - 1];
}
status_t CFtpFileInfoList::DelTop()
{
	if(this->IsEmpty())
		return ERROR;
	this->mTop--;
	DelNode(this->mIndex[this->mTop]);
	return OK;
}
status_t CFtpFileInfoList::Clear()
{
	while(this->DelTop());
	return OK;
}

CFtpFileInfo * CFtpFileInfoList::GetElem(int mIndex)
{
	if(mIndex < 0 || mIndex >= this->mTop)
		return NULL;
	return this->mIndex[mIndex];
}

CFtpFileInfo * CFtpFileInfoList::BSearchNode(CFtpFileInfo *ftpfileinfo,int order)
{
	return this->GetElem(this->BSearch(ftpfileinfo,order));
}
int CFtpFileInfoList::BSearch(CFtpFileInfo *ftpfileinfo,int order)
{
	int find,pos;
	pos = this->BSearchPos(ftpfileinfo,order,&find);
	if(find) return pos;
	return -1;
}

status_t CFtpFileInfoList::InsertElemPtr(int i, CFtpFileInfo *ftpfileinfo)
{
	ASSERT(ftpfileinfo);
	ASSERT(i >= 0 && i <= this->mTop);

	this->AutoResize();
	ASSERT(!this->IsFull());
	for(int k = this->mTop; k > i; k--)
	{
		this->mIndex[k] = this->mIndex[k - 1];
	}
	this->mIndex[i] = ftpfileinfo;
	this->mTop++;
	return OK;
}

status_t CFtpFileInfoList::InsertElem(int i, CFtpFileInfo *ftpfileinfo)
{
	ASSERT(ftpfileinfo);
	CFtpFileInfo *tmp = this->CloneNode(ftpfileinfo);
	if(!this->InsertElemPtr(i,tmp))
	{
		DelNode(tmp);
		return ERROR;
	}
	return OK;
}

CFtpFileInfo * CFtpFileInfoList::RemoveElem(int index)
{
	CFtpFileInfo *p;
	
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

status_t CFtpFileInfoList::DelElem(int i)
{
	CFtpFileInfo *p = this->RemoveElem(i);
	if(p != NULL)
	{
		DelNode(p);
		return OK;
	}
	return ERROR;
}

status_t CFtpFileInfoList::InsOrderedPtr(CFtpFileInfo *ftpfileinfo, int order,int unique)
{
	int pos,find;
	pos = this->BSearchPos(ftpfileinfo,order,&find);
	if(find && unique)
		return ERROR;
	return this->InsertElemPtr(pos,ftpfileinfo);
}

status_t CFtpFileInfoList::InsOrdered(CFtpFileInfo *ftpfileinfo, int order,int unique)
{
	ASSERT(ftpfileinfo);
	int pos,find;
	pos = this->BSearchPos(ftpfileinfo,order,&find);
	if(find && unique)
		return ERROR;
	return this->InsertElem(pos,ftpfileinfo);
}

int CFtpFileInfoList::BSearchPos(CFtpFileInfo *ftpfileinfo, int order, int *find_flag)
{
	int low,high,mid,comp;
	
	low = 0; 
	high = this->GetLen() - 1;

	while(low<=high)
	{
		mid = (low+high) >> 1;

		comp = CompNode(mIndex[mid],ftpfileinfo);
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

status_t CFtpFileInfoList::Sort(int order)
{
	int i,len;
	CFtpFileInfoList tmp;

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
status_t CFtpFileInfoList::Copy(CFtpFileInfoList *stk)
{
	ASSERT(stk);
	this->Destroy();
	this->Init(stk->GetLen());
	for(int i = 0; i < stk->GetLen(); i++)
	{
		CFtpFileInfo *p = stk->GetElem(i);
		ASSERT(p);
		this->Push(p);
	}
	return OK;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t CFtpFileInfoList::CopyNode(CFtpFileInfo *dst, CFtpFileInfo *src)
{
	return dst->Copy(src);
}

CFtpFileInfo * CFtpFileInfoList::CloneNode(CFtpFileInfo *ftpfileinfo)
{
	CFtpFileInfo *tmp;
	NEW(tmp,CFtpFileInfo);
	tmp->Init();
	CopyNode(tmp,ftpfileinfo);
	return tmp;
}

status_t CFtpFileInfoList::DelNode(CFtpFileInfo *ftpfileinfo)
{
	DEL(ftpfileinfo);
	return OK;
}

int CFtpFileInfoList::CompNode(CFtpFileInfo *ftpfileinfo1, CFtpFileInfo *ftpfileinfo2)
{
	ASSERT(ftpfileinfo1 && ftpfileinfo2);
	if(this->callback_comp)
	{
		param_comp[0] = ftpfileinfo1;
		param_comp[1] = ftpfileinfo2;
		return this->callback_comp(param_comp);
	}
	else
	{
		return ftpfileinfo1->Comp(ftpfileinfo2);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
status_t CFtpFileInfoList::GetFileList(const char *dir, bool is_abs_path,bool utf8)
{
	ASSERT(dir);

	LOCAL_MEM(dir_local);
	const char *save_dir = dir;
	if(utf8)
	{
		dir_local.Puts(dir);
		CEncoder::EncodingConvert(ENCODING_UTF8,LOCAL_ENCODING,&dir_local);
		dir = dir_local.CStr();
	}

	CMem mem_dir(dir);
	if(!CDirMgr::IsDirExist(&mem_dir))
	{
		if(CDirMgr::IsFileExist(&mem_dir))
		{
			CFtpFileInfo info;
			info.Init();
			info.SetName(save_dir);
			info.SetIsDir(0);
			info.SetSize(CDirMgr::GetFileSize(dir));
			return this->Push(&info);
		}
	}

	char filename[MAX_PATH];
    char old_path[MAX_PATH];    
	void *p[MAX_CALLBACK_PARAMS];
	
	bool separator_ended = false;
	int32_t len = crt_strlen(dir);
	ASSERT(len > 0);

	if(dir[len-1] == crt_get_path_splitor())
		separator_ended = true;

	LOCAL_MEM(tmp);

    crt_get_cur_dir(old_path, MAX_PATH);
	if(crt_need_chdir_before_open())
		crt_chdir(dir);

	p[1] = filename;

	ASSERT(crt_open_dir(dir,p));	

	while(crt_read_dir(p))
	{
		if(filename[0] == '.')
			continue;
		tmp.SetSize(0);
		if(is_abs_path)
		{
			tmp.Puts(dir);
			if(!separator_ended)
				tmp.Putc(crt_get_path_splitor());
		}
		tmp.Puts(filename);

		if(utf8)
		{
			CEncoder::EncodingConvert(LOCAL_ENCODING,ENCODING_UTF8,&tmp);
		}
		
		CFtpFileInfo info;
		info.Init();
		info.SetName(&tmp);
		info.SetIsDir(p[2] != 0);
		memcpy(&info.mSize,p+3,sizeof(fsize_t));
		memcpy(&info.mLastWriteTime,p+5,sizeof(int64_t));
		this->Push(&info);
	}
	crt_close_dir(p);
	if(crt_need_chdir_before_open())
		crt_chdir(old_path);
	return OK;
}

status_t CFtpFileInfoList::SearchDir(const char *dir, bool recursive)
{
	this->Clear();
	
	this->GetFileList(dir,1);

	if(recursive)
	{
		for(int i = 0; i < this->GetLen(); i++)
		{
			CFtpFileInfo *p = this->GetElem(i);
			ASSERT(p);
			if(p->mIsDir)
			{
				this->GetFileList(p->GetName(),1);
			}
		}
	}
	return OK;
}

status_t CFtpFileInfoList::SaveToXml(CFileBase *file)
{
	ASSERT(file);
	file->SetSize(0);
	
	if(LOCAL_ENCODING == ENCODING_GBK)
		file->Puts("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	else
		file->Puts("<?xml version=\"1.0\" encoding=\"utf8\"?>\r\n");

	file->Puts("<AllFtpFileInfo>\r\n");
	for(int i = 0; i < this->GetLen(); i++)
	{
		CFtpFileInfo *p = this->GetElem(i);
		ASSERT(p);

		file->Puts("<FtpFileInfo>\r\n");
		p->SaveToXml(file);
		file->Puts("</FtpFileInfo>\r\n");
	}
	file->Puts("</AllFtpFileInfo>\r\n");

	return OK;
}

status_t CFtpFileInfoList::SaveToXml(const char *filename)
{
	CFile file;
	file.Init();
	ASSERT(file.OpenFile(filename,"wb+"));
	return this->SaveToXml(&file);
}

status_t CFtpFileInfoList::LoadFromXml(CFileBase *file)
{
	ASSERT(file);

	CXml xml;
	xml.Init();
	ASSERT(xml.LoadXml(file));

	this->Clear();

	CXmlNode *px = xml.GetRoot();
	ASSERT(px);
	px = px->GetChild(0);

	while(px)
	{
		CFtpFileInfo info;
		info.Init();
		info.LoadFromXml(px);
		this->Push(&info);
		px = px->next;
	}
	return OK;
}

status_t CFtpFileInfoList::LoadFromXml(const char *filename)
{
	CFile file;
	file.Init();
	if(file.OpenFile(filename,"rb"))
	{
		return this->LoadFromXml(&file);
	}
	return ERROR;
}

status_t CFtpFileInfoList::RemoveDirPrefix(const char *dir)
{
	ASSERT(dir);
	int len = strlen(dir);
	
	ASSERT(len > 0);
	if(dir[len-1] != '/' || dir[len-1] != '\\')
		len = len + 1;

	LOCAL_MEM(mem);

	for(int i = 0; i < this->GetLen(); i++)
	{
		CFtpFileInfo *p = this->GetElem(i);
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

static status_t comp_only_by_name(void **p)
{
	CFtpFileInfo *info1 = (CFtpFileInfo*)p[0];
	CFtpFileInfo *info2 = (CFtpFileInfo*)p[1];
	ASSERT(info1 && info2);
	return crt_stricmp(info1->GetName(),info2->GetName());
}

int CFtpFileInfoList::SearchOnlyByName(const char *name)
{
	CALLBACK_FUNC(old);
	old = this->callback_comp;
	this->callback_comp = comp_only_by_name;

	CFtpFileInfo tmp;
	tmp.Init();
	tmp.SetName(name);

	int ret = this->SearchPos(&tmp);
	
	this->callback_comp = old;
	return ret;
}
