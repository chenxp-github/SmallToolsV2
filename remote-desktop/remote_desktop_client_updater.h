#ifndef __REMOTE_DESKTOP_CLIENT_UPDATER_H
#define __REMOTE_DESKTOP_CLIENT_UPDATER_H

/*##Begin Inlcudes##*/
#include "taskmgr.h"
#include "remote_desktop_pixel_buffer.h"
#include "remote_desktop_client.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "minitask.h"
#include "memstk.h"
#include "closure.h"
#include "remote_desktop_input.h"

class CRetVal_RdGetNextScreen;
class CRemoteDesktopClientUpdater
/*##Begin Bases##*/
/*##End Bases##*/
{
public:
	enum{
		EVENT_RECV_NEW_SCREEN=1,
		EVENT_RECV_MONITOR_NAMES,
	};
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();
    TASK_CONTAINER_DEFINE();

public:
    int m_frame_no;
    CRemoteDesktopPixelBuffer m_cur_screen;
    CRemoteDesktopClient *m_client;
    int m_remote_monitor_index;
    int64_t m_total_transfer_size;
    CMemStk m_remote_monitor_names;
/*##End Members##*/
	int m_auto_update_task;
	CClosure m_callback;
	int m_delay_per_frame;
	int m_byte_per_pixel;
public:
    CRemoteDesktopClientUpdater();
    virtual ~CRemoteDesktopClientUpdater();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

    status_t Copy(CRemoteDesktopClientUpdater *_p);
    int Comp(CRemoteDesktopClientUpdater *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetFrameNo();
    CRemoteDesktopPixelBuffer* GetCurScreen();
    CRemoteDesktopClient* GetClient();
    int GetRemoteMonitorIndex();
    int64_t GetTotalTransferSize();
    CMemStk* GetRemoteMonitorNames();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetFrameNo(int _frame_no);
    status_t SetClient(CRemoteDesktopClient *_client);
    status_t SetRemoteMonitorIndex(int _remote_monitor_index);
    status_t SetTotalTransferSize(int64_t _total_transfer_size);
/*##End Setter_H##*/

	status_t StartAutoUpdateTask();
	status_t StopAutoUpdateTask();
	status_t AutoUpdateTaskRun(CMiniTask *task, uint32_t interval);
	status_t OnGetNextScreen(CRetVal_RdGetNextScreen *ret);
	CClosure *Callback();
	status_t NotifyRecvNewScreen();

	status_t SendInput(CRemoteDesktopInput *input);
	status_t SetDelayPerFrame(uint32_t ms);
	status_t SetBytePerPixel(int bpp);
};

#endif
