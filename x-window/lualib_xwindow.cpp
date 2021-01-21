#include "lualib_xwindow.h"
#include "lualib_xdisplay.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CxWindow,xwindow)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CxWindow,xwindow)
LUA_NEW_USER_DATA_FUNC(CxWindow,xwindow,XWINDOW)
LUA_GC_FUNC(CxWindow,xwindow)
LUA_IS_SAME_FUNC(CxWindow,xwindow)
LUA_TO_STRING_FUNC(CxWindow,xwindow)

bool is_xwindow(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XWINDOW,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return xwindow_is_userdata_valid(ud);  
}
/****************************************/
static int xwindow_new(lua_State *L)
{
    CxWindow *pt;
    NEW(pt,CxWindow);
    pt->Init();
    xwindow_new_userdata(L,pt,0);
    return 1;
}

static int xwindow_fullscreen(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int fullscreen = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pxwindow->FullScreen(fullscreen);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_minimize(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int minm = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pxwindow->Minimize(minm);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_maximize(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int maxm = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pxwindow->Maximize(maxm);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_gettoplevelwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);

    CxWindow *win;
    NEW(win,CxWindow);
    win->Init();

    if(pxwindow->GetTopLevelWindow(win))
    {
        xwindow_new_userdata(L,win,0);
        return 1;
    }
    else
    {
        DEL(win);
        return 0;
    }
}
static int xwindow_printwindowtree(lua_State *L)
{
    CxWindow *root = get_xwindow(L,1);
    ASSERT(root);
    int _ret_0 = (int)CxWindow::PrintWindowTree(root);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_lower(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->Lower();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_raise(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->Raise();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_setbounds(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int width = (int)lua_tointeger(L,4);
    int height = (int)lua_tointeger(L,5);
    int _ret_0 = (int)pxwindow->SetBounds(x,y,width,height);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_getbounds(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_1=0;
    int _ret_2=0;
    int _ret_3=0;
    int _ret_4=0;
    if(pxwindow->GetBounds(&_ret_1,&_ret_2,&_ret_3,&_ret_4))
    {
        lua_pushinteger(L,(int)_ret_1);
        lua_pushinteger(L,(int)_ret_2);
        lua_pushinteger(L,(int)_ret_3);
        lua_pushinteger(L,(int)_ret_4);
        return 4;
    }
    else
    {
        return 0;
    }
}
static int xwindow_hide(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->Hide();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_show(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->Show();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_getdisplay(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    CxDisplay *_ret_0 = pxwindow->GetDisplay();
    if(_ret_0==NULL)return 0;
    xdisplay_new_userdata(L,_ret_0,1);
    return 1;
}

static status_t xwindow_callback_on_traverse_window_tree(lua_State *L, int _cb_id, bool _once,CxWindow *win,int level,status_t *_ret0)
{
    if(_cb_id == LUA_REFNIL)
        return ERROR;
    ASSERT(L);
    ASSERT(win);
    ASSERT(_ret0);

    CLuaVm vm;
    vm.Init(L);

    lua_rawgeti(L,LUA_REGISTRYINDEX,_cb_id);
    if(_once)luaL_unref(L,LUA_REGISTRYINDEX,_cb_id);
    xwindow_new_userdata(L,win,true);
    lua_pushinteger(L,level);
    vm.Run(2,1);
    *_ret0 = (status_t)lua_toboolean(L,-1);
    vm.ClearStack();
    return OK;
}

static int xwindow_traversewindowtree(lua_State *L)
{
    CxWindow *root = get_xwindow(L,1);
    ASSERT(root);
    int on_traverse_window_tree =CLuaVm::ToFunction(L,2);
    int level = (int)lua_tointeger(L,3);

    BEGIN_CLOSURE(traverse)
    {
        CLOSURE_PARAM_PTR(CxWindow*,win,1);
        CLOSURE_PARAM_INT(level,2);
        CLOSURE_PARAM_PTR(lua_State*,L,10);
        CLOSURE_PARAM_INT(on_traverse_window_tree,11);

        status_t ret = FALSE;

        xwindow_callback_on_traverse_window_tree(
            L,on_traverse_window_tree,false,win,level,&ret
        );

        return ret;
    }
    END_CLOSURE(traverse);

    traverse.SetParamPointer(10,L);
    traverse.SetParamInt(11,on_traverse_window_tree);

    int _ret_0 = (int)CxWindow::TraverseWindowTree(root,&traverse,level);
    CLuaVm::ReleaseFunction(L,on_traverse_window_tree);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int xwindow_setdockmode(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int dock = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pxwindow->SetDockMode(dock);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_getchildwindowslen(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->GetChildWindowsLen();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xwindow_getchildwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int index = (int)lua_tointeger(L,2)-1; //lua index is from 1

    CxWindow *win;
    NEW(win,CxWindow);
    win->Init();

    if(pxwindow->GetChildWindow(index,win))
    {
        xwindow_new_userdata(L,win,0);
        return 1;
    }
    else
    {
        DEL(win);
        return 0;
    }
}
static int xwindow_clear(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->Clear();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_getparentwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);

    CxWindow *win;
    NEW(win,CxWindow);
    win->Init();

    if(pxwindow->GetParentWindow(win))
    {
        xwindow_new_userdata(L,win,0);
        return 1;
    }
    else
    {
        DEL(win);
        return 0;
    }
}
static int xwindow_getrootwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    CxWindow *win;
    NEW(win,CxWindow);
    win->Init();

    if(pxwindow->GetRootWindow(win))
    {
        xwindow_new_userdata(L,win,0);
        return 1;
    }
    else
    {
        DEL(win);
        return 0;
    }
    return 1;
}
static int xwindow_updatewindowtree(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->UpdateWindowTree();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_gethostpid(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    pid_t pid = 0;
    pxwindow->GetHostPid(&pid);
    if(pid)
    {
        lua_pushinteger(L,pid);
        return 1;
    }
    else
    {
        return 0;
    }
}
static int xwindow_setdisplay(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    CxDisplay *display = get_xdisplay(L,2);
    ASSERT(display);
    int _ret_0 = (int)pxwindow->SetDisplay(display);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_getnativexwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    uint32_t _ret_0 = (uint32_t)pxwindow->GetNativeXWindow();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int xwindow_setnativexwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    uint32_t _nativexwindow = (uint32_t)lua_tointeger(L,2);
    int _ret_0 = (int)pxwindow->SetNativeXWindow(_nativexwindow);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_attach(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    uint32_t _nativexwindow = (uint32_t)lua_tointeger(L,2);
    int _ret_0 = (int)pxwindow->Attach(_nativexwindow);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_isattached(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->IsAttached();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_mapwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->MapWindow();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_unmapwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->UnmapWindow();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_withdrawwindow(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->WithdrawWindow();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_mapraised(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->MapRaised();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_fetchname(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    LOCAL_MEM(name);
    if(pxwindow->FetchName(&name))
    {
        lua_pushstring(L,name.CStr());
        return 1;
    }
    return 0;
}
static int xwindow_storename(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    const char* name = (const char*)lua_tostring(L,2);
    int _ret_0 = (int)pxwindow->StoreName(name);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_getwmname(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    LOCAL_MEM(wm_name);
    if(pxwindow->GetWMName(&wm_name))
    {
        lua_pushstring(L,wm_name.CStr());
        return 1;
    }
    return 0;
}
static int xwindow_close(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int _ret_0 = (int)pxwindow->Close();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int xwindow_getwindowproperty(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    const char* atom_name = (const char*)lua_tostring(L,2);
    int offset = (int)lua_tointeger(L,3);
    int items = (int)lua_tointeger(L,4);
    LOCAL_MEM_WITH_SIZE(mem,32*1024);
    int _ret_0 = (int)pxwindow->GetWindowProperty(atom_name,offset,items,&mem);
    lua_pushboolean(L,_ret_0);
    lua_pushlstring(L,mem.GetRawBuf(),mem.GetSize());
    return 2;
}
static int xwindow_setwindowtype(lua_State *L)
{
	CxWindow *pxwindow = get_xwindow(L,1);
	ASSERT(pxwindow);
	const char* wm_type = (const char*)lua_tostring(L,2);
	ASSERT(wm_type);
	int on = (int)lua_toboolean(L,3);
	int _ret_0 = (int)pxwindow->SetWindowType(wm_type,on);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int xwindow_setdecorations(lua_State *L)
{
	CxWindow *pxwindow = get_xwindow(L,1);
	ASSERT(pxwindow);
	int decorations = (int)lua_tointeger(L,2);
	int _ret_0 = (int)pxwindow->SetDecorations(decorations);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int xwindow_stayabove(lua_State *L)
{
	CxWindow *pxwindow = get_xwindow(L,1);
	ASSERT(pxwindow);
	int above = (int)lua_toboolean(L,2);
	int _ret_0 = (int)pxwindow->StayAbove(above);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int xwindow_staybelow(lua_State *L)
{
	CxWindow *pxwindow = get_xwindow(L,1);
	ASSERT(pxwindow);
	int below = (int)lua_toboolean(L,2);
	int _ret_0 = (int)pxwindow->StayBelow(below);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int xwindow_setfunctions(lua_State *L)
{
	CxWindow *pxwindow = get_xwindow(L,1);
	ASSERT(pxwindow);
	int functions = (int)lua_tointeger(L,2);
	int _ret_0 = (int)pxwindow->SetFunctions(functions);
	lua_pushboolean(L,_ret_0);
	return 1;
}
static int xwindow_getmapstate(lua_State *L)
{
	CxWindow *pxwindow = get_xwindow(L,1);
	ASSERT(pxwindow);
	int _ret_0 = (int)pxwindow->GetMapState();
	lua_pushinteger(L,_ret_0);
	return 1;
}
static status_t xwindow_setwmname(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    
    CMem mem(name);
    pxwindow->SetWMName(&mem);
    return 0;
}

static status_t xwindow_move(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    int x = lua_tointeger(L,2);
    int y = lua_tointeger(L,3);
    pxwindow->Move(x,y);
    return 0;
}
static status_t xwindow_getnetwmname(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);

    LOCAL_MEM(name);
    pxwindow->GetNetWMName(&name);
    
    lua_pushstring(L,name.CStr());
    return 1;
}

static status_t xwindow_setnetwmname(lua_State *L)
{
    CxWindow *pxwindow = get_xwindow(L,1);
    ASSERT(pxwindow);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = pxwindow->SetNetWMName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static const luaL_Reg xwindow_lib[] = {
    {"__gc",xwindow_gc_},
    {"__tostring",xwindow_tostring_},
    {"__is_same",xwindow_issame_},
    {"new",xwindow_new},
    {"FullScreen",xwindow_fullscreen},
    {"Minimize",xwindow_minimize},
    {"Maximize",xwindow_maximize},
    {"GetTopLevelWindow",xwindow_gettoplevelwindow},
    {"PrintWindowTree",xwindow_printwindowtree},
    {"Lower",xwindow_lower},
    {"Raise",xwindow_raise},
    {"SetBounds",xwindow_setbounds},
    {"GetBounds",xwindow_getbounds},
    {"Hide",xwindow_hide},
    {"Show",xwindow_show},
    {"GetDisplay",xwindow_getdisplay},
    {"TraverseWindowTree",xwindow_traversewindowtree},
    {"SetDockMode",xwindow_setdockmode},
    {"GetChildWindowsLen",xwindow_getchildwindowslen},
    {"GetChildWindow",xwindow_getchildwindow},
    {"Clear",xwindow_clear},
    {"GetParentWindow",xwindow_getparentwindow},
    {"GetRootWindow",xwindow_getrootwindow},
    {"UpdateWindowTree",xwindow_updatewindowtree},
    {"GetHostPid",xwindow_gethostpid},
    {"SetDisplay",xwindow_setdisplay},
    {"GetNativeXWindow",xwindow_getnativexwindow},
    {"SetNativeXWindow",xwindow_setnativexwindow},
    {"Attach",xwindow_attach},
    {"IsAttached",xwindow_isattached},
    {"MapWindow",xwindow_mapwindow},
    {"UnmapWindow",xwindow_unmapwindow},
    {"WithdrawWindow",xwindow_withdrawwindow},
    {"MapRaised",xwindow_mapraised},
    {"FetchName",xwindow_fetchname},
    {"StoreName",xwindow_storename},
    {"GetWMName",xwindow_getwmname},
    {"Close",xwindow_close},
    {"GetWindowProperty",xwindow_getwindowproperty},
	{"SetWindowType",xwindow_setwindowtype},
	{"SetDecorations",xwindow_setdecorations},
	{"StayAbove",xwindow_stayabove},
	{"StayBelow",xwindow_staybelow},
	{"SetFunctions",xwindow_setfunctions},
	{"GetMapState",xwindow_getmapstate},
    {"SetWMName",xwindow_setwmname},  
    {"Move",xwindow_move},  
    {"GetNetWMName",xwindow_getnetwmname},
    {"SetNetWMName",xwindow_setnetwmname},    
    {NULL, NULL}
};
static int luaL_register_xwindow(lua_State *L)
{
    luaL_newmetatable(L, LUA_USERDATA_XWINDOW);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L,xwindow_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,xwindow_lib);
    return 1;
}

int luaopen_xwindow(lua_State *L)
{
    luaL_requiref(L, "XWindow",luaL_register_xwindow,1);
    lua_pop(L, 1);
    return 0;
}
