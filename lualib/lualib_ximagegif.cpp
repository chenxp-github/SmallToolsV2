#include "lualib_ximagegif.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_ximage.h"
#include "lualib_filebase.h"
#include "lua_helper.h"

static bool ximagegif_is_userdata_valid(lua_userdata *ud)
{
    if(ud == NULL)return false;
    if(ud->p == NULL)return false;
    if(ud->__weak_ref_id == 0) return false;
    CHECK_IS_UD_READABLE(CxImageGIF,ud);
    CxImageGIF *p = (CxImageGIF*)ud->p;
    return p->__weak_ref_id == ud->__weak_ref_id;
}    

bool is_ximagegif(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XIMAGEGIF,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return ximagegif_is_userdata_valid(ud);  
}

CxImageGIF *get_ximagegif(lua_State *L, int idx)
{
    lua_userdata *ud = NULL;
    if(is_ximagegif(L,idx))
    {
        ud = (lua_userdata*)lua_touserdata(L,idx);		
    }
    ASSERT(ud);
    return (CxImageGIF *)ud->p;
} 

lua_userdata *ximagegif_new_userdata(lua_State *L,CxImageGIF *pobj,int is_weak)
{
    lua_userdata *ud = (lua_userdata*)lua_newuserdata(L,sizeof(lua_userdata));
    ASSERT(ud && pobj);
    ud->p = pobj;
    ud->is_attached = is_weak;
    ud->__weak_ref_id = pobj->__weak_ref_id;
    luaL_getmetatable(L,LUA_USERDATA_XIMAGEGIF);
    lua_setmetatable(L,-2);
    return ud;
}

static int ximagegif_gc_(lua_State *L)
{
    if(!is_ximagegif(L,1)) return 0;

    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);		
    ASSERT(ud);

    if(!(ud->is_attached))
    {
        CxImageGIF *pximagegif = (CxImageGIF*)ud->p;
        DEL(pximagegif);
    }
    return 0;
}

static int ximagegif_issame_(lua_State *L)
{
    CxImageGIF *pximagegif1 = get_ximagegif(L,1);
    ASSERT(pximagegif1);
    CxImageGIF *pximagegif2 = get_ximagegif(L,2);
    ASSERT(pximagegif2);
    int is_same = (pximagegif1==pximagegif2);
    lua_pushboolean(L,is_same);
    return 1;
}

static int ximagegif_tostring_(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    char buf[1024];
    sprintf(buf,"userdata:ximagegif:%p",pximagegif);
    lua_pushstring(L,buf);
    return 1;
}

/****************************************************/
static status_t ximagegif_new(lua_State *L)
{
    CxImageGIF *pximagegif;
    NEW(pximagegif,CxImageGIF);
    pximagegif->Init();
    ximagegif_new_userdata(L,pximagegif,0);
    return 1;
}

static status_t ximagegif_decodeextension(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    BOOL ret0 = pximagegif->DecodeExtension(fp);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ximagegif_encodeloopextension(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    pximagegif->EncodeLoopExtension(fp);
    return 0;
}

static status_t ximagegif_encodebody(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    BOOL blocalcolormap = (BOOL)lua_toboolean(L,3);
    pximagegif->EncodeBody(fp,blocalcolormap);
    return 0;
}

static status_t ximagegif_encodeextension(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    pximagegif->EncodeExtension(fp);
    return 0;
}

static status_t ximagegif_encodecomment(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    pximagegif->EncodeComment(fp);
    return 0;
}

static status_t ximagegif_encodeheader(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    pximagegif->EncodeHeader(fp);
    return 0;
}

static status_t ximagegif_encodergb(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    BOOL ret0 = pximagegif->EncodeRGB(fp);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ximagegif_getpixelindex(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    int x = (int)lua_tointeger(L,2);
    int y = (int)lua_tointeger(L,3);
    int ret0 = pximagegif->GetPixelIndex(x,y);
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t ximagegif_gifnextpixel(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    int ret0 = pximagegif->GifNextPixel();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t ximagegif_decode(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    BOOL ret0 = pximagegif->Decode(fp);
    lua_pushboolean(L,ret0);
    return 1;
}

status_t to_ximage_array(lua_State *L, int index, CxImage **imgs, int *imgs_size)
{
    ASSERT(L && imgs && imgs_size);    
    if(!lua_istable(L,index))
        return ERROR;
    int top = lua_gettop(L);
    lua_pushnil(L);

    int max = *imgs_size;
    int k = 0;
    while (lua_next(L, index) != 0) 
    {
        ASSERT(k < max);
        imgs[k] = get_ximage(L,-1);
        k++;
        lua_pop(L, 1);
    }
    *imgs_size = k;
    lua_settop(L,top);
    return OK;
}

static status_t ximagegif_encode_v1(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);

    #define MAX_FRAMES 10000
    CxImage *pimages[MAX_FRAMES];
    int size = MAX_FRAMES;
    to_ximage_array(L,3,pimages,&size);
    ASSERT(size > 0);

    BOOL blocalcolormap = (BOOL)lua_toboolean(L,4);
    BOOL blocaldispmeth = (BOOL)lua_toboolean(L,5);
    BOOL ret0 = pximagegif->Encode(
        fp,pimages,size,
        blocalcolormap,blocaldispmeth,
        how_to_get_lua_running_flag()
    );

    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ximagegif_encode_v2(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    CFileBase *fp = get_filebase(L,2);
    ASSERT(fp);
    BOOL ret0 = pximagegif->Encode(fp);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t ximagegif_encode(lua_State *L)
{
    if(is_filebase(L,2) && lua_istable(L,3))
    {
        return ximagegif_encode_v1(L);
    }

    if(is_filebase(L,2))
    {
        return ximagegif_encode_v2(L);
    }

    return 0;
}

static status_t ximagegif_setloops(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    int loops = (int)lua_tointeger(L,2);
    pximagegif->SetLoops(loops);
    return 0;
}

static status_t ximagegif_getloops(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    int ret0 = pximagegif->GetLoops();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t ximagegif_setcomment(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);
    const char* sz_comment_in = (const char*)lua_tostring(L,2);
    ASSERT(sz_comment_in);
    pximagegif->SetComment(sz_comment_in);
    return 0;
}

static status_t ximagegif_getcomment(lua_State *L)
{
    CxImageGIF *pximagegif = get_ximagegif(L,1);
    ASSERT(pximagegif);

    char buf[LBUF_SIZE]={0};
    
    pximagegif->GetComment(buf);
    lua_pushstring(L,buf);
    return 1;
}

/****************************************************/
static const luaL_Reg ximagegif_funcs_[] = {
    {"__gc",ximagegif_gc_},
    {"__tostring",ximagegif_tostring_},
    {"__is_same",ximagegif_issame_},
    {"new",ximagegif_new},
    {"DecodeExtension",ximagegif_decodeextension},
    {"EncodeLoopExtension",ximagegif_encodeloopextension},
    {"EncodeBody",ximagegif_encodebody},
    {"EncodeExtension",ximagegif_encodeextension},
    {"EncodeComment",ximagegif_encodecomment},
    {"EncodeHeader",ximagegif_encodeheader},
    {"EncodeRGB",ximagegif_encodergb},
    {"GetPixelIndex",ximagegif_getpixelindex},
    {"GifNextPixel",ximagegif_gifnextpixel},
    {"Decode",ximagegif_decode},
    {"Encode",ximagegif_encode},
    {"SetLoops",ximagegif_setloops},
    {"GetLoops",ximagegif_getloops},
    {"SetComment",ximagegif_setcomment},
    {"GetComment",ximagegif_getcomment},
    {NULL,NULL},
};

const luaL_Reg* get_ximagegif_funcs()
{
    return ximagegif_funcs_;
}

static int luaL_register_ximagegif(lua_State *L)
{	
    static luaL_Reg _ximagegif_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_ximagegif_funcs_,&_index,get_ximage_funcs(),false);
    CLuaVm::CombineLuaFuncTable(_ximagegif_funcs_,&_index,get_ximagegif_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_XIMAGEGIF);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_ximagegif_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_ximagegif_funcs_);
    return 1;
}        

int luaopen_ximagegif(lua_State *L)
{
    luaL_requiref(L, "XImageGIF",luaL_register_ximagegif,1);
    lua_pop(L, 1);
    return 0;
}        

