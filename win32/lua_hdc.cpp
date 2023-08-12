#include "lua_hdc.h"
#include "sys_log.h"
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
	flags = 0;
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
		if(IsCreate())
		{
			::DeleteDC(hdc);
		}
        else
		{
			::ReleaseDC(hwnd,hdc);
		}
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

status_t CLuaHdc::Create(const wchar_t *lpszDriver, const wchar_t  *lpszDevice)
{
	this->hdc = ::CreateDCW(lpszDriver,lpszDevice,NULL,NULL);
	this->SetIsCreate(true);
	return this->hdc!=0;
}

