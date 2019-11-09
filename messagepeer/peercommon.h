#ifndef __PEERCOMMON_H
#define __PEERCOMMON_H

enum{
    PEER_FUNC_INIT_NAME = 0x10000,
    PEER_FUNC_INIT_CHECK_FAIL,
    PEER_FUNC_USER,  //keep last
};

#define MESSAGE_PEER_VERSION 1
#define MESSAGE_PEER_SENDING_QUEUE_LEN 4096

enum{
	PEER_EVENT_GOT_MESSAGE = 9000,
	PEER_EVENT_CONNECTED,
	PEER_EVENT_DISCONNECTED,
	PEER_EVENT_STOPPED,
	PEER_EVENT_CAN_FETCH_MESSAGE,
};

typedef struct {
	int16_t size;
	int16_t version;
	int32_t socket_rw_timeout;
}MessagePeerInitParam_t;

#endif
