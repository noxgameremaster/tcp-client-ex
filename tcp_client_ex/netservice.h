
#ifndef NET_SERVICE_H__
#define NET_SERVICE_H__

#include "netobject.h"
#include <functional>
struct WSAData;

class NetService : public NetObject
{
    using wsa_deleter = std::function<void(WSAData *)>;
private:
    std::unique_ptr<WSAData, wsa_deleter> m_wsaData;

public:
    NetService(NetObject *parent = nullptr);
    ~NetService();

private:
    bool WsaStartup();
    bool WsaCleanup();

protected:
    virtual bool OnInitialize();
    virtual void OnDeinitialize();
    virtual bool OnStarted();
    virtual void OnStopped();

public:
    bool Startup();
    void Shutdown();
};

#endif

