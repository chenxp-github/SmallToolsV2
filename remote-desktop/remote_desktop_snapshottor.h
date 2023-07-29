#ifndef __REMOTE_DESKTOP_SNAPSHOTTOR_H
#define __REMOTE_DESKTOP_SNAPSHOTTOR_H

/*##Begin Inlcudes##*/
#include "taskmgr.h"
#include "remote_desktop_pixel_buffer.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRemoteDesktopSnapshottor
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
    CRemoteDesktopPixelBuffer m_last_frame_data;
    int64_t m_last_frame_timestamp;
    int m_snapshot_min_interval;
/*##End Members##*/
public:
    CRemoteDesktopSnapshottor();
    virtual ~CRemoteDesktopSnapshottor();

    status_t InitBasic();
    status_t Init(CTaskMgr *_taskmgr);
    status_t Destroy();

    status_t Copy(CRemoteDesktopSnapshottor *_p);
    int Comp(CRemoteDesktopSnapshottor *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetName();
    const char* GetNameStr();
    CRemoteDesktopPixelBuffer* GetLastFrameData();
    int64_t GetLastFrameTimestamp();
    int GetSnapshotMinInterval();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetName(CMem *_name);
    status_t SetName(const char *_name);
    status_t SetLastFrameData(CRemoteDesktopPixelBuffer *_last_frame_data);
    status_t SetLastFrameTimestamp(int64_t _last_frame_timestamp);
    status_t SetSnapshotMinInterval(int _snapshot_min_interval);
/*##End Setter_H##*/

	virtual status_t TakeSnapshot(CRemoteDesktopPixelBuffer *pixelBuf);
	status_t TakeSnapshot();
};

#endif
