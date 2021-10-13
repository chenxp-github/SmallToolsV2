#include "lualib_commontime.h"
#include "mem_tool.h"
#include "syslog.h"

LUA_IS_VALID_USER_DATA_FUNC(CCommonTime,commontime)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CCommonTime,commontime)
LUA_NEW_USER_DATA_FUNC(CCommonTime,commontime,COMMONTIME)
LUA_GC_FUNC(CCommonTime,commontime)
LUA_IS_SAME_FUNC(CCommonTime,commontime)
LUA_TO_STRING_FUNC(CCommonTime,commontime)

bool is_commontime(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_COMMONTIME,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return commontime_is_userdata_valid(ud);  
}

/****************************************************/
static status_t commontime_new(lua_State *L)
{
    CCommonTime *pcommontime;
    NEW(pcommontime,CCommonTime);
    pcommontime->Init();
    commontime_new_userdata(L,pcommontime,0);
    return 1;
}

static status_t commontime_getyear(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetYear();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_getmonth(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetMonth();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_getday(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetDay();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_gethour(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetHour();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_getminute(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetMinute();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_getsecond(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetSecond();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_getmsec(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetMsec();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_getweekday(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetWeekDay();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_getyearday(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int ret0 = pcommontime->GetYearDay();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_setyear(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _year = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetYear(_year);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setmonth(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _month = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetMonth(_month);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setday(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _day = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetDay(_day);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_sethour(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _hour = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetHour(_hour);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setminute(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _minute = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetMinute(_minute);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setsecond(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _second = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetSecond(_second);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setmsec(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _msec = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetMsec(_msec);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setweekday(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _week_day = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetWeekDay(_week_day);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setyearday(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    int _year_day = (int)lua_tointeger(L,2);
    status_t ret0 = pcommontime->SetYearDay(_year_day);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_setnow(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    status_t ret0 = pcommontime->SetNow();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_set(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    uint64_t tm = (uint64_t)lua_tointeger(L,2);
    status_t ret0 = pcommontime->Set(tm);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_getlong(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    uint64_t ret0 = pcommontime->GetLong();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t commontime_setsystemtime(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    status_t ret0 = pcommontime->SetSystemTime();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t commontime_tostring(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    LOCAL_MEM(mem);
    pcommontime->SaveReadableString(&mem);    
    lua_pushstring(L,mem.CStr());
    return 1;
}

static status_t commontime_loadstring(lua_State *L)
{
    CCommonTime *pcommontime = get_commontime(L,1);
    ASSERT(pcommontime);
    const char* tm_str = (const char*)lua_tostring(L,2);
    ASSERT(tm_str);
    CMem mem(tm_str);
    status_t ret0 = pcommontime->LoadReadableString(&mem);
    lua_pushboolean(L,ret0);
    return 1;
}

/****************************************************/
static const luaL_Reg commontime_funcs_[] = {
    {"__gc",commontime_gc_},
    {"__tostring",commontime_tostring_},
    {"__is_same",commontime_issame_},
    {"new",commontime_new},
    {"GetYear",commontime_getyear},
    {"GetMonth",commontime_getmonth},
    {"GetDay",commontime_getday},
    {"GetHour",commontime_gethour},
    {"GetMinute",commontime_getminute},
    {"GetSecond",commontime_getsecond},
    {"GetMsec",commontime_getmsec},
    {"GetWeekDay",commontime_getweekday},
    {"GetYearDay",commontime_getyearday},
    {"SetYear",commontime_setyear},
    {"SetMonth",commontime_setmonth},
    {"SetDay",commontime_setday},
    {"SetHour",commontime_sethour},
    {"SetMinute",commontime_setminute},
    {"SetSecond",commontime_setsecond},
    {"SetMsec",commontime_setmsec},
    {"SetWeekDay",commontime_setweekday},
    {"SetYearDay",commontime_setyearday},
    {"SetNow",commontime_setnow},
    {"Set",commontime_set},
    {"GetLong",commontime_getlong},
    {"SetSystemTime",commontime_setsystemtime},
    {"ToString",commontime_tostring},
    {"LoadString",commontime_loadstring},    
    {NULL,NULL},
};

const luaL_Reg* get_commontime_funcs()
{
    return commontime_funcs_;
}

static int luaL_register_commontime(lua_State *L)
{	
    static luaL_Reg _commontime_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_commontime_funcs_,&_index,get_commontime_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_COMMONTIME);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_commontime_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_commontime_funcs_);
    return 1;
}        

int luaopen_commontime(lua_State *L)
{
    luaL_requiref(L, "CommonTime",luaL_register_commontime,1);
    lua_pop(L, 1);
    return 0;
}        

