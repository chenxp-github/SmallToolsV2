#ifndef __REMOTE_DESKTOP_SESSION_H
#define __REMOTE_DESKTOP_SESSION_H

/*##Begin Inlcudes##*/
#include "taskmgr.h"
#include "remote_desktop_pixel_buffer.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "remote_desktop_pixel_buffer_list.h"

class CRemoteDesktopSession
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();
    TASK_CONTAINER_DEFINE();

public:
    CMem m_name;
    uint32_t m_flags;
    uint32_t m_lifetime;
    CRemoteDesktopPixelBuffer m_cur_screen;
    int m_frameNo;
/*##End Members##*/

public:
    CRemoteDesktopSession();
    virtual ~CRemoteDesktopSession();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

    status_t Copy(CRemoteDesktopSession *_p);
    int Comp(CRemoteDesktopSession *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetName();
    const char* GetNameStr();
    uint32_t GetFlags();
    uint32_t GetLifetime();
    CRemoteDesktopPixelBuffer* GetCurScreen();
    int GetFrameNo();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetName(CMem *_name);
    status_t SetName(const char *_name);
    status_t SetFlags(uint32_t _flags);
    status_t SetLifetime(uint32_t _lifetime);
    status_t SetCurScreen(CRemoteDesktopPixelBuffer *_cur_screen);
    status_t SetFrameNo(int _frameno);
/*##End Setter_H##*/
	status_t IncLifetime(uint32_t ms);
};

#endif
