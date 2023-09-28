#include "ftputils.h"
#include "sys_log.h"

status_t CFtpUtils::ParseRequest(CFileBase *line, CFileBase *verb, CFileBase *params)
{
	ASSERT(line && verb && params);
	line->Seek(0);
	line->SetSplitChars(" ");
	line->ReadString(verb);
	line->ReadLeftStr(params,TRUE);
	params->Trim();
	return OK;
}

status_t CFtpUtils::ParsePortParams(CFileBase *params, CFileBase *ip, int32_t *port)
{
	ASSERT(params && ip && port);

	LOCAL_MEM(mem);

	params->SetSplitChars(",");
	ip->SetSize(0);
	
	params->Seek(0);
	
	params->ReadString(&mem);
	ip->Puts(&mem);
	ip->Putc('.');
	params->ReadString(&mem);
	ip->Puts(&mem);
	ip->Putc('.');
	params->ReadString(&mem);
	ip->Puts(&mem);
	ip->Putc('.');
	params->ReadString(&mem);
	ip->Puts(&mem);
	
	params->ReadString(&mem);
	int32_t p1 = atoi(mem.CStr());

	params->ReadString(&mem);
	int32_t p2 = atoi(mem.CStr());

	*port = p1 * 256 + p2;

	return OK;
}

status_t CFtpUtils::ToUnixPath(CMem *mem)
{
	ASSERT(mem);
	
	fsize_t i, len;
	len = mem->StrLen();
	char *p = mem->GetRawBuf();

	for(i = 0; i < len; i++)
	{
		if(p[i] == '\\')
			p[i] = '/';
	}

	return OK;
}

int32_t CFtpUtils::GetUniqueTcpPort()
{
	static int32_t port = 1982;
	return port++;
}

status_t CFtpUtils::FindNearestIp(CFileBase *ip_list, const char *ip, CFileBase *dst)
{
	ASSERT(ip_list && ip && dst);

	LOCAL_MEM(mem);

	int32_t max_match = 0;
	dst->StrCpy(ip);

	if(dst->StrCmp("127.0.0.1") == 0)
		return OK;

	ip_list->Seek(0);
	while(ip_list->ReadLine(&mem))
	{
		int32_t i, match = 0;
		for(i = 0; i < (int32_t)strlen(ip); i++)
		{
			if(ip[i] == mem.C(i))
				match++;
			else
				break;
		}

		if(match > max_match)
		{
			max_match = match;
			dst->StrCpy(&mem);
		}
	}

	return OK;
}

status_t CFtpUtils::TimeToFtpTimeStr(time_t _time, CFileBase *out)
{
	struct tm *tm = localtime(&_time);
	
	out->SetSize(0);
	switch(tm->tm_mon + 1)
	{
		case 1:out->Puts("Jan");break;
		case 2:out->Puts("Feb");break;
		case 3:out->Puts("Mar");break;
		case 4:out->Puts("Apr");break;
		case 5:out->Puts("May");break;
		case 6:out->Puts("Jun");break;
		case 7:out->Puts("Jul");break;
		case 8:out->Puts("Aug");break;
		case 9:out->Puts("Sep");break;
		case 10:out->Puts("Oct");break;
		case 11:out->Puts("Nov");break;
		case 12:out->Puts("Dec");break;
	}

	out->Printf(" %d ",tm->tm_mday);
	out->Printf("%02d:%02d", tm->tm_hour,tm->tm_min);
	return OK;
}

status_t CFtpUtils::ParseUrl(CMem *url,CFileBase *host, int32_t *port, CFileBase *path)
{
	ASSERT(url && host && port && path);
	host->SetSize(0);
	path->SetSize(0);
	*port = 21; //default
	LOCAL_MEM(mem);
	

	if(!url->StartWith("ftp",0,1))
	{
		path->StrCpy(url);
		return OK;
	}
	url->SetSplitChars("/:");
	url->Seek(0);
	url->ReadString(&mem); //ftp
	url->ReadString(&mem); //host
	host->StrCpy(&mem);
	fsize_t old_off = url->GetOffset();
	url->ReadWord(&mem);
	if(mem.C(0) == ':')
	{
		url->ReadString(&mem);
		*port = atoi(mem.CStr());
	}
	else
	{
		url->Seek(old_off);
	}
	url->ReadLeftStr(path,1);
	if(path->StrLen() <= 0)
		path->StrCpy("/");
	return OK;
}