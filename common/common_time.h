#ifndef __COMMON_TIME_H
#define __COMMON_TIME_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "xml.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CCommonTime
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_minute;
    int m_second;
    int m_msec;
    int m_week_day;
    int m_year_day;
/*##End Members##*/

public:
    CCommonTime();
    virtual ~CCommonTime();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CCommonTime *_p);
    int Comp(CCommonTime *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetYear();
    int GetMonth();
    int GetDay();
    int GetHour();
    int GetMinute();
    int GetSecond();
    int GetMsec();
    int GetWeekDay();
    int GetYearDay();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetYear(int _year);
    status_t SetMonth(int _month);
    status_t SetDay(int _day);
    status_t SetHour(int _hour);
    status_t SetMinute(int _minute);
    status_t SetSecond(int _second);
    status_t SetMsec(int _msec);
    status_t SetWeekDay(int _week_day);
    status_t SetYearDay(int _year_day);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/

/*##Begin XML##*/
    status_t LoadXml(CXmlNode *_root);
    status_t SaveXml(CFileBase *_xml);
    status_t LoadXml(const char *fn, const char *path);
    status_t SaveXml(const char *fn, const char *node_name);
/*##End XML##*/
    status_t SetNow();
    status_t Set(int64_t tm);
    int64_t GetLong();

    status_t SetSystemTime();
    status_t SaveReadableString(CFileBase *out);
    status_t LoadReadableString(CFileBase *in);

    status_t Clear();
    status_t Forward(int64_t msec);

    static int GetTimeZone();
};

#endif
