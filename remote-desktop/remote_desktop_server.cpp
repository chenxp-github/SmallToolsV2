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
static status_t SendLocaInput(CRemoteDesktopServer *self,CRemoteDesktopInput *input)
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

//#if _SUPPORT_X11_
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include "remote_desktop_snapshottor_x11.h"
#include "scancode_keycode_map_x11.h"
static status_t SendLocaInput(CRemoteDesktopServer *self,CRemoteDesktopInput *input)
{	
    ASSERT(input);
    ASSERT(self);
    ASSERT(self->m_SnapshottorManager);

    CMem *monitorName = input->GetMonitorName();
    ASSERT(monitorName);

    CRemoteDesktopSnapshottorManager *snapshotManager = self->m_SnapshottorManager;

    CRemoteDesktopSnapshottor *snapshottor = snapshotManager->GetByName(monitorName);
    ASSERT(snapshottor);
    ASSERT(snapshottor->GetType() == CRemoteDesktopSnapshottor::SNAPSHOTTOR_TYPE_X11);
    CRemoteDesktopSnapshottor_X11 *x11_ss = (CRemoteDesktopSnapshottor_X11*)snapshottor;

    int sh = x11_ss->GetSrcHeight();
    int sw = x11_ss->GetSrcWidth();
    if(sw <= 0)return ERROR;
    if(sh <= 0)return ERROR;

    CxDisplay *xdisplay = x11_ss->GetXDisplay();
    ASSERT(xdisplay);
    CxWindow *xwindow = x11_ss->GetXWindow();
    ASSERT(xwindow);

    if(input->GetType() == RD_INPUT_TYPE_MOUSE_INPUT)
    {
        CRemoteDesktopMouseInput *mouseInput = input->GetMouseInput();
        ASSERT(mouseInput);

        XTestFakeMotionEvent(
            xdisplay->GetNativeXDisplay(),
            -1,
            mouseInput->GetDx()*sw/65536,
            mouseInput->GetDy()*sh/65536,
            CurrentTime
        );

        if(mouseInput->GetDwFlags() & RD_MOUSEEVENTF_LEFTDOWN)
        {
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),1,1,CurrentTime);
        }
        else if(mouseInput->GetDwFlags() & RD_MOUSEEVENTF_LEFTUP)
        {
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),1,0,CurrentTime);
        }
        else if(mouseInput->GetDwFlags() & RD_MOUSEEVENTF_RIGHTDOWN)
        {
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),3,1,CurrentTime);
        }
        else if(mouseInput->GetDwFlags() & RD_MOUSEEVENTF_RIGHTUP)
        {
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),3,0,CurrentTime);
        }
        else if(mouseInput->GetDwFlags() & RD_MOUSEEVENTF_MIDDLEDOWN)
        {
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),2,1,CurrentTime);
        }
        else if(mouseInput->GetDwFlags() & RD_MOUSEEVENTF_MIDDLEUP)
        {
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),2,0,CurrentTime);
        }
        else if(mouseInput->GetDwFlags() & RD_MOUSEEVENTF_WHEEL)
        {
            int id = 4;
            if(mouseInput->GetMouseData()&0x80000000) //<0
            {
                id = 5;
            }
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),id,1,CurrentTime);
            XTestFakeButtonEvent(xdisplay->GetNativeXDisplay(),id,0,CurrentTime);
        }

        xdisplay->Flush();
    }
    else if(input->GetType() == RD_INPUT_TYPE_KEYBOARD_INPUT)
    {
        CRemoteDesktopKeybdInput *keybdInput = input->GetKeybdInput();
        ASSERT(keybdInput);

        KeySym keySym = scancode_to_keycode(keybdInput->GetWScan());
        KeyCode keyCode = XKeysymToKeycode(xdisplay->GetNativeXDisplay(),keySym);

        if(keybdInput->GetDwFlags() & RD_KEYEVENTF_KEYUP)
        {
            XTestFakeKeyEvent(xdisplay->GetNativeXDisplay(),keyCode,0,CurrentTime);

        }
        else
        {
            XTestFakeKeyEvent(xdisplay->GetNativeXDisplay(),keyCode,1,CurrentTime);
        }
    }

    return OK;
}
//#endif

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

    int res = SendLocaInput(this,_param.GetInput());

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

