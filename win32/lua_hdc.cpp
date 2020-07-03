#include "lua_hdc.h"
#include "syslog.h"
#include "mem_tool.h"

CLuaHdc::CLuaHdc()
{
    this->InitBasic();
}

CLuaHdc::~CLuaHdc()
{
    this->Destroy();
}

status_t CLuaHdc::InitBasic()
{
    WEAK_REF_CLEAR();
    hdc = NULL;
    hwnd = NULL;
    return OK;
}

status_t CLuaHdc::Init()
{
    this->InitBasic();
    
    return OK;
}

status_t CLuaHdc::Destroy()
{
    WEAK_REF_DESTROY();
    if(hdc)
    {
        ::ReleaseDC(hwnd,hdc);
        hdc = NULL;
    }
    this->InitBasic();
    return OK;
}

status_t CLuaHdc::Set(HWND hwnd, HDC hdc)
{
    ASSERT(this->hdc == NULL);
    this->hwnd = hwnd;
    this->hdc = hdc;
    return OK;
}
