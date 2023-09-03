#include "lualib_win32.h"
#include "lualib_luahdc.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "globals.h"
#include "lualib_minibson.h"
#include "monitor_enum.h"
/****************************************************/
status_t win32_callback_onwindowmessage(lua_State *L, int _funcid, bool _once, uint32_t hwnd, uint32_t msg, uint32_t wparam, uint32_t lparam)
{
    if(_funcid == LUA_REFNIL)
    {
        return ERROR;
    }

    CLuaVm vm;
    vm.Init(L);
    lua_rawgeti(L,LUA_REGISTRYINDEX,_funcid);
    if(_once)
    {
        luaL_unref(L,LUA_REGISTRYINDEX,_funcid);
    }
    lua_pushinteger(L,hwnd);
    lua_pushinteger(L,msg);
    lua_pushinteger(L,wparam);
    lua_pushinteger(L,lparam);
    vm.Run(4,0);
    vm.ClearStack();
    return OK;
}


static status_t win32_getclientrect(lua_State *L)
{
    HWND hwnd = (HWND)lua_tointeger(L,1);
    RECT r;
    ::GetClientRect(hwnd,&r);
    lua_pushinteger(L,r.left);
    lua_pushinteger(L,r.top);
    lua_pushinteger(L,r.right);
    lua_pushinteger(L,r.bottom);
    return 4;
}

static status_t win32_getasynckeystate(lua_State *L)
{
    int vk = (int)lua_tointeger(L,1);
    int ret0 = ::GetAsyncKeyState(vk);
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t win32_getcursorpos(lua_State *L)
{
    POINT p;
    GetCursorPos(&p);
    lua_pushinteger(L,p.x);
    lua_pushinteger(L,p.y);
    return 2;
}

static status_t win32_setcursorpos(lua_State *L)
{
    int x = (int)lua_tointeger(L,1);
    int y = (int)lua_tointeger(L,2);
    int ret0 = ::SetCursorPos(x,y);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t win32_getdesktopwindow(lua_State *L)
{
    HWND ret0 = ::GetDesktopWindow();
    lua_pushinteger(L,(lua_Integer)ret0);
    return 1;
}

static status_t win32_getdc(lua_State *L)
{
    HWND hwnd = (HWND)lua_tointeger(L,1);  
    
    CLuaHdc *hdc;
    NEW(hdc,CLuaHdc);
    hdc->Init();
    hdc->Set(hwnd,::GetDC(hwnd));

    luahdc_new_userdata(L,hdc,0);
    return 1;
}

static status_t win32_getwindowdc(lua_State *L)
{
    HWND hwnd = (HWND)lua_tointeger(L,1);  
    
    CLuaHdc *hdc;
    NEW(hdc,CLuaHdc);
    hdc->Init();
    hdc->Set(hwnd,::GetWindowDC(hwnd));

    luahdc_new_userdata(L,hdc,0);
    return 1;
}

static status_t win32_getpixel(lua_State *L)
{
    CLuaHdc *hdc = get_luahdc(L,1);
    ASSERT(hdc);    
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    uint32_t ret0 = ::GetPixel(hdc->hdc,x,y);
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t win32_sendmessage(lua_State *L)
{
    HWND hwnd = (HWND)lua_tointeger(L,1);
    uint32_t message = (uint32_t)lua_tointeger(L,2);
    uint32_t wparam = (uint32_t)lua_tointeger(L,3);
    uint32_t lparam = (uint32_t)lua_tointeger(L,4);
    uint32_t ret0 = ::SendMessageW(hwnd,message,wparam,lparam);
    lua_pushinteger(L,ret0);
    return 1;
}
static status_t win32_bitblt(lua_State *L)
{
    CLuaHdc *hdc = get_luahdc(L,1);
    ASSERT(hdc);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int width = (int)lua_tointeger(L,4);
    int height = (int)lua_tointeger(L,5);
    CLuaHdc *hdc_src = get_luahdc(L,6);
    ASSERT(hdc_src);
    int left = (int)lua_tointeger(L,7);
    int top = (int)lua_tointeger(L,8);
    uint32_t op = (uint32_t)lua_tointeger(L,9);
    status_t ret0 = ::BitBlt(hdc->hdc,x,y,width,height,hdc_src->hdc,left,top,op);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t win32_getwindowrect(lua_State *L)
{
    HWND hwnd = (HWND)lua_tointeger(L,1);
	
	RECT r;
	::GetWindowRect(hwnd,&r);

	int rr[4];
	rr[0] = r.left;
	rr[1] = r.top;
	rr[2] = r.right;
	rr[3] = r.bottom;

	CLuaVm::PushIntArray(L,rr,4);
	
    return 1;
}

static status_t win32_setfocus(lua_State *L)
{
    HWND hwnd = (HWND)lua_tointeger(L,1);
	HWND r = ::SetFocus(hwnd);
	lua_pushinteger(L,(int_ptr_t)r);
	return 1;
}

static status_t win32_enablewindow(lua_State *L)
{
    int_ptr_t hwnd = (int_ptr_t)lua_tointeger(L,1);
    bool enable = lua_toboolean(L,2)!=0;
    bool ret0 = ::EnableWindow((HWND)hwnd,enable) != 0;
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t MOUSEINPUT_FromLua(lua_State *L, int tindex, MOUSEINPUT *mi)
{
	ASSERT(L && mi);

    int _top = lua_gettop(L);
	
    lua_pushstring(L,"dx");
    lua_gettable(L, tindex);
    int32_t _dx = (int32_t)lua_tointeger(L,-1);
    mi->dx = _dx;
    lua_settop(L,_top);
	
    lua_pushstring(L,"dy");
    lua_gettable(L, tindex);
    int32_t _dy = (int32_t)lua_tointeger(L,-1);
    mi->dy = _dy;
    lua_settop(L,_top);
	
    lua_pushstring(L,"mouseData");
    lua_gettable(L, tindex);
    uint32_t _mousedata = (uint32_t)lua_tointeger(L,-1);
    mi->mouseData = _mousedata;
    lua_settop(L,_top);
	
    lua_pushstring(L,"dwFlags");
    lua_gettable(L, tindex);
    uint32_t _dwflags = (uint32_t)lua_tointeger(L,-1);
    mi->dwFlags = _dwflags;
    lua_settop(L,_top);
	
    lua_pushstring(L,"time");
    lua_gettable(L, tindex);
    uint32_t _time = (uint32_t)lua_tointeger(L,-1);
    mi->time = _time;
    lua_settop(L,_top);
	
    lua_pushstring(L,"dwExtraInfo");
    lua_gettable(L, tindex);
    uint32_t _dwextrainfo = (uint32_t)lua_tointeger(L,-1);
    mi->dwExtraInfo = _dwextrainfo;
    lua_settop(L,_top);
	
    return OK;
}


static status_t KEYDBINPUT_FromLua(lua_State *L, int tindex,KEYBDINPUT *ki)
{
	ASSERT(L && ki);

    int _top = lua_gettop(L);
	
    lua_pushstring(L,"wVk");
    lua_gettable(L, tindex);
    uint16_t _wvk = (uint16_t)lua_tointeger(L,-1);
    ki->wVk = _wvk;
    lua_settop(L,_top);
	
    lua_pushstring(L,"wScan");
    lua_gettable(L, tindex);
    uint16_t _wscan = (uint16_t)lua_tointeger(L,-1);
    ki->wScan = _wscan;
    lua_settop(L,_top);
	
    lua_pushstring(L,"dwFlags");
    lua_gettable(L, tindex);
    uint32_t _dwflags = (uint32_t)lua_tointeger(L,-1);
    ki->dwFlags = _dwflags;
    lua_settop(L,_top);
	
    lua_pushstring(L,"time");
    lua_gettable(L, tindex);
    uint32_t _time = (uint32_t)lua_tointeger(L,-1);
    ki->time = _time;
    lua_settop(L,_top);
	
    lua_pushstring(L,"dwExtraInfo");
    lua_gettable(L, tindex);
    uint32_t _dwextrainfo = (uint32_t)lua_tointeger(L,-1);
    ki->dwExtraInfo = _dwextrainfo;
    lua_settop(L,_top);
	
    return OK;
}

static status_t HARDWAREINPUT_FromLua(lua_State *L, int tindex, HARDWAREINPUT *hi)
{
	ASSERT(L && hi);
    int _top = lua_gettop(L);
	
    lua_pushstring(L,"uMsg");
    lua_gettable(L, tindex);
    uint32_t _umsg = (uint32_t)lua_tointeger(L,-1);
    hi->uMsg = _umsg;
    lua_settop(L,_top);
	
    lua_pushstring(L,"wParamL");
    lua_gettable(L, tindex);
    uint16_t _wparaml = (uint16_t)lua_tointeger(L,-1);
    hi->wParamL = _wparaml;
    lua_settop(L,_top);
	
    lua_pushstring(L,"wParamH");
    lua_gettable(L, tindex);
    uint16_t _wparamh = (uint16_t)lua_tointeger(L,-1);
    hi->wParamH = _wparamh;
    lua_settop(L,_top);
	return OK;
}

static status_t INPUT_FromLua(lua_State *L, int tindex, INPUT *in)
{
	ASSERT(L && in);
    int _top = lua_gettop(L);
	
    lua_pushstring(L,"type");
    lua_gettable(L, tindex);
    uint32_t _type = (uint32_t)lua_tointeger(L,-1);
    in->type = _type;
    lua_settop(L,_top);

	if(in->type == INPUT_MOUSE)
    {
		lua_pushstring(L,"mi");
		lua_gettable(L,tindex);
		MOUSEINPUT_FromLua(L,lua_gettop(L),&in->mi);
		lua_settop(L,_top);
	}
	
	if(in->type == INPUT_KEYBOARD)
    {
		lua_pushstring(L,"ki");
		lua_gettable(L,tindex);
		KEYDBINPUT_FromLua(L,lua_gettop(L),&in->ki);
		lua_settop(L,_top);
	}
	
	if(in->type == INPUT_HARDWARE)
    {
		lua_pushstring(L,"hi");
		lua_gettable(L,tindex);
		HARDWAREINPUT_FromLua(L,lua_gettop(L),&in->hi);
		lua_settop(L,_top);
	}

    return OK;
}

static status_t win32_sendinput(lua_State *L)
{
	if(!lua_istable(L,1))
        return 0;
	
	int k = 0;
	INPUT input_buf[64];
	memset(input_buf,0,sizeof(input_buf));

    int top = lua_gettop(L);
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) 
    {
		ASSERT(k < 64);		
		INPUT_FromLua(L,lua_gettop(L),&input_buf[k++]);
        lua_pop(L, 1);
    }
    lua_settop(L,top);

    int ret0 = SendInput(k,input_buf,sizeof(INPUT));
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t win32_mapvirtualkey(lua_State *L)
{
    uint32_t ucode = (uint32_t)lua_tointeger(L,1);
    uint32_t umaptype = (uint32_t)lua_tointeger(L,2);
    uint32_t ret0 = ::MapVirtualKey(ucode,umaptype);
    lua_pushinteger(L,ret0);
    return 1;
}
static status_t win32_getsystemmetrics(lua_State *L)
{
    int index = (int)lua_tointeger(L,1);
    int ret0 = ::GetSystemMetrics(index);
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t win32_getdevicecaps(lua_State *L)
{
    CLuaHdc *hdc = get_luahdc(L,1);
    ASSERT(hdc);
    int index = (int)lua_tointeger(L,2);
    int ret0 = ::GetDeviceCaps(hdc->hdc,index);
    lua_pushinteger(L,ret0);
    return 1;
}
static status_t win32_movewindow(lua_State *L)
{
    uint32_t hwnd = (uint32_t)lua_tointeger(L,1);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int nwidth = (int)lua_tointeger(L,4);
    int nheight = (int)lua_tointeger(L,5);
    int brepaint = lua_toboolean(L,6);
    BOOL ret0 = ::MoveWindow((HWND)hwnd,x,y,nwidth,nheight,brepaint!=0);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t win32_getalldisplaymonitors(lua_State *L)
{
	CMiniBson *bson;
	NEW(bson,CMiniBson);
	bson->Init();
	bson->StartDocument();
	GetAllDisplayMonitors(bson);
	bson->EndDocument();
    minibson_new_userdata(L,bson,0);
    return 1;
}

extern HWND GetConsoleWindow();
static status_t win32_getconsolewindow(lua_State *L)
{
    HWND ret0 = GetConsoleWindow();
    lua_pushinteger(L,(lua_Integer)ret0);
    return 1;
}

static status_t win32_showwindow(lua_State *L)
{
    HWND hwnd = (HWND)lua_tointeger(L,1);
    uint32_t ncmdshow = (uint32_t)lua_tointeger(L,2);
    int ret0 = ::ShowWindow(hwnd,ncmdshow);
    lua_pushinteger(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg win32_funcs_[] = {
    {"GetAsyncKeyState",win32_getasynckeystate},
    {"GetClientRect",win32_getclientrect},
    {"GetCursorPos",win32_getcursorpos},
    {"SetCursorPos",win32_setcursorpos},
    {"GetDesktopWindow",win32_getdesktopwindow},
    {"GetDC",win32_getdc},
	{"GetWindowDC",win32_getdc},
    {"GetPixel",win32_getpixel},	
    {"SendMessage",win32_sendmessage},
    {"BitBlt",win32_bitblt},	
    {"GetWindowRect",win32_getwindowrect},    
    {"SetFocus",win32_setfocus},	
    {"EnableWindow",win32_enablewindow},
    {"SendInput",win32_sendinput},	
    {"MapVirtualKey",win32_mapvirtualkey},
    {"GetSystemMetrics",win32_getsystemmetrics},
    {"GetDeviceCaps",win32_getdevicecaps},
	{"MoveWindow",win32_movewindow},
    {"GetAllDisplayMonitors",win32_getalldisplaymonitors},
	{"GetConsoleWindow",win32_getconsolewindow},
    {"ShowWindow",win32_showwindow},
    {NULL,NULL},
};

const luaL_Reg* get_win32_funcs()
{
    return win32_funcs_;
}

static int luaL_register_win32(lua_State *L)
{	
    static luaL_Reg _win32_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_win32_funcs_,&_index,get_win32_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_WIN32);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_win32_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_win32_funcs_);
    return 1;
}        

int luaopen_win32(lua_State *L)
{
    luaL_requiref(L, "Win32",luaL_register_win32,1);
    lua_pop(L, 1);
    return 0;
}        

