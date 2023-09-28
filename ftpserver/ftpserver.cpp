#include "ftpserver.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "taskftpaccepter.h"
#include "encoder.h"

CFtpServer::CFtpServer()
{
    this->InitBasic();
}

CFtpServer::~CFtpServer()
{
    this->Destroy();
}

status_t CFtpServer::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();
    this->m_configs.InitBasic();
    this->m_flags = 0;
    this->m_fix_client_ip.InitBasic();
    this->m_fix_server_ip.InitBasic();
/*##End InitBasic##*/
	m_task_accepter = 0;
	m_encoding = LOCAL_ENCODING;
    return OK;
}

status_t CFtpServer::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
/*##Begin Init##*/
    TASK_CONTAINER_INIT(_taskmgr);
    this->m_fix_client_ip.Init();
    this->m_fix_server_ip.Init();
/*##End Init##*/
	this->m_configs.Init(64);
    return OK;
}

status_t CFtpServer::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    this->m_configs.Destroy();
    this->m_fix_client_ip.Destroy();
    this->m_fix_server_ip.Destroy();
/*##End Destroy##*/
	QuitTask(&m_task_accepter);
    this->InitBasic();
    return OK;
}

/*@@Begin Function GetConfigs@@*/
CFtpConfigList* CFtpServer::GetConfigs()
{
    return &m_configs;
}
/*@@End  Function GetConfigs@@*/


/*@@Begin Function GetFlags@@*/
uint32_t CFtpServer::GetFlags()
{
    return m_flags;
}
/*@@End  Function GetFlags@@*/

/*@@Begin Function GetFixClientIp@@*/
CMem* CFtpServer::GetFixClientIp()
{
    return &m_fix_client_ip;
}
/*@@End  Function GetFixClientIp@@*/

/*@@Begin Function GetFixClientIpStr@@*/
const char* CFtpServer::GetFixClientIpStr()
{
    return m_fix_client_ip.CStr();
}
/*@@End  Function GetFixClientIpStr@@*/

/*@@Begin Function SetFixClientIp@@*/
status_t CFtpServer::SetFixClientIp(CMem *_fix_client_ip)
{
    ASSERT(_fix_client_ip);
    return this->m_fix_client_ip.Copy(_fix_client_ip);
}
/*@@End  Function SetFixClientIp@@*/

/*@@Begin Function SetFixClientIp-CStr@@*/
status_t CFtpServer::SetFixClientIp(const char *_fix_client_ip)
{
    CMem tmp(_fix_client_ip);
    return this->SetFixClientIp(&tmp);
}
/*@@End  Function SetFixClientIp-CStr@@*/

/*@@Begin Function GetFixServerIp@@*/
CMem* CFtpServer::GetFixServerIp()
{
    return &m_fix_server_ip;
}
/*@@End  Function GetFixServerIp@@*/

/*@@Begin Function GetFixServerIpStr@@*/
const char* CFtpServer::GetFixServerIpStr()
{
    return m_fix_server_ip.CStr();
}
/*@@End  Function GetFixServerIpStr@@*/

/*@@Begin Function SetFixServerIp@@*/
status_t CFtpServer::SetFixServerIp(CMem *_fix_server_ip)
{
    ASSERT(_fix_server_ip);
    return this->m_fix_server_ip.Copy(_fix_server_ip);
}
/*@@End  Function SetFixServerIp@@*/

/*@@Begin Function SetFixServerIp-CStr@@*/
status_t CFtpServer::SetFixServerIp(const char *_fix_server_ip)
{
    CMem tmp(_fix_server_ip);
    return this->SetFixServerIp(&tmp);
}
/*@@End  Function SetFixServerIp-CStr@@*/
/*@@ Insert Function Here @@*/
status_t CFtpServer::AddConfig(const char *user, const char *pass, const char *root)
{
	ASSERT(user && pass && root);
	CFtpConfig config;
	config.Init();
	config.SetUserName(user);
	config.SetPassword(pass);
	config.SetRootPath(root);
	config.mEncoding = m_encoding;
	return m_configs.Push(&config);
}

status_t CFtpServer::Start(int port)
{
	ASSERT(port > 0);
	ASSERT(!IsTask(m_task_accepter));

	CTaskFtpAccepter *boss;
	NEW(boss,CTaskFtpAccepter);
	boss->Init(GetTaskMgr(),port);
	boss->SetConfigs(&m_configs);
	boss->SetEncoding(m_encoding);
	
	if(AllowAnonymouse())
	{
		CFtpConfig *first = m_configs.GetElem(0);
		ASSERT(first);
		boss->AllowAnonymouse(1,first->mRootPath->CStr());
	}

	if(m_fix_client_ip.StrLen()>0)	
	{
		boss->SetFixedClientIp(m_fix_client_ip.CStr());	
	}	

    if(m_fix_server_ip.StrLen() > 0)
    {
        boss->SetFixedServerIp(m_fix_server_ip.CStr());
    }
	
	boss->Resume();
	return OK;
}

status_t CFtpServer::SetEncoding(int encoding)
{
	m_encoding = encoding;
	return OK;
}

