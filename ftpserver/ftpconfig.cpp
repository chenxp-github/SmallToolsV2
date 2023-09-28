#include "ftpconfig.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "encoder.h"

CFtpConfig::CFtpConfig()
{
	this->InitBasic();
}
CFtpConfig::~CFtpConfig()
{
	this->Destroy();
}
status_t CFtpConfig::InitBasic()
{
	this->mPassword = NULL;
	this->mUserName = NULL;
	this->mRootPath = NULL;
	mEncoding = LOCAL_ENCODING;
	return OK;
}
status_t CFtpConfig::Init()
{
	this->InitBasic();
	NEW(this->mPassword,CMem);
	this->mPassword->Init();
	this->mPassword->Malloc(128);
	
	NEW(this->mUserName,CMem);
	this->mUserName->Init();
	this->mUserName->Malloc(128);

	NEW(this->mRootPath,CMem);
	this->mRootPath->Init();
	this->mRootPath->Malloc(1024);

	return OK;
}
status_t CFtpConfig::Destroy()
{
	DEL(this->mPassword);
	DEL(this->mUserName);
	DEL(this->mRootPath);
	this->InitBasic();
	return OK;
}
status_t CFtpConfig::Copy(CFtpConfig *p)
{
	ASSERT(p);
	if(this == p)return OK;	
	this->mPassword->Copy(p->mPassword);
	this->mUserName->Copy(p->mUserName);
	this->mRootPath->Copy(p->mRootPath);
	mEncoding = p->mEncoding;
	return OK;
}
status_t CFtpConfig::Comp(CFtpConfig *p)
{
	ASSERT(p);
	return this->mUserName->StrCmp(p->mUserName);
}
status_t CFtpConfig::Print()
{
	return TRUE;
}

status_t CFtpConfig::SetUserName(const char *user_name)
{
	ASSERT(user_name);
	this->mUserName->StrCpy(user_name);
	return OK;
}

status_t CFtpConfig::SetPassword(const char *pass)
{
	ASSERT(pass);
	this->mPassword->StrCpy(pass);
	return OK;
}

status_t CFtpConfig::Empty()
{
	this->mUserName->StrCpy("");
	this->mPassword->StrCpy("");
	this->mRootPath->StrCpy("");
	return OK;
}

status_t CFtpConfig::IsEmpty()
{
	return this->mUserName->StrLen() <= 0;
}

status_t CFtpConfig::SetRootPath(const char *root)
{
	ASSERT(root);
	this->mRootPath->StrCpy(root);
	return OK;
}
