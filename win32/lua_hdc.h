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
public:
	status_t Set(HWND hwnd, HDC hdc);
    CLuaHdc();
    virtual ~CLuaHdc();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();
};

#endif
