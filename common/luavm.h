#ifndef __LUAVM_H
#define __LUAVM_H

#include "cruntime.h"
#include "mem.h"
#include "memstk.h"
#include "lua.hpp"
#include "raw_weak_pointer.h"

#define MAX_LUA_FUNCS 4096

typedef struct {
    int is_attached;
    CRawWeakPointer weak_ptr;
}lua_userdata;

#define LUA_IS_VALID_USER_DATA_FUNC(ctype,type_lwr) \
static bool type_lwr##_is_userdata_valid(lua_userdata *ud)\
{\
    if(ud == NULL)return false;\
    if(!ud->weak_ptr.IsValid())return false;\
    return true;\
}\

#define LUA_GET_OBJ_FROM_USER_DATA_FUNC(ctype,type_lwr)\
ctype *get_##type_lwr(lua_State *L, int idx)\
{\
    lua_userdata *ud = NULL;\
    if(is_##type_lwr(L,idx))\
    {\
        ud = (lua_userdata*)lua_touserdata(L,idx);\
    }\
    ASSERT(ud);\
    return (ctype*)ud->weak_ptr.GetRawPtr();\
}\

#define LUA_NEW_USER_DATA_FUNC(ctype,type_lwr,type_upr)\
lua_userdata *type_lwr##_new_userdata(lua_State *L,ctype *pobj,int is_weak)\
{\
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));\
    ASSERT(ud && pobj);\
    ud->weak_ptr.Init();\
    WEAK_REF_CONTEXT_CREATE(pobj->__weak_ref_context);\
    ud->weak_ptr.WeakRef(pobj,pobj->__weak_ref_context);\
    ud->is_attached = is_weak;\
    luaL_getmetatable(L,LUA_USERDATA_##type_upr);\
    lua_setmetatable(L,-2);\
    return ud;\
}\

#define LUA_GC_FUNC(ctype,type_lwr)\
static int type_lwr##_gc_(lua_State *L)\
{\
    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);\
    ASSERT(ud);\
    if(!is_##type_lwr(L,1))\
    {\
        ud->weak_ptr.Destroy();\
        return 0;\
    }\
    if(!(ud->is_attached))\
    {\
        ctype *p = (ctype*)ud->weak_ptr.GetRawPtr();\
        DEL(p);\
    }\
    ud->weak_ptr.Destroy();\
    return 0;\
}\

#define LUA_IS_SAME_FUNC(ctype,type_lwr)\
static int type_lwr##_issame_(lua_State *L)\
{\
    ctype *p1 = get_##type_lwr(L,1);\
    ASSERT(p1);\
    ctype *p2 = get_##type_lwr(L,2);\
    ASSERT(p2);\
    int is_same = (p1==p2);\
    lua_pushboolean(L,is_same);\
    return 1;\
}\

#define LUA_TO_STRING_FUNC(ctype,type_lwr)\
static int type_lwr##_tostring_(lua_State *L)\
{\
    ctype *p = get_##type_lwr(L,1);\
    char buf[1024];\
    sprintf(buf,"userdata:%s:%p",#type_lwr,p);\
    lua_pushstring(L,buf);\
    return 1;\
}\


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

    status_t SetRunningFlag(int running);
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
