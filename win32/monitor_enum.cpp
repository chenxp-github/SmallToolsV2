#include "monitor_enum.h"

#undef WINVER
#define WINVER 0x500
#include "minibson.h"
#include "sys_log.h"

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData)
{
	MONITORINFOEX mi;
	mi.cbSize=sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor,&mi);
	
	void **params = (void**)dwData;
	ASSERT(params);
	
	LPARAM index = (LPARAM)params[0];

	params[0] = (void*)(index+1);
	CMiniBson *bson = (CMiniBson*)params[1];
	ASSERT(bson);
	
	fsize_t off0;
	bson->StartDocument(index,&off0);

	bson->PutString("szDevice",mi.szDevice);
	bson->PutInt32("dwFlags",mi.dwFlags);

	fsize_t off;
	bson->StartDocument("rcMonitor",&off);
	bson->PutInt32("left",mi.rcMonitor.left);
	bson->PutInt32("top",mi.rcMonitor.top);
	bson->PutInt32("right",mi.rcMonitor.right);
	bson->PutInt32("bottom",mi.rcMonitor.bottom);
	bson->EndDocument(off);


	bson->StartDocument("rcWork",&off);
	bson->PutInt32("left",mi.rcWork.left);
	bson->PutInt32("top",mi.rcWork.top);
	bson->PutInt32("right",mi.rcWork.right);
	bson->PutInt32("bottom",mi.rcWork.bottom);
	bson->EndDocument(off);

	bson->EndDocument(off0);
	return true;
}

int GetAllDisplayMonitors(CMiniBson *bson)
{
	ASSERT(bson);
	fsize_t off;
	bson->StartArray("monitors",&off);
	void *params[2];
	params[0] = 0;
	params[1] = (void*)bson;
	EnumDisplayMonitors(NULL,NULL,MonitorEnumProc,(LPARAM)params);
	bson->EndArray(off,(int_ptr_t)params[0]);
	return OK;
}

