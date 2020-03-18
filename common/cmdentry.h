#ifndef __S2_CMDENTRY_H
#define __S2_CMDENTRY_H

#include "cruntime.h"
#include "filebase.h"
#include "mem.h"

#undef OPTIONAL

class CCmdEntry{
public:
    WEAK_REF_DEFINE();

    enum{
        TYPE_KEY=1,
        TYPE_VALUE,
        TYPE_KEY_VALUE,
        TYPE_KEY_EQUAL_VALUE,
        TYPE_KEY_SPACE_VALUE,
    };

    enum{
        MUST=1,
        OPTIONAL,
    };

public:
    int m_KeyType;
    CMem m_Key;
    CMem m_Value;
    CMem m_Help;
    int m_Option;
public:
    status_t SaveToArgv(int *argc, char **argv);
    status_t SetOption(int option);
    int GetOption();
    status_t ParseArgv(CMem *argv1, CMem *argv2,int *need_argv2);
    bool IsMatchKeyType(CMem *str);
    CCmdEntry();
    virtual ~CCmdEntry();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CCmdEntry *_p);
    int Comp(CCmdEntry *_p);
    status_t Print(CFileBase *_buf);
    int GetKeyType();
    CMem* GetKey();
    CMem* GetValue();
    status_t SetKeyType(int _keytype);
    status_t SetKey(CMem* _key);
    status_t SetValue(CMem* _value);
    const char* GetKeyStr();
    const char* GetValueStr();
    status_t SetKey(const char *_key);
    status_t SetValue(const char *_value);
    CMem* GetHelp();
    status_t SetHelp(CMem* _help);
    const char* GetHelpStr();
    status_t SetHelp(const char *_help);
};

#endif
