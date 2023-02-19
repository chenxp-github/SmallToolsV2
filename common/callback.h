#ifndef __S2_CALLBACK_H
#define __S2_CALLBACK_H

#include "cruntime.h"
#include "filebase.h"
#include "closure.h"


class CCallback{
public:
    enum{
        E_TIMEOUT = 0xffffffff,
        E_OK = OK,
    };

    int m_CallbackId;
    int m_Time,m_Timeout;
    CClosure *i_Closure;
    CCallback *next;
public:
	status_t ResetTimeout();
    status_t OnTimeout();
    status_t DelClosure();
    bool IsTimeout();
    status_t IncTime(int ms);
    status_t SetTimeout(int to);
    CClosure* GetClosure();
    status_t SetClosure(CClosure *closure);
    CCallback();
    virtual ~CCallback();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CCallback *_p);
    int Comp(CCallback *_p);
    status_t Print(CFileBase *_buf);
    int GetCallbackId();
    status_t SetCallbackId(int _callbackid);
};

#endif
