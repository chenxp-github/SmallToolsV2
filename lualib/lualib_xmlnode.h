#ifndef __LUALIB_XMLNODE_H
#define __LUALIB_XMLNODE_H

#include "xml.h"
#include "luavm.h"

#define LUA_USERDATA_XMLNODE "lua_userdata_xmlnode"

CXmlNode* get_xmlnode(lua_State *L, int idx);
lua_userdata *xmlnode_new_userdata(lua_State *L,CXmlNode *pobj,int is_weak);
int luaopen_xmlnode(lua_State *L);
bool is_xmlnode(lua_State *L, int idx);
const luaL_Reg* get_xmlnode_funcs();

#endif
