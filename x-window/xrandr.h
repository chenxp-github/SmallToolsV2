#ifndef __X_RANDR_H
#define __X_RANDR_H

#include <X11/extensions/Xrandr.h>

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CxRandr
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

/*##End Members##*/

public:
    CxRandr();
    virtual ~CxRandr();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

/*##Begin Getter_H##*/
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/

    static XRRMonitorInfo *GetMonitors(Display *dpy, Window window, Bool get_active, int *nmonitors);

};

#endif
