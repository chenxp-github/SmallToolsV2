#include "remote_desktop_snapshottor_manager.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "common_time.h"

CRemoteDesktopSnapshottorManager::CRemoteDesktopSnapshottorManager()
{
    this->InitBasic();
}

CRemoteDesktopSnapshottorManager::~CRemoteDesktopSnapshottorManager()
{
    this->Destroy();
}

status_t CRemoteDesktopSnapshottorManager::InitBasic()
{
/*##Begin InitBasic##*/
    CRemoteDesktopSnapshottorList::InitBasic();
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopSnapshottorManager::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
/*##Begin Init##*/
    CRemoteDesktopSnapshottorList::Init(_taskmgr);
/*##End Init##*/
    return OK;
}

status_t CRemoteDesktopSnapshottorManager::Destroy()
{
/*##Begin Destroy##*/
    CRemoteDesktopSnapshottorList::Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopSnapshottorManager::Copy(CRemoteDesktopSnapshottorManager *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRemoteDesktopSnapshottorList::Copy(_p);
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopSnapshottorManager::Comp(CRemoteDesktopSnapshottorManager *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopSnapshottorManager::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRemoteDesktopSnapshottorList::Print(_buf);
/*##End Print##*/
    return OK;
}

/*@@ Insert Function Here @@*/

status_t CRemoteDesktopSnapshottorManager::TakeScreenSnapshot(CMem *name,CRemoteDesktopPixelBuffer *pixelBuffer)
{
	ASSERT(name && pixelBuffer);

	CRemoteDesktopSnapshottor *snapshottor = this->GetByName(name);
	ASSERT(snapshottor);
	
	CCommonTime tm;
	tm.Init();
	tm.SetNow();

	int64_t now = tm.GetLong();
	if(now - snapshottor->GetLastFrameTimestamp() >= snapshottor->GetSnapshotMinInterval())
	{
		snapshottor->TakeSnapshot();
	}

	pixelBuffer->Copy(snapshottor->GetLastFrameData());
	return OK;	
}

status_t CRemoteDesktopSnapshottorManager::TakeScreenSnapshot(const char *name,CRemoteDesktopPixelBuffer *pixelBuffer)
{	
	ASSERT(name && pixelBuffer);
	CMem mem(name);
	return this->TakeScreenSnapshot(&mem,pixelBuffer);
}

status_t CRemoteDesktopSnapshottorManager::TakeScreenSnapshot(CRemoteDesktopPixelBuffer *pixelBuffer)
{
	ASSERT(pixelBuffer);

	CRemoteDesktopSnapshottor *snapshottor = this->GetElem(0);
	ASSERT(snapshottor);

	CCommonTime tm;
	tm.Init();
	tm.SetNow();
	
	int64_t now = tm.GetLong();
	if(now - snapshottor->GetLastFrameTimestamp() >= snapshottor->GetSnapshotMinInterval())
	{
		snapshottor->TakeSnapshot();
	}
	
	pixelBuffer->Copy(snapshottor->GetLastFrameData());
	return OK;	
}

status_t CRemoteDesktopSnapshottorManager::GetNameList(CMemStk *names)
{
	ASSERT(names);
	for(int i = 0; i < this->GetLen(); i++)
	{
		CRemoteDesktopSnapshottor *p = this->GetElem(i);
		names->Push(p->GetName());
	}
	return OK;
}

