#ifndef __CONFIG_H
#define __CONFIG_H

#define _LINUX_     1 
#define _WIN32_     0 
#define _WM6X_      0
#define _WIN64_     0
#define _MAC64_     0
#define _IOS32_     0
#define _GBAVM_     0
#define _WINCE_     0

#ifdef ANDROID_NDK
#define _ANDROID_   1
#endif

#define _ASC_		1
#define _UNICODE_	1
#define _DEBUG_		0

#ifdef __x86_64__
#define _LINUX64_   1
#undef _LINUX_
#undef _WIN32_
#endif

#ifdef __aarch64__
#define _LINUX64_   1
#undef _LINUX_
#undef _WIN32_
#endif

#ifdef WIN32 
#undef _LINUX_
#undef  _LINUX64_
#undef _WIN32_
#define _WIN32_ 1
#endif

#if _ANDROID_
#undef _LINUX64_
#undef _LINUX_
#endif

#if _LINUX_ || _LINUX64_ || _ANDROID_
#define _IS_LINUX_ 1
#endif

#if _WIN32_ || _WIN64_ 
#define _IS_WINDOWS_ 1
#endif

#ifdef USE_X11
#define _SUPPORT_X11_ 1
#endif

#endif


