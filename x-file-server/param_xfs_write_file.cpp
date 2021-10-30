#include "param_xfs_write_file.h"
#include "sys_log.h"
#include "mem_tool.h"

CParam_XfsWriteFile::CParam_XfsWriteFile()
{
    this->InitBasic();
}

CParam_XfsWriteFile::~CParam_XfsWriteFile()
{
    this->Destroy();
}

status_t CParam_XfsWriteFile::InitBasic()
{
/*##Begin InitBasic##*/
    CRpcParamBase::InitBasic();
    this->m_handle = 0;
    this->m_offset = 0;
    this->m_data = NULL;
    this->m_data_size = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CParam_XfsWriteFile::Init()
{
    this->InitBasic();
/*##Begin Init##*/
    CRpcParamBase::Init();
/*##End Init##*/
    return OK;
}

status_t CParam_XfsWriteFile::Destroy()
{
/*##Begin Destroy##*/
    CRpcParamBase::Destroy();
    DEL_ARRAY(m_data);
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CParam_XfsWriteFile::Copy(CParam_XfsWriteFile *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    CRpcParamBase::Copy(_p);
    int i = 0;

    this->m_handle = _p->m_handle;
    this->m_offset = _p->m_offset;
    this->AllocData(_p->m_data_size);
    for(i = 0; i < m_data_size; i++)
    {
        this->m_data[i] = _p->m_data[i];
    }
/*##End Copy##*/
    return OK;
}

int CParam_XfsWriteFile::Comp(CParam_XfsWriteFile *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CParam_XfsWriteFile::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    CRpcParamBase::Print(_buf);
    int i = 0;

    _buf->Log("handle = %d",m_handle);
    _buf->Log("offset = %lld",m_offset);
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

/*@@Begin Function GetHandle@@*/
int CParam_XfsWriteFile::GetHandle()
{
    return m_handle;
}
/*@@End  Function GetHandle@@*/

/*@@Begin Function GetOffset@@*/
int64_t CParam_XfsWriteFile::GetOffset()
{
    return m_offset;
}
/*@@End  Function GetOffset@@*/

/*@@Begin Function GetData@@*/
uint8_t* CParam_XfsWriteFile::GetData()
{
    return m_data;
}
/*@@End  Function GetData@@*/

/*@@Begin Function GetDataSize@@*/
int CParam_XfsWriteFile::GetDataSize()
{
    return m_data_size;
}
/*@@End  Function GetDataSize@@*/

/*@@Begin Function GetDataElem@@*/
uint8_t CParam_XfsWriteFile::GetDataElem(int _index)
{
    ASSERT(this->m_data);
    ASSERT(_index >= 0 && _index < m_data_size);
    return m_data[_index];
}
/*@@End  Function GetDataElem@@*/

/*@@Begin Function SetHandle@@*/
status_t CParam_XfsWriteFile::SetHandle(int _handle)
{
    this->m_handle = _handle;
    return OK;
}
/*@@End  Function SetHandle@@*/

/*@@Begin Function SetOffset@@*/
status_t CParam_XfsWriteFile::SetOffset(int64_t _offset)
{
    this->m_offset = _offset;
    return OK;
}
/*@@End  Function SetOffset@@*/

/*@@Begin Function AllocData@@*/
status_t CParam_XfsWriteFile::AllocData(int _len)
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
status_t CParam_XfsWriteFile::SetData(uint8_t _data[], int _len)
{
    ASSERT(_data);
    this->AllocData(_len);
    memcpy(this->m_data,_data,sizeof(uint8_t)*_len);
    return OK;
}
/*@@End  Function SetData@@*/

/*@@Begin Function SetData_V2@@*/
status_t CParam_XfsWriteFile::SetData(uint8_t *_data[], int _len)
{
    ASSERT(_data);
    this->AllocData(_len);
    memcpy(this->m_data,_data,sizeof(uint8_t)*_len);
    return OK;
}
/*@@End  Function SetData_V2@@*/

/*@@Begin Function SetDataElem@@*/
status_t CParam_XfsWriteFile::SetDataElem(int _index,uint8_t _data)
{
    ASSERT(this->m_data);
    ASSERT(_index >= 0 && _index < m_data_size);
    this->m_data[_index] = _data;
    return OK;
}
/*@@End  Function SetDataElem@@*/

status_t CParam_XfsWriteFile::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
/*##Begin SaveBson_1##*/
    if(this->m_data)
    {
        _bson->PutBinary("data",m_data,sizeof(uint8_t)*m_data_size);
    }
    _bson->PutInt32("handle",m_handle);
    _bson->PutInt64("offset",m_offset);
/*##End SaveBson_1##*/
    return OK;
}

status_t CParam_XfsWriteFile::SaveBson(CMem *_mem)
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

status_t CParam_XfsWriteFile::LoadBson(CMiniBson *_bson)
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
    BSON_CHECK(_bson->GetInt32("handle",&m_handle));
    BSON_CHECK(_bson->GetInt64("offset",&m_offset));
/*##End LoadBson_1##*/
    return OK;
}

status_t CParam_XfsWriteFile::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/
