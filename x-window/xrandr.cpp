#include "xrandr.h"
#include "syslog.h"
#include "mem_tool.h"

CxRandr::CxRandr()
{
    this->InitBasic();
}

CxRandr::~CxRandr()
{
    this->Destroy();
}

status_t CxRandr::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
/*##End InitBasic##*/
    return OK;
}

status_t CxRandr::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CxRandr::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}


/*@@ Insert Function Here @@*/
XRRMonitorInfo *CxRandr::GetMonitors(Display *dpy, Window window, Bool get_active, int *nmonitors)
{
    ASSERT(dpy && window && nmonitors);
    return XRRGetMonitors(dpy,window,get_active,nmonitors);
}

