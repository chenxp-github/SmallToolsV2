#ifndef __FTPCONFIG_H
#define __FTPCONFIG_H

#include "mem.h"
#include "memstk.h"

class CFtpConfig{
public:
	CMem *mUserName;
	CMem *mPassword;
	CMem *mRootPath;
	int mEncoding;
public:
	status_t SetRootPath(const char *root);
	status_t IsEmpty();
	status_t Empty();
	status_t SetPassword(const char *pass);
	status_t SetUserName(const char *user_name);
	CFtpConfig();
	virtual ~CFtpConfig();
	status_t Init();
	status_t Destroy();
	status_t Copy(CFtpConfig *p);
	status_t Comp(CFtpConfig *p);
	status_t Print();
	status_t InitBasic();
};

#endif