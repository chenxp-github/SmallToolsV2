#ifndef __HTTPHEADER_H
#define __HTTPHEADER_H

#include "cruntime.h"
#include "commonarray.h"
#include "httppair.h"

class CHttpHeader{
public:
    CCommonArray<CHttpPair> *mParis;
    CMem *mMethod;
    CMem *mRequestUrl;
    CMem *mHttpVersion;
    int mRetCode;
public:
    status_t GetContentRange(fsize_t *start, fsize_t *end);
    status_t Clear();
    int GetRetCode();
    status_t GetLocation(CMem *location);
    status_t DelPair(const char *key);
    status_t DelPair(CMem *key);
    status_t Save(CFileBase *file);
    fsize_t GetContentLength();
    CHttpPair* GetPair(const char *key);
    CHttpPair* GetPair(CMem *key);
    bool IsChunked();
    status_t SetPair(const char *key, const char *value);
    status_t Load(CFileBase *file);
    status_t SetPair(CMem *key, CMem *value);
    int KeyToIndex(CMem *key);
    CHttpPair* GetPairByIndex(int index);
    status_t SetMethod(CMem *method);
    status_t SetMethod(const char *method);
    status_t SetRequestUrl(CMem *requesturl);
    status_t SetRequestUrl(const char *requesturl);
    status_t SetHttpVersion(CMem *httpversion);
    status_t SetHttpVersion(const char *httpversion);
    CMem* GetMethod();
    const char* GetMethodStr();
    CMem* GetRequestUrl();
    const char* GetRequestUrlStr();
    CMem* GetHttpVersion();
    const char* GetHttpVersionStr();

    CHttpHeader();
    virtual ~CHttpHeader();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CHttpHeader *p);
    int Comp(CHttpHeader *p);
    status_t Print(CFileBase *_buf);
};

#endif
