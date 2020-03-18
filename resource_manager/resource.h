#ifndef __S2_RESOURCE_H
#define __S2_RESOURCE_H

#include "cruntime.h"
#include "filebase.h"
#include "mem.h"
#include "minibson.h"

#define HASH_ENTRY_DEFINE(type)\
type *__next;\
int32_t __next_index,__cur_index\

#define HASH_ENTRY_CLEAR()\
__next = NULL;\
__next_index=-1;__cur_index=-1\


class CResource{
public:
    enum{
        INNER_BUF_SIZE = 256,
    };

    enum{
        TYPE_UNKNOWN = 0,
        TYPE_STRING,
        TYPE_CSS,
        TYPE_9_PNG,
        TYPE_PACKED_IMAGE,
        TYPE_RAW_IMAGE,
    };
	WEAK_REF_DEFINE();
    HASH_ENTRY_DEFINE(CResource);
public:
    int32_t m_Type;

    char _id_buf[INNER_BUF_SIZE];
    CMem m_Id;

    char _theme_buf[INNER_BUF_SIZE];
    CMem m_Theme;

    char _language_buf[INNER_BUF_SIZE];
    CMem m_Language;

    CMiniBson m_BsonData;    
public:
    status_t UnSerialize(CFileBase *buf);
    status_t Serialize(CFileBase *buf);
    CResource();
    virtual ~CResource();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CResource *_p);
    int Comp(CResource *_p);
    status_t Print(CFileBase *_buf);
    int32_t GetType();
    CMem* GetId();
    CMem* GetTheme();
    CMem* GetLanguage();
    CMiniBson* GetBsonData();
    status_t SetType(int32_t _type);
    status_t SetId(CMem* _id);
    status_t SetTheme(CMem* _theme);
    status_t SetLanguage(CMem* _language);
    status_t SetBsonData(CMiniBson* _bsondata);
    const char* GetIdStr();
    const char* GetThemeStr();
    const char* GetLanguageStr();
    status_t SetId(const char *_id);
    status_t SetTheme(const char *_theme);
    status_t SetLanguage(const char *_language);
};

#endif
