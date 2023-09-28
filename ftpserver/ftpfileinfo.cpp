#include "ftpfileinfo.h"
#include "sys_log.h"
#include "mem_tool.h"

CFtpFileInfo::CFtpFileInfo()
{
	this->InitBasic();
}
CFtpFileInfo::~CFtpFileInfo()
{
	this->Destroy();
}
status_t CFtpFileInfo::InitBasic()
{
	this->mName = NULL;
	this->mLastWriteTime = 0;
	this->mSize = 0;
	this->mMd5Sum = NULL;
	return OK;
}
status_t CFtpFileInfo::Init()
{
	this->InitBasic();

	NEW(this->mName,CMem);
	this->mName->Init();
	this->SetName("");

	NEW(this->mMd5Sum,CMem);
	this->mMd5Sum->Init();
	this->SetMd5Sum("");

	return OK;
}
status_t CFtpFileInfo::Destroy()
{
	DEL(this->mName);
	DEL(this->mMd5Sum);
	this->InitBasic();
	return OK;
}
status_t CFtpFileInfo::Copy(CFtpFileInfo *p)
{
	ASSERT(p && this != p);
	this->SetName(p->mName);
	this->mLastWriteTime = p->mLastWriteTime;
	this->mSize = p->mSize;
	this->SetMd5Sum(p->mMd5Sum);
	this->mIsDir = p->mIsDir;
	return OK;
}
int CFtpFileInfo::Comp(CFtpFileInfo *p)
{
	ASSERT(p);

	int ret = 0;

	ret = this->mIsDir - p->mIsDir;
	if(ret != 0)return ret;

	ret = this->mName->StrICmp(p->mName);
	if(ret != 0)return ret;

	return 0;
}
status_t CFtpFileInfo::Print()
{
	return OK;
}

const char * CFtpFileInfo::GetName()
{
	return this->mName->CStr();
}
int64_t CFtpFileInfo::GetLastWriteTime()
{
	return this->mLastWriteTime;
}
fsize_t CFtpFileInfo::GetSize()
{
	return this->mSize;
}
const char * CFtpFileInfo::GetMd5Sum()
{
	return this->mMd5Sum->CStr();
}
status_t CFtpFileInfo::SetName(const char *name)
{
	CMem mem(name);
	return this->SetName(&mem);
}
status_t CFtpFileInfo::SetName(CMem *name)
{
	ASSERT(name);
	this->mName->Destroy();
	this->mName->Init();
	this->mName->Copy(name);
	return OK;
}
status_t CFtpFileInfo::SetLastWriteTime(int64_t lastwritetime)
{
	this->mLastWriteTime = lastwritetime;
	return OK;
}
status_t CFtpFileInfo::SetSize(fsize_t size)
{
	this->mSize = size;
	return OK;
}
status_t CFtpFileInfo::SetMd5Sum(const char *md5sum)
{
	CMem mem(md5sum);
	return this->SetMd5Sum(&mem);
}
status_t CFtpFileInfo::SetMd5Sum(CMem *md5sum)
{
	ASSERT(md5sum);
	this->mMd5Sum->Destroy();
	this->mMd5Sum->Init();
	this->mMd5Sum->Copy(md5sum);
	return OK;
}

status_t CFtpFileInfo::LoadFromXml(CXmlNode *node)
{
	ASSERT(node);

	LOCAL_MEM(mem);

	CXmlNode *px;

	px = node->GetChildByName("Name");
	ASSERT(px);
	px->GetStringValue(&mem);
	this->SetName(&mem);

	px = node->GetChildByName("Size");
	ASSERT(px);
	px->GetStringValue(&mem);
	this->SetSize(crt_str_to_fsize(mem.CStr()));

	px = node->GetChildByName("LastWriteTime");
	ASSERT(px);
	px->GetStringValue(&mem);
	this->SetLastWriteTime(crt_str_to_fsize(mem.CStr()));

	px = node->GetChildByName("Md5Sum");
	ASSERT(px);
	px->GetStringValue(&mem);
	this->SetMd5Sum(&mem);
	
	px = node->GetChildByName("IsDir");
	ASSERT(px);
	px->GetStringValue(&mem);
	this->SetIsDir(atoi(mem.CStr()) != 0);

	return OK;
}

status_t CFtpFileInfo::SaveToXml(CFileBase *xml)
{
	ASSERT(xml);
	char str[128];

	xml->Printf("<Name><![CDATA[%s]]></Name>\r\n",this->GetName());
	crt_fsize_to_str(this->GetSize(),str);
	xml->Printf("<Size>%s</Size>\r\n",str);
	crt_fsize_to_str(this->GetLastWriteTime(),str);
	xml->Printf("<LastWriteTime>%s</LastWriteTime>\r\n",str);
	xml->Printf("<Md5Sum>%s</Md5Sum>\r\n",this->GetMd5Sum());
	xml->Printf("<IsDir>%d</IsDir>\r\n",this->GetIsDir());
	return OK;
}

bool CFtpFileInfo::IsDir()
{
	return this->GetIsDir();
}
bool CFtpFileInfo::GetIsDir()
{
	return this->mIsDir;
}
status_t CFtpFileInfo::SetIsDir(bool isdir)
{
	this->mIsDir = isdir;
	return OK;
}
