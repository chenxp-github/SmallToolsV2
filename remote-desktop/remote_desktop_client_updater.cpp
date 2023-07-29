#include "remote_desktop_client_updater.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "minitask.h"
#include "linkrpc_rd_types.h"
#include "callback.h"
#include "weak_pointer.h"

CRemoteDesktopClientUpdater::CRemoteDesktopClientUpdater()
{
    this->InitBasic();
}

CRemoteDesktopClientUpdater::~CRemoteDesktopClientUpdater()
{
    this->Destroy();
}

status_t CRemoteDesktopClientUpdater::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    TASK_CONTAINER_CLEAR();
    this->m_frame_no = 0;
    this->m_cur_screen.InitBasic();
    this->m_client = NULL;
    this->m_remote_monitor_index = 0;
    this->m_total_transfer_size = 0;
    this->m_remote_monitor_names.InitBasic();
/*##End InitBasic##*/
	m_auto_update_task = 0;
	m_callback.InitBasic();
	m_delay_per_frame = 100;
	m_byte_per_pixel = 2;
    return OK;
}

status_t CRemoteDesktopClientUpdater::Init(CTaskMgr *_taskmgr)
{
    this->InitBasic();
/*##Begin Init##*/
    TASK_CONTAINER_INIT(_taskmgr);
    this->m_cur_screen.Init();
    this->m_remote_monitor_names.Init();
/*##End Init##*/
	m_callback.Init();
    return OK;
}

status_t CRemoteDesktopClientUpdater::Destroy()
{
	m_callback.Destroy();
	QuitTask(&m_auto_update_task);
/*##Begin Destroy##*/
    WEAK_REF_DESTROY();
    this->m_cur_screen.Destroy();
    this->m_remote_monitor_names.Destroy();
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopClientUpdater::Copy(CRemoteDesktopClientUpdater *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
    this->m_frame_no = _p->m_frame_no;
    this->m_cur_screen.Copy(&_p->m_cur_screen);
    this->m_client = _p->m_client;
    this->m_remote_monitor_index = _p->m_remote_monitor_index;
    this->m_total_transfer_size = _p->m_total_transfer_size;
    this->m_remote_monitor_names.Copy(&_p->m_remote_monitor_names);
/*##End Copy##*/
    return OK;
}

int CRemoteDesktopClientUpdater::Comp(CRemoteDesktopClientUpdater *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopClientUpdater::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    _buf->Log("frame_no = %d",m_frame_no);
    _buf->Log("cur_screen = {");
    _buf->IncLogLevel(1);
    this->m_cur_screen.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
    _buf->Log("client = 0x%p",m_client);
    _buf->Log("remote_monitor_index = %d",m_remote_monitor_index);
    _buf->Log("total_transfer_size = %lld",m_total_transfer_size);
    _buf->Log("remote_monitor_names = {");
    _buf->IncLogLevel(1);
    this->m_remote_monitor_names.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("");
    _buf->Log("},");
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetClient@@*/
CRemoteDesktopClient* CRemoteDesktopClientUpdater::GetClient()
{
    return m_client;
}
/*@@End  Function GetClient@@*/

/*@@Begin Function SetClient@@*/
status_t CRemoteDesktopClientUpdater::SetClient(CRemoteDesktopClient *_client)
{
    this->m_client = _client;
    return OK;
}
/*@@End  Function SetClient@@*/


/*@@Begin Function GetFrameNo@@*/
int CRemoteDesktopClientUpdater::GetFrameNo()
{
    return m_frame_no;
}
/*@@End  Function GetFrameNo@@*/

/*@@Begin Function GetCurScreen@@*/
CRemoteDesktopPixelBuffer* CRemoteDesktopClientUpdater::GetCurScreen()
{
    return &m_cur_screen;
}
/*@@End  Function GetCurScreen@@*/

/*@@Begin Function SetFrameNo@@*/
status_t CRemoteDesktopClientUpdater::SetFrameNo(int _frame_no)
{
    this->m_frame_no = _frame_no;
    return OK;
}
/*@@End  Function SetFrameNo@@*/

/*@@Begin Function GetRemoteMonitorIndex@@*/
int CRemoteDesktopClientUpdater::GetRemoteMonitorIndex()
{
    return m_remote_monitor_index;
}
/*@@End  Function GetRemoteMonitorIndex@@*/

/*@@Begin Function GetRemoteMonitorNames@@*/
CMemStk* CRemoteDesktopClientUpdater::GetRemoteMonitorNames()
{
    return &m_remote_monitor_names;
}
/*@@End  Function GetRemoteMonitorNames@@*/

/*@@Begin Function SetRemoteMonitorIndex@@*/
status_t CRemoteDesktopClientUpdater::SetRemoteMonitorIndex(int _remote_monitor_index)
{
    this->m_remote_monitor_index = _remote_monitor_index;
    return OK;
}
/*@@End  Function SetRemoteMonitorIndex@@*/

/*@@Begin Function GetTotalTransferSize@@*/
int64_t CRemoteDesktopClientUpdater::GetTotalTransferSize()
{
    return m_total_transfer_size;
}
/*@@End  Function GetTotalTransferSize@@*/

/*@@Begin Function SetTotalTransferSize@@*/
status_t CRemoteDesktopClientUpdater::SetTotalTransferSize(int64_t _total_transfer_size)
{
    this->m_total_transfer_size = _total_transfer_size;
    return OK;
}
/*@@End  Function SetTotalTransferSize@@*/
/*@@ Insert Function Here @@*/

status_t CRemoteDesktopClientUpdater::StartAutoUpdateTask()
{
	ASSERT(!IsTask(m_auto_update_task));

	BEGIN_MINI_TASK(task)
	{
		CRemoteDesktopClientUpdater *self;
		status_t Run(uint32_t interval)
		{
			ASSERT(self->m_client);
			if(self->m_client->IsConnected())
			{
				self->AutoUpdateTaskRun(this,interval);
			}
			else
			{
				this->Sleep(100);
			}
			return OK;
		}
	}
	END_MINI_TASK(task);
	
	task->Init(GetTaskMgr());
	task->self = this;
	task->Start(1);    
	m_auto_update_task = task->GetId();
	return OK;
}

status_t CRemoteDesktopClientUpdater::StopAutoUpdateTask()
{
	QuitTask(&m_auto_update_task);
	return OK;
}

enum{
	STEP_IDLE,
	STEP_LOGIN,
	STEP_GET_MONITOR_NAMES,
	STEP_CHECK_MONITOR_NAME,
	STEP_GET_NEXT_SCREEN,
};

status_t CRemoteDesktopClientUpdater::AutoUpdateTaskRun(CMiniTask *task, uint32_t interval)
{
	if(task->Step() == STEP_IDLE)
	{
		task->Sleep(100);
	}
	
	if(task->Step() == STEP_LOGIN)
	{
		BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_RdLogin)
		{
			CLOSURE_PARAM_WEAKPTR(CRemoteDesktopClientUpdater*,self,10);
			CLOSURE_PARAM_PTR(CMiniTask*,task,11);
			
			if(_e == CCallback::E_OK)
			{
				if(_ret.GetRetVal0() == OK)
				{
					task->Goto(STEP_GET_MONITOR_NAMES);
					return OK;
				}
				else
				{
					XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,"Login fail, RetVal0=%d",_ret.GetRetVal0());
				}
			}

			task->Sleep(1000);
			task->Goto(STEP_LOGIN);
			return OK;
		}
		END_NEW_LINKRPC_CALLBACK(done,CRetVal_RdLogin);
	
		CWeakPointer<CRemoteDesktopClientUpdater> wp(this);
		done->SetParamWeakPointer(10,&wp);
		done->SetParamPointer(11,task);

		m_client->Login(REMOTE_DESKTOP_VERSION,done);
		task->Goto(STEP_IDLE);
	}

	if(task->Step() == STEP_GET_MONITOR_NAMES)
	{
		BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_RdGetMonitorList)
		{
			CLOSURE_PARAM_WEAKPTR(CRemoteDesktopClientUpdater*,self,10);
			CLOSURE_PARAM_PTR(CMiniTask*,task,11);

			if(_e == CCallback::E_OK)
			{
				self->m_remote_monitor_names.Clear();
				for(int i = 0; i < _ret.GetMonitorsSize(); i++)
				{
					CMem *monitor = _ret.GetMonitorsElem(i);
					self->m_remote_monitor_names.Push(monitor);
				}
				self->Callback()->Run(CRemoteDesktopClientUpdater::EVENT_RECV_MONITOR_NAMES);
				task->Goto(STEP_CHECK_MONITOR_NAME);
			}
			else
			{
				task->Sleep(1000);
				task->Goto(STEP_GET_MONITOR_NAMES);
			}
			return OK;
		}
		END_NEW_LINKRPC_CALLBACK(done,CRetVal_RdGetMonitorList);
		
		CWeakPointer<CRemoteDesktopClientUpdater> wp(this);
		done->SetParamWeakPointer(10,&wp);
		done->SetParamPointer(11,task);
		m_client->GetMonitorList(done);
		task->Goto(STEP_IDLE);
	}

	if(task->Step() == STEP_CHECK_MONITOR_NAME)
	{

		if(m_remote_monitor_names.GetLen() <= 0)
		{
			task->Sleep(100);
			task->Goto(STEP_GET_MONITOR_NAMES);
			return OK;
		}

		if(m_remote_monitor_index >= 0)
		{

			CMem *monitor_name = m_remote_monitor_names.GetElem(m_remote_monitor_index);
			if(monitor_name)
			{
				task->Goto(STEP_GET_NEXT_SCREEN);
			}
			else
			{
				task->Sleep(100);
			}
			
		}
		else
		{
			task->Sleep(100);
		}
	}

	if(task->Step() == STEP_GET_NEXT_SCREEN)
	{
		CMem *monitor_name = m_remote_monitor_names.GetElem(m_remote_monitor_index);
		ASSERT(monitor_name);
		
		BEGIN_NEW_LINKRPC_CALLBACK(done,CRetVal_RdGetNextScreen)
		{
			CLOSURE_PARAM_WEAKPTR(CRemoteDesktopClientUpdater*,self,10);
			CLOSURE_PARAM_PTR(CMiniTask*,task,11);
			
			if(_e == CCallback::E_OK)
			{
				if(_ret.GetRes() == OK)
				{
					self->OnGetNextScreen(&_ret);
				}
				else
				{
					XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,"GetNextScreen fail, res=%d",_ret.GetRes());
				}
			}
			else
			{
				XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,"GetNextScreen:timeout");
			}

			task->Goto(STEP_GET_NEXT_SCREEN);
			task->Sleep(self->m_delay_per_frame);
			return OK;
		}
		END_NEW_LINKRPC_CALLBACK(done,CRetVal_RdGetNextScreen);
		
		CWeakPointer<CRemoteDesktopClientUpdater> wp(this);
		done->SetParamWeakPointer(10,&wp);
		done->SetParamPointer(11,task);
		m_client->GetNextScreen(monitor_name,m_frame_no,m_byte_per_pixel,m_frame_no == 0,done);
		task->Goto(STEP_IDLE);
	}

	return OK;
}

status_t CRemoteDesktopClientUpdater::OnGetNextScreen(CRetVal_RdGetNextScreen *ret)
{
	ASSERT(ret);

    int full = ret->GetFull();
    CRemoteDesktopPixelBufferList *diff = ret->GetDiff();
	ASSERT(diff);
	
	m_total_transfer_size += diff->GetTotalPixelDataSize();

	if(full)
	{
		CRemoteDesktopPixelBuffer *pixBuf = diff->GetElem(0);
		ASSERT(pixBuf);
		pixBuf->DecompressData();
		m_cur_screen.Transfer(pixBuf);
		m_frame_no++;
		this->NotifyRecvNewScreen();
	}
	else
	{
		if(diff->GetLen() > 0)
		{
			diff->DecompressAll();
			ASSERT(m_cur_screen.ApplyPatch(diff));
			this->NotifyRecvNewScreen();
		}
		m_frame_no++;
	}
	return OK;
}

CClosure *CRemoteDesktopClientUpdater::Callback()
{
	return &m_callback;
}

status_t CRemoteDesktopClientUpdater::NotifyRecvNewScreen()
{
	m_callback.SetParamPointer(1,&m_cur_screen);
	m_callback.Run(EVENT_RECV_NEW_SCREEN);
	return OK;
}

status_t CRemoteDesktopClientUpdater::SendInput(CRemoteDesktopInput *input)
{
	ASSERT(input);
	ASSERT(m_client);
	if(m_client->GetSendingQueueLength("") < 512)
	{
		return m_client->SendInput(input,NULL);
	}
	return ERROR;
}

status_t CRemoteDesktopClientUpdater::SetDelayPerFrame(uint32_t ms)
{
	if(ms < 20)ms=20;
	m_delay_per_frame = ms;
	return OK;
}

status_t CRemoteDesktopClientUpdater::SetBytePerPixel(int bpp)
{
	ASSERT(bpp >=1 && bpp <=4);
	m_byte_per_pixel = bpp;
	return OK;
}

