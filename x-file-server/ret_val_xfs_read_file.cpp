#include "ret_val_xfs_read_file.h"
#include "sys_log.h"
#include "mem_tool.h"

CRetVal_XfsReadFile::CRetVal_XfsReadFile()
{
    this->InitBasic();
}

CRetVal_XfsReadFile::~CRetVal_XfsReadFile()
{
    this->Destroy();
}

status_t CRetVal_XfsReadFile::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_result = 0;
    this->m_data = NULL;
    this->m_data_size = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRetVal_XfsReadFile::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CRetVal_XfsReadFile::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    DEL_ARRAY(m_data);
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRetVal_XfsReadFile::Copy(CRetVal_XfsReadFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    int i = 0;

    this->m_result = _p->m_result;
    this->AllocData(_p->m_data_size);
    for(i = 0; i < m_data_size; i++)
    {
        this->m_data[i] = _p->m_data[i];
    }
/*##End Copy##*/
    return OK;
}

int CRetVal_XfsReadFile::Comp(CRetVal_XfsReadFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRetVal_XfsReadFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    int i = 0;

    _buf->Log("result = %d",m_result);
    _buf->Log("data = [");
    _buf->IncLogLevel(1);
    for(i = 0; i < m_data_size; i++)
    {
        _buf->Log("%d,",m_data[i]);
    }
    _buf->IncLogLevel(-1);
    _buf->Log("]");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetResult@@*/
int CRetVal_XfsReadFile::GetResult()
{
    return m_result;
}
/*@@End  Function GetResult@@*/

/*@@Begin Function GetData@@*/
uint8_t* CRetVal_XfsReadFile::GetData()
{
    return m_data;
}
/*@@End  Function GetData@@*/

/*@@Begin Function GetDataSize@@*/
int CRetVal_XfsReadFile::GetDataSize()
{
    return m_data_size;
}
/*@@End  Function GetDataSize@@*/

/*@@Begin Function GetDataElem@@*/
uint8_t CRetVal_XfsReadFile::GetDataElem(int _index)
{
    ASSERT(this->m_data);
    ASSERT(_index >= 0 && _index < m_data_size);
    return m_data[_index];
}
/*@@End  Function GetDataElem@@*/

/*@@Begin Function SetResult@@*/
status_t CRetVal_XfsReadFile::SetResult(int _result)
{
    this->m_result = _result;
    return OK;
}
/*@@End  Function SetResult@@*/

/*@@Begin Function AllocData@@*/
status_t CRetVal_XfsReadFile::AllocData(int _len)
{
    if(m_data_size == _len)
        return OK;
    DEL_ARRAY(this->m_data);
    if(_len > 0)
    {
        NEW_ARRAY(this->m_data,uint8_t,_len);
        memset(this->m_data,0,sizeof(uint8_t)*_len);
    }
    this->m_data_size = _len;
    return OK;
}
/*@@End  Function AllocData@@*/

/*@@Begin Function SetData@@*/
status_t CRetVal_XfsReadFile::SetData(uint8_t _data[], int _len)
{
    ASSERT(_data);
    this->AllocData(_len);
    memcpy(this->m_data,_data,sizeof(uint8_t)*_len);
    return OK;
}
/*@@End  Function SetData@@*/

/*@@Begin Function SetData_V2@@*/
status_t CRetVal_XfsReadFile::SetData(uint8_t *_data[], int _len)
{
    ASSERT(_data);
    this->AllocData(_len);
    memcpy(this->m_data,_data,sizeof(uint8_t)*_len);
    return OK;
}
/*@@End  Function SetData_V2@@*/

/*@@Begin Function SetDataElem@@*/
status_t CRetVal_XfsReadFile::SetDataElem(int _index,uint8_t _data)
{
    ASSERT(this->m_data);
    ASSERT(_index >= 0 && _index < m_data_size);
    this->m_data[_index] = _data;
    return OK;
}
/*@@End  Function SetDataElem@@*/

status_t CRetVal_XfsReadFile::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    if(this->m_data)
    {
        _bson->PutBinary("data",m_data,sizeof(uint8_t)*m_data_size);
    }
    _bson->PutInt32("result",m_result);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRetVal_XfsReadFile::SaveBson(CMem *_mem)
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

status_t CRetVal_XfsReadFile::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin LoadBson_1##*/
    /*******data start********/{
    CMem _tmp_bin;
    if(_bson->GetBinary("data",&_tmp_bin))
    {
        int _tmp_len = (int)_tmp_bin.GetSize()/sizeof(uint8_t);
        this->SetData((uint8_t*)_tmp_bin.GetRawBuf(),_tmp_len);
    }
    /*******data end********/}
    BSON_CHECK(_bson->GetInt32("result",&m_result));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRetVal_XfsReadFile::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
