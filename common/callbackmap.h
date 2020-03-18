#ifndef __CALLBACKMAP_H
#define __CALLBACKMAP_H
#include "cruntime.h"
#include "filebase.h"
#include "callback.h"
#include "closure.h"
#include "taskmgr.h"

class CCallbackMap{
public:
	TASK_CONTAINER_DEFINE();
    WEAK_REF_DEFINE();
public:
	int m_AutoTimeoutTimer;
	CCallback **m_Data;
	int m_Capacity;
	int m_Size;
	int m_UniqueId;
public:	
	status_t AddClosure(CClosure *closure, int cbid);
	CCallback * Get(int cbid);
	status_t AutoRemoveTimeoutCallback();
	status_t IncAllCallbackTime(int ms);
	status_t StartAutoTimeoutTimer();
	bool IsEmpty();
	int GetSize();
	status_t Del(CCallback *key);
	CCallback* Remove(CCallback *key);
	status_t EnumAll(CClosure *closure);		
	CCallback* Get(CCallback *key);	
	status_t Put(CCallback *callback);
	status_t PutPtr(CCallback *ptr);
	int HashCode(CCallback *callback);
	static bool Equals(CCallback *callback1, CCallback *callback2);	
	static CCallback * CloneNode(CCallback *callback);
	status_t DelNode(CCallback *callback);
	CCallbackMap();
	virtual ~CCallbackMap();
	status_t InitBasic();
	status_t Init(CTaskMgr *mgr,int capacity);
	status_t Destroy();
	status_t Print(CFileBase *_buf);
	int AllocUniqueId();
};

#endif
