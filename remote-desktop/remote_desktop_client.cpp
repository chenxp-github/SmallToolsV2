#include "remote_desktop_client.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "linkrpc_rd_types.h"

CRemoteDesktopClient::CRemoteDesktopClient()
{
    this->InitBasic();
}

CRemoteDesktopClient::~CRemoteDesktopClient()
{
    this->Destroy();
}

status_t CRemoteDesktopClient::InitBasic()
{
    CRpcServiceBase::InitBasic();
    return OK;
}

status_t CRemoteDesktopClient::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CRpcServiceBase::Init(mgr);

    return OK;
}

status_t CRemoteDesktopClient::Destroy()
{
    CRpcServiceBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopClient::OnSocketConnected()
{
    return OK;
}

status_t CRemoteDesktopClient::OnSocketDisconnected()
{
    return OK;
}

status_t CRemoteDesktopClient::OnRequest(CPeerMessage *_message)
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

/*@@Begin Function Login @@*/
status_t CRemoteDesktopClient::Login(int _version, CClosure *_callback)
{
    CParam_RdLogin _param;
    _param.Init();
    _param.SetVersion(_version);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_RD_LOGIN,_cbid);
}
/*@@End Function Login @@*/

/*@@Begin Function GetMonitorList @@*/
status_t CRemoteDesktopClient::GetMonitorList(CClosure *_callback)
{
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(NULL,METHOD_RD_GETMONITORLIST,_cbid);
}
/*@@End Function GetMonitorList @@*/



/*@@Begin Function GetNextScreen @@*/
status_t CRemoteDesktopClient::GetNextScreen(CMem *_monitorName, int _curFrameNo, int _bytePerPixel, int _full, CClosure *_callback)
{
    ASSERT(_monitorName);
    CParam_RdGetNextScreen _param;
    _param.Init();
    _param.SetMonitorName(_monitorName);
    _param.SetCurFrameNo(_curFrameNo);
    _param.SetBytePerPixel(_bytePerPixel);
    _param.SetFull(_full);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_RD_GETNEXTSCREEN,_cbid);
}
/*@@End Function GetNextScreen @@*/

/*@@Begin Function SendInput @@*/
status_t CRemoteDesktopClient::SendInput(CRemoteDesktopInput *_input, CClosure *_callback)
{
    ASSERT(_input);
    CParam_RdSendInput _param;
    _param.Init();
    _param.SetInput(_input);
    int _cbid = this->AddCallback(_callback);
    return this->SendRequest(&_param,METHOD_RD_SENDINPUT,_cbid);
}
/*@@End Function SendInput @@*/
/*@@ Insert Function Here @@*/

