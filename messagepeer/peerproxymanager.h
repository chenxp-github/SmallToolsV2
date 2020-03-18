#ifndef __PEERPROXYMANAGER_H
#define __PEERPROXYMANAGER_H

#include "cruntime.h"
#include "peerproxystk.h"

class CPeerProxyManager{
public:
    CPeerProxyStk *mAllPeers;
public:
    status_t DeletePeer(int index);
    bool IsPeerExist(CMem *name);
    int NameToIndex(CMem *name);
    CPeerProxy * GetPeerByName(CMem *name);
    CPeerProxy * GetPeerByName(const char *name);
    int GetTotalPeers();
    CPeerProxy * GetPeer(int i);
    status_t AddPeer(CPeerProxy *peer);
    CPeerProxyManager();
    virtual ~CPeerProxyManager();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    int GetAliveClientNumber();
    status_t GetPeerProxyIpAddress(const char *peer_name, CMem *ip, int *port);
};

#endif

