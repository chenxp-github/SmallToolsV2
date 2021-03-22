#include "lualib_xmlnode.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_filebase.h"
#include "lualib_mem.h"
#include "lualib_memfile.h"

LUA_IS_VALID_USER_DATA_FUNC(CXmlNode,xmlnode)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CXmlNode,xmlnode)
LUA_NEW_USER_DATA_FUNC(CXmlNode,xmlnode,XMLNODE)
LUA_GC_FUNC(CXmlNode,xmlnode)
LUA_IS_SAME_FUNC(CXmlNode,xmlnode)
LUA_TO_STRING_FUNC(CXmlNode,xmlnode)

bool is_xmlnode(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XMLNODE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return xmlnode_is_userdata_valid(ud);  
}

/****************************************************/
static status_t xmlnode_new(lua_State *L)
{
    CXmlNode *pxmlnode;
    NEW(pxmlnode,CXmlNode);
    pxmlnode->Init();
    xmlnode_new_userdata(L,pxmlnode,0);
    return 1;
}

static status_t xmlnode_addattrib_v1(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    status_t ret0 = pxmlnode->AddAttrib(file);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_addattrib_v2(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    const char* attrib = (const char*)lua_tostring(L,2);
    ASSERT(attrib);
    const char* val = (const char*)lua_tostring(L,3);
    ASSERT(val);
    status_t ret0 = pxmlnode->AddAttrib(attrib,val);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_addchild(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CXmlNode *node = get_xmlnode(L,2);
    ASSERT(node);
    status_t ret0 = pxmlnode->AddChild(node);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_addvaluedata_v1(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    status_t ret0 = pxmlnode->AddValueData(file);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_addvaluedata_v2(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    const char* str = (const char*)lua_tostring(L,2);
    ASSERT(str);
    status_t ret0 = pxmlnode->AddValueData(str);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_destroy(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    status_t ret0 = pxmlnode->Destroy();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_getattrib(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    CFileBase *val = get_filebase(L,3);
    ASSERT(val);
    status_t ret0 = pxmlnode->GetAttrib(name,val);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_getnextattrib(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CFileBase *name = get_filebase(L,2);
    ASSERT(name);
    CFileBase *val = get_filebase(L,3);
    ASSERT(val);
    status_t ret0 = pxmlnode->GetNextAttrib(name,val);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_setname_v1(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CMem *name = get_mem(L,2);
    ASSERT(name);
    status_t ret0 = pxmlnode->SetName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_setname_v2(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    const char* name = (const char*)lua_tostring(L,2);
    ASSERT(name);
    status_t ret0 = pxmlnode->SetName(name);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_setvaluetype(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    int type = (int)lua_tointeger(L,2);
    status_t ret0 = pxmlnode->SetValueType(type);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_writetofile(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    status_t ret0 = pxmlnode->WriteToFile(file);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xmlnode_getname(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    const char* ret0 = pxmlnode->GetName();
    ASSERT(ret0);
    lua_pushstring(L,ret0);
    return 1;
}

static status_t xmlnode_getpath(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    LOCAL_MEM(path);
    pxmlnode->GetPath(&path);
    lua_pushlstring(L,path.CStr(),path.StrLen());
    return 1;
}

static status_t xmlnode_getstringvalue(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    
    CMem mem;
    mem.Init();    
	
    if(pxmlnode->GetStringValue(&mem))
	{
		lua_pushlstring(L,mem.CStr(),mem.StrLen());
		return 1;
	}
	return 0;
}

static status_t xmlnode_getchild_v1(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    int i = (int)lua_tointeger(L,2);
    CXmlNode *ret0 = pxmlnode->GetChild(i);
    if(ret0)
    {
        xmlnode_new_userdata(L,ret0,1);
        return 1;
    }
    return 0;
}

static status_t xmlnode_getchild_v2(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CXmlNode *ret0 = pxmlnode->GetChild();
    if(ret0)
    {
        xmlnode_new_userdata(L,ret0,1);
        return 1;
    }
    return 0;
}

static status_t xmlnode_getchildbyname(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    const char* child_name = (const char*)lua_tostring(L,2);
    ASSERT(child_name);
    CXmlNode *ret0 = pxmlnode->GetChildByName(child_name);
    if(ret0)
    {
        xmlnode_new_userdata(L,ret0,1);
        return 1;
    }
    return 0;
}

static status_t xmlnode_getnodebypath(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    const char* path = (const char*)lua_tostring(L,2);
    ASSERT(path);
    CXmlNode *ret0 = pxmlnode->GetNodeByPath(path);
    ASSERT(ret0);
    xmlnode_new_userdata(L,ret0,1);
    return 1;
}

static status_t xmlnode_getparent(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CXmlNode *ret0 = pxmlnode->GetParent();
    ASSERT(ret0);
    xmlnode_new_userdata(L,ret0,1);
    return 1;
}

static status_t xmlnode_getvalue(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CMem *ret0 = pxmlnode->GetValue();
    if(ret0)
    {
        mem_new_userdata(L,ret0,1);
        return 1;
    }
    return 0;
}

static status_t xmlnode_addvaluedata(lua_State *L)
{
    if(lua_isstring(L,2))
    {
        return xmlnode_addvaluedata_v2(L);
    }

    if(is_filebase(L,2))
    {
        return xmlnode_addvaluedata_v1(L);
    }

    return 0;
}

static status_t xmlnode_setname(lua_State *L)
{
    if(is_mem(L,2))
    {
        return xmlnode_setname_v1(L);
    }

    if(lua_isstring(L,2))
    {
        return xmlnode_setname_v2(L);
    }

    return 0;
}

static status_t xmlnode_addattrib(lua_State *L)
{
    if(lua_isstring(L,2) && lua_isstring(L,3))
    {
        return xmlnode_addattrib_v2(L);
    }

    if(is_filebase(L,2))
    {
        return xmlnode_addattrib_v1(L);
    }

    return 0;
}
static status_t xmlnode_getnext(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    CXmlNode *ret0 = pxmlnode->GetNext();
    if(ret0)
    {
        xmlnode_new_userdata(L,ret0,1);
        return 1;
    }
    return 0;
}

static status_t xmlnode_getchild(lua_State *L)
{
    if(lua_isinteger(L,2))
    {
        return xmlnode_getchild_v1(L);
    }
    return xmlnode_getchild_v2(L);
}

static status_t xmlnode_restartattrib(lua_State *L)
{
    CXmlNode *pxmlnode = get_xmlnode(L,1);
    ASSERT(pxmlnode);
    status_t ret0 = pxmlnode->RestartAttrib();
    lua_pushboolean(L,ret0);
    return 1;
}
/****************************************************/
static const luaL_Reg xmlnode_funcs_[] = {
    {"new",xmlnode_new},    
    {"__gc",xmlnode_gc_},
    {"__tostring",xmlnode_tostring_},
    {"__is_same",xmlnode_issame_},
    {"GetNextAttrib",xmlnode_getnextattrib},
    {"AddAttrib",xmlnode_addattrib},
    {"GetAttrib",xmlnode_getattrib},
    {"GetChild",xmlnode_getchild},
    {"GetNodeByPath",xmlnode_getnodebypath},
    {"WriteToFile",xmlnode_writetofile},
    {"GetChildByName",xmlnode_getchildbyname},
    {"SetName",xmlnode_setname},
    {"AddValueData",xmlnode_addvaluedata},
    {"SetValueType",xmlnode_setvaluetype},
    {"AddChild",xmlnode_addchild},
    {"GetStringValue",xmlnode_getstringvalue},
    {"GetParent",xmlnode_getparent},
    {"GetName",xmlnode_getname},
    {"Destroy",xmlnode_destroy},
    {"GetPath",xmlnode_getpath},
    {"GetValue",xmlnode_getvalue},
    {"GetNext",xmlnode_getnext},    
    {"RestartAttrib",xmlnode_restartattrib},
    {NULL,NULL},
};

const luaL_Reg* get_xmlnode_funcs()
{
    return xmlnode_funcs_;
}

static int luaL_register_xmlnode(lua_State *L)
{	
    static luaL_Reg _xmlnode_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_xmlnode_funcs_,&_index,get_xmlnode_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_XMLNODE);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_xmlnode_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_xmlnode_funcs_);
    return 1;
}        

int luaopen_xmlnode(lua_State *L)
{
    luaL_requiref(L, "XmlNode",luaL_register_xmlnode,1);
    lua_pop(L, 1);
    return 0;
}        

