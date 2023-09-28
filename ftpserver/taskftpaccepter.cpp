#include "taskftpaccepter.h"
#include "taskftpserver.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "encoder.h"

CTaskFtpAccepter::CTaskFtpAccepter()
{
	this->InitBasic();
}
CTaskFtpAccepter::~CTaskFtpAccepter()
{
	this->Destroy();
}
status_t CTaskFtpAccepter::InitBasic()
{
	CTask::InitBasic();
	this->mServer = NULL;
	this->iConfigs = NULL;
	this->mAllowAnonymous = FALSE;
	this->mAnonymousRoot = NULL;
	this->mFixedClientIp = NULL;
	mEncoding = LOCAL_ENCODING;
	this->mFixedServerIp = NULL;
	return OK;
}

status_t CTaskFtpAccepter::Init(CTaskMgr *mgr,int port)
{
	this->InitBasic();
	CTask::Init(mgr);
	NEW(this->mServer,CTcpServer);
	this->mServer->Init();
	this->mServer->InitServer();
	this->mServer->SetPort(port);
	this->mServer->SetBlocking(0);

	NEW(this->mAnonymousRoot,CMem);
	this->mAnonymousRoot->Init();
	this->mAnonymousRoot->Malloc(1024);


	NEW(this->mFixedClientIp,CMem);
	this->mFixedClientIp->Init();
	this->mFixedClientIp->Malloc(1024);

	NEW(this->mFixedServerIp,CMem);
	this->mFixedServerIp->Init();
	this->mFixedServerIp->Malloc(1024);

	return OK;
}

status_t CTaskFtpAccepter::Destroy()
{
	DEL(this->mFixedServerIp);
	DEL(this->mFixedClientIp);
	DEL(this->mAnonymousRoot);
	DEL(this->mServer);
	CTask::Destroy();
	this->InitBasic();
	return OK;
}

status_t CTaskFtpAccepter::Run(uint32_t interval)
{
	int32_t snum = this->mServer->Accept();
	if(snum > 0)
	{
		LOCAL_MEM(mem);
		int32_t port;

		this->mServer->GetClientIpAndPort(&mem,&port);

		if(mFixedClientIp->StrLen() > 0)
		{
			mem.StrCpy(mFixedClientIp);
		}

		ASSERT(this->iConfigs);
		CTaskFtpServer *ftp;
		NEW(ftp,CTaskFtpServer);
		ftp->Init(this->GetTaskMgr());
		ftp->SetServerSocket(snum,mem.CStr());
		ftp->Start();
		ftp->SetConfigs(this->iConfigs);
		ftp->AllowAnonymouse(this->mAllowAnonymous);
		ftp->SetAnonymouseRootPath(this->mAnonymousRoot->CStr());
		ftp->SetEncoding(mEncoding);
		if(mFixedServerIp->StrLen() > 0)
		{
			ftp->SetFixServerIp(mFixedServerIp->CStr());
		}
		ftp->PrepareResponse("220 moon ftp server by chenxp");

		XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
			"new ftp server %d, %s:%d",ftp->GetId(),mem.CStr(),port
		);
	}
	
	this->Sleep(10);
	return OK;
}

status_t CTaskFtpAccepter::SetConfigs(CFtpConfigList *i_list)
{
	this->iConfigs = i_list;
	return OK;
}

status_t CTaskFtpAccepter::AllowAnonymouse(status_t enable, const char *root)
{
	ASSERT(root);
	this->mAnonymousRoot->StrCpy(root);
	this->mAllowAnonymous = enable;
	return OK;
}
status_t CTaskFtpAccepter::SetFixedClientIp(const char *ipaddr)
{
	ASSERT(ipaddr);
	this->mFixedClientIp->StrCpy(ipaddr);
	return OK;
}
status_t CTaskFtpAccepter::SetFixedServerIp(const char *ipaddr)
{
	ASSERT(ipaddr);
	this->mFixedServerIp->StrCpy(ipaddr);
	return OK;
}
status_t CTaskFtpAccepter::SetEncoding(int encoding)
{
	mEncoding = encoding;
	return OK;
}

