#include "epoll.h"
#include "syslog.h"
#include "mem_tool.h"

#ifndef EPOLLRDHUP
#define EPOLLRDHUP EPOLLHUP
#endif

CEpoll::CEpoll()
{
    this->InitBasic();
}
CEpoll::~CEpoll()
{
    this->Destroy();
}
status_t CEpoll::InitBasic()
{
#if HAVE_WINDOWS_H
	memset(&m_fd_set,0,sizeof(m_fd_set));
#else
    m_Events = NULL;
    m_CurEventsLen = 0;
    m_MaxEventsLen = 0;
    m_EpollHandle = 0;
    m_WaitFail = 0;
#endif
    return OK;
}
status_t CEpoll::Init(int max_)
{
    this->InitBasic();
#if HAVE_WINDOWS_H
#else
    ASSERT(m_EpollHandle == 0);
    m_EpollHandle = epoll_create(max_);
    ASSERT(m_EpollHandle > 0);
    ZALLOC(m_Events,struct epoll_event, max_);
    m_MaxEventsLen = max_;
#endif
    return OK;
}
status_t CEpoll::Destroy()
{   
#if HAVE_WINDOWS_H
#else
    if(m_EpollHandle)
    {
        close(m_EpollHandle);
        m_EpollHandle = 0;
    }
    FREE(m_Events);
    this->InitBasic();
#endif
    return OK;
}

status_t CEpoll::AddFd(int fd)
{
#if HAVE_WINDOWS_H
	FD_SET(fd,&m_fd_set);
	return OK;
#else
    struct epoll_event ev;
    ev.events= EPOLLIN | EPOLLRDHUP;
    ev.data.fd= fd;
    int ret = epoll_ctl(m_EpollHandle,EPOLL_CTL_ADD,fd,&ev);
    return ret == 0;
#endif
}

status_t CEpoll::DelFd(int fd)
{
#if HAVE_WINDOWS_H
	FD_CLR(fd,&m_fd_set);
	return OK;
#else
    int r = epoll_ctl(m_EpollHandle,EPOLL_CTL_DEL,fd,NULL);
	return r == 0;
#endif
}

#if HAVE_WINDOWS_H

static status_t copy_fd_set(fd_set *to, fd_set *from)
{
	to->fd_count = from->fd_count;
	for(uint32_t i = 0; i < from->fd_count; i++)
	{
		to->fd_array[i] = from->fd_array[i];
	}
	return OK;
}

#endif

status_t CEpoll::AutoRemoveHungupFds(uint32_t opt)
{
#if HAVE_WINDOWS_H

#else
    for(int i = 0; i < m_CurEventsLen; i++)
    {
        if(m_Events[i].events&EPOLLRDHUP)
        {
            if(this->DelFd(m_Events[i].data.fd))
            {
                if(opt & WAIT_OPT_OPEN_LOG)
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                        "socket %d is auto removed from epoll",
                        m_Events[i].data.fd
                    );
                }
            }
            else
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                    "fail to auto remove socket %d from epoll",
                    m_Events[i].data.fd
                );
            }
        }
    }
#endif
    return OK;
}

int CEpoll::Wait(int ms, uint32_t opt)
{
#if HAVE_WINDOWS_H	
	fd_set read_set,error_set;
	copy_fd_set(&read_set,&m_fd_set);
	copy_fd_set(&error_set,&m_fd_set);
	
	struct timeval timeout; 
	timeout.tv_sec = (ms/1000); 
	timeout.tv_usec =(ms%1000)*1000;
	
	int res = ::select(0,&read_set,NULL,&error_set,&timeout);
	if(res <= 0)crt_msleep(ms);

	if(opt & WAIT_OPT_AUTO_REMOVE)
    {        
		for(uint32_t i = 0; i < error_set.fd_count; i++)
		{
			FD_CLR(error_set.fd_array[i],&m_fd_set);
			if(opt & WAIT_OPT_OPEN_LOG)
            {
                XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
                    "socket %d is auto removed from epoll",
                    error_set.fd_array[i]
				);
            }
		}
    }

	return OK;
#else
    int ret = epoll_wait(m_EpollHandle,m_Events,m_MaxEventsLen,ms);
    if(ret == 0)
    {
        m_WaitFail = 0;
    }
    else
    {
        m_WaitFail ++;
        //force sleep to avoid cpu usage is too high
        if(m_WaitFail > 100) 
        {
            m_WaitFail = 0;
            crt_msleep(1);
        }
    }

    m_CurEventsLen = ret;
    ASSERT(m_CurEventsLen < m_MaxEventsLen);
    if(opt & WAIT_OPT_AUTO_REMOVE)
    {
        this->AutoRemoveHungupFds(opt);
    }
    return ret;
#endif
}

int CEpoll::Wait(int ms)
{
    return this->Wait(ms,WAIT_OPT_AUTO_REMOVE|WAIT_OPT_OPEN_LOG);
}

status_t CEpoll::AutoRemoveFd(int fd)
{
#if HAVE_WINDOWS_H
	if(FD_ISSET(fd,&m_fd_set) && this->DelFd(fd))
	{
		XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
            "socket %d is removed from epoll",fd
        );
	}
#else
    if(m_EpollHandle <= 0)
        return OK;
    if(this->DelFd(fd))
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
            "socket %d is removed from epoll",fd
        );
    }
#endif
    return OK;
}