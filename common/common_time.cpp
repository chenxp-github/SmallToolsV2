#include "common_time.h"
#include "syslog.h"
#include "mem_tool.h"

#define YEAR0 1900

CCommonTime::CCommonTime()
{
    this->InitBasic();
}

CCommonTime::~CCommonTime()
{
    this->Destroy();
}

status_t CCommonTime::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_year = 0;
    this->m_month = 0;
    this->m_day = 0;
    this->m_hour = 0;
    this->m_minute = 0;
    this->m_second = 0;
    this->m_msec = 0;
    this->m_week_day = 0;
    this->m_year_day = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CCommonTime::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CCommonTime::Destroy()
{
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CCommonTime::Copy(CCommonTime *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_year = _p->m_year;
    this->m_month = _p->m_month;
    this->m_day = _p->m_day;
    this->m_hour = _p->m_hour;
    this->m_minute = _p->m_minute;
    this->m_second = _p->m_second;
    this->m_msec = _p->m_msec;
    this->m_week_day = _p->m_week_day;
    this->m_year_day = _p->m_year_day;
/*##End Copy##*/
    return OK;
}

int CCommonTime::Comp(CCommonTime *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CCommonTime::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("year = %d",m_year);
    _buf->Log("month = %d",m_month);
    _buf->Log("day = %d",m_day);
    _buf->Log("hour = %d",m_hour);
    _buf->Log("minute = %d",m_minute);
    _buf->Log("second = %d",m_second);
    _buf->Log("msec = %d",m_msec);
    _buf->Log("week_day = %d",m_week_day);
    _buf->Log("year_day = %d",m_year_day);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetYear@@*/
int CCommonTime::GetYear()
{
    return m_year;
}
/*@@End  Function GetYear@@*/

/*@@Begin Function GetMonth@@*/
int CCommonTime::GetMonth()
{
    return m_month;
}
/*@@End  Function GetMonth@@*/

/*@@Begin Function GetDay@@*/
int CCommonTime::GetDay()
{
    return m_day;
}
/*@@End  Function GetDay@@*/

/*@@Begin Function GetHour@@*/
int CCommonTime::GetHour()
{
    return m_hour;
}
/*@@End  Function GetHour@@*/

/*@@Begin Function GetMinute@@*/
int CCommonTime::GetMinute()
{
    return m_minute;
}
/*@@End  Function GetMinute@@*/

/*@@Begin Function GetSecond@@*/
int CCommonTime::GetSecond()
{
    return m_second;
}
/*@@End  Function GetSecond@@*/

/*@@Begin Function GetMsec@@*/
int CCommonTime::GetMsec()
{
    return m_msec;
}
/*@@End  Function GetMsec@@*/

/*@@Begin Function SetYear@@*/
status_t CCommonTime::SetYear(int _year)
{
    this->m_year = _year;
    return OK;
}
/*@@End  Function SetYear@@*/

/*@@Begin Function SetMonth@@*/
status_t CCommonTime::SetMonth(int _month)
{
    this->m_month = _month;
    return OK;
}
/*@@End  Function SetMonth@@*/

/*@@Begin Function SetDay@@*/
status_t CCommonTime::SetDay(int _day)
{
    this->m_day = _day;
    return OK;
}
/*@@End  Function SetDay@@*/

/*@@Begin Function SetHour@@*/
status_t CCommonTime::SetHour(int _hour)
{
    this->m_hour = _hour;
    return OK;
}
/*@@End  Function SetHour@@*/

/*@@Begin Function SetMinute@@*/
status_t CCommonTime::SetMinute(int _minute)
{
    this->m_minute = _minute;
    return OK;
}
/*@@End  Function SetMinute@@*/

/*@@Begin Function SetSecond@@*/
status_t CCommonTime::SetSecond(int _second)
{
    this->m_second = _second;
    return OK;
}
/*@@End  Function SetSecond@@*/

/*@@Begin Function SetMsec@@*/
status_t CCommonTime::SetMsec(int _msec)
{
    this->m_msec = _msec;
    return OK;
}
/*@@End  Function SetMsec@@*/

/*@@Begin Function SaveBson_1@@*/
status_t CCommonTime::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    _bson->PutInt32("day",m_day);
    _bson->PutInt32("hour",m_hour);
    _bson->PutInt32("minute",m_minute);
    _bson->PutInt32("month",m_month);
    _bson->PutInt32("msec",m_msec);
    _bson->PutInt32("second",m_second);
    _bson->PutInt32("year",m_year);
    return OK;
}
/*@@End  Function SaveBson_1@@*/

/*@@Begin Function SaveBson_2@@*/
status_t CCommonTime::SaveBson(CMem *_mem)
{
    ASSERT(_mem);
    CMiniBson _bson;
    _bson.Init();
    _bson.SetRawBuf(_mem);
    _bson.StartDocument();
    this->SaveBson(&_bson);
    _bson.EndDocument();
    _mem->SetSize(_bson.GetDocumentSize());
    return OK;
}
/*@@End  Function SaveBson_2@@*/

/*@@Begin Function LoadBson_1@@*/
status_t CCommonTime::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
    BSON_CHECK(_bson->GetInt32("day",&m_day));
    BSON_CHECK(_bson->GetInt32("hour",&m_hour));
    BSON_CHECK(_bson->GetInt32("minute",&m_minute));
    BSON_CHECK(_bson->GetInt32("month",&m_month));
    BSON_CHECK(_bson->GetInt32("msec",&m_msec));
    BSON_CHECK(_bson->GetInt32("second",&m_second));
    BSON_CHECK(_bson->GetInt32("year",&m_year));
    return OK;
}
/*@@End  Function LoadBson_1@@*/

/*@@Begin Function LoadBson_2@@*/
status_t CCommonTime::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}
/*@@End  Function LoadBson_2@@*/

status_t CCommonTime::LoadXml(CXmlNode *_root)
{
    ASSERT(_root);
    CMem *value = _root->GetValue();
    ASSERT(value);
    this->LoadReadableString(value);
    return OK;
}

status_t CCommonTime::SaveXml(CFileBase *_xml)
{
    ASSERT(_xml);
    LOCAL_MEM(str);
    this->SaveReadableString(&str);
    _xml->Log("%s",str.CStr());
    return OK;
}

/*@@Begin Function GetWeekDay@@*/
int CCommonTime::GetWeekDay()
{
    return m_week_day;
}
/*@@End  Function GetWeekDay@@*/

/*@@Begin Function GetYearDay@@*/
int CCommonTime::GetYearDay()
{
    return m_year_day;
}
/*@@End  Function GetYearDay@@*/

/*@@Begin Function SetWeekDay@@*/
status_t CCommonTime::SetWeekDay(int _week_day)
{
    this->m_week_day = _week_day;
    return OK;
}
/*@@End  Function SetWeekDay@@*/

/*@@Begin Function SetYearDay@@*/
status_t CCommonTime::SetYearDay(int _year_day)
{
    this->m_year_day = _year_day;
    return OK;
}
/*@@End  Function SetYearDay@@*/
/*@@ Insert Function Here @@*/

status_t CCommonTime::Set(int64_t lt)
{
    struct tm *local;    
    lt += (GetTimeZone()*3600*1000);
    time_t t = (time_t)(lt/1000);
    local=gmtime(&t);
    if(local == NULL)
        return ERROR;
    m_year = local->tm_year+YEAR0;
    m_month = local->tm_mon+1;
    m_day = local->tm_mday;
    m_hour = local->tm_hour;
    m_minute = local->tm_min;
    m_second = local->tm_sec;
    m_msec = (int)(lt%1000);
    m_week_day = local->tm_wday;
    m_year_day = local->tm_yday;
    return OK;
}

status_t CCommonTime::SetNow()
{
#if HAVE_WINDOWS_H
    SYSTEMTIME st;
    GetLocalTime(&st);
	int64_t lt = time(NULL);
	this->Set(lt*1000);
	this->SetMsec(st.wMilliseconds);	
#else
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->Set(((int64_t)tv.tv_sec)*1000);
    this->SetMsec(tv.tv_usec/1000);
#endif    
    return OK;
}

int64_t CCommonTime::GetLong()
{
    struct tm local;
    memset(&local,0,sizeof(local));
    local.tm_year=m_year-YEAR0;
    local.tm_mon=m_month-1;
    local.tm_mday=m_day;
    local.tm_hour=m_hour;
    local.tm_min=m_minute;
    local.tm_sec=m_second;
    int64_t t = mktime(&local);
    return t*1000+m_msec;
}

status_t CCommonTime::SetSystemTime()
{
#if HAVE_WINDOWS_H
    SYSTEMTIME st;
    st.wYear = m_year;
    st.wMonth = m_month;
    st.wDay = m_day;
    st.wHour = m_hour;
    st.wMinute = m_minute;
    st.wSecond = m_second;
    st.wDayOfWeek = m_week_day;
    st.wMilliseconds = m_msec;
    SetLocalTime(&st);
#else    
    struct timeval tv;
    int64_t lval = this->GetLong();
    tv.tv_sec = lval / 1000;
    tv.tv_usec = (m_msec*1000);
    settimeofday(&tv, NULL);
#endif
    return OK;
}

status_t CCommonTime::SaveReadableString(CFileBase *out)
{
    ASSERT(out);
    out->Printf("%04d-%02d-%02d %02d:%02d:%02d.%03d",
        m_year,m_month,m_day,
        m_hour,m_minute,m_second,m_msec
    );
    return OK;
}

status_t CCommonTime::LoadReadableString(CFileBase *in)
{
    ASSERT(in);

    LOCAL_MEM(mem);
    
    in->ReadString(&mem);
    m_year = atoi(mem.CStr());
    
    in->ReadString(&mem);
    m_month = atoi(mem.CStr());
    
    in->ReadString(&mem);
    m_day = atoi(mem.CStr());
    
    in->ReadString(&mem);
    m_hour = atoi(mem.CStr());
    
    in->ReadString(&mem);
    m_minute = atoi(mem.CStr());
    
    in->ReadString(&mem);
    m_second = atoi(mem.CStr());
    
    in->ReadString(&mem);
    m_msec = atoi(mem.CStr());
    
    return OK;
}

status_t CCommonTime::Clear()
{
    SAVE_WEAK_REF_ID(*this,w);
    this->Destroy();
    this->Init();
    RESTORE_WEAK_REF_ID(*this,w);
    return OK;
}

status_t CCommonTime::Forward(int64_t msec)
{
    int64_t l = this->GetLong();
    return this->Set(l+msec);
}

#define INVALID_TIME_ZONE 8888

static int cached_time_zone = INVALID_TIME_ZONE;
int CCommonTime::GetTimeZone()
{
    if(cached_time_zone != INVALID_TIME_ZONE)
    {
        return cached_time_zone;
    }
	time_t time_utc = 0;
	struct tm *p_tm_time;	
	p_tm_time = localtime( &time_utc );
	cached_time_zone = (p_tm_time->tm_hour > 12)?( p_tm_time->tm_hour-=24):p_tm_time->tm_hour;
	return cached_time_zone;
}

