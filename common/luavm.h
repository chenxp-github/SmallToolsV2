#ifndef __LUAVM_H
#define __LUAVM_H

#include "cruntime.h"
#include "mem.h"
#include "memstk.h"
#include "lua.hpp"

#define MAX_LUA_FUNCS 4096

typedef struct {
    void *p;
    int is_attached;
    int __weak_ref_id;
}lua_userdata;

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
class CLuaVm{
public:
    lua_State *mL;
    bool mIsAttached;
public:
	static status_t AddLuaSearchPath(lua_State *L,const char *path, const char *type, const char *ext, bool recursive);
	static status_t CombineLuaFuncTable(luaL_Reg *dest, int *index, const luaL_Reg *from, bool is_final);
    static status_t PushStringArray(lua_State *L, CMemStk *stk);
    static status_t ToStringArray(lua_State *L, int index, CMemStk *stk);
    static status_t ToIntArray(lua_State *L, int index, int *a, int *size);
	static status_t PushIntArray(lua_State *L,int *a, int size);
    int ToFunction(int index);
    status_t PushFunction(int ref_);
    status_t ReleaseFunction(int ref_);
    static int ToFunction(lua_State *L,int index);
    static status_t PushFunction(lua_State *L,int ref_);
    static status_t ReleaseFunction(lua_State *L,int ref_);
    status_t GetGlobalFunction(const char *name);
    status_t PushBoolean(bool b);
    status_t PushInteger(int i);
    status_t NewTable();
    int32_t GetTop();
    status_t SetTable(int idx);
    status_t SetGlobal(const char *name);
    status_t FullGC();
    status_t SetRunTimeout(uint32_t to);
    status_t ClearStack();
    lua_State *GetLuaState();
    status_t StepRun(int32_t nargs);
    status_t PrepareStepRun();
    static status_t DumpStack(lua_State *L);
    status_t LoadMem(CMem *mem, const char *name);
    status_t LoadMem(const char *buf, int size, const char *name);
    status_t Pop(int num);
    lua_Number ToNumber(int idx);
    status_t PushNumber(lua_Number num);
    status_t PushString(const char *str);
    int GetGlobal(const char *name);
    status_t Run(int nargs, int nresults);
    status_t LoadFile(const char *filename);
    status_t ReportError();
    CLuaVm();
    virtual ~CLuaVm();
    status_t Init();
    status_t Init(lua_State *L);
    status_t Destroy();
    status_t Copy(CLuaVm *p);
    status_t Comp(CLuaVm *p);
    status_t Print();
    status_t InitBasic();
};

#endif
