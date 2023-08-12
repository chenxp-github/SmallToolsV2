#ifndef __LUA_HDC_H
#define __LUA_HDC_H

#include "mem.h"
#include "memfile.h"

class CLuaHdc{
public:
    WEAK_REF_DEFINE();
public:
    HWND hwnd;
    HDC hdc;
	uint32_t flags;
public:

	FLAG_FUNC(flags,IsCreate,0x00000001);

	status_t Set(HWND hwnd, HDC hdc);
    CLuaHdc();
    virtual ~CLuaHdc();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();
	status_t Create(const wchar_t *lpszDriver, const wchar_t  *lpszDevice);
};

#endif
