#include "remote_desktop_server.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "linkrpc_rd_types.h"
#include "remote_desktop_pixel_buffer.h"
#include "remote_desktop_snapshottor_manager.h"
#include "remote_desktop_input.h"
#include "remote_desktop_common.h"

CRemoteDesktopServer::CRemoteDesktopServer()
{
    this->InitBasic();
}

CRemoteDesktopServer::~CRemoteDesktopServer()
{
    this->Destroy();
}

status_t CRemoteDesktopServer::InitBasic()
{
    CRpcServiceBase::InitBasic();
	m_SessionManager.InitBasic();
	m_SnapshottorManager = NULL;
    return OK;
}

status_t CRemoteDesktopServer::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CRpcServiceBase::Init(mgr);
	m_SessionManager.Init(mgr);
	m_SessionManager.StartAutoClearTask();
    return OK;
}

status_t CRemoteDesktopServer::Destroy()
{
	m_SessionManager.Destroy();
    CRpcServiceBase::Destroy();
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopServer::OnSocketConnected()
{
    return OK;
}

status_t CRemoteDesktopServer::OnSocketDisconnected()
{
    return OK;
}

status_t CRemoteDesktopServer::OnRequest(CPeerMessage *_message)
{
    ASSERT(_message);

    int method = _message->GetFunc();
    this->SetDestPeerName(_message->GetFrom());

	m_SessionManager.SessionReactive(_message->GetFrom());

/*##Begin OnRequest_Cpp ##*/
    switch(method)
    {
        case METHOD_RD_LOGIN:
            this->OnLogin(_message);
            break;
        case METHOD_RD_GETMONITORLIST:
            this->OnGetMonitorList(_message);
            break;
        case METHOD_RD_GETNEXTSCREEN:
            this->OnGetNextScreen(_message);
            break;
        case METHOD_RD_SENDINPUT:
            this->OnSendInput(_message);
            break;
        default:
            XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
                "unknown method: %d",method
            );
            break;
    }
/*##End OnRequest_Cpp ##*/
    return OK;
}

/*@@Begin Function OnLogin @@*/
status_t CRemoteDesktopServer::OnLogin(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_RdLogin _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_RdLogin _ret;
    _ret.Init();
	
	if(_param.GetVersion() != REMOTE_DESKTOP_VERSION)
	{
		_ret.SetRetVal0(-1);
		return this->SendReturnValue(&_context,&_ret);
	}

	CMem *sessionName = _message->GetFrom();
	ASSERT(sessionName);

	if(m_SessionManager.GetSession(sessionName))
	{
		m_SessionManager.DelSession(sessionName);
	}

	if(m_SessionManager.CreateSession(sessionName))
	{
		_ret.SetRetVal0(OK);
	}
	else
	{
		_ret.SetRetVal0(ERROR);
	}
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnLogin @@*/

/*@@Begin Function OnGetMonitorList @@*/
status_t CRemoteDesktopServer::OnGetMonitorList(CPeerMessage *_message)
{
    ASSERT(_message);
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_RdGetMonitorList _ret;
    _ret.Init();

	ASSERT(m_SnapshottorManager);

	CMemStk list;
	list.Init();
	m_SnapshottorManager->GetNameList(&list);

	_ret.AllocMonitors(list.GetLen());
	for(int i = 0; i < list.GetLen(); i++)
	{
		_ret.SetMonitorsElem(i,list.GetElem(i));
	}
    
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnGetMonitorList @@*/

/*@@Begin Function OnGetNextScreen @@*/
status_t CRemoteDesktopServer::OnGetNextScreen(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_RdGetNextScreen _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_RdGetNextScreen _ret;
    _ret.Init();

	CRemoteDesktopSession *session = m_SessionManager.GetSession(_message->GetFrom());
	if(!session)
	{
		_ret.SetRes(-2);
		this->SendReturnValue(&_context,&_ret);
		return OK;
	}

	ASSERT(m_SnapshottorManager);
	
	CRemoteDesktopPixelBuffer pbuf;
	pbuf.Init();

	if(!m_SnapshottorManager->TakeScreenSnapshot(_param.GetMonitorName(),&pbuf))
	{
		_ret.SetRes(-3);
		this->SendReturnValue(&_context,&_ret);
		return OK;
	}
	
	if(!pbuf.ChangeBpp(_param.GetBytePerPixel()))
	{
		_ret.SetRes(-4);
		this->SendReturnValue(&_context,&_ret);
		return OK;
	}

	CRemoteDesktopPixelBufferList *diff = _ret.GetDiff();
	ASSERT(diff);

	CRemoteDesktopPixelBuffer *cur_screen = session->GetCurScreen();
	ASSERT(cur_screen);

	if(_param.GetFull() || pbuf.IsSizeOrBppChanged(cur_screen) || session->GetFrameNo()!=_param.GetCurFrameNo())
	{
		cur_screen->Transfer(&pbuf);		
		session->SetFrameNo(_param.GetCurFrameNo()+1);
		diff->CopySingleScreen(cur_screen);
		diff->CompressAll();
		_ret.SetFull(1);
		_ret.SetFrameNo(session->GetFrameNo());
		//XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,"full size=%d",diff->GetTotalPixelDataSize());
	}
	else
	{	
		cur_screen->ChangeBpp(pbuf.GetBytePerPixel());
		cur_screen->Diff(&pbuf,64,64,diff);
		cur_screen->Transfer(&pbuf);	
		diff->CompressAll();
		_ret.SetFull(0);
		session->SetFrameNo(_param.GetCurFrameNo()+1);
		_ret.SetFrameNo(session->GetFrameNo());
		//XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,"part(%d) size=%d",diff->GetLen(),diff->GetTotalPixelDataSize());
	}

	_ret.SetRes(OK);
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnGetNextScreen @@*/


#if _IS_WINDOWS_
static status_t SendLocaInput(CRemoteDesktopInput *input)
{
	ASSERT(input);

	INPUT localIput;
	memset(&localIput,0,sizeof(localIput));

	localIput.type = input->GetType();

	if(input->GetType() == RD_INPUT_TYPE_KEYBOARD_INPUT)
	{
		CRemoteDesktopKeybdInput *ki = input->GetKeybdInput();
		ASSERT(ki);
		localIput.ki.dwFlags = ki->GetDwFlags();
		localIput.ki.wScan = ki->GetWScan();
		localIput.ki.wVk = ki->GetWVk();
	}
	else if(input->GetType() == RD_INPUT_TYPE_MOUSE_INPUT)
	{
		CRemoteDesktopMouseInput *mi = input->GetMouseInput();
		ASSERT(mi);
		localIput.mi.dwFlags = mi->GetDwFlags();
		localIput.mi.dx = mi->GetDx();
		localIput.mi.dy = mi->GetDy();
		localIput.mi.mouseData = mi->GetMouseData();
	}
	else
	{
		XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,"not supported input type %u",input->GetType());
		return ERROR;
	}

	::SendInput(1,&localIput,sizeof(localIput));
	return OK;
}

#else
static status_t SendLocaInput(CRemoteDesktopInput *input)
{
	return OK;
}

#endif


/*@@Begin Function OnSendInput @@*/
status_t CRemoteDesktopServer::OnSendInput(CPeerMessage *_message)
{
    ASSERT(_message);
    CParam_RdSendInput _param;
    _param.Init();
    ASSERT( _param.LoadBson(_message->GetBody()) );
    ///////////////
    ///////////////
    CRpcCallContext _context;
    _context.Init();
    _context.FromPeerMessage(_message); 
    
    CRetVal_RdSendInput _ret;
    _ret.Init();

	int res = SendLocaInput(_param.GetInput());

    _ret.SetRes(res);
    this->SendReturnValue(&_context,&_ret);
    return OK;
}
/*@@End Function OnSendInput @@*/
/*@@ Insert Function Here @@*/

status_t CRemoteDesktopServer::SetRemoteDesktopSnapshottorManager(CRemoteDesktopSnapshottorManager *mgr)
{
	m_SnapshottorManager = mgr;
	return OK;
}

