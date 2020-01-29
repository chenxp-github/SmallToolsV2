#include "peerproxymanager.h"
#include "syslog.h"

CPeerProxyManager::CPeerProxyManager()
{
    this->InitBasic();
}
CPeerProxyManager::~CPeerProxyManager()
{
    this->Destroy();
}
status_t CPeerProxyManager::InitBasic()
{
    this->mAllPeers = NULL;
    return OK;
}
status_t CPeerProxyManager::Init()
{
    this->Destroy();
    NEW(this->mAllPeers,CPeerProxyStk);
    this->mAllPeers->Init(1024);
    return OK;
}
status_t CPeerProxyManager::Destroy()
{
    DEL(this->mAllPeers);
    this->InitBasic();
    return OK;
}

status_t CPeerProxyManager::AddPeer(CPeerProxy *peer)
{
    ASSERT(peer);
    return this->mAllPeers->InsOrderedPtr(peer,0,1);
}

CPeerProxy * CPeerProxyManager::GetPeer(int i)
{
    return this->mAllPeers->GetElem(i);
}

int CPeerProxyManager::GetTotalPeers()
{
    return this->mAllPeers->GetLen();
}

CPeerProxy * CPeerProxyManager::GetPeerByName(CMem *name)
{
    CPeerProxy tmp;
    tmp.mPeerName = name; //only use name to compare
    CPeerProxy *p = this->mAllPeers->BSearchNode(&tmp,0);
    tmp.mPeerName = NULL;
    return p;
}

int CPeerProxyManager::NameToIndex(CMem *name)
{
    CPeerProxy tmp;
    tmp.mPeerName = name; //only use name to compare
    int index = this->mAllPeers->BSearch(&tmp,0);    
    tmp.mPeerName = NULL;
    return index;
}

bool CPeerProxyManager::IsPeerExist(CMem *name)
{
    return this->GetPeerByName(name) != NULL;
}

status_t CPeerProxyManager::DeletePeer(int index)
{
    return this->mAllPeers->DelElem(index);
}

int CPeerProxyManager::GetAliveClientNumber()
{
    int c = 0;
    for(int i = 0; i < mAllPeers->GetLen(); i++)
    {
        CPeerProxy *proxy = mAllPeers->GetElem(i);
        if((!proxy->IsServerSide()) && proxy->IsConnectionAlive())
            c++;
    }
    return c;
}

CPeerProxy * CPeerProxyManager::GetPeerByName(const char *name)
{
    ASSERT(name);
    CMem mem(name);
    return this->GetPeerByName(&mem);
}