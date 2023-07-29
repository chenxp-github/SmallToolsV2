#include "ret_val_rd_get_monitor_list.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_RdGetMonitorList::CRetVal_RdGetMonitorList()
{
    this->InitBasic();
}

CRetVal_RdGetMonitorList::~CRetVal_RdGetMonitorList()
{
    this->Destroy();
}

status_t CRetVal_RdGetMonitorList::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_monitors = NULL;
    this->m_monitors_size = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_RdGetMonitorList::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_RdGetMonitorList::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    DEL_ARRAY(m_monitors);
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_RdGetMonitorList::Copy(CRetVal_RdGetMonitorList *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    int i = 0;

    this->AllocMonitors(_p->m_monitors_size);
    for(i = 0; i < m_monitors_size; i++)
    {
        this->m_monitors[i].Copy(&_p->m_monitors[i]);
    }
/*##End Copy##*/
    return OK;
}

int CRetVal_RdGetMonitorList::Comp(CRetVal_RdGetMonitorList *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_RdGetMonitorList::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    int i = 0;

    _buf->Log("monitors = [");
    _buf->IncLogLevel(1);
    for(i = 0; i < m_monitors_size; i++)
    {
        _buf->Log("[%d] = %s",i,
            m_monitors[i].StrLen()>0?m_monitors[i].CStr():"<null>"
        );
    }
    _buf->IncLogLevel(-1);
    _buf->Log("]");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetMonitors@@*/
CMem* CRetVal_RdGetMonitorList::GetMonitors()
{
    return m_monitors;
}
/*@@End  Function GetMonitors@@*/

/*@@Begin Function GetMonitorsSize@@*/
int CRetVal_RdGetMonitorList::GetMonitorsSize()
{
    return m_monitors_size;
}
/*@@End  Function GetMonitorsSize@@*/

/*@@Begin Function GetMonitorsElem@@*/
CMem* CRetVal_RdGetMonitorList::GetMonitorsElem(int _index)
{
    ASSERT(this->m_monitors);
    ASSERT(_index >= 0 && _index < m_monitors_size);
    return &m_monitors[_index];
}
/*@@End  Function GetMonitorsElem@@*/

/*@@Begin Function AllocMonitors@@*/
status_t CRetVal_RdGetMonitorList::AllocMonitors(int _len)
{
    if(m_monitors_size == _len)
        return OK;
    DEL_ARRAY(this->m_monitors);
    if(_len > 0)
    {
        NEW_ARRAY(this->m_monitors,CMem,_len);
        for(int i = 0; i < _len; i++)
        {
            this->m_monitors[i].Init();
        }
    }
    this->m_monitors_size = _len;
    return OK;
}
/*@@End  Function AllocMonitors@@*/

/*@@Begin Function SetMonitors@@*/
status_t CRetVal_RdGetMonitorList::SetMonitors(CMem _monitors[], int _len)
{
    ASSERT(_monitors);
    this->AllocMonitors(_len);
    for(int i = 0; i < _len; i++)
    {
        this->m_monitors[i].Copy(&_monitors[i]);
    }
    return OK;
}
/*@@End  Function SetMonitors@@*/

/*@@Begin Function SetMonitors_V2@@*/
status_t CRetVal_RdGetMonitorList::SetMonitors(CMem *_monitors[], int _len)
{
    ASSERT(_monitors);
    this->AllocMonitors(_len);
    for(int i = 0; i < _len; i++)
    {
        this->m_monitors[i].Copy(_monitors[i]);
    }
    return OK;
}
/*@@End  Function SetMonitors_V2@@*/

/*@@Begin Function SetMonitorsElem@@*/
status_t CRetVal_RdGetMonitorList::SetMonitorsElem(int _index,CMem *_monitors)
{
    ASSERT(this->m_monitors);
    ASSERT(_index >= 0 && _index < m_monitors_size);
    ASSERT(_monitors);
    this->m_monitors[_index].Copy(_monitors);
    return OK;
}
/*@@End  Function SetMonitorsElem@@*/

/*@@Begin Function SetMonitorsElem-CStr@@*/
status_t CRetVal_RdGetMonitorList::SetMonitorsElem(int _index,const char *_monitors)
{
    ASSERT(this->m_monitors);
    ASSERT(_index >= 0 && _index < m_monitors_size);
    ASSERT(_monitors);
    CMem tmp(_monitors);
    this->m_monitors[_index].Copy(&tmp);
    return OK;
}
/*@@End  Function SetMonitorsElem-CStr@@*/

status_t CRetVal_RdGetMonitorList::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    /******monitors begin*******/{
    fsize_t _off;
    char _index_name[64];
    _bson->StartArray("monitors",&_off);
    for(int i = 0; i < m_monitors_size; i++)
    {
        sprintf(_index_name,"%d",i);
        _bson->PutString(_index_name,&m_monitors[i]);
    }
    _bson->EndArray(_off,m_monitors_size);
    /*******monitors end********/}
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_RdGetMonitorList::SaveBson(CMem *_mem)
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

status_t CRetVal_RdGetMonitorList::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /******monitors begin*******/{
    CMiniBson _doc;
    int _array_len = 0;
    if(_bson->GetArray("monitors",&_doc,&_array_len))
    {
        this->AllocMonitors(_array_len);
        for(int i = 0; i < m_monitors_size; i++)
        {
            CMem __str;
            BSON_CHECK(_doc.GetString(NULL,&__str));
            this->SetMonitorsElem(i,&__str);
        }
    }
    /*******monitors end********/}
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_RdGetMonitorList::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
