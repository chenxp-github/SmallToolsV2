#include "lualib_serial.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_filebase.h"

static bool serial_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CHECK_IS_UD_READABLE(CSerial,ud);
    CSerial *p = (CSerial*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}    

bool is_serial(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_SERIAL,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return serial_is_userdata_valid(ud);  
}

CSerial *get_serial(lua_State *L, int idx)
{
    lua_userdata *ud = NULL;
    if(is_serial(L,idx))
    {
        ud = (lua_userdata*)lua_touserdata(L,idx);		
    }
    ASSERT(ud);
    return (CSerial *)ud->p;
} 

lua_userdata *serial_new_userdata(lua_State *L,CSerial *pobj,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pobj);
    ud->p = pobj;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pobj->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_SERIAL);
    lua_setmetatable(L,-2);
    return ud;
}

static int serial_gc_(lua_State *L)
{
    if(!is_serial(L,1)) return 0;

    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);		
    ASSERT(ud);

    if(!(ud->is_attached))
    {
        CSerial *pserial = (CSerial*)ud->p;
        DEL(pserial);
    }
    return 0;
}

static int serial_issame_(lua_State *L)
{
    CSerial *pserial1 = get_serial(L,1);
    ASSERT(pserial1);
    CSerial *pserial2 = get_serial(L,2);
    ASSERT(pserial2);
    int is_same = (pserial1==pserial2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int serial_tostring_(lua_State *L)
{
    CSerial *pserial = get_serial(L,1);
    char buf[1024];
    sprintf(buf,"userdata:serial:%p",pserial);
    lua_pushstring(L,buf);
    return 1;
}

/****************************************************/
static status_t serial_enabledtrhandshake(lua_State *L)
{
    CSerial *pserial = get_serial(L,1);
    ASSERT(pserial);
    bool enable = lua_toboolean(L,2)!=0;
    status_t ret0 = pserial->EnableDtrHandshake(enable);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t serial_configure(lua_State *L)
{
    CSerial *pserial = get_serial(L,1);
    ASSERT(pserial);
    int baudrate = (int)lua_tointeger(L,2);
    int databits = (int)lua_tointeger(L,3);
    int stopbits = (int)lua_tointeger(L,4);
    const char *parity = lua_tostring(L,5);
    ASSERT(parity);
    status_t ret0 = pserial->Configure(baudrate,databits,stopbits,parity[0]);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t serial_open(lua_State *L)
{
    CSerial *pserial = get_serial(L,1);
    ASSERT(pserial);
    const char* dev_name = (const char*)lua_tostring(L,2);
    ASSERT(dev_name);
    status_t ret0 = pserial->Open(dev_name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t serial_destroy(lua_State *L)
{
    CSerial *pserial = get_serial(L,1);
    ASSERT(pserial);
    status_t ret0 = pserial->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}
static status_t serial_new(lua_State *L)
{
    CSerial *pserial;
    NEW(pserial,CSerial);
    pserial->Init();
    serial_new_userdata(L,pserial,0);
    return 1;
}
/****************************************************/
static const luaL_Reg serial_funcs_[] = {
    {"__gc",serial_gc_},
    {"__tostring",serial_tostring_},
    {"__is_same",serial_issame_},
    {"new",serial_new},
    {"EnableDtrHandshake",serial_enabledtrhandshake},
    {"Configure",serial_configure},
    {"Open",serial_open},
    {"Destroy",serial_destroy},
    {NULL,NULL},
};

const luaL_Reg* get_serial_funcs()
{
    return serial_funcs_;
}

static int luaL_register_serial(lua_State *L)
{	
    static luaL_Reg _serial_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_serial_funcs_,&_index,get_filebase_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_serial_funcs_,&_index,get_serial_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_SERIAL);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_serial_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_serial_funcs_);
    return 1;
}        

int luaopen_serial(lua_State *L)
{
    luaL_requiref(L, "Serial",luaL_register_serial,1);
    lua_pop(L, 1);
    return 0;
}        

