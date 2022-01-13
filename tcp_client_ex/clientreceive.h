
#ifndef CLIENT_RECEIVE_H__
#define CLIENT_RECEIVE_H__

#include "netservice.h"
#include <thread>
#include <vector>

class WinSocket;
class SocketSet;
class ClientWorker;
class PacketBufferFix;
class LoopThread;

class ClientReceive : public NetService
{
    static constexpr size_t read_receive_buffer_count = 10240;
private:
    std::shared_ptr<WinSocket> m_netsocket;
    std::unique_ptr<SocketSet> m_readFds;
    std::shared_ptr<PacketBufferFix> m_packetBuffer;
    std::unique_ptr<ClientWorker> m_networker;
    std::unique_ptr<LoopThread> m_receiveThread;

    bool m_stopped;

public:
    explicit ClientReceive(std::shared_ptr<WinSocket> &sock, NetObject *parent = nullptr);
    ~ClientReceive() override;

private:
    bool NotifyErrorToOwner();
    bool ErrorBufferIsFull();
    void OnDisconnected(WinSocket *sock);
    void ReceiveFrom(WinSocket *sock);
    bool DoTask();

    bool OnInitialize() override;
    void OnDeinitialize() override;
    bool OnStarted() override;
    void OnStopped() override;

private:
    DECLARE_SIGNAL(OnReceivePushStream)
private:
    DECLARE_SIGNAL(OnDisconnect, socket_type)
};

#endif

