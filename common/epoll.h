#ifndef __S2_EPOLL_H
#define __S2_EPOLL_H

#include "cruntime.h"
#include "filebase.h"

#if HAVE_WINDOWS_H
#else
#include <sys/epoll.h>
#endif


class CEpoll{
public:
    enum{
        WAIT_OPT_AUTO_REMOVE = 0x01,
        WAIT_OPT_OPEN_LOG = 0x02,
    };
public:

#if HAVE_WINDOWS_H
	fd_set m_fd_set;
#else
    int m_MaxEventsLen;
    int m_CurEventsLen;
    struct epoll_event *m_Events;
    int m_EpollHandle;
    int m_WaitFail;
#endif

public:
    int Wait(int ms);
    int Wait(int ms,uint32_t opt);
    status_t AutoRemoveHungupFds(uint32_t opt);
    status_t DelFd(int fd);
    status_t AddFd(int fd);
    CEpoll();
    virtual ~CEpoll();
    status_t InitBasic();
    status_t Init(int max_=1024);
    status_t Destroy();
};

#endif
