#include "lualib_cfunc.h"
#include "mem_tool.h"
#include "sys_log.h"
#include "misc.h"
#include "lua_helper.h"
#include "minitask.h"

/****************************************/
static int cfunc_memcmp(lua_State *L)
{
    int_ptr_t s1 = (int_ptr_t)lua_tointeger(L,1);
    int_ptr_t s2 = (int_ptr_t)lua_tointeger(L,2);
    int_ptr_t n = (int_ptr_t)lua_tointeger(L,3);
    int _ret_0 = (int)memcmp((const void*)s1,(const void*)s2,(size_t)n);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int cfunc_wild_match(lua_State *L)
{
    const char* pattern = (const char*)lua_tostring(L,1);
    ASSERT(pattern);
    const char* str = (const char*)lua_tostring(L,2);
    ASSERT(str);
    int _ret_0 = (int)wild_match(pattern,str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int cfunc_memcpy(lua_State *L)
{
    int_ptr_t s1 = (int_ptr_t)lua_tointeger(L,1);
    int_ptr_t s2 = (int_ptr_t)lua_tointeger(L,2);
    int_ptr_t n = (int_ptr_t)lua_tointeger(L,3);
    memcpy((void*)s1,(const void*)s2,(size_t)n);
    return 0;
}

#if _IS_LINUX_
static int cfunc_getpid(lua_State *L)
{
    lua_pushinteger(L,getpid());
    return 1;
}
#endif

static status_t cfunc_callback_ongethostbyname(lua_State *L, int _funcid, bool _once, CMemStk *ip)
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

	if(ip && ip->GetLen() > 0)
    {
		CLuaVm::PushStringArray(L,ip);
		vm.Run(1,0);
	}
	else
	{
		vm.Run(0,0);
	}
    vm.ClearStack();
    return OK;
}

static status_t cfunc_gethostbyname(lua_State *L)
{
    const char* host = (const char*)lua_tostring(L,1);
    ASSERT(host);
    int ongethostbyname = CLuaVm::ToFunction(L,2);
    
	int_ptr_t *context = NULL;
	crt_host_to_ip_async(host,&context);
	
	
	BEGIN_MINI_TASK(task)
	{
		int_ptr_t *context;
		lua_State *L;
		int ongethostbyname;

		status_t Run(uint32_t interval)
		{
			int *running = how_to_get_lua_running_flag(L);
			ASSERT(running && (*running));
			ASSERT(this->context);

			if(this->context[0]) //complete
			{
				if(this->context[1] == 0) //fail
				{
					cfunc_callback_ongethostbyname(L,ongethostbyname,true,NULL);
				}
				else
				{
					CMem text((const char*)&context[5]);
					
					LOCAL_MEM(mem);
					text.Seek(0);
					
					CMemStk ip;
					ip.Init();
					
					while(text.ReadLine(&mem))
					{
						ip.Push(&mem);
					}
					
					cfunc_callback_ongethostbyname(L,ongethostbyname,true,&ip);
				}
				crt_free_host_to_ip_context(&this->context);
				this->context = NULL;
				this->Quit();
			}
			else
			{
				this->Sleep(10);
			}		
			return OK;
		}
	}
	END_MINI_TASK(task);
	
	task->Init(how_to_get_global_taskmgr(L));
	task->context = context;
	task->L = L;
	task->ongethostbyname = ongethostbyname;
	task->Start(1);    
    return 0;
}

static const luaL_Reg cfunc_lib[] = {
    {"memcmp",cfunc_memcmp},
    {"memcpy",cfunc_memcpy},
    {"wild_match",cfunc_wild_match},
#if _IS_LINUX_
    {"getpid",cfunc_getpid},
#endif
	{"gethostbyname",cfunc_gethostbyname},
    {NULL, NULL}
};

static int luaL_register_cfunc(lua_State *L)
{   
    luaL_newmetatable(L, LUA_USERDATA_CFUNC);
    lua_pushvalue(L, -1);   
    lua_setfield(L, -2, "__index"); 
    luaL_setfuncs(L,cfunc_lib,0);
    lua_pop(L, 1);
    luaL_newlib(L,cfunc_lib);
    return 1;
}

int luaopen_cfunc(lua_State *L)
{
    luaL_requiref(L, "CFunc",luaL_register_cfunc,1);
    lua_pop(L, 1);
    return 0;
}
