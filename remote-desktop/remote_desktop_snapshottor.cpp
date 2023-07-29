#include "remote_desktop_snapshottor.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "common_time.h"

CRemoteDesktopSnapshottor::CRemoteDesktopSnapshottor()
{
    this->InitBasic();
}

CRemoteDesktopSnapshottor::~CRemoteDesktopSnapshottor()
{
    this->Destroy();
}

status_t CRemoteDesktopSnapshottor::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();
    this->m_name.InitBasic();
    this->m_type = 0;
    this->m_last_frame_data.InitBasic();
    this->m_last_frame_timestamp = 0;
    this->m_snapshot_min_interval = 30;
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopSnapshottor::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
/*##Begin Init##*/
    TASK_CONTAINER_INIT(_taskmgr);
    this->m_name.Init();
    this->m_last_frame_data.Init();
/*##End Init##*/
    return OK;
}

status_t CRemoteDesktopSnapshottor::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    this->m_name.Destroy();
    this->m_last_frame_data.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopSnapshottor::Copy(CRemoteDesktopSnapshottor *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_name.Copy(&_p->m_name);
    this->m_type = _p->m_type;
    this->m_last_frame_data.Copy(&_p->m_last_frame_data);
    this->m_last_frame_timestamp = _p->m_last_frame_timestamp;
    this->m_snapshot_min_interval = _p->m_snapshot_min_interval;
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopSnapshottor::Comp(CRemoteDesktopSnapshottor *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    return m_name.StrCmp(_p->GetName());
}

status_t CRemoteDesktopSnapshottor::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);

    _buf->Log("name = %s",
        m_name.StrLen()>0?m_name.CStr():"<null>"
    );
    _buf->Log("type = %d",m_type);
    _buf->Log("last_frame_data = {");
    _buf->IncLogLevel(1);
    this->m_last_frame_data.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("last_frame_timestamp = %lld",m_last_frame_timestamp);
    _buf->Log("snapshot_min_interval = %d",m_snapshot_min_interval);
/*##End Print##*/
    return OK;
}


/*@@Begin Function GetName@@*/
CMem* CRemoteDesktopSnapshottor::GetName()
{
    return &m_name;
}
/*@@End  Function GetName@@*/

/*@@Begin Function GetNameStr@@*/
const char* CRemoteDesktopSnapshottor::GetNameStr()
{
    return m_name.CStr();
}
/*@@End  Function GetNameStr@@*/

/*@@Begin Function SetName@@*/
status_t CRemoteDesktopSnapshottor::SetName(CMem *_name)
{
    ASSERT(_name);
    return this->m_name.Copy(_name);
}
/*@@End  Function SetName@@*/

/*@@Begin Function SetName-CStr@@*/
status_t CRemoteDesktopSnapshottor::SetName(const char *_name)
{
    CMem tmp(_name);
    return this->SetName(&tmp);
}
/*@@End  Function SetName-CStr@@*/

/*@@Begin Function GetLastFrameData@@*/
CRemoteDesktopPixelBuffer* CRemoteDesktopSnapshottor::GetLastFrameData()
{
    return &m_last_frame_data;
}
/*@@End  Function GetLastFrameData@@*/

/*@@Begin Function GetLastFrameTimestamp@@*/
int64_t CRemoteDesktopSnapshottor::GetLastFrameTimestamp()
{
    return m_last_frame_timestamp;
}
/*@@End  Function GetLastFrameTimestamp@@*/

/*@@Begin Function SetLastFrameData@@*/
status_t CRemoteDesktopSnapshottor::SetLastFrameData(CRemoteDesktopPixelBuffer *_last_frame_data)
{
    ASSERT(_last_frame_data);
    return this->m_last_frame_data.Copy(_last_frame_data);
}
/*@@End  Function SetLastFrameData@@*/

/*@@Begin Function SetLastFrameTimestamp@@*/
status_t CRemoteDesktopSnapshottor::SetLastFrameTimestamp(int64_t _last_frame_timestamp)
{
    this->m_last_frame_timestamp = _last_frame_timestamp;
    return OK;
}
/*@@End  Function SetLastFrameTimestamp@@*/

/*@@Begin Function GetSnapshotMinInterval@@*/
int CRemoteDesktopSnapshottor::GetSnapshotMinInterval()
{
    return m_snapshot_min_interval;
}
/*@@End  Function GetSnapshotMinInterval@@*/

/*@@Begin Function SetSnapshotMinInterval@@*/
status_t CRemoteDesktopSnapshottor::SetSnapshotMinInterval(int _snapshot_min_interval)
{
    this->m_snapshot_min_interval = _snapshot_min_interval;
    return OK;
}
/*@@End  Function SetSnapshotMinInterval@@*/

/*@@Begin Function GetType@@*/
int CRemoteDesktopSnapshottor::GetType()
{
    return m_type;
}
/*@@End  Function GetType@@*/

/*@@Begin Function SetType@@*/
status_t CRemoteDesktopSnapshottor::SetType(int _type)
{
    this->m_type = _type;
    return OK;
}
/*@@End  Function SetType@@*/
/*@@ Insert Function Here @@*/
status_t CRemoteDesktopSnapshottor::TakeSnapshot(CRemoteDesktopPixelBuffer *pixelBuf)
{
	ASSERT(!"Not expected here.");
	return ERROR;
}

status_t CRemoteDesktopSnapshottor::TakeSnapshot()
{
	if(this->TakeSnapshot(&m_last_frame_data))
	{
		CCommonTime tm;
		tm.Init();
		tm.SetNow();
		m_last_frame_timestamp = tm.GetLong();
		return OK;
	}
	return ERROR;	
}


