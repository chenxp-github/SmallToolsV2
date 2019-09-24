#ifndef __S2_HTTPPAIR_H
#define __S2_HTTPPAIR_H

#include "cruntime.h"
#include "filebase.h"
#include "mem.h"

class CHttpPair{
public:
    CMem m_Key;
    CMem m_Value;
public:
    status_t ToString(CMem *out);
    CHttpPair();
    virtual ~CHttpPair();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CHttpPair *_p);
    int Comp(CHttpPair *_p);
    status_t Print(CFileBase *_buf);
    CMem* GetKey();
    CMem* GetValue();
    status_t SetKey(CMem* _key);
    status_t SetValue(CMem* _value);
    const char* GetKeyStr();
    const char* GetValueStr();
    status_t SetKey(const char *_key);
    status_t SetValue(const char *_value);
};

#endif
