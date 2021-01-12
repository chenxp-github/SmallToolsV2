#include "lualib_xrandr.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_xdisplay.h"
#include "lualib_xwindow.h"

/****************************************************/
static status_t xrandr_getmonitors(lua_State *L)
{
    int_ptr_t dpy = (int_ptr_t)lua_tointeger(L,1);
    int_ptr_t window = (int_ptr_t)lua_tointeger(L,2);
    bool get_active = (bool)lua_toboolean(L,3);

    int n = 0;
    XRRMonitorInfo *infos = CxRandr::GetMonitors((Display*)dpy,(Window)window,get_active,&n);
    if(!infos)return 0;

    lua_newtable(L);
    int top = lua_gettop(L);

    for(int i = 0; i < n; i++)
    {
        XRRMonitorInfo *info = &infos[i];

        lua_pushinteger(L,i+1);

        lua_newtable(L);
        int top1 = lua_gettop(L);

        lua_pushstring(L,"automatic");
        lua_pushboolean(L,info->automatic);
        lua_settable(L,top1);

        lua_pushstring(L,"height");
        lua_pushinteger(L,info->height);
        lua_settable(L,top1);

        lua_pushstring(L,"mheight");
        lua_pushinteger(L,info->mheight);
        lua_settable(L,top1);

        lua_pushstring(L,"mwidth");
        lua_pushinteger(L,info->mwidth);
        lua_settable(L,top1);

        lua_pushstring(L,"name");
        lua_pushstring(L, XGetAtomName((Display*)dpy,info->name));
        lua_settable(L,top1);

        lua_pushstring(L,"noutput");
        lua_pushinteger(L,info->noutput);
        lua_settable(L,top1);

        lua_pushstring(L,"primary");
        lua_pushboolean(L,info->primary);
        lua_settable(L,top1);

        lua_pushstring(L,"width");
        lua_pushinteger(L,info->width);
        lua_settable(L,top1);

        lua_pushstring(L,"x");
        lua_pushinteger(L,info->x);
        lua_settable(L,top1);

        lua_pushstring(L,"y");
        lua_pushinteger(L,info->y);
        lua_settable(L,top1);

        lua_settable(L,top);
    }

    return 1;
}

/****************************************************/
static const luaL_Reg xrandr_funcs_[] = {
    {"GetMonitors",xrandr_getmonitors},
    {NULL,NULL},
};

const luaL_Reg* get_xrandr_funcs()
{
    return xrandr_funcs_;
}

static int luaL_register_xrandr(lua_State *L)
{	
    static luaL_Reg _xrandr_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_xrandr_funcs_,&_index,get_xrandr_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_XRANDR);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_xrandr_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_xrandr_funcs_);
    return 1;
}        

int luaopen_xrandr(lua_State *L)
{
    luaL_requiref(L, "XRandr",luaL_register_xrandr,1);
    lua_pop(L, 1);
    return 0;
}        

