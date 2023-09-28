#ifndef __TASKFTPACCEPTER_H
#define __TASKFTPACCEPTER_H

#include "taskmgr.h"
#include "tcp.h"
#include "ftpconfiglist.h"

class CTaskFtpAccepter:public CTask{
public:
	CTcpServer *mServer;
	CFtpConfigList *iConfigs;	
	CMem *mAnonymousRoot;
	CMem *mFixedClientIp;
	CMem *mFixedServerIp;
	status_t mAllowAnonymous;
	int mEncoding;
public:
	status_t SetFixedServerIp(const char *ipaddr);
	status_t SetFixedClientIp(const char *ipaddr);
	status_t AllowAnonymouse(status_t enable, const char *root);
	status_t SetConfigs(CFtpConfigList *i_list);
	CTaskFtpAccepter();
	virtual ~CTaskFtpAccepter();
	status_t Init(CTaskMgr *mgr,int port);
	status_t Destroy();
	status_t InitBasic();
	status_t Run(uint32_t interval);
	status_t SetEncoding(int encoding);
};

#endif
