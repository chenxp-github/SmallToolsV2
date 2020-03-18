#include "lualib_filebase.h"
#include "lualib_stream.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"
#include "lualib_file.h"
#include "lualib_mem.h"
#include "lualib_memfile.h"
#include "lualib_stream.h"
#include "lualib_tcpsocket.h"
#include "lualib_serial.h"
#include "lualib_queuefile.h"

#define CAN_NOT_REACH() ASSERT(0);return 0

LUA_IS_VALID_USER_DATA_FUNC(CFileBase,filebase)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CFileBase,filebase)
LUA_NEW_USER_DATA_FUNC(CFileBase,filebase,FILEBASE)
LUA_GC_FUNC(CFileBase,filebase)
LUA_IS_SAME_FUNC(CFileBase,filebase)
LUA_TO_STRING_FUNC(CFileBase,filebase)

bool is_filebase(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_FILEBASE,
        LUA_USERDATA_FILE,
        LUA_USERDATA_MEM,
        LUA_USERDATA_MEMFILE,
        LUA_USERDATA_STREAM,
		LUA_USERDATA_TCPSOCKET,
        LUA_USERDATA_SERIAL,
        LUA_USERDATA_QUEUEFILE,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return filebase_is_userdata_valid(ud);  
}
/****************************************/
static int filebase_isend(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->IsEnd();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_charat(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int index = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pfilebase->CharAt(index);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_getlastchar(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int empty_included = (int)lua_toboolean(L,2);
    int _ret_0 = (int)pfilebase->GetLastChar(empty_included!=0);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_getc(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->Getc();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_ungetc(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->UnGetc();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_getmaxsize(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t _ret_0 = (fsize_t)pfilebase->GetMaxSize();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_getoffset(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t _ret_0 = (fsize_t)pfilebase->GetOffset();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_getsize(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t _ret_0 = (fsize_t)pfilebase->GetSize();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_putc(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int ch = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pfilebase->Putc(ch);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_puts_v1(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *str = get_filebase(L,2);
    ASSERT(str);
    int _ret_0 = (int)pfilebase->Puts(str);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_puts_v2(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    
    size_t len = 0;
    const char* str = (const char*)lua_tolstring(L,2,&len);
    ASSERT(str);
    int _ret_0 = (int)pfilebase->Write(str,len);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_puts(lua_State *L)
{
    if(lua_isstring(L,2))
        return filebase_puts_v2(L);
    else if(is_filebase(L,2))
        return filebase_puts_v1(L);
    CAN_NOT_REACH();
}


static int filebase_readleftstr(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int skip_empty = (int)lua_toboolean(L,3);
    int _ret_0 = (int)pfilebase->ReadLeftStr(file,skip_empty);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readword(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadWord(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readwordwithemptychar(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadWordWithEmptyChar(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readwordwithemptychar_reverse(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadWordWithEmptyChar_Reverse(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readword_reverse(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadWord_Reverse(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_searchbin(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file_bin = get_filebase(L,2);
    ASSERT(file_bin);
    int _ret_0 = (int)pfilebase->SearchBin(file_bin);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_searchstr(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* str = (const char*)lua_tostring(L,2);
	ASSERT(str);
    int case_sensive = (int)lua_toboolean(L,3);
    int word_only = (int)lua_toboolean(L,4);
    int _ret_0 = (int)pfilebase->SearchStr(str,case_sensive,word_only);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_seek(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t off = (fsize_t)lua_tointeger(L,2);
    fsize_t _ret_0 = (fsize_t)pfilebase->Seek(off);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_seekback(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t back_bytes = (fsize_t)lua_tointeger(L,2);
    fsize_t _ret_0 = (fsize_t)pfilebase->SeekBack(back_bytes);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_seekend(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t _ret_0 = (fsize_t)pfilebase->SeekEnd();
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_split(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->Split(file);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_writefile_v1(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->WriteFile(file);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_writefile_v2(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int start = (int)lua_tointeger(L,3);
    int ws = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pfilebase->WriteFile(file,start,ws);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_writefile(lua_State *L)
{
    if(is_filebase(L,2) && lua_isinteger(L,3) && lua_isinteger(L,4))
        return filebase_writefile_v2(L);
    if(is_filebase(L,2))
        return filebase_writefile_v1(L);
    CAN_NOT_REACH();
}


static int filebase_writetofile_v1(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* fn = (const char*)lua_tostring(L,2);
	ASSERT(fn);
    int _ret_0 = (int)pfilebase->WriteToFile(fn);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_writetofile_v2(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* fn = (const char*)lua_tostring(L,2);
	ASSERT(fn);
    int start = (int)lua_tointeger(L,3);
    int wsize = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pfilebase->WriteToFile(fn,start,wsize);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_writetofile_v3(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int start = (int)lua_tointeger(L,3);
    int wsize = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pfilebase->WriteToFile(file,start,wsize);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_writetofile(lua_State *L)
{
    if(is_filebase(L,2))
        return filebase_writetofile_v3(L);
    if(lua_isstring(L,2) && lua_isinteger(L,3) && lua_isinteger(L,4))
        return filebase_writetofile_v2(L);
    if(lua_isstring(L,2))
        return filebase_writetofile_v1(L);
    CAN_NOT_REACH();
}

static int filebase_strcmp_v1(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *str = get_filebase(L,2);
    ASSERT(str);
    int _ret_0 = (int)pfilebase->StrCmp(str);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_strcmp_v2(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* str = (const char*)lua_tostring(L,2);
	ASSERT(str);
    int _ret_0 = (int)pfilebase->StrCmp(str);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_strcmp(lua_State *L)
{
    if(lua_isstring(L,2))
        return filebase_strcmp_v2(L);
    else if(is_filebase(L,2))
        return filebase_strcmp_v1(L);
    CAN_NOT_REACH();
}

static int filebase_stricmp_v1(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *str = get_filebase(L,2);
    ASSERT(str);
    int _ret_0 = (int)pfilebase->StrICmp(str);
    lua_pushinteger(L,_ret_0);
    return 1;
}



static int filebase_stricmp_v2(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* str = (const char*)lua_tostring(L,2);
	ASSERT(str);
    int _ret_0 = (int)pfilebase->StrICmp(str);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_stricmp(lua_State *L)
{
    if(lua_isstring(L,2))
        return filebase_stricmp_v2(L);
    else if(is_filebase(L,2))
        return filebase_stricmp_v1(L);
    CAN_NOT_REACH();
}

static int filebase_read(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *buf = get_filebase(L,2);
    ASSERT(buf);
    buf->SetSize(0);
    int n = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pfilebase->WriteToFile(buf,pfilebase->GetOffset(),n);
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_readzeroendstring(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *str = get_filebase(L,2);
    ASSERT(str);
    int _ret_0 = (int)pfilebase->ReadZeroEndString(str);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_strlen(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t _ret_0 = (fsize_t)pfilebase->StrLen();
    lua_pushinteger(L,_ret_0);
    return 1;
}

static int filebase_write(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *buf = get_filebase(L,2);
    ASSERT(buf);
    int n = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pfilebase->WriteFile(buf,0,n);
    lua_pushinteger(L,_ret_0);
    return 1;
}
static int filebase_isemptychar(lua_State *L)
{
    int ch = (int)lua_tointeger(L,1);
    int _ret_0 = (int)CFileBase::IsEmptyChar(ch);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_delete(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t del_size = (fsize_t)lua_tointeger(L,2);
    status_t _ret_0 = pfilebase->Delete(del_size);
    lua_pushboolean(L,_ret_0!=0);
    return 1;
}
static int filebase_deletelast(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t bytes = (fsize_t)lua_tointeger(L,2);
    int _ret_0 = (int)pfilebase->DeleteLast(bytes);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_dump(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->Dump();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_endwith(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* str = (const char*)lua_tostring(L,2);
	ASSERT(str);
    int case_sensive = (int)lua_toboolean(L,3);
    int skip_empty = (int)lua_toboolean(L,4);
    int _ret_0 = (int)pfilebase->EndWith(str,case_sensive,skip_empty);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_fillblock(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int fill_size = (int)lua_tointeger(L,2);
    int fill_ch = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pfilebase->FillBlock(fill_size,fill_ch);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_insert_v1(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->Insert(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int filebase_insert_v2(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int start = (int)lua_tointeger(L,3);
    int size = (int)lua_tointeger(L,4);
    int _ret_0 = (int)pfilebase->Insert(file,start,size);
    lua_pushboolean(L,_ret_0);
    return 1;
}

static int filebase_insert(lua_State *L)
{
    if(lua_isinteger(L,3) && lua_isinteger(L,4))
        return filebase_insert_v2(L);
    else if(is_filebase(L,2))
        return filebase_insert_v1(L);
    CAN_NOT_REACH();
}

static int filebase_isspchar(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int ch = (int)lua_tointeger(L,2);
    int _ret_0 = (int)pfilebase->IsSpChar(ch);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_loadfile(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
	LOCAL_MEM(buf);
    const char* fn = (const char*)lua_to_local_string(L,2,&buf);
    int _ret_0 = (int)pfilebase->LoadFile(fn);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readcstr(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadCStr(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readline(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadLine(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readline_reverse(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadLine_Reverse(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readquotestr(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* begin_and_end_char = (const char*)lua_tostring(L,2);
	ASSERT(begin_and_end_char);
    CFileBase *file = get_filebase(L,3);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadQuoteStr(begin_and_end_char[0],begin_and_end_char[1],file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readstring(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadString(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_readstring_reverse(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    int _ret_0 = (int)pfilebase->ReadString_Reverse(file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_replacestr(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* src_str = (const char*)lua_tostring(L,2);
    const char* des_str = (const char*)lua_tostring(L,3);
	ASSERT(src_str && des_str);
    int case_sensive = (int)lua_toboolean(L,4);
    int word_only = (int)lua_toboolean(L,5);
    CFileBase *new_file = get_filebase(L,6);
    ASSERT(new_file);
	if(src_str[0] == 0)
	{
		new_file->SetSize(0);
		new_file->WriteFile(pfilebase);
		return 0;
	}
    int _ret_0 = (int)pfilebase->ReplaceStr(src_str,des_str,case_sensive,word_only,new_file);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_reverse(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int start = (int)lua_tointeger(L,2);
    int end = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pfilebase->Reverse(start,end);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_setchar(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int index = (int)lua_tointeger(L,2);
    int ch = (int)lua_tointeger(L,3);
    int _ret_0 = (int)pfilebase->SetChar(index,ch);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_setfilename(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* fn = (const char*)lua_tostring(L,2);
	ASSERT(fn);
    int _ret_0 = (int)pfilebase->SetFileName(fn);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_setsize(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    fsize_t ssize = (fsize_t)lua_tointeger(L,2);
    int _ret_0 = (int)pfilebase->SetSize(ssize);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_setsplitchars(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* sp_str = (const char*)lua_tostring(L,2);
    ASSERT(sp_str);
    int len = strlen(sp_str);

    char *sp_str_malloc;
    MALLOC(sp_str_malloc,char,len+1);
    crt_strcpy(sp_str_malloc,sp_str);

    int _ret_0 = (int)pfilebase->SetSplitChars(sp_str_malloc);
    pfilebase->is_sp_chars_malloc = true;
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_skipemptychars(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->SkipEmptyChars();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_skipemptychars_reverse(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->SkipEmptyChars_Reverse();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_startwith(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* str = (const char*)lua_tostring(L,2);
	ASSERT(str);
    int case_sensive = (int)lua_toboolean(L,3);
    int skip_empty = (int)lua_toboolean(L,4);
    int _ret_0 = (int)pfilebase->StartWith(str,case_sensive,skip_empty);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_strcat(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* str = (const char*)lua_tostring(L,2);
	ASSERT(str);
    int _ret_0 = (int)pfilebase->StrCat(str);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_strlwr(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->StrLwr();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_strupr(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->StrUpr();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_trim(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    int _ret_0 = (int)pfilebase->Trim();
    lua_pushboolean(L,_ret_0);
    return 1;
}
static int filebase_getfilename(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    const char* _ret_0 = (const char*)pfilebase->GetFileName();
    lua_pushstring(L,_ret_0);
    return 1;
}
static int filebase_readlinewitheol(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    CFileBase *eol = get_filebase(L,3);
    ASSERT(eol);
    int _ret_0 = (int)pfilebase->ReadLineWithEol(file,eol);
    lua_pushboolean(L,_ret_0);
    return 1;
}
static status_t filebase_nextword(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);

    LOCAL_MEM_WITH_SIZE(mem,LBUF_SIZE*8);    
    if(pfilebase->ReadWord(&mem))
    {
        lua_pushlstring(L,mem.CStr(),mem.StrLen());
        return 1;
    }
    return 0;
}

static status_t filebase_nextstring(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);

    LOCAL_MEM_WITH_SIZE(mem,LBUF_SIZE*8);    
    if(pfilebase->ReadString(&mem))
    {
        lua_pushlstring(L,mem.CStr(),mem.StrLen());
        return 1;
    }
    return 0;
}

static status_t filebase_nextline(lua_State *L)
{
    CFileBase *pfilebase = get_filebase(L,1);
    ASSERT(pfilebase);

    LOCAL_MEM_WITH_SIZE(mem,LBUF_SIZE*8);
    if(pfilebase->ReadLine(&mem))
    {
        lua_pushlstring(L,mem.CStr(),mem.StrLen());
        return 1;
    }
    return 0;
}

static const luaL_Reg filebase_lib[] = {
    {"__gc",filebase_gc_},
    {"__tostring",filebase_tostring_},
    {"__is_same",filebase_issame_},
    {"IsEnd",filebase_isend},
    {"CharAt",filebase_charat},
    {"GetLastChar",filebase_getlastchar},
    {"Getc",filebase_getc},
    {"UnGetc",filebase_ungetc},
    {"GetMaxSize",filebase_getmaxsize},
    {"GetOffset",filebase_getoffset},
    {"GetSize",filebase_getsize},
    {"Putc",filebase_putc},
    {"Puts",filebase_puts},
    {"ReadLeftStr",filebase_readleftstr},
    {"ReadWord",filebase_readword},
    {"ReadWordWithEmptyChar",filebase_readwordwithemptychar},
    {"ReadWordWithEmptyChar_Reverse",filebase_readwordwithemptychar_reverse},
    {"ReadWord_Reverse",filebase_readword_reverse},
    {"SearchBin",filebase_searchbin},
    {"SearchStr",filebase_searchstr},
    {"Seek",filebase_seek},
    {"SeekBack",filebase_seekback},
    {"SeekEnd",filebase_seekend},
    {"Split",filebase_split},
    {"WriteFile",filebase_writefile},
    {"WriteToFile",filebase_writetofile},
    {"StrCmp",filebase_strcmp},
    {"StrICmp",filebase_stricmp},
    {"Read",filebase_read},
    {"ReadZeroEndString",filebase_readzeroendstring},
    {"StrLen",filebase_strlen},
    {"Write",filebase_write},
    {"IsEmptyChar",filebase_isemptychar},
    {"Delete",filebase_delete},
    {"DeleteLast",filebase_deletelast},
    {"Dump",filebase_dump},
    {"EndWith",filebase_endwith},
    {"FillBlock",filebase_fillblock},
    {"Insert",filebase_insert},
    {"IsSpChar",filebase_isspchar},
    {"LoadFile",filebase_loadfile},
    {"ReadCStr",filebase_readcstr},
    {"ReadLine",filebase_readline},
    {"ReadLine_Reverse",filebase_readline_reverse},
    {"ReadQuoteStr",filebase_readquotestr},
    {"ReadString",filebase_readstring},
    {"ReadString_Reverse",filebase_readstring_reverse},
    {"ReplaceStr",filebase_replacestr},
    {"Reverse",filebase_reverse},
    {"SetChar",filebase_setchar},
    {"SetFileName",filebase_setfilename},
    {"SetSize",filebase_setsize},
    {"SetSplitChars",filebase_setsplitchars},
    {"SkipEmptyChars",filebase_skipemptychars},
    {"SkipEmptyChars_Reverse",filebase_skipemptychars_reverse},
    {"StartWith",filebase_startwith},
    {"StrCat",filebase_strcat},
    {"StrLwr",filebase_strlwr},
    {"StrUpr",filebase_strupr},
    {"Trim",filebase_trim},
    {"GetFileName",filebase_getfilename},
    {"ReadLineWithEol",filebase_readlinewitheol},
    {"NextWord",filebase_nextword},
    {"NextString",filebase_nextstring},
    {"NextLine",filebase_nextline},
    {NULL, NULL}
};

const luaL_Reg* get_filebase_funcs()
{
    return filebase_lib;
}

static int luaL_register_filebase(lua_State *L)
{	
    static luaL_Reg _filebase_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_filebase_funcs_,&_index,get_filebase_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_FILEBASE);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_filebase_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_filebase_funcs_);
    return 1;
}        

int luaopen_filebase(lua_State *L)
{
    luaL_requiref(L, "FileBase",luaL_register_filebase,1);
    lua_pop(L, 1);
    return 0;
}
