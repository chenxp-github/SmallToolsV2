#include "x_file_client.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "linkrpc_xfs_types.h"

CxFileClient::CxFileClient()
{
    this->InitBasic();
}

CxFileClient::~CxFileClient()
{
    this->Destroy();
}

status_t CxFileClient::InitBasic()
{
    CRpcServiceBase::InitBasic();
    return OK;
}

status_t CxFileClient::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CRpcServiceBase::Init(mgr);

    return OK;
}

status_t CxFileClient::Destroy()
{
    CRpcServiceBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CxFileClient::OnSocketConnected()
{
    return OK;
}

status_t CxFileClient::OnSocketDisconnected()
{
    return OK;
}

status_t CxFileClient::OnRequest(CPeerMessage *_message)
{
    ASSERT(_message);

    int method = _message->GetFunc();
    this->SetDestPeerName(_message->GetFrom());

/*##Begin OnRequest_Cpp ##*/
    switch(method)
    {
        default:
            XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
                "unknown method: %d",method
            );
            break;
    }
/*##End OnRequest_Cpp ##*/
    return OK;
}

/*@@Begin Function ListFolder @@*/
status_t CxFileClient::ListFolder(CMem *_path, CClosure *_callback)
{
    CParam_XfsListFolder _param;
    _param.Init();
    _param.SetPath(_path);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_LISTFOLDER,_cbid);
}
/*@@End Function ListFolder @@*/


/*@@Begin Function GetInfo @@*/
status_t CxFileClient::GetInfo(CMem *_path, CClosure *_callback)
{
    CParam_XfsGetInfo _param;
    _param.Init();
    _param.SetPath(_path);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_GETINFO,_cbid);
}
/*@@End Function GetInfo @@*/

/*@@Begin Function ChangeCurrentDirectory @@*/
status_t CxFileClient::ChangeCurrentDirectory(CMem *_dir, CClosure *_callback)
{
    CParam_XfsChangeCurrentDirectory _param;
    _param.Init();
    _param.SetDir(_dir);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_CHANGECURRENTDIRECTORY,_cbid);
}
/*@@End Function ChangeCurrentDirectory @@*/

/*@@Begin Function GetCurrentDirectory @@*/
status_t CxFileClient::GetCurrentDirectory(CClosure *_callback)
{
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(NULL,METHOD_XFS_GETCURRENTDIRECTORY,_cbid);
}
/*@@End Function GetCurrentDirectory @@*/

/*@@Begin Function OpenFile @@*/
status_t CxFileClient::OpenFile(CMem *_path, CMem *_mode, CClosure *_callback)
{
    CParam_XfsOpenFile _param;
    _param.Init();
    _param.SetPath(_path);
    _param.SetMode(_mode);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_OPENFILE,_cbid);
}
/*@@End Function OpenFile @@*/

/*@@Begin Function CloseFile @@*/
status_t CxFileClient::CloseFile(int _handle, CClosure *_callback)
{
    CParam_XfsCloseFile _param;
    _param.Init();
    _param.SetHandle(_handle);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_CLOSEFILE,_cbid);
}
/*@@End Function CloseFile @@*/

/*@@Begin Function ReadFile @@*/
status_t CxFileClient::ReadFile(int _handle, int64_t _offset, int _size, CClosure *_callback)
{
    CParam_XfsReadFile _param;
    _param.Init();
    _param.SetHandle(_handle);
    _param.SetOffset(_offset);
    _param.SetSize(_size);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_READFILE,_cbid);
}
/*@@End Function ReadFile @@*/

/*@@Begin Function WriteFile @@*/
status_t CxFileClient::WriteFile(int _handle, int64_t _offset, uint8_t *_data,int _data_len, CClosure *_callback)
{
    CParam_XfsWriteFile _param;
    _param.Init();
    _param.SetHandle(_handle);
    _param.SetOffset(_offset);
    _param.m_data = _data;
    _param.m_data_size = _data_len;
    int _cbid = this->AddCallback(_callback);
    this->SendRequest(&_param,METHOD_XFS_WRITEFILE,_cbid);
    _param.m_data = NULL;
    _param.m_data_size = 0;
    return OK;
}
/*@@End Function WriteFile @@*/

/*@@Begin Function CreateFolder @@*/
status_t CxFileClient::CreateFolder(CMem *_path, CClosure *_callback)
{
    CParam_XfsCreateFolder _param;
    _param.Init();
    _param.SetPath(_path);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_CREATEFOLDER,_cbid);
}
/*@@End Function CreateFolder @@*/

/*@@Begin Function DeleteFolder @@*/
status_t CxFileClient::DeleteFolder(CMem *_path, CClosure *_callback)
{
    CParam_XfsDeleteFolder _param;
    _param.Init();
    _param.SetPath(_path);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_DELETEFOLDER,_cbid);
}
/*@@End Function DeleteFolder @@*/

/*@@Begin Function DeleteFile @@*/
status_t CxFileClient::DeleteFile(CMem *_path, CClosure *_callback)
{
    CParam_XfsDeleteFile _param;
    _param.Init();
    _param.SetPath(_path);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_XFS_DELETEFILE,_cbid);
}
/*@@End Function DeleteFile @@*/
/*@@ Insert Function Here @@*/
