
#ifndef NET_SERVICE_H__
#define NET_SERVICE_H__

#include "netobject.h"
#include <functional>
#include <atomic>

struct WSAData;

class NetService : public NetObject
{
    using wsa_deleter = std::function<void(WSAData *)>;
private:
    std::unique_ptr<WSAData, wsa_deleter> m_wsaData;
    std::atomic<bool> m_turnOn;

public:
    NetService(NetObject *parent = nullptr);
    ~NetService() override;

private:
    bool WsaStartup();
    bool WsaCleanup();

protected:
    virtual bool OnInitialize();
    virtual void OnDeinitialize();
    virtual bool OnStarted();
    virtual void OnStopped();

    std::string GetErrorMessage();
    virtual void OnError(const std::string &/*title*/, const std::string &/*content*/) {}
    void NotifyOccurError(NetService *net, const std::string &errorTitle, const std::string &errorMessage);

public:
    bool Startup();
    void Shutdown();
};

#endif

