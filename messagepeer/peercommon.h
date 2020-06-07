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

#define PEER_GLOBAL_CONTEXT_DEFINE()\
const void *_peer_global_context_;\
void SetPeerGlobalContext(const void *g)\
{\
	_peer_global_context_=g;\
}\
const void* GetPeerGlobalContext()\
{\
    return _peer_global_context_;\
}\

#define PEER_GLOBAL_CONTEXT_CLEAR()\
_peer_global_context_=NULL;

#define PEER_GLOBALS(p)\
CPeerGlobals *p=(CPeerGlobals*)_peer_global_context_;\
ASSERT(p)\

#define PEER_GLOBALS_FROM_OBJ(p,obj)\
CPeerGlobals *p=(CPeerGlobals*)(obj)->_peer_global_context_;\
ASSERT(p)\

#define GLOBAL_PEER_CALLBACK_MAP(map)\
PEER_GLOBALS(_g##map);\
CCallbackMap *map=_g##map->GetCallbackMap();\
ASSERT(map)\

#define GLOBAL_PEER_MANAGER(mgr)\
PEER_GLOBALS(_g##mgr);\
CPeerProxyManager *mgr=_g##mgr->GetPeerProxyManager();\
ASSERT(mgr)\

#define PASS_GLOBAL_PEER_CONTEXT(from,to)\
(to)->SetPeerGlobalContext((from)->GetPeerGlobalContext())\

#endif
