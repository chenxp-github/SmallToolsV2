#ifndef __FTP_SERVER_H
#define __FTP_SERVER_H

#include "taskmgr.h"
#include "ftpconfiglist.h"
#include "mem.h"
#include "memfile.h"

class CFtpServer
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();
    TASK_CONTAINER_DEFINE();

public:
    CFtpConfigList m_configs;
    uint32_t m_flags;
    CMem m_fix_client_ip;
    CMem m_fix_server_ip;
/*##End Members##*/
	int m_task_accepter;
	int m_encoding;
public:
	FLAG_FUNC(m_flags,AllowAnonymouse,0x00000001);

    CFtpServer();
    virtual ~CFtpServer();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

/*##Begin Getter_H##*/
    CFtpConfigList* GetConfigs();
    uint32_t GetFlags();
    CMem* GetFixClientIp();
    const char* GetFixClientIpStr();
    CMem* GetFixServerIp();
    const char* GetFixServerIpStr();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetFixClientIp(CMem *_fix_client_ip);
    status_t SetFixClientIp(const char *_fix_client_ip);
    status_t SetFixServerIp(CMem *_fix_server_ip);
    status_t SetFixServerIp(const char *_fix_server_ip);
/*##End Setter_H##*/

	status_t AddConfig(const char *user, const char *pass, const char *root);
	status_t Start(int port);
	status_t SetEncoding(int encoding);
};

#endif
