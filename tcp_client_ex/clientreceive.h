
#ifndef CLIENT_RECEIVE_H__
#define CLIENT_RECEIVE_H__

#include "netservice.h"
#include <thread>
#include <vector>

class WinSocket;
class SocketSet;
class ClientWorker;
class LoopThread;

class ClientReceive : public NetService
{
    static constexpr size_t read_receive_buffer_count = 10240;
private:
    std::shared_ptr<WinSocket> m_netsocket;
    std::unique_ptr<SocketSet> m_readFds;
    std::unique_ptr<ClientWorker> m_networker;
    std::unique_ptr<LoopThread> m_receiveThread;

public:
    explicit ClientReceive(NetObject *parent = nullptr);
    ~ClientReceive() override;

private:
    void OnInitialOnce() override;
    bool NotifyErrorToOwner();
    bool ErrorBufferIsFull();
    void OnDisconnected(WinSocket *sock);
    bool ReceiveFrom(WinSocket *sock);
    bool DoTask();

    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

    std::string ObjectName() const override
    {
        return "ClientReceive";
    }

public:
    void SetReceiveSocket(std::shared_ptr<WinSocket> &sock);

    void DebugShowReceive();

private:
    DECLARE_SIGNAL(OnReceivePushStream)
private:
    DECLARE_SIGNAL(OnDisconnect, socket_type)
};

#endif

