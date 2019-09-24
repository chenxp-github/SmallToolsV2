#ifndef __LUALIB_XML_H
#define __LUALIB_XML_H

#include "xml.h"
#include "luavm.h"

#define LUA_USERDATA_XML "lua_userdata_xml"

CXml* get_xml(lua_State *L, int idx);
lua_userdata *xml_new_userdata(lua_State *L,CXml *pobj,int is_weak);
int luaopen_xml(lua_State *L);
bool is_xml(lua_State *L, int idx);
const luaL_Reg* get_xml_funcs();

#endif
