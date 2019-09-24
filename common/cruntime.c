#include "cruntime.h"

#if _LINUX_
#include "cc936.c"
#include "platform/crt_linux.c"
#endif
/////////////////////////////////////////////////////////////
#if _LINUX64_
#include "cc936.c"
#include "platform/crt_linux64.c"
#endif
/////////////////////////////////////////////////////////////
#if _WIN32_
#include "platform/crt_win32.c"
#endif
