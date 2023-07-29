#ifndef __REMOTE_DESKTOP_SNAPSHOTTOR_MANAGER_H
#define __REMOTE_DESKTOP_SNAPSHOTTOR_MANAGER_H

/*##Begin Inlcudes##*/
#include "remote_desktop_snapshottor_list.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "memstk.h"

class CRemoteDesktopSnapshottorManager
/*##Begin Bases##*/
:public CRemoteDesktopSnapshottorList
/*##End Bases##*/
{
/*##Begin Members##*/
/*##End Members##*/

public:
    CRemoteDesktopSnapshottorManager();
    virtual ~CRemoteDesktopSnapshottorManager();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

    status_t Copy(CRemoteDesktopSnapshottorManager *_p);
    int Comp(CRemoteDesktopSnapshottorManager *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/
	
	status_t TakeScreenSnapshot(CMem *name,CRemoteDesktopPixelBuffer *pixelBuffer);
	status_t TakeScreenSnapshot(const char *name,CRemoteDesktopPixelBuffer *pixelBuffer);
	status_t TakeScreenSnapshot(CRemoteDesktopPixelBuffer *pixelBuffer);
	status_t GetNameList(CMemStk *names);
};

#endif
