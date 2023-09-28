#ifndef __FTPUTILS_H
#define __FTPUTILS_H

#include "mem.h"
#include "memfile.h"

#define  MAX_CALLBACK_PARAMS 16

class CFtpUtils{
public:
	static status_t ParseUrl(CMem *url,CFileBase *host, int32_t *port, CFileBase *path);
	static status_t TimeToFtpTimeStr(time_t tm,CFileBase *out);
	static status_t FindNearestIp(CFileBase *ip_list, const char *ip, CFileBase *dst);
	static status_t ToUnixPath(CMem *mem);
	static status_t ParsePortParams(CFileBase *params, CFileBase *ip, int32_t  *port);
	static status_t ParseRequest(CFileBase *line, CFileBase *verb, CFileBase *params);
	static int32_t GetUniqueTcpPort();
};

#endif
