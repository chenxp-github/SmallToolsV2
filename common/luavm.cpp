#include "luavm.h"
#include "syslog.h"
#include "mem_tool.h"
#include "dirmgr.h"

extern "C"{
    void luaC_fullgc (lua_State *L, int isemergency);    
    void lua_set_running_flag(lua_State *L, int running);
}

static void hook_yield(lua_State *L, lua_Debug *ar) 
{ 
    lua_yield(L, 0); 
} 

static const char __Key = 'k';
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
CLuaVm::CLuaVm()
{
    this->InitBasic();
}
CLuaVm::~CLuaVm()
{
    this->Destroy();
}
status_t CLuaVm::InitBasic()
{
    this->mL = NULL;
    this->mIsAttached = false;
    return OK;
}
status_t CLuaVm::Init(lua_State *L)
{
    this->InitBasic();
    this->mL = L;
    this->mIsAttached = true;
    return OK;
}

status_t CLuaVm::Init()
{
    this->InitBasic();
    this->mL = luaL_newstate(); 
    ASSERT(this->mL);
    luaL_openlibs(this->mL);    
    return OK;
}

status_t CLuaVm::Destroy()
{
    if(mL != NULL && !mIsAttached)
    {
        lua_close(mL);
        mL = NULL;
    }
    this->InitBasic();
    return OK;
}
status_t CLuaVm::Copy(CLuaVm *p)
{
    if(this == p)
        return OK;
    //add your code
    return OK;
}
status_t CLuaVm::Comp(CLuaVm *p)
{
    return 0;
}
status_t CLuaVm::Print()
{
    //add your code
    return TRUE;
}

status_t CLuaVm::ReportError()
{
    ASSERT(mL);
    const char *msg = lua_tostring(mL, -1);
    ASSERT(msg);
    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
        "%s",msg
    );
    lua_pop(mL, 1);
    return OK;
}

status_t CLuaVm::LoadFile(const char *filename)
{
    ASSERT(filename && mL);

    if(luaL_loadfile(mL,filename))
    {
        this->ReportError();
        return ERROR;
    }

    return OK;
}

status_t CLuaVm::Run(int nargs, int nresults)
{
    int result = lua_pcall(mL, nargs, nresults, 0);
    if(result && (result != LUA_YIELD))
    {
        this->ReportError();
        return ERROR;
    }
    return OK;
}

int CLuaVm::GetGlobal(const char *name)
{
    ASSERT(name);
    return lua_getglobal(this->mL,name);
}

status_t CLuaVm::PushString(const char *str)
{
    ASSERT(str);
    lua_pushstring(this->mL,str);
    return OK;
}

status_t CLuaVm::PushNumber(lua_Number num)
{
    lua_pushnumber(this->mL,num);
    return OK;
}

lua_Number CLuaVm::ToNumber(int idx)
{
    return lua_tonumber(this->mL,idx);
}

status_t CLuaVm::Pop(int num)
{
    lua_pop(this->mL,num);
    return OK;
}

status_t CLuaVm::LoadMem(const char *buf, int size, const char *name)
{   
    if(luaL_loadbuffer(this->mL,buf,size,name))
    {
        this->ReportError();
        return ERROR;
    }

    return OK;
}

status_t CLuaVm::LoadMem(CMem *mem, const char *name)
{
    ASSERT(mem && name);
    return this->LoadMem(mem->GetRawBuf(),(int)mem->GetSize(),name);
}

status_t CLuaVm::DumpStack(lua_State *L)
{
    ASSERT(L);
    int i;
    int top = lua_gettop(L);

    for(i=1;i<=top;i++)
    {
        int t = lua_type(L, i);
        switch (t) 
        {        
            case LUA_TSTRING: 
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                    "string: '%s'", lua_tostring(L, i)); 
            break;

            case LUA_TBOOLEAN: 
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                    lua_toboolean(L, i) ? "bool: true" : "bool: false"); 
            break;
            
            case LUA_TNUMBER:
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                    "number: %g", lua_tonumber(L, i)); 
            break;
            
            default:
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                    "%s:", lua_typename(L, t)); 
            break;
        }
    }
    return OK;
}

status_t CLuaVm::PrepareStepRun()
{
    if(lua_pcall(mL, 0, 0, 0) != LUA_OK)
    {
        this->ReportError();
        return ERROR;
    }
    lua_sethook(mL, hook_yield, LUA_MASKCOUNT, 256); 
    return OK;
}

status_t CLuaVm::StepRun(int32_t nargs)
{
    int32_t err = lua_resume(mL, mL, nargs);    

    if(err == LUA_OK)
        return 0;
    if(err == LUA_YIELD)
        return 1;
    this->ReportError();
    return -1;
}

lua_State *CLuaVm::GetLuaState()
{
    return mL;
}

status_t CLuaVm::ClearStack()
{
    lua_settop(mL,0);
    return OK;
}

status_t CLuaVm::SetRunTimeout(uint32_t to)
{
    return OK;
}

status_t CLuaVm::FullGC()
{
#ifndef NOT_USE_LUA_FULL_GC
    luaC_fullgc(mL,0);
#endif
    return OK;
}

status_t CLuaVm::SetGlobal(const char *name)
{
    ASSERT(name);
    lua_setglobal(this->mL,name);
    return OK;
}

status_t CLuaVm::SetTable(int idx)
{
    lua_settable(this->mL,idx);
    return OK;
}

int32_t CLuaVm::GetTop()
{
    return lua_gettop(this->mL);
}

status_t CLuaVm::NewTable()
{
    lua_newtable(this->mL);
    return OK;
}

status_t CLuaVm::PushInteger(int i)
{
    lua_pushinteger(mL,i);
    return OK;
}

status_t CLuaVm::PushBoolean(bool b)
{
    lua_pushboolean(mL,(int)b);
    return OK;
}

status_t CLuaVm::GetGlobalFunction(const char *name)
{
    return this->GetGlobal(name) == LUA_TFUNCTION;
}

int CLuaVm::ToFunction(lua_State *L,int index)
{
    if(lua_isfunction(L,index))
    {
		lua_pushvalue(L,index);
		return luaL_ref(L,LUA_REGISTRYINDEX);
	}
    else
	{
		return LUA_REFNIL;
	}
}

status_t CLuaVm::PushFunction(lua_State *L,int ref_)
{
    if(ref_ != LUA_REFNIL)
    {
        lua_rawgeti(L,LUA_REGISTRYINDEX,ref_);
        return OK;
    }

    return ERROR;
}

status_t CLuaVm::ReleaseFunction(lua_State *L,int ref_)
{
    if(ref_ != LUA_REFNIL)
    {
        luaL_unref(L,LUA_REGISTRYINDEX,ref_);
        return OK;
    }
    return ERROR;
}

int CLuaVm::ToFunction(int index)
{
    return ToFunction(mL,index);
}

status_t CLuaVm::PushFunction(int ref_)
{
    return PushFunction(mL,ref_);
}

status_t CLuaVm::ReleaseFunction(int ref_)
{
    return ReleaseFunction(mL,ref_);
}

status_t CLuaVm::PushIntArray(lua_State *L,int *a, int size)
{
    ASSERT(a);
    
	lua_newtable(L);
	int top = lua_gettop(L);
	
	for(int i = 0; i < size; i++)
	{
		lua_pushinteger(L,i+1);
		lua_pushinteger(L,a[i]);
		lua_settable(L, top);
	}
    return OK;
}


status_t CLuaVm::ToIntArray(lua_State *L, int index, int *a, int *size)
{
    ASSERT(a && size);
    
    int k = 0;
    int max_size = *size;
    
    *size = 0;

    if(!lua_istable(L,index))
        return ERROR;

    int top = lua_gettop(L);

    lua_pushnil(L);
    while (lua_next(L, index) != 0) 
    {
        if(k >= max_size)break;
        a[k++] = (int)lua_tointeger(L,-1);
        if(k >= max_size)break;
        lua_pop(L, 1);
    }
    lua_settop(L,top);

    *size = k;
    return OK;
}

status_t CLuaVm::ToStringArray(lua_State *L, int index, CMemStk *stk)
{
    ASSERT(L && stk);    
    if(!lua_istable(L,index))
        return ERROR;
    int top = lua_gettop(L);
    lua_pushnil(L);
    while (lua_next(L, index) != 0) 
    {
        stk->Push(lua_tostring(L,-1));
        lua_pop(L, 1);
    }
    lua_settop(L,top);
    return OK;
}

status_t CLuaVm::PushStringArray(lua_State *L, CMemStk *stk)
{
    ASSERT(L && stk);    
    
    if(stk->GetLen() <=0)
        return ERROR;

    lua_newtable(L);
    int top = lua_gettop(L);
    for(int i = 0; i < stk->GetLen(); i++)
    {
        CMem *pmem = stk->GetElem(i);
        ASSERT(pmem);
        lua_pushinteger(L,i+1);
        lua_pushlstring(L,pmem->CStr(),pmem->StrLen());
        lua_settable(L, top);
    }
    return OK;
}


status_t CLuaVm::CombineLuaFuncTable(luaL_Reg *dest, int *index, const luaL_Reg *from, bool is_final)
{
	ASSERT(dest&&index&&from);
	
	int i = 0;
	int k = *index;

	while (true)
	{
		const char *name = from[i].name;
		if(name == NULL)break;
		
		if(!is_final)
		{

			if((name[0] == '_' && name[1] == '_') || 
			   (name[0] == 'n' && name[1] == 'e' && name[2] == 'w'))
			{
				i++;
				continue;
			}
		}

		ASSERT(k < MAX_LUA_FUNCS);
		dest[k].name = name;
		dest[k].func = from[i].func;
		i++;k++;
	}

	if(is_final)
	{
		dest[k].name = NULL;
		dest[k].func = NULL;		
	}
	
	*index = k;
	return OK;
}

static status_t escape_windows_path(const char *path,CMem *out)
{
    ASSERT(path && out);
    int len = strlen(path);
    
    for(int i = 0; i < len; i++)
    {
        if(path[i]=='\\')
        {
            out->Puts("/"); //escape
        }
        else
        {
            out->Putc(path[i]);
        }
    }
    return OK;
}

status_t CLuaVm::AddLuaSearchPath(lua_State *L,const char *path, const char *type, const char *ext, bool recursive)
{
    ASSERT(L && path && type && ext);
   
    LOCAL_MEM(abs_path);
    abs_path.StrCpy(path);
    CDirMgr::ToAbsPath(&abs_path);
    
    if(!CDirMgr::IsDirExist(&abs_path))
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
			"path \"%s\" is not exist",abs_path.CStr()
		);
        return ERROR;
    }
    
    LOCAL_MEM_WITH_SIZE(code,32*1024);
    LOCAL_MEM(tmp);
    
    code.Printf("package.%s=package.%s..\";",type,type);
    
    escape_windows_path(abs_path.CStr(),&tmp);
    code.Puts(&tmp);
    
    code.Printf("/?.%s;",ext);
    BEGIN_CLOSURE(search_dir)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(const char*, fullname,2);
        CLOSURE_PARAM_PTR(CMem*,code,10);
        CLOSURE_PARAM_PTR(const char*,ext,11);
        
        if(event == CDirMgr::EVENT_BEGIN_DIR)
        {
            LOCAL_MEM(tmp);			
			escape_windows_path(fullname,&tmp);			
            code->Puts(&tmp);
            code->Printf("/?.%s;",ext);
        }
        return OK;
    }
    END_CLOSURE(search_dir);
    
    search_dir.SetParamPointer(10,&code);
    search_dir.SetParamPointer(11,(void*)ext);
    
    CDirMgr::SearchDir(abs_path.CStr(),recursive,&search_dir);
    code.Puts("\"\n");
    
    CLuaVm tmp_vm;
    tmp_vm.Init(L);
    
    tmp_vm.LoadMem(&code,"lua_package_path");
    tmp_vm.Run(0,0);
    tmp_vm.ClearStack();
    return OK;
}

status_t CLuaVm::SetRunningFlag(int running)
{
#ifndef NOT_USE_LUA_SET_RUNNING_FLAG
    lua_set_running_flag(mL,running);
#endif
    return OK;
}
